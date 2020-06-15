/*
 * 
 *  Copyright (C) 2007 TU Delft. All rights reserved.
 *  
 *  This class implemens the isotropic elastic-damage material
 *  This represents the material at a point in space.
 *  It is implemented in such a way that can be used for any
 *  discretisation methods, say finite elements, EFG and so on.
 *  
 *  Author: V.P. Nguyen, V.P.Nguyen@tudelft.nl
 *  Date: 30 November 2007
 *
 */

#include <jem/base/System.h>
#include <jem/base/limits.h>
#include <jem/base/Error.h>
#include <jem/base/PrecheckException.h>
#include <jem/base/Array.h>
#include <jem/base/array/tensor.h>
#include <jem/util/Properties.h>
#include <jem/numeric/algebra/matmul.h>
#include <jem/numeric/algebra/MatmulChain.h>
#include <jem/numeric/utilities.h>
#include <jem/io/NumberFormat.h>
#include <jem/io/PrintWriter.h>
#include <algorithm>

#include "utilities.h"
#include "DamageMaterial.h"
#include "voigtUtilities.h"
#include "Invariants.h"

using namespace jem;

// using namespace jem::io;

using jem::io::NumberFormat;
using jem::io::PrintWriter;
using jem::numeric::matmul;
using jem::numeric::MatmulChain;
using jem::util::Properties;
using jive::Vector;


static const double EPS = 1e-10;

typedef Tuple<double,3> Vec3;
typedef Tuple<double,6> Vec6;



//=======================================================================
//   class DamageMaterial
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------

const char*  DamageMaterial::SOFTENING_PROP       = "softening";
const char*  DamageMaterial::EQUISTRAIN_PROP      = "equistrain";
const char*  DamageMaterial::KAPPAI_PROP          = "kappaI";
const char*  DamageMaterial::KAPPAC_PROP          = "kappaC";
const char*  DamageMaterial::TENSILE_PROP         = "ft";
const char*  DamageMaterial::COMPRESSIVE_PROP     = "fc";
const char*  DamageMaterial::FRACTURE_ENERGY_PROP = "gf";
const char*  DamageMaterial::ALPHA_PROP           = "alpha";
const char*  DamageMaterial::BETA_PROP            = "beta";
const char*  DamageMaterial::ETA_PROP             = "eta";
const char*  DamageMaterial::B_PROP               = "b";
const char*  DamageMaterial::LENGTH_PROP          = "lengthscale";
const char*  DamageMaterial::RATE_FACTOR_PROP     = "rateFactor";

const char*  DamageMaterial::LAMBDA_PROP          = "lambda";

const char*  DamageMaterial::MAZARS_EQUI_STRAIN   = "Mazars";
const char*  DamageMaterial::MISES_EQUI_STRAIN    = "vonMises";
const char*  DamageMaterial::RANKINE_EQUI_STRAIN  = "Rankine";
const char*  DamageMaterial::ENERGY_STRAIN        = "Energy";

const char*  DamageMaterial::PERFECT_SOFTENING    = "perfect";
const char*  DamageMaterial::LINEAR_SOFTENING     = "linear";
const char*  DamageMaterial::EXPONENT1_SOFTENING  = "exponential1"; // ki,alpha, beta
const char*  DamageMaterial::EXPONENT2_SOFTENING  = "exponential2"; // ki, kc
const char*  DamageMaterial::EXPONENT3_SOFTENING  = "exponential2Reg"; // Jirasek regularised of 2
const char*  DamageMaterial::POWER_SOFTENING      = "power";
const char*  DamageMaterial::HYPERBOLIC_SOFTENING = "hyperbolic";
const char*  DamageMaterial::EXPOENERGY_SOFTENING = "expoEnergy";

const double DamageMaterial::CRITICAL_DAMAGE      = 0.99999999;

vector<String> DamageMaterial::equiStrainDefs   (4);
vector<String> DamageMaterial::softeningLawDefs (8);


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


DamageMaterial::DamageMaterial

  ( idx_t rank, const Properties& globdat )
    : Material ( rank, globdat )
{
  kappaI_     = 0.;
  kappaC_     = 0.;
  alpha_      = 0.;
  beta_       = 0.;
  eta_        = 1.;
  b_          = 0.;
  c_          = 0.;
  rateFac_    = 0.;

  softening_  = Linear;
  equiStrain_ = VonMises ;

  elasticMat_ = newInstance<HookeMaterial> ( rank, globdat );

  elasticMod_ .resize ( STRAIN_COUNTS[rank], STRAIN_COUNTS[rank] );

  equiStrainDefs[0] = MAZARS_EQUI_STRAIN  ;
  equiStrainDefs[1] = MISES_EQUI_STRAIN   ;
  equiStrainDefs[2] = RANKINE_EQUI_STRAIN ;
  equiStrainDefs[3] = ENERGY_STRAIN ;

  softeningLawDefs[0] = PERFECT_SOFTENING ;
  softeningLawDefs[1] = LINEAR_SOFTENING ;
  softeningLawDefs[2] = EXPONENT1_SOFTENING ;
  softeningLawDefs[3] = EXPONENT2_SOFTENING ;
  softeningLawDefs[4] = POWER_SOFTENING ;
  softeningLawDefs[5] = HYPERBOLIC_SOFTENING ;
  softeningLawDefs[6] = EXPONENT3_SOFTENING ;
  softeningLawDefs[7] = EXPOENERGY_SOFTENING ; 

  historyNames_.resize ( 3 );

  historyNames_[0] = "kappa";
  historyNames_[1] = "loading";
  historyNames_[2] = "dissipation";
}


