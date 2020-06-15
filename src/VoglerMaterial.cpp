/*
 * 
 *  Copyright (C) 2014 TU Delft. All rights reserved.
 *  
 *  This class implements the material model for polymers
 *  from Vogler et al. (2013)
 *  
 *  Author: F.P. van der Meer, f.p.vandermeer@tudelft.nl
 *  Date: October 2014
 *
 */


#include <jem/base/limits.h>
#include <jem/base/Float.h>
#include <jem/base/Error.h>
#include <jem/base/PrecheckException.h>
#include <jem/io/PrintWriter.h>
#include <jem/numeric/algebra/LUSolver.h>
#include <jem/numeric/algebra/utilities.h>
#include <jem/numeric/func/UserFunc.h>
#include <jem/util/Properties.h>
#include <jem/numeric/algebra/matmul.h>
#include <jem/numeric/utilities.h>
#include <jem/base/System.h>
#include <jive/util/FuncUtils.h>

#include "utilities.h"
#include "VoglerMaterial.h"

using namespace jem;
using jem::numeric::matmul;
using jem::numeric::LUSolver;
using jem::numeric::UserFunc;
using jem::io::PrintWriter;
using jive::util::FuncUtils;

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


VoglerMaterial::VoglerMaterial 

  ( idx_t rank, const Properties& globdat )
    : Super ( 3, globdat ), localRank_ ( rank ), globdat_ ( globdat )

{
  rmTolerance_ = 1.e-10;
  rmMaxIter_ = 100;
  ffac_ = 1.;

  // preallocate memory for update 

  prealloc_.resize ( 6+3, 6 );
}


VoglerMaterial::~VoglerMaterial ()
{}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void VoglerMaterial::configure ( const Properties& props )
{
  Super::configure ( props );

  props.find ( rmTolerance_, "rmTolerance" );
  props.find ( rmMaxIter_, "rmMaxIter" );
  props.find ( ffac_, "ffac" );
  
  props.get ( poissonP_, "poissonP" );

  Array<Ref<Function> >  alphas ( 6 );

  alphas[0] = makeFunc_ ( props, "valpha1" );
  alphas[1] = makeFunc_ ( props, "valpha2" );
  alphas[2] = makeFunc_ ( props, "valpha3t" );
  alphas[3] = makeFunc_ ( props, "valpha3c" );
  alphas[4] = makeFunc_ ( props, "valpha32t" );
  alphas[5] = makeFunc_ ( props, "valpha32c" );

  // need to read state, because Super is 3D

  if ( localRank_ == 2  )
  {
    props.get( state_, STATE_PROP );
  }

  if ( state_ == "PLANE_STRESS" )
  {
    System::warn() << "VoglerMaterial does not have a consistent "
      << "implementation of plane stress plasticity." << endl;
  }

  // initialize history variables

  historyNames_.resize ( 9 );
  historyNames_[0] = "epsp_11";
  historyNames_[1] = "epsp_22";
  historyNames_[2] = "epsp_33";
  historyNames_[3] = "epsp_12";
  historyNames_[4] = "epsp_23";
  historyNames_[5] = "epsp_31";
  historyNames_[6] = "epspeq";
  historyNames_[7] = "diss";
  historyNames_[8] = "loading";

  // compute dependent variables

  beta1_ = 1. + poissonP_;
  beta2_ = 1. + poissonP_;
  beta32_ = 0.25 * ( 1. - poissonP_ );
  Mat66 mmat = YieldFunc_::fillKMat ( beta1_, beta2_, beta32_, 0. );

  // initialize some matrices

  for ( idx_t i = 0; i < 6; ++i )
  {
    for ( idx_t j = 0; j < 6; ++j )
    {
      dmat_(i,j) = materialStiffMat_(i,j);
    }
  }

  // initialize yield function

  y_ = newInstance<YieldFunc_> ( dmat_, mmat, alphas, ffac_ );
  y_-> setRmSettings ( rmTolerance_, rmMaxIter_ );
}

//-----------------------------------------------------------------------
//   makeFunc_
//-----------------------------------------------------------------------