DamageMaterial::~DamageMaterial ()
{} 

DamageMaterial::Hist_::Hist_ () : kappa(0.), loading(0.)
{
}

//-----------------------------------------------------------------------
//   allocPoints
//-----------------------------------------------------------------------

void DamageMaterial::allocPoints ( idx_t count )
{
  // NB: this version of pushBack calls the copy constructor
  // this does not work when Hist_ contains Arrays

  preHist_.pushBack ( Hist_(), count );
  newHist_.pushBack ( Hist_(), count );
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void DamageMaterial::configure ( const Properties& props )
{
  using jem::maxOf;
  using std::find;

  elasticMat_->configure ( props );

  bool ki = props.find ( kappaI_, KAPPAI_PROP, 0.0, maxOf( kappaI_) );
  bool gf = props.find ( gf_, FRACTURE_ENERGY_PROP, 0.0, maxOf ( gf_ ) );

  if ( !ki )
  {
    props.get ( ft_, TENSILE_PROP, 0.0, maxOf ( ft_ ) );

    kappaI_ = ft_ / elasticMat_->giveYoung ();
  }
  else
  {
    ft_ = kappaI_ * elasticMat_->giveYoung ();
  }

  props.find ( c_,      LENGTH_PROP,          0.0, maxOf ( c_)  );

  // read type of softening law used

  String      softening;

  props.get ( softening, SOFTENING_PROP  );

  if ( find ( softeningLawDefs.begin (),
              softeningLawDefs.end   (),
              softening ) == softeningLawDefs.end () )
  {
    throw Error (
      JEM_FUNC,
      String("unexpected definition of softening law!!!\n") +
      String("Supported softening laws include: \n") +
      PERFECT_SOFTENING   + String(", ") + LINEAR_SOFTENING + String(", ") +
      EXPONENT1_SOFTENING + String(", ") + EXPONENT2_SOFTENING + String(", ") +
      POWER_SOFTENING     + String(", ") + HYPERBOLIC_SOFTENING
    );
  }

  // define correctly the parameters for each softening law

  if      ( softening == EXPONENT1_SOFTENING )
  {
    alpha_ = 1.;
    props.find ( alpha_,  ALPHA_PROP,  0.0, maxOf ( alpha_  ) );

    if ( !gf )
    {
      props.get ( beta_,   BETA_PROP,   0.0, maxOf ( beta_   ) );
    }

    softening_ = Exponential3Params;
  }
  else if ( softening == EXPONENT2_SOFTENING )
  {
    props.find ( lambda_, LAMBDA_PROP, 0.0, 1000.0 );
    props.get  ( kappaC_, KAPPAC_PROP, 0.0, maxOf ( kappaC_ ) );

    softening_ = Exponential2Params;
  }
  else if ( softening == EXPONENT3_SOFTENING )
  {
    softening_ = Exponential2ParamsReg;
  }
  else if ( softening == POWER_SOFTENING )
  {
    props.get ( kappaC_, KAPPAC_PROP, 0.0, maxOf ( kappaC_ ) );
    props.get ( alpha_,  ALPHA_PROP,  0.0, maxOf ( alpha_  ) );
    props.get ( beta_,   BETA_PROP,   0.0, maxOf ( beta_   ) );

    softening_ = Power;
  }
  else if ( softening == HYPERBOLIC_SOFTENING )
  {
    props.get ( b_, B_PROP, 0.0, maxOf ( b_ ) );

    softening_ = Hyperbolic;
  }
  else if ( softening == LINEAR_SOFTENING )
  {
    softening_ = Linear;

    props.find ( kappaC_, KAPPAC_PROP, 0.0, maxOf ( kappaC_ ) );
  }
  else if ( softening == EXPOENERGY_SOFTENING )
  {
    props.get ( kappaI_,  KAPPAI_PROP,  0.0, maxOf ( kappaI_  ) );
    props.get ( kappaC_,  KAPPAC_PROP,  0.0, maxOf ( kappaC_  ) );
    props.get ( s_,       "s",          0.0, maxOf ( s_       ) );

    softening_ = ExpoEnergy;
  }

  // read type of equivalent strain definition

  String equiStrain;

  props.get ( equiStrain, EQUISTRAIN_PROP );

  if ( find ( equiStrainDefs.begin (), 
              equiStrainDefs.end   (), 
              equiStrain ) == equiStrainDefs.end () )
  {
    throw Error (
      JEM_FUNC,
      String ("unexpected definition of equivalent strain!!!\n") +
      String ("Available ones are: ") + MAZARS_EQUI_STRAIN + String(", ") +
      MISES_EQUI_STRAIN + String(", ") + RANKINE_EQUI_STRAIN + String(", ") );
  }

  if      ( equiStrain == MISES_EQUI_STRAIN )
  {
    if ( props.find ( fc_, COMPRESSIVE_PROP, 0.0, maxOf ( fc_ ) ) )
    {
      eta_ = fc_ / ft_;
    }
    else
    {
      props.find ( eta_, ETA_PROP, 0.0, maxOf ( eta_ ) ); // f_c / f_t
    }
    equiStrain_ = VonMises;

    vonMisesEqv_.init ( eta_, elasticMat_->givePoisson ( ) );
  }
  else if ( equiStrain == MAZARS_EQUI_STRAIN )
  {
    equiStrain_ = Mazars;
  }
  else if ( equiStrain == RANKINE_EQUI_STRAIN )
  {
    equiStrain_ = Rankine;
  }
  else if ( equiStrain == ENERGY_STRAIN )
  {
    equiStrain_ = StrainEnergy;
  }

  elasticMat_->getStiffMat(elasticMod_ );

  setMaxLe_();

  // viscosity

  if ( props.find ( rateFac_, RATE_FACTOR_PROP, 0., maxOf(rateFac_) ) )
  {
    if ( rateFac_ > 0. )
    {
      viscous_ = true;
      irfac_   = 1./rateFac_;
    }
  }
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void DamageMaterial::getConfig ( const Properties& conf ) const
{
  elasticMat_->getConfig ( conf );

  conf.set ( SOFTENING_PROP,  softeningLawDefs[softening_]  );
  conf.set ( EQUISTRAIN_PROP, equiStrainDefs[equiStrain_] );

  conf.set ( KAPPAI_PROP, kappaI_ );
  conf.set ( KAPPAC_PROP, kappaC_ );
  conf.set ( FRACTURE_ENERGY_PROP, gf_ );

  conf.set ( RATE_FACTOR_PROP, rateFac_ );

  if ( c_ != 0 ) conf.set ( LENGTH_PROP, c_ );
}

//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------

// Remark: applicable to both local and nonlocal damage model (integral or gradient
// enhanced)

void  DamageMaterial::update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      idx_t                 ipoint )
{
  // Get the nonlocal equivalent strain:
  //
  // The nonlocal equi. strain is computed at the global level and stored
  // in the final component of the strain vector, hence
  // equiStrain = strain[n];

  const idx_t n = stress.size ();
  const idx_t m = strain.size ();

  double    equiStrain;

  if ( n == m ) // local damage models
  {
    equiStrain = getEquiStrain ( strain );
  }
  else
  {
    equiStrain = strain[n];
  }

  // Get the history variable of the previous converged load step

  double kappa0 = preHist_[ipoint].kappa;

  // Compute the loading function f

  idx_t  load;
  double dkdeeq;
  double kappa  = getKappa_ ( dkdeeq, load, equiStrain, ipoint );

  // compute the damage variable

  double omega0 = damageEvolution_ ( kappa0 );
  double omega  = damageEvolution_ ( kappa  );

  // secant stiffness matrix only

  stiff         = ( 1.0 - omega ) * elasticMod_;

  // Compute stress vector

  matmul ( stress, elasticMod_, strain[slice(0,n)] ); // tmp: effective stress

  if ( load && m==n )
  {
    // apply chain rule for tangent 
    // NB: when m!=n, omega does not depend on strain

    Vector dkde ( strain.size() );
    getdEquidEps ( dkde, strain );
    double dodk = getdOmegadKappa ( kappa );

    TensorIndex i,j;

    stiff(i,j) -= stress[i] * dodk * dkdeeq * dkde[j];
  }

  double dG = 0.;

  if ( load )
  {
    // dG = 1/2 * dOmega * strain_{i} . elasticMod . strain_{i-1}

    dG  = .5 * ( omega - omega0 ) * dot ( stress, strain[slice(0,n)] );

    // correction, since old strain should have been used
    // (this correction assumes kappa is linear in strain)

    dG *= kappa0 / kappa; 
  }

  stress *= ( 1. - omega ); // from effective stress to nominal stress

  // update history variables

  newHist_[ipoint].kappa       = kappa;
  newHist_[ipoint].loading     = load;
  newHist_[ipoint].dissipation = preHist_[ipoint].dissipation + dG;

  latestHist_ = &newHist_;
}

// ----------------------------------------------------------------
//  update (overloaded version)
// ----------------------------------------------------------------

void  DamageMaterial::update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      idx_t                 ipoint,
      double                he )
{
  // Get the nonlocal equivalent strain:

  double    equiStrain = getEquiStrain ( strain );

  // Get the history variable of the previous converged load step

  double    kappa0     = preHist_[ipoint].kappa ;

  // Compute the loading function f

  idx_t  load;
  double dkdeeq;
  double kappa  = getKappa_ ( dkdeeq, load, equiStrain, ipoint );

  // compute the damage variable

  double omega0 = damageEvolution_ ( kappa0 );
  double omega  = damageEvolution_ ( kappa, he );

  // secant stiffness matrix

  stiff  = ( 1.0 - omega ) * elasticMod_;

  // Compute stress vector

  matmul ( stress, elasticMod_, strain ); // temporary: effective stress

  if ( load )
  {
    // apply chain rule for tangent 

    Vector dkde ( strain.size() );
    getdEquidEps ( dkde, strain );
    double dodk = getdOmegadKappa ( kappa, he );

    TensorIndex i,j;

    stiff(i,j) -= stress[i] * dodk * dkde[j];
  }

  double dG = ( omega - omega0 ) * dot ( stress, strain );
  stress *= ( 1. - omega );  // finally: nominal stress

  // update history variables

  newHist_[ipoint].kappa       = kappa;
  newHist_[ipoint].loading     = load;
  newHist_[ipoint].dissipation = preHist_[ipoint].dissipation + dG;
}


// --------------------------------------------------------------------
//  commit
// --------------------------------------------------------------------

void  DamageMaterial::commit()
{
  newHist_.swap ( preHist_ );

  latestHist_ = &preHist_;
}

//-----------------------------------------------------------------------
//   clone
//-----------------------------------------------------------------------

Ref<Material> DamageMaterial::clone () const

{
  // use default copy constructor

  return newInstance<DamageMaterial> ( *this );
}

// -------------------------------------------------------------------
//  giveElasticMatrix
// -------------------------------------------------------------------

Matrix  DamageMaterial::giveElasticMatrix ( ) const
{
  return elasticMod_;
}

//-----------------------------------------------------------------------
//   setMaxLe_
//-----------------------------------------------------------------------

void DamageMaterial::setMaxLe_ () 

{
  if ( softening_ == Perfect )
  {
    maxLe_ = maxOf(maxLe_);
  }
  else if ( softening_ == Linear || softening_ == Exponential3Params ) 
  {
    maxLe_ = 2. * gf_ / kappaI_ / ft_;
  }
  else if ( softening_ == Exponential2Params )
  {
    maxLe_ = lambda_ * ( 2. * kappaC_ / kappaI_ - 1. );
  }

}

//-----------------------------------------------------------------------
//   getKappa_
//-----------------------------------------------------------------------