Ref<Function> VoglerMaterial::makeFunc_ 

  ( const Properties& props,
    const String&     name ) const

{
  String args = "x";
  props.find ( args, "args" );

  Ref<Function> func = FuncUtils::newFunc ( args, name, props, globdat_ );

  FuncUtils::resolve ( *func, globdat_ );

  return func;
}

//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void VoglerMaterial::getConfig ( const Properties& conf ) const
{
  Super::getConfig ( conf );

  conf.set ( "poissonP", poissonP_ );
  conf.set ( "rmMaxIter", rmMaxIter_ );
  conf.set ( "rmTolerance", rmTolerance_ );
  conf.set ( "ffac", ffac_ );

  if ( localRank_ == 2 )
  {
    conf.set ( STATE_PROP, state_ );
  }
}

//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------

void VoglerMaterial::update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      idx_t                 ipoint )

{
  if ( localRank_ == 3 )
  {
    Super::update ( stress, stiff, strain, ipoint );
  }
  else
  {
    // call update function on 3D strain, 
    // Super::update will perform transformation and call mUpdate

    Matrix stiff66 ( prealloc_(slice(0,6),ALL) );
    Vector strain6 ( prealloc_(6,ALL) );
    Vector stress6 ( prealloc_(7,ALL) );

    Vec6 eps ( fill3DStrain ( strain ) );
    for ( idx_t i = 0; i < 6; ++i ) strain6[i] = eps[i];

    Super::update ( stress6, stiff66, strain6, ipoint );

    // extract 2D stress and stiffness values 

    reduce3DVector ( stress, stress6 );
    reduce3DMatrix ( stiff, stiff66 );
  }
}

//-----------------------------------------------------------------------
//   mUpdate
//-----------------------------------------------------------------------

void VoglerMaterial::mUpdate

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      idx_t                 ipoint )

{
  // compute 3D stress in local coordinate frame
  // with preferred direction in first entry (eps_11=eps_f)

  Mat66  dcon;
  Vec6   eps   ( fill3DStrain ( strain ) ); 
  bool   loading = false;

  Vec6   depsp;

  double epspeq0 = preHist_[ipoint].epspeq;
  Vec6   sigtr = matmul ( dmat_, Vec6(eps-preHist_[ipoint].epsp) );
  Vec6   sig = sigtr;

  double dgam = 0.;

  if ( y_->isPlastic ( sigtr, epspeq0 ) )
  {
    try
    {
      // classical return mapping scheme starting with dgam = 0.

      dgam = y_->findRoot ( 0. );
    }
    catch ( const Exception& ex )
    {
      handleException_ ( ex, strain, ipoint );

      // more robust return mapping scheme

      double gmin, gmax, fmin, fmax;

      y_->findBounds ( gmin, gmax, fmin, fmax );

      dgam = y_->findRoot ( gmin, gmax, fmin, fmax );
    }
    sig = y_->getStress();
    dcon = y_->getTangent( dgam );
    Vec6 depsp = dgam * y_->getNg();


    double dG = dot ( sig, depsp );
    newHist_[ipoint].epsp = preHist_[ipoint].epsp + depsp;
    newHist_[ipoint].epspeq = y_->getEpspeq();
    newHist_[ipoint].dissipation = preHist_[ipoint].dissipation + dG;
    loading = true;
  }
  else
  {
    dcon = dmat_;

    newHist_[ipoint].epsp = preHist_[ipoint].epsp;
    newHist_[ipoint].epspeq = preHist_[ipoint].epspeq;
    newHist_[ipoint].dissipation = preHist_[ipoint].dissipation;
  }
  newHist_[ipoint].loading = loading;

  latestHist_ = &newHist_;

  // use initial stiffness for oscillating point in desparateMode

  if ( desperateMode_ )
  {
    if ( ! useSecant_[ipoint] )
    {
      bool switches = ( loading != preHist_[ipoint].loading );

      if ( switches != hasSwitched_[ipoint] )
      {
        useSecant_[ipoint] = true;
      }
      if ( useSecant_[ipoint] )
      {
        dcon = dmat_;
      }
    }
  }
  reduce3DMatrix_ ( stiff, dcon );
  reduce3DVector_ ( stress, sig );
}