double DamageMaterial::getKappa_

  (       double&  dkde,
          idx_t&   load,
    const double   eqstrain,
    const idx_t    ipoint ) const

{
  double kappa;
  double kappa0 = preHist_[ipoint].kappa;
  double f = eqstrain - kappa0;

  dkde = 1.;

  if ( f == 0.0 )
  {
    kappa = kappa0;
    load  = preHist_[ipoint].loading;
  }  
  else 
  {
    if ( f > 0.0 )
    {  
      load = ( eqstrain > kappaI_ );

      if ( load )
      {
        kappa = updateKappa_ ( dkde, eqstrain, kappa0 );
      }
      else
      {
        kappa = eqstrain;
      }
    }
    else 
    {
      kappa = kappa0;
      load  = 0;
    }
  }
  return kappa;
}

//-----------------------------------------------------------------------
//   updateKappa_
//-----------------------------------------------------------------------

double DamageMaterial::updateKappa_

  (       double&  dkde,
    const double   eqstrain,
    const double   kappa0 ) const

{
  if ( isViscous() )
  {
    double potential = 1. + eqstrain - kappa0;
    double tmp       = irfac_ / potential;
    double kappaDot  = irfac_ - tmp;

    double kappaExpl = kappa0 + kappaDot * dt_;

    if ( kappaExpl < eqstrain )
    {
      dkde =  dt_ * tmp / ( potential );
      return kappaExpl;
    }
  }

  dkde = 1.;
  return eqstrain;
}


//-----------------------------------------------------------------------
//   damageEvolution_
//-----------------------------------------------------------------------

// Remark: The following Fortran-like implementation is ugly
// but efficient. There is an alternative using policy-based
// class by writing orthogonal classes for every softening
// law. It is, however, slower than the current implementation.

double DamageMaterial::damageEvolution_

  ( double k ) const

{
  double omega = 0.0;

  if      ( softening_ == Perfect )
  {
    omega = perfectSoftening( kappaI_, k ); 
  }
  else if ( softening_ == Linear )
  {
    omega = linearSoftening( kappaI_, kappaC_ , k );
  } 
  else if ( softening_ == Exponential3Params )
  {
    omega = exponentialSoftening1 ( kappaI_, alpha_, beta_, k );
  }
  else if ( softening_ == Exponential2Params )
  {
    omega = exponentialSoftening2 ( kappaI_, kappaC_ , k );
  }
  else if ( softening_ == Power )
  {
    omega = powerSoftening ( kappaI_, kappaC_, alpha_, beta_, k );
  }
  else if ( softening_ == Hyperbolic )
  {
    omega = hyperbolicSoftening ( kappaI_, b_, k );
  }
  else if ( softening_ == ExpoEnergy )
  {
    omega = expoEnergySoftening ( kappaI_, kappaC_, s_, k );
  }

  return omega;
}

//-----------------------------------------------------------------------
//   damageEvolution_
//-----------------------------------------------------------------------

double DamageMaterial::damageEvolution_

  ( double k, double he ) const

{
  double omega = 0.0;

  if      ( softening_ == Linear )
  {
    omega = linearSoftening ( kappaI_, k, he, maxLe_ );
  }
  else if ( softening_ == Exponential3Params )
  {
    omega = exponentialSoftening1 ( kappaI_, alpha_, gf_, ft_ , k, he );
  }
  else if ( softening_ == Exponential2Params )
  {
    omega = exponentialSoftening2 ( kappaI_, kappaC_ , k, lambda_, he );
  }
  else if ( softening_ == Exponential2ParamsReg )
  {
    omega = exponentialSoftening3 ( kappaI_, k, gf_, ft_, he );
  }
  else 
  {
    throw Error ( JEM_FUNC, String( "element-size regularization not implemented ") + 
        "for this softening law" );
  }

  return omega;
}

//-----------------------------------------------------------------------
//   getdOmegadKappa
//-----------------------------------------------------------------------

double DamageMaterial:: getdOmegadKappa

    ( double k )  const

{
  double dOmega = 0.0;

  if           ( softening_ == Perfect  )
  {
    dOmega = getDerivPerfectSoftening( kappaI_, k );
  } 
  else if      ( softening_ ==  Linear )
  {
    dOmega = getDerivLinearSoftening( kappaI_, kappaC_ , k );
  } 
  else if      ( softening_ == Exponential3Params )
  {
    dOmega = getDerivExponentialSoftening1 ( kappaI_, alpha_, beta_, k );
  }
  else if      ( softening_ == Exponential2Params )
  {
    dOmega = getDerivExponentialSoftening2 ( kappaI_, kappaC_, k );
  }
  else if      ( softening_ == Power )
  {
    dOmega = getDerivPowerSoftening ( kappaI_, kappaC_, alpha_, beta_, k );
  }
  else if      ( softening_ == Hyperbolic )
  {
    dOmega = getDerivHyperbolicSoftening ( kappaI_, b_,k );
  }
  else if      ( softening_ == ExpoEnergy )
  {
    dOmega = getDerivExpoEnergySoftening ( kappaI_, kappaC_, s_, k );
  }
  return dOmega;
}

//-----------------------------------------------------------------------
//   getdOmegadKappa (for regularised local damage)
//-----------------------------------------------------------------------

double DamageMaterial:: getdOmegadKappa

    ( double k, double he )  const

{
  double dOmega = 0.0;

  if       ( softening_ == Linear )
  {
    dOmega = getDerivLinearSoftening ( kappaI_, k, he, maxLe_ );
  }
  else if ( softening_ == Exponential3Params )
  {
    dOmega = getDerivExponentialSoftening1 ( kappaI_, alpha_, gf_, ft_, k, he );
  }
  else if  ( softening_ == Exponential2Params )
  {
    dOmega = getDerivExponentialSoftening2 
                ( kappaI_, kappaC_, k, lambda_, he );
  }
  else if  ( softening_ == Exponential2ParamsReg )
  {
    dOmega = getDerivExponentialSoftening3
                ( kappaI_, k, gf_, ft_, he );
  }
  return dOmega;
}

//-----------------------------------------------------------------------
//   getEquiStrain
//-----------------------------------------------------------------------

double DamageMaterial::getEquiStrain

  ( const Vector& strain ) 

{
  MatmulChain<double,1>     mc1;

  double eqvStr = 0.0;

  if      ( equiStrain_ == VonMises  )
  {
    double I1, J2;

    StrainInvariants inv ( elasticMat_->fill3DStrain ( strain ) );

    inv.getI1andJ2  ( I1, J2 );  

    eqvStr = vonMisesEqv_ ( I1, J2 );
  } 
  else if ( equiStrain_ == Mazars  )
  {
    StrainInvariants inv ( elasticMat_->fill3DStrain ( strain ) );

    Vec3 princ = inv.getPrincipalValues ();

    eqvStr  = 0.0;

    for ( idx_t i = 0; i < 3; i++ )
    {
      if ( princ[i] > 0 ) eqvStr += princ[i] * princ[i];
    }
    eqvStr = ::sqrt ( eqvStr );
  }
  else if ( equiStrain_ == Rankine  )
  {
    double young  = elasticMat_->giveYoung ();

    Vector stress = mc1.matmul ( elasticMod_, strain );
    
    StressInvariants inv ( elasticMat_->fill3DStress ( stress ) );

    Vec3 princ = inv.getPrincipalValues ();

    eqvStr = equiStrainRankine ( young, princ );
  }
  else if ( equiStrain_ == StrainEnergy  )
  {
    eqvStr = 0.5 * dot ( strain, mc1.matmul ( elasticMod_, strain ) );
  }

  return eqvStr;
}

// --------------------------------------------------------------------
//  getdEquidEps
// --------------------------------------------------------------------

void DamageMaterial::getdEquidEps

  ( const Vector& debde,
    const Vector& strain ) 