//-----------------------------------------------------------------------
//   handleException_
//-----------------------------------------------------------------------

void VoglerMaterial::handleException_

  ( const Exception&      ex,
    const Vector&         strain,
    const idx_t           ipoint )

{
  Ref<PrintWriter> out = dynamicCast<PrintWriter>(y_->getDbgWriter());

  idx_t nold = out->nformat.getFractionDigits();
  out->nformat.setFractionDigits ( 10 );

  if ( ex.what().equals ( "No convergence" ) )
  {
    *out << "No convergence in return mapping algorithm " << endl;
  }
  else if ( ex.what().equals ( "nan" ) )
  {
    *out << "NaN detected in return mapping algorithm " << endl;
  }
  else if ( ex.what().equals ( "Negative dgam" ) )
  {
    *out << "Negative increment found" << endl;
  }
  else
  {
    *out << "Caught unkown exception: " << ex.what() << endl;
  }
  *out << "epsp " << preHist_[ipoint].epsp << endl;
  *out << "epspeq0 " << preHist_[ipoint].epspeq << endl;
  *out << "strain " << strain << endl << endl;;
  out->nformat.setFractionDigits ( nold );
}

//-----------------------------------------------------------------------
//   getDissipationStress
//-----------------------------------------------------------------------

void VoglerMaterial::getDissipationStress

    ( const Vector&   sstar,
      const Vector&   strain,
      const idx_t     ipoint ) const

{
  // compute 3D stress in local coordinate frame
  // with preferred direction in first entry (eps_11=eps_f)

  Mat66  dcon;
  Vec6   eps   ( fill3DStrain ( strain ) ); 

  // NB, use preHist. After commit, this contains the new values,
  // after cancel the old ones. In both cases the starting values
  // for the next solve
 
  double epspeq = preHist_[ipoint].epspeq;
  Vec6   epsp = preHist_[ipoint].epsp;
  Vec6   sig = matmul ( dmat_, Vec6(eps-epsp) );
  Vec6   sstar6;

  double dgam = 0.;

  if ( y_->isPlastic ( sig, epspeq ) )
  {
    y_->resetDGam();
    dcon = y_->getTangent( dgam );
    Vec6 epstmp = 2. * epsp - eps;
    sstar6 = sig + matmul ( dcon.transpose(), epstmp );
  }
  else
  {
    sstar6 = matmul ( dmat_, epsp );
  }
  reduce3DVector_ ( sstar, sstar6 );

  // sstar = 0.;
}

//-----------------------------------------------------------------------
//   getHistory
//-----------------------------------------------------------------------

void VoglerMaterial::getHistory

  ( const Vector&  hvals,
    const idx_t    mpoint ) const

{
  (*latestHist_)[mpoint].toVector ( hvals );
}

//-----------------------------------------------------------------------
//   setHistory
//-----------------------------------------------------------------------

void VoglerMaterial::setHistory

  ( const Vec6&    epsp,
    const double   epspeq,
    const idx_t    ipoint )

{
  preHist_[ipoint].epspeq = epspeq;
  preHist_[ipoint].epsp   = epsp;
}

//-----------------------------------------------------------------------
//   commit
//-----------------------------------------------------------------------

void VoglerMaterial::commit () 

{
  // System::out() << "depsp0 " << newHist_[0].epsp-preHist_[0].epsp << endl;
  newHist_.swap ( preHist_ );

  latestHist_ = &preHist_;
}

//-----------------------------------------------------------------------
//   clone
//-----------------------------------------------------------------------

Ref<Material> VoglerMaterial::clone () const

{
  // use default copy constructor

  return newInstance<VoglerMaterial> ( *this );
}

//-----------------------------------------------------------------------
//   deviatoric
//-----------------------------------------------------------------------

Tuple<double,6> VoglerMaterial::deviatoric

  ( const Vec6&   full,
    const double  p ) const