{
  const idx_t strCount = strain.size ( );

  if      ( equiStrain_ == VonMises  )
  {
    double I1, J2, dvmdI1, dvmdJ2;

    Vec6  dI1;
    Vec6  dJ2;
    Vec6  debde6;
    
    StrainInvariants inv ( elasticMat_->fill3DStrain ( strain ) );

    inv.getI1J2andGrads ( I1, J2, dI1, dJ2 ); 

    vonMisesEqv_.getGrads ( dvmdI1, dvmdJ2, I1, J2 );

    debde6 = dvmdI1 * dI1 + dvmdJ2 * dJ2;

    elasticMat_->reduce3DStrainGrad ( debde, debde6 );
  }

  else if ( equiStrain_ == Mazars  )
  {
    if ( strCount == 1 ) 
    {
      debde = evalHeaviside ( strain[0] ); 
    }
    else
    {
      getdEquidEpsMazars_ ( debde, strain );
    }

  } 

  else if ( equiStrain_ == Rankine  )
  {
    MatmulChain<double,1>     mc1;

    double young  = elasticMat_->giveYoung   ();

    Vector stress = mc1.matmul ( elasticMod_, strain );

    getdEquidEpsRankine_ ( debde, stress, young, elasticMod_ );
  }

  else if ( equiStrain_ == StrainEnergy  )
  {
    MatmulChain<double,1>     mc1;

    matmul ( debde, elasticMod_, strain );
  }
}

// ---------------------------------------------------------------
//  isFullyDamaged ( idx_t mpoint )
// ---------------------------------------------------------------

bool DamageMaterial::isFullyDamaged ( idx_t mpoint ) const
{
  double kappa = newHist_[mpoint].kappa ;
  double omega = damageEvolution_ ( kappa );

  return ( 1.0 - omega < EPS );
}

// -------------------------------------------------------------------
//  getHistory
// -------------------------------------------------------------------

void DamageMaterial::getHistory 

    ( const Vector&         hvals,
      const idx_t           mpoint ) const

{
  (*latestHist_)[mpoint].toVector ( hvals );
}

// -------------------------------------------------------------------
//  maxAllowedLength
// -------------------------------------------------------------------

double DamageMaterial::maxAllowedLength () const

{
  return .99 * maxLe_;
}

// ---------------------------------------------------------------
//  compute the derivatives of equivalent strain
//  w.r.t the strain vector
//  Mazars formulas
// ---------------------------------------------------------------

void  DamageMaterial:: getdEquidEpsMazars_

  ( const Vector&   debde,
    const Vector&   strain )

{
  Vec6  debde6;

  debde6 = 0.;

  // 3D problem

  StrainInvariants inv ( elasticMat_->fill3DStrain ( strain ) );

  Vec3      invs, princ;
  Vec6      dI1, dI2, dI3;

  inv.getInvsAndGrads ( invs, princ, dI1, dI2, dI3 );

  double epsBar = 0.0;

  for ( idx_t i = 0; i < 3; i++ )
  {
    if ( princ[i] > 0 ) epsBar += princ[i] * princ[i] ;
  }

  epsBar = ::sqrt ( epsBar );

  if ( epsBar < EPS ) 
  {
    debde = 0.;
    return;
  }

  double I1 = invs[0];
  double I2 = invs[1];

  for ( idx_t i = 0 ; i < 3; i++ )
  {
    double vi  = princ[i];

    if ( vi > 0. )
    {
      double tmp = 1.0 / ( 3.0 * vi * vi - 2.0 * I1 * vi + I2);

      for ( idx_t j = 0; j < 6; ++j )
      {
        double dEidEj = tmp * ( vi * vi * dI1[j] - vi * dI2[j] + dI3[j] );
        debde6[j] += vi * dEidEj;
      }
    }
  }
  debde6 *= (1.0 / epsBar) ;

  elasticMat_->reduce3DStrainGrad ( debde, debde6 );
}


// ---------------------------------------------------------------
//  compute the derivatives of equivalent strain
//  w.r.t the strain vector
//  Rankine formulas
// ---------------------------------------------------------------

void DamageMaterial::getdEquidEpsRankine_

  ( const Vector&   debde, 
    const Vector&   stress,
    const double    young,
    const Matrix&   De )

{
  Vec6                   debds6;
  MatmulChain<double,1>  mc1;
  StressInvariants       inv ( elasticMat_->fill3DStress ( stress ) );

  debds6 = 0.;

  Vec3 invs;
  Vec3 princ = inv.getPrincipalValues ( invs );

  // compute the equivalent strain

  double eqvStr  = equiStrainRankine ( young, princ );

  if ( eqvStr < EPS ) 
  {
    debde = 0.;
    return;
  }

  // compute derivatives of invariants w.r.t stress vector

  Vec6 dI1dSigma = inv.getGradI1();
  Vec6 dI2dSigma = inv.getGradI2();
  Vec6 dI3dSigma = inv.getGradI3();

  for ( idx_t i = 0; i < 3; i++)
  {
    double vi = princ[i];

    if ( vi > 0. )
    {
      double tmp = 1.0 / ( 3.0 * vi * vi -
                   2.0 * invs[0] * vi + invs[1] );

      for ( idx_t j = 0; j < 6; ++j )
      {
        double dSigIdSigj = tmp * (  vi * vi * dI1dSigma[j] -
                            vi * dI2dSigma[j] + dI3dSigma[j] );

        debds6[j] += vi * dSigIdSigj ;
      }
    }
  }

  Vector debds ( stress.size() );

  elasticMat_->reduce3DStressGrad ( debds, debds6 );

  debde  = mc1.matmul ( De, debds );
  debde *= 1.0 / ( young * young * eqvStr);
}