{
  Vec6 ret = full;

  ret[0] -= p;
  ret[1] -= p;
  ret[2] -= p;

  return ret;
}

//-----------------------------------------------------------------------
//   giveHistory
//-----------------------------------------------------------------------

double VoglerMaterial::giveHistory ( const idx_t ip ) const
{
  return (*latestHist_)[ip].epspeq;
}

//-----------------------------------------------------------------------
//   allocPoints
//-----------------------------------------------------------------------

void VoglerMaterial::allocPoints

  ( idx_t count )

{
  for ( idx_t i = 0; i < count; ++i )
  {
    preHist_.pushBack ( Hist_() );
    newHist_.pushBack ( Hist_() );
  }
}

//-----------------------------------------------------------------------
//   giveDissipation
//-----------------------------------------------------------------------

double VoglerMaterial::giveDissipation ( const idx_t ipoint ) const
{   
  return (*latestHist_)[ipoint].dissipation;
}

//-----------------------------------------------------------------------
//   reduce3DMatrix_
//-----------------------------------------------------------------------

void VoglerMaterial::reduce3DMatrix_

  ( const Matrix&   m,
    const Mat66&    t ) const 

{
  if ( m.size(0) == 3 )
  {
    JEM_ASSERT ( state_ == "PLANE_STRAIN" );
    m(0,0) = t(0,0); m(0,1) = t(0,1); m(0,2) = t(0,3);
    m(1,0) = t(1,0); m(1,1) = t(1,1); m(1,2) = t(1,3);
    m(2,0) = t(3,0); m(2,1) = t(3,1); m(2,2) = t(3,3);
  }
  else
  {
    for ( idx_t i = 0; i < 6; ++i ) 
    {
      for ( idx_t j = 0; j < 6; ++j )
      {
        m(i,j) = t(i,j);
      }
    }
  }
}

//-----------------------------------------------------------------------
//   reduce3DVector_
//-----------------------------------------------------------------------

void VoglerMaterial::reduce3DVector_

  ( const Vector&  v,
    const Vec6&    t ) const 

{
  // reduce a full 3D tuple to a 2D or 3D vector
  // (works the same for both stress and strain)

  if ( v.size() == 3 )
  {
    v[0] = t[0];
    v[1] = t[1];
    v[2] = t[3];
  }
  else
  {
    v[0] = t[0];
    v[1] = t[1];
    v[2] = t[2];
    v[3] = t[3];
    v[4] = t[4];
    v[5] = t[5];
  }
}
//-----------------------------------------------------------------------
//   Hist_ constructor
//-----------------------------------------------------------------------

VoglerMaterial::Hist_::Hist_ () : epspeq ( 0. ), dissipation ( 0. )
{
  epsp = 0.;
  loading = false;
}

//-----------------------------------------------------------------------
//   Hist_ print function
//-----------------------------------------------------------------------

void VoglerMaterial::Hist_::print () const

{
  System::out() << "epsp " << epsp << ", epspeq " << epspeq << endl;
}

// -------------------------------------------------------------------
//  Hist_::toVector
// -------------------------------------------------------------------

inline void VoglerMaterial::Hist_::toVector

 ( const Vector&  vec ) const

{
  vec[0] = epsp[0];
  vec[1] = epsp[1];
  vec[2] = epsp[2];
  vec[3] = epsp[3];
  vec[4] = epsp[4];
  vec[5] = epsp[5];
  vec[6] = epspeq;
  vec[7] = dissipation;
  vec[8] = loading;
}

//-----------------------------------------------------------------------
//   YieldFunc_ constructor
//-----------------------------------------------------------------------

VoglerMaterial::YieldFunc_::YieldFunc_

    ( const Mat66&       dmat,
      const Mat66&       mmat,
      const Array<Ref<Function> > alphas,
      const double       ffac )

  : dmat_ ( dmat ), mmat_ ( mmat ), mfac_ ( 1. - ffac )

{
  DM_ = matmul ( dmat_, mmat_ );

  alpha1_   = alphas[0];
  alpha2_   = alphas[1];
  alpha3t_  = alphas[2];
  alpha3c_  = alphas[3];
  alpha32t_ = alphas[4];
  alpha32c_ = alphas[5];

  dbgOut_ = newInstance<PrintWriter>( &System::debug("vogler") );
}

//-----------------------------------------------------------------------
//   YieldFunc_::setRmSettings
//-----------------------------------------------------------------------

void VoglerMaterial::YieldFunc_::setRmSettings

  ( const double rmTolerance,
    const idx_t  rmMaxIter )

{
  rmTol_ = rmTolerance;
  maxIter_ = rmMaxIter;
}

//-----------------------------------------------------------------------
//   YieldFunc_::isPlastic
//-----------------------------------------------------------------------

bool VoglerMaterial::YieldFunc_::isPlastic

  ( const Vec6&   sigtr,
    const double  epspeq0 )

{
  // update all variables that are constant inside the return 
  // mapping scheme

  epspeq0_ = epspeq0;
  epspeq_ = epspeq0;
  sigtr_ = sigtr;

  return evalTrial_() > -rmTol_;
}

//-----------------------------------------------------------------------
//   YieldFunc_::findRoot
//-----------------------------------------------------------------------

double VoglerMaterial::YieldFunc_::findRoot

  ( const double dgam0 )

{
  double dgam = dgam0;

  for ( idx_t irm = 0; irm < maxIter_; )
  {
    double crit = eval_ ( dgam );

    if ( jem::numeric::abs(crit) < rmTol_ )
    {
      *dbgOut_ << " return mapping converged in " << irm 
        << " iterations,  criterion: " << jem::numeric::abs(crit) << endl;
      break;
    }
    if ( ++irm == maxIter_ )
    {
      throw Exception ( JEM_FUNC, "No convergence" );
    }
    if ( jem::Float::isNaN(dfdg_) )
    {
      throw Exception ( JEM_FUNC, "nan" );
    }
    *dbgOut_ << " return mapping iteration " << irm << " dgam " <<
      dgam << ", criterion: " << crit << ", dfdg " << dfdg_ << endl;

    double ddgam = crit / dfdg_;

    dgam -= ddgam;
  }
  if ( dgam < -1.e-12 )
  {
    throw Exception ( JEM_FUNC, "Negative dgam" );
  }
  dbgOut_->flush();
  return dgam;
}

//-----------------------------------------------------------------------
//   YieldFunc_::findBounds
//-----------------------------------------------------------------------

void  VoglerMaterial::YieldFunc_::findBounds

  (       double&      gmin,
          double&      gmax,
          double&      fmin,
          double&      fmax )

{
  gmin = 0.;
  gmax = -1.;
  fmax = -1.;

  fmin = evalTrial_();

  JEM_ASSERT ( fmin > 0. );

  double dgam = 1.e-16;

  while ( gmax < 0. )
  {
    double crit = eval_ ( dgam );

    if ( crit > 0. )
    {
      gmin = dgam;
      fmin = crit;
    }
    else
    {
      gmax = dgam;
      fmax = crit;
    }
    dgam *= 10.;
  }
}

//-----------------------------------------------------------------------
//   YieldFunc_::findRoot (with bounds)
//-----------------------------------------------------------------------

double VoglerMaterial::YieldFunc_::findRoot

  (       double gmin,
          double gmax,
          double fmin,
          double fmax )

{
  double dgam;
  try
  {
    double dgam0 = estimateRoot_ ( gmin, gmax, fmin, fmax );

    dgam = findRoot ( dgam0 );
  }
  catch ( const Exception& ex )
  {
    improveBounds_ ( gmin, gmax, fmin, fmax );

    dgam = findRoot ( gmin, gmax, fmin, fmax );
  }
  return dgam;
}

//-----------------------------------------------------------------------
//   YieldFunc_::improveBounds_
//-----------------------------------------------------------------------

void  VoglerMaterial::YieldFunc_::improveBounds_

  (       double&      gmin,
          double&      gmax,
          double&      fmin,
          double&      fmax )