// ======================================================================
//   Implementation of related functions
// ======================================================================


//-----------------------------------------------------------------------
//   perfectSoftening
//-----------------------------------------------------------------------

double                      perfectSoftening

  ( const double Ki, const double K )
{
  if ( K < Ki )
  {
    return 0.0;
  }

  double omega = 1.0 - Ki / K;

  return omega;
}

//-----------------------------------------------------------------------
//   getDerivPerfectSoftening
//-----------------------------------------------------------------------

double                      getDerivPerfectSoftening

    ( const double Ki, const double K )

{
  if ( K < Ki )
  {
    return 0.0;
  }

  return  Ki / ( K * K );
}


//-----------------------------------------------------------------------
//   linearSoftening
//-----------------------------------------------------------------------

double                      linearSoftening

  ( const double Ki,
    const double Kc,
    const double K )
{
  if ( K < Ki )
  {
    return 0.0;
  }

  // System::out() << "damage starting " << Ki << " < " << K << " < " 
    // << Kc << endl;
  double omega = K > Kc ? 1.0 : Kc * (K - Ki) / (K * (Kc - Ki));
 
  if ( omega >= DamageMaterial::CRITICAL_DAMAGE )
  {
    omega = DamageMaterial::CRITICAL_DAMAGE;
  }

  return omega;
}

double                      linearSoftening

 ( const double Ki,
   const double K,
   const double Le,
   const double maxLe ) 

{
  double Kc = maxLe / Le * Ki;

  return linearSoftening ( Ki, Kc, K );
}


//-----------------------------------------------------------------------
//   getDerivLinearSoftening
//-----------------------------------------------------------------------

double                      getDerivLinearSoftening

    ( const double Ki,
      const double Kc,
      const double K )

{
  return  (K < Kc) ?  ( Ki * Kc ) / ( (Kc - Ki) * K * K ) : 0.0;
}

double                      getDerivLinearSoftening

 ( const double Ki,
   const double K,
   const double Le,
   const double maxLe ) 

{
  double Kc = maxLe / Le * Ki;

  return getDerivLinearSoftening ( Ki, Kc, K );
}


//-----------------------------------------------------------------------
//   exponentialSoftening1
//-----------------------------------------------------------------------

double                      exponentialSoftening1

  ( const double Ki,
    const double alpha,
    const double beta,
    const double K )
{
  if ( K < Ki ) return 0.0;

  double omega = 1.0 - (Ki / K) * ( 1.0 - alpha + alpha * exp (-beta * (K - Ki) ) );

  return omega;
}

// overloaded version

double                      exponentialSoftening1

  ( const double Ki,
    const double alpha,
    const double gf, const double ft,
    const double K,
    const double he )
{
  double beta  = alpha / ( -0.5 * Ki + gf / ft / he );

  return exponentialSoftening1 ( Ki, alpha, beta, K );
}

//-----------------------------------------------------------------------
//   getDerivExponentialSoftening1
//-----------------------------------------------------------------------

double                      getDerivExponentialSoftening1

  ( const double Ki,
    const double alpha,
    const double beta,
    const double K )
{
  const double h = K - Ki;        
  const double a = Ki / (K * K);
  const double b = a * K;
  const double c = alpha * exp(-beta * h );

  return a * (1.0 - alpha + c) + beta * b * c;
}

// overloaded version

double                      getDerivExponentialSoftening1

  ( const double Ki,
    const double alpha,
    const double gf, const double ft,
    const double K,
    const double he )
{
  double beta    = alpha / ( -0.5 * Ki + gf / ft /he );

  const double h = K - Ki;        
  const double a = Ki / (K * K);
  const double b = a * K;
  const double c = alpha * exp(-beta * h );

  return a * (1.0 - alpha + c) + beta * b * c;
}

//-----------------------------------------------------------------------
//    exponentialSoftening2
//-----------------------------------------------------------------------

double                       exponentialSoftening2

  ( const double Ki,
    const double Kc,
    const double K )
{
  if ( K < Ki )
  {
    return 0.0;
  }

  double omega = 1.0 - (Ki / K) * exp ( (Ki - K) / (Kc - Ki) );

  return omega;
}

// overloaded version

double                       exponentialSoftening2

  ( const double Ki,
    const double Kc,
    const double K, const double lambda,
    const double he )
{
  if ( K < Ki )
  {
    return 0.0;
  }

  double KC    = lambda / he * ( Kc - 0.5 * Ki ) + 0.5 * Ki;

  double omega = 1.0 - (Ki / K) * exp ( (Ki - K) / (KC - Ki) );

  return omega;  
}

//-----------------------------------------------------------------------
//    getDerivExponentialSoftening2
//-----------------------------------------------------------------------

double                       getDerivExponentialSoftening2

    ( const double Ki,
      const double Kc,
      const double K )

{ 
  double a = Ki - K;
  double b = Kc - Ki;
  double c = exp ( a / b );
  double d = Ki / (K * K);

  return d * c + d * K * c / b;
}

// overloaded version