{
  idx_t np = 20;
  double dg = ( gmax - gmin ) / double(np);
  double dgam = gmin;

  while ( dgam < gmax )
  {
    dgam += dg;
    double crit = eval_ ( dgam );

    if ( crit > 0. )
    { 
      gmin = dgam;
      fmin = crit;
    }
    else
    {
      gmax = dgam;
      fmax = crit;
      break;
    }
  }
}

//-----------------------------------------------------------------------
//   YieldFunc_::estimateRoot_
//-----------------------------------------------------------------------

double VoglerMaterial::YieldFunc_::estimateRoot_

  ( const double       gmin,
    const double       gmax,
    const double       fmin,
    const double       fmax ) const

{
  // linear interpolation

  // fmin  o___ 
  //           ---___
  // phi=0 |----------o-----------|------
  //                      ---___
  // fmax  -                    --o
  //     gmin        ret        gmax

  return gmin + fmin * (gmax-gmin) / (fmin-fmax);
}

//-----------------------------------------------------------------------
//   YieldFunc_::eval_
//-----------------------------------------------------------------------

double VoglerMaterial::YieldFunc_::eval_

  ( const double dgam )

{
  updateDGamDependent_ ( dgam );

  return .5*dot(sig_,matmul(kmat_,sig_)) + dot(lvec_,sig_) - 1.;
}


//-----------------------------------------------------------------------
//   YieldFunc_::resetDGam
//-----------------------------------------------------------------------

void VoglerMaterial::YieldFunc_::resetDGam () 

{
  Vec6   DMs, FDMs, MFDMs;
  double a1, a2, a3, a32;
  double da1, da2, da3, da32;

  // update stress

  fmat_ = 0.;
  for ( idx_t i = 0; i < 6; ++i )
  {
    fmat_(i,i) = 1.;
  }
  sig_ = sigtr_;

  ng_ = matmul ( mmat_, sig_ );
  epspeq_ = epspeq0_;

  // update hardening parameters

  getAlphas_ ( a1, a2, a3, a32, epspeq_, sig_ );

  kmat_ = fillKMat ( a1, a2, a32, mfac_ );
  lvec_ = fillLVec ( a3, mfac_ );

  // update dfdg

  matmul ( DMs, dmat_, ng_ );
  double dedg = vnorm_(ng_);  

  getDAlphas_ ( da1, da2, da3, da32, epspeq_, sig_ );

  Mat66 dKde = fillKMat ( da1, da2, da32, mfac_ );
  Vec6  dLde = fillLVec ( da3, mfac_ );

  Vec6  dsigdg = -DMs;                           // F*D*M*sig
  Vec6  dfdsig =  matmul ( kmat_, sig_) + lvec_; // K*sig+L

  dfdep_ = .5 * dot ( sig_, matmul ( dKde, sig_ ) ) + dot ( sig_, dLde );
  dfdg_  = dfdep_ * dedg + dot ( dfdsig, dsigdg );
}

//-----------------------------------------------------------------------
//   YieldFunc_::getDbgWriter
//-----------------------------------------------------------------------

Ref<Writer> VoglerMaterial::YieldFunc_::getDbgWriter ()  const

{
  return dbgOut_;
}

//-----------------------------------------------------------------------
//   YieldFunc_::updateDGamDependent_
//-----------------------------------------------------------------------

void VoglerMaterial::YieldFunc_::updateDGamDependent_

  ( const double dgam ) 

{
  Vec6   DMs, FDMs, MFDMs;
  double a1, a2, a3, a32;
  double da1, da2, da3, da32;

  // update stress

  fmat_ = getFMat_ ( dgam );
  matmul ( sig_, fmat_, sigtr_ );

  // update equivalent plastic strain

  ng_ = matmul ( mmat_, sig_ );
  double dedg0 = vnorm_( ng_ );
  epspeq_ = epspeq0_ + dgam * dedg0;

  // update hardening parameters

  getAlphas_ ( a1, a2, a3, a32, epspeq_, sig_ );

  kmat_ = fillKMat ( a1, a2, a32, mfac_ );
  lvec_ = fillLVec ( a3, mfac_ );

  // update dfdg

  matmul ( DMs, dmat_, ng_ );
  matmul ( FDMs, fmat_, DMs );
  matmul ( MFDMs, mmat_, FDMs );

  double dedg = vnorm_(ng_) - dgam * dot(dvnorm_(ng_),MFDMs);  

  getDAlphas_ ( da1, da2, da3, da32, epspeq_, sig_ );

  Mat66 dKde = fillKMat ( da1, da2, da32, mfac_ );
  Vec6  dLde = fillLVec ( da3, mfac_ );

  Vec6  dsigdg = -matmul ( fmat_, DMs );         // F*D*M*sig
  Vec6  dfdsig =  matmul ( kmat_, sig_) + lvec_; // K*sig+L

  dfdep_ = .5 * dot ( sig_, matmul ( dKde, sig_ ) ) + dot ( sig_, dLde );
  dfdg_  = dfdep_ * dedg + dot ( dfdsig, dsigdg );
}

//-----------------------------------------------------------------------
//   YieldFunc_::getEpspeq
//-----------------------------------------------------------------------

double VoglerMaterial::YieldFunc_::getEpspeq () const

{
  return epspeq_;
}

//-----------------------------------------------------------------------
//   YieldFunc_::getStress
//-----------------------------------------------------------------------

Tuple<double,6> VoglerMaterial::YieldFunc_::getStress () const

{
  return sig_;
}

//-----------------------------------------------------------------------
//   YieldFunc_::getNg
//-----------------------------------------------------------------------

Tuple<double,6> VoglerMaterial::YieldFunc_::getNg () const

{
  return ng_;
}

//-----------------------------------------------------------------------
//   YieldFunc_::getTangent
//-----------------------------------------------------------------------

Tuple<double,6,6> VoglerMaterial::YieldFunc_::getTangent 

  ( const double dgam ) const

{
  // compute tangent parameter
  // NB: assumes that updateDGamDependent or resetDGam has been called

  Mat66 FD = matmul ( fmat_, dmat_ );
  Vec6  nf = matmul ( kmat_, sig_ ) + lvec_;
  Vec6  depdng = dgam * dvnorm_(ng_);
  nf += dfdep_ * matmul ( mmat_, depdng );  // not in paper (M symmetric)

  Vec6  Hng = matmul ( FD, ng_ );
  Vec6  nfH = matmul ( FD.transpose(), nf ); 

  double eta = -dfdep_ * vnorm_(ng_);
  double denom = eta + dot ( nf, Hng );
  Mat66 dcon;

  for ( idx_t i = 0; i < 6; ++i )
  {
    for ( idx_t j = 0; j < 6; ++j )
    {
      dcon(i,j) = FD(i,j) - Hng[i] * nfH[j] / denom;
    }
  }
  return dcon;
}

//-----------------------------------------------------------------------
//   YieldFunc_::evalTrial_
//-----------------------------------------------------------------------

double VoglerMaterial::YieldFunc_::evalTrial_

  ()

{
  double a1, a2, a3, a32;

  getAlphas_ ( a1, a2, a3, a32, epspeq0_, sigtr_ );

  kmat_ = fillKMat ( a1, a2, a32, mfac_ );
  lvec_ = fillLVec ( a3, mfac_ );

  return .5*dot(sigtr_,matmul(kmat_,sigtr_)) + dot(lvec_,sigtr_) - 1.;
}

//-----------------------------------------------------------------------
//   getAlphas_
//-----------------------------------------------------------------------

void  VoglerMaterial::YieldFunc_::getAlphas_

  (       double& a1,
          double& a2,
          double& a3,
          double& a32,
    const double  epspeq,
    const Vec6    stress ) const

{
  double i3 = mfac_ * stress[0] + stress[1] + stress[2];

  a1  = alpha1_->eval ( epspeq );
  a2  = alpha2_->eval ( epspeq );
  a3  = (i3 > 0. ? alpha3t_ : alpha3c_)->eval ( epspeq );
  a32 = (i3 > 0. ? alpha32t_ : alpha32c_)->eval ( epspeq );
}