double                       getDerivExponentialSoftening2

    ( const double Ki,
      const double Kc,
      const double K, const double lambda,
      const double he )

{
  double KC= lambda / he * ( Kc - 0.5 * Ki ) + 0.5 * Ki;

  double a = Ki - K;
  double b = KC - Ki;
  double c = exp ( a / b );
  double d = Ki / (K * K);

  return d * c + d * K * c / b;
}

// Jirasek regularised exponential softening law

double                  exponentialSoftening3

  ( const double Ki, const double K,
    const double gf, const double ft, 
    const double he )
{
  if ( K < Ki )
  {
    return 0.0;
  }

  double Kc    = 0.5 * Ki + gf / ft / he;
  double omega = 1.0 - (Ki / K) * exp ( (Ki - K) / (Kc - Ki) );

  return  omega;
}

double                  getDerivExponentialSoftening3

   ( const double Ki, const double K,
     const double gf, const double ft,
     const double he )
{
  double Kc = 0.5 * Ki + gf / ft / he;

  double a  = Ki - K;
  double b  = Kc - Ki;
  double c  = exp ( a / b );
  double d  = Ki / (K * K);

  return d * c + d * K * c / b;
}

//-----------------------------------------------------------------------
//    hyperbolicSoftening
//-----------------------------------------------------------------------

double                       hyperbolicSoftening

  ( const double Ki,
    const double b,
    const double K )
{
  if ( K < Ki )
  {
    return 0.0;
  }

  double omega = 1.0 - 1.0 / ( 1.0 + b * ( K - Ki ) );

  return isTiny ( 1.0 - omega ) ? DamageMaterial::CRITICAL_DAMAGE : omega;  
}

//-----------------------------------------------------------------------
//    getDerivExponentialSoftening2
//-----------------------------------------------------------------------

double                       getDerivHyperbolicSoftening

    ( const double Ki,
      const double b,
      const double K )

{

  return b / ( 1.0 + b * ( K - Ki ) ) / ( 1.0 + b * ( K - Ki ) ) ;
 
}


//-----------------------------------------------------------------------
//  powerSoftening
//-----------------------------------------------------------------------

// particularly implemented for the composite compact tension test

double                      powerSoftening

  ( const double Ki,
    const double Kc,
    const double alpha,
    const double beta,
    const double K )
{
  // the damage evolution in Geers - Enhanced solution control

  double omega;

  if ( K < Ki )
  {
    omega = 0.0;
  }
  else
  {
    const double gamma = 0.01;
    const double eta   = 1.0 - gamma;
    const idx_t  n     = -5;

    double  muy  = ( gamma - n * eta ) / ( n * eta * ::pow ( Ki, n ) );

    omega        = 1.0 - gamma * Ki / K - eta * ::pow ( K / Ki, n) *
                    ::exp ( muy * ( ::pow ( K, n ) - ::pow( Ki, n ) ) );  
  }
  
  return omega; 
}

//-----------------------------------------------------------------------
//  getDerivPowerSoftening
//-----------------------------------------------------------------------

double                      getDerivPowerSoftening

  ( const double Ki,
    const double Kc,
    const double alpha,
    const double beta,
    const double K )
{
  const double gamma = 0.01;
  const double eta   = 1.0 - gamma;
  const idx_t  n     = -5;

  double  muy  = ( gamma - n * eta ) / ( n * eta * ::pow ( Ki, n ) );

  double fac1  =  exp ( muy * ( ::pow ( K, n )  - ::pow( Ki, n ) ) );

  double ret   =  gamma * Ki / K / K - eta * n * ::pow ( K / Ki, n-1) / Ki * fac1
                 - eta * pow ( K / Ki, n) * fac1 * muy * n * ::pow ( K, n-1);
 
  return ret;
}

//-----------------------------------------------------------------------
//   expoEnergySoftening
//-----------------------------------------------------------------------


double                     expoEnergySoftening

 ( const double kappaI, const double kappaC,
   const double s, const double kappa )
{
  if ( kappa < kappaI ) return 0.;

  double tem = pow ( ( kappa - kappaI ) / kappaC, s );

  return ( 1. - exp ( - tem ) );
}

//-----------------------------------------------------------------------
//   getDerivExpoEnergySoftening
//-----------------------------------------------------------------------


double                      getDerivExpoEnergySoftening

  ( const double kappaI, const double kappaC,
    const double s, const double kappa )
{
  if ( kappa < kappaI ) return 0.;

  double tem1 = pow ( ( kappa - kappaI ) / kappaC, s   );
  double tem2 = pow ( ( kappa - kappaI ) / kappaC, s-1 );

  return  exp ( - tem1 ) * tem2 * s / kappaC;
}

//-----------------------------------------------------------------------
//   equiStrainRankine
//-----------------------------------------------------------------------

double                      equiStrainRankine

  ( const double  young,
    const Vec3&   princ )

{
  // This function computes the stress version of the Mazars criterion
  // I would not call that Rankine... [fpm]

  double ret   = 0.0;

  for ( idx_t i = 0; i < 3; i++ )
  {
    double x =  princ[i] ;

    if ( x > 0. ) ret += x * x ;
  }

  return ::sqrt ( ret ) / young;
}