//-----------------------------------------------------------------------
//   getDAlphas_
//-----------------------------------------------------------------------

void  VoglerMaterial::YieldFunc_::getDAlphas_

  (       double& da1,
          double& da2,
          double& da3,
          double& da32,
    const double  epspeq,
    const Vec6    stress ) const

{
  double i3 = mfac_ * stress[0] + stress[1] + stress[2];

  da1  = alpha1_->deriv ( epspeq );
  da2  = alpha2_->deriv ( epspeq );
  da3  = ( i3 > 0. ? alpha3t_ : alpha3c_)->deriv ( epspeq );
  da32 = ( i3 > 0. ? alpha32t_ : alpha32c_)->deriv ( epspeq );
}

//-----------------------------------------------------------------------
//   getFMat_
//-----------------------------------------------------------------------

Tuple<double,6,6> VoglerMaterial::YieldFunc_::getFMat_

  ( const double dgam ) const

{
  // F = [ I + dgam*D*M ]^(-1)

  Mat66  ret;
  double d;

  for ( idx_t i = 0; i < 6; ++i )
  {
    for ( idx_t j = 0; j < 6; ++j )
    {
      ret(i,j) = dgam * DM_(i,j);
    }
    ret(i,i) += 1.;
  }
  LUSolver::invert ( ret, d );

  return ret;
}

//-----------------------------------------------------------------------
//   fillKMat
//-----------------------------------------------------------------------

Tuple<double,6,6> VoglerMaterial::YieldFunc_::fillKMat

  ( const double a1,
    const double a2,
    const double a32,
    const double mfac )

{
  Mat66 ret;
  ret = 0.;

  double twoa32 = 2.*a32;

  ret(0,0) = mfac*mfac*twoa32;
  ret(0,1) = mfac*twoa32;
  ret(0,2) = ret(0,1);
  ret(1,0) = ret(0,1);
  ret(2,0) = ret(0,2);

  ret(1,1) = .5*a1+twoa32;   ret(1,2) = -.5*a1+twoa32;
  ret(2,1) = ret(1,2);       ret(2,2) = ret(1,1);
  ret(3,3) = 2.*a2;
  ret(4,4) = 2.*a1;
  ret(5,5) = 2.*a2;

  return ret;
}

//-----------------------------------------------------------------------
//   fillLVec
//-----------------------------------------------------------------------

Tuple<double,6> VoglerMaterial::YieldFunc_::fillLVec

  ( const double a3,
    const double mfac )

{
  Vec6 ret;

  ret[0] = mfac*a3;
  ret[1] = ret[2] = a3;
  ret[3] = ret[4] = ret[5] = 0.;

  return ret;
}

//-----------------------------------------------------------------------
//   vnorm_
//-----------------------------------------------------------------------

double VoglerMaterial::YieldFunc_::vnorm_

    ( const Vec6& v ) const

{
  // norm that defines equivalent plastic strain
  // adapted for voigt notation

  double ret2 = 0.;

  ret2 += .50 * v[0] * v[0];
  ret2 += .50 * v[1] * v[1];
  ret2 += .50 * v[2] * v[2];
  ret2 += .25 * v[3] * v[3];
  ret2 += .25 * v[4] * v[4];
  ret2 += .25 * v[5] * v[5];

  return sqrt(ret2);
}

//-----------------------------------------------------------------------
//   dvnorm_
//-----------------------------------------------------------------------

Tuple<double,6> VoglerMaterial::YieldFunc_::dvnorm_

    ( const Vec6& v ) const

{
  // gradient of vnorm (d(sqrt(vJv))/d(v) = Jv/sqrt(vJv))

  Vec6 ret;

  double vnorm = vnorm_(v);

  ret[0] = .50 * v[0] / vnorm;
  ret[1] = .50 * v[1] / vnorm;
  ret[2] = .50 * v[2] / vnorm;
  ret[3] = .25 * v[3] / vnorm;
  ret[4] = .25 * v[4] / vnorm;
  ret[5] = .25 * v[5] / vnorm;

  return ret;
}

