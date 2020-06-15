#include <jem/base/limits.h>
#include <jem/base/Error.h>
#include <jem/base/System.h>
#include <jem/base/Array.h>
#include <jem/base/PrecheckException.h>
#include <jem/util/Properties.h>
#include <jem/numeric/algebra/matmul.h>
#include <jem/numeric/algebra/MatmulChain.h>
#include <jem/numeric/algebra/EigenUtils.h>
#include <jem/numeric/algebra/LUSolver.h>
#include <jem/numeric/utilities.h>

#include "ThermNames.h"
#include "utilities.h"
#include "OrthotropicMaterial.h"

using namespace jem;
using jem::numeric::matmul;
using jem::numeric::MatmulChain;
using jem::numeric::invert;
using jem::numeric::LUSolver;
using jem::io::endl;

const double one_third = 0.33333333;

typedef MatmulChain<double,1>   MChain1;
typedef MatmulChain<double,3>   MChain3;

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  OrthotropicMaterial::YOUNG_1_PROP    = "young1";
const char*  OrthotropicMaterial::YOUNG_2_PROP    = "young2";
const char*  OrthotropicMaterial::YOUNG_3_PROP    = "young3";
const char*  OrthotropicMaterial::ALPHA_1_PROP    = "alpha1";
const char*  OrthotropicMaterial::ALPHA_2_PROP    = "alpha2";
const char*  OrthotropicMaterial::ALPHA_3_PROP    = "alpha3";
const char*  OrthotropicMaterial::POISSON_12_PROP = "poisson12";
const char*  OrthotropicMaterial::POISSON_23_PROP = "poisson23";
const char*  OrthotropicMaterial::POISSON_31_PROP = "poisson31";
const char*  OrthotropicMaterial::SHEAR_12_PROP   = "shear12";
const char*  OrthotropicMaterial::SHEAR_23_PROP   = "shear23";
const char*  OrthotropicMaterial::SHEAR_31_PROP   = "shear31";
const char*  OrthotropicMaterial::STATE_PROP      = "state";
const char*  OrthotropicMaterial::THETA_PROP      = "theta";



//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


OrthotropicMaterial::OrthotropicMaterial 

  ( const idx_t        rank,
    const Properties&  globdat )

  : Material ( rank, globdat )

{
  JEM_PRECHECK ( rank >= 2 && rank <= 3 );

  pCount_  = 0;

  young1_    = 1.0;
  young2_    = 1.0;
  young3_    = 1.0;
  alpha1_    = 0.0;
  alpha2_    = 0.0;
  alpha3_    = 0.0;
  poisson12_ =  .0;
  poisson23_ =  .0;
  poisson31_ =  .0;
  shear12_   = 0.5;
  shear23_   = 0.5;
  shear31_   = 0.5;
  theta_     = 0.0;


  const idx_t strCount = STRAIN_COUNTS[rank_];

  transformMat_     . resize ( strCount, strCount );
  transformMat_     = 0.0;
  transformMatInv_  . resize ( strCount, strCount );
  transformMatInv_  = 0.0;
  tt_               . ref    ( transformMatInv_.transpose() );
  stiffMat_         . resize ( strCount, strCount );
  stiffMat_         = 0.0;
  materialCompMat_  . resize ( strCount, strCount );
  materialCompMat_  = 0.0;
  materialStiffMat_ . resize ( strCount, strCount );
  materialStiffMat_ = 0.0;

  thermStrain_      . resize ( strCount );
  thermStrain_      = 0.0;

  preallocated_.resize ( 2*strCount+2, strCount );
  tmp66_       .resize ( 6, 6 );

  // store indices of material stress components that appear in sigma*n

  iNormal_.resize ( rank_ );

  if ( rank == 2 )
  {
    iNormal_[0] = 1;
    iNormal_[1] = 2;
  }
  else
  {
    iNormal_[0] = 1;
    iNormal_[1] = 3;
    iNormal_[2] = 4;
  }
}


OrthotropicMaterial::~OrthotropicMaterial ()
{}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void OrthotropicMaterial::configure 

  ( const Properties& props )

{
  using jem::maxOf;

  bool isotropic = false;

  JEM_PRECHECK ( rank_ > 1 );

  props.get ( young1_   , YOUNG_1_PROP   , 0.0, maxOf( young1_ ) );
  props.get ( poisson12_, POISSON_12_PROP, 0.0, 1.0 );

  if ( props.find ( young2_, YOUNG_2_PROP, 0.0, maxOf( young2_) ) )
  {
    props.get ( shear12_  , SHEAR_12_PROP, 0.0, maxOf( shear12_) );
    props.find( theta_, THETA_PROP, -90. , 90. );
  }
  else
  {
    isotropic = true;
    young2_ = young1_;
    shear12_ = young1_ / 2. / ( 1. + poisson12_ );
    theta_ = 0.;
  }

  props.find ( alpha1_   , ALPHA_1_PROP   , 0.0, maxOf( alpha1_ ) );
  props.find ( alpha2_   , ALPHA_2_PROP   , 0.0, maxOf( alpha2_ ) );

  if ( rank_ == 2  )
  {
    props.get ( state_, STATE_PROP);
  }
  else
  {
    state_ = "NOT_PLANE";
  }

  if ( rank_ == 3 || state_ == "PLANE_STRAIN" )
  {
    if ( isotropic ) 
    {
      poisson23_ = poisson12_;
    }
    else
    {
      props.get ( poisson23_, POISSON_23_PROP, 0.0, 0.5 );
    }

    if ( props.find ( young3_, YOUNG_3_PROP, 0.0, maxOf( young3_ ) ) )
    {
      // completely orthotropic material

      props.get ( shear23_  , SHEAR_23_PROP  , 0.0, maxOf( shear23_) );
      props.get ( poisson31_, POISSON_31_PROP, 0.0, 0.5 );
      props.get ( shear31_  , SHEAR_31_PROP  , 0.0, maxOf( shear31_) );
      props.find( alpha3_   , ALPHA_3_PROP   , 0.0, maxOf( alpha3_ ) );
    }
    else
    {
      // transversely isotropic material

      alpha3_    = alpha2_;
      young3_    = young2_;
      poisson31_ = poisson12_;
      shear31_   = shear12_;
      shear23_   = young2_ / ( 2. + 2. * poisson23_ );

      props.find( alpha3_   , ALPHA_3_PROP   , 0.0, maxOf( alpha3_ ) );
    }
  }

  // compute the elastic stiffness matrix, only one time

  computeTransformMats_ ();
  computeStiffMat_ ();

  // updateThermStrain ( globdat ); // TODO: do this somewhere
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void OrthotropicMaterial::getConfig

  ( const Properties& conf ) const 

{
  if ( rank_ == 2 )
  {
    conf.set ( STATE_PROP     , state_     );
  }

  conf.set ( YOUNG_1_PROP   , young1_    );
  conf.set ( YOUNG_2_PROP   , young2_    );
  conf.set ( POISSON_12_PROP, poisson12_ );
  conf.set ( SHEAR_12_PROP  , shear12_   );
  conf.set ( THETA_PROP     , theta_     );
  conf.set ( ALPHA_1_PROP   , alpha1_    );
  conf.set ( ALPHA_2_PROP   , alpha2_    );


  if ( rank_ == 3 || state_ == "PLANE_STRAIN" )
  {
    conf.set ( YOUNG_3_PROP   , young3_    );
    conf.set ( ALPHA_3_PROP   , alpha3_    );
    conf.set ( POISSON_23_PROP, poisson23_ );
    conf.set ( POISSON_31_PROP, poisson31_ );
    conf.set ( SHEAR_23_PROP  , shear23_   );
    conf.set ( SHEAR_31_PROP  , shear31_   );
  }
}

//-----------------------------------------------------------------------
//   elasticUpdate
//-----------------------------------------------------------------------

void OrthotropicMaterial::elasticUpdate

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain )

{
  Vector mechStrain ( strain - thermStrain_ );

  // return the elastic stiffness matrix

  stiff = stiffMat_;   

  // compute the stress vector

  matmul ( stress, stiffMat_, mechStrain );

}

//-----------------------------------------------------------------------
//   transform
//-----------------------------------------------------------------------

void OrthotropicMaterial::transform

( const Vector&         strain,
  Vector&               mStrain) const

{
  Vector mechStrain ( strain - thermStrain_ );
  matmul ( mStrain,         tt_, mechStrain );
}

//-----------------------------------------------------------------------
//   transformInv
//-----------------------------------------------------------------------

void OrthotropicMaterial::transformInv

( const Vector&         mStress,
  Vector&               stress) const

{
  matmul (  stress, transformMatInv_, mStress );
}

//-----------------------------------------------------------------------
//   changeFrame
//-----------------------------------------------------------------------

void OrthotropicMaterial::changeFrame

( const Matrix&         mStiff,
  Matrix&               stiff) const

{
  MChain3      mc3;
  stiff = mc3.matmul ( transformMatInv_, mStiff, tt_ );
}

//-----------------------------------------------------------------------
//   mUpdate
//-----------------------------------------------------------------------

void OrthotropicMaterial::mUpdate

( Vector&               mStress,
  Matrix&               mStiff,
  const Vector&         mStrain,
  const idx_t           ipoint )

{
  // compute the stress vector

  matmul ( mStress, materialStiffMat_, mStrain );
}


//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------

void OrthotropicMaterial::update

( Vector&               stress,
  Matrix&               stiff,
  const Vector&         strain,
  const idx_t           ipoint )

{
  const idx_t strCount = strain.size();

  Matrix mStiff  ( preallocated_(slice(0,strCount),ALL) );
  Vector mStrain ( preallocated_(strCount,ALL) );
  Vector mStress ( preallocated_(strCount+1,ALL) );

  transform( strain, mStrain );
  mStiff = materialStiffMat_;

  mUpdate(mStress, mStiff, mStrain, ipoint );

  transformInv( mStress, stress );
  changeFrame( mStiff, stiff );
}

//-----------------------------------------------------------------------
//   clone
//-----------------------------------------------------------------------

Ref<Material> OrthotropicMaterial::clone () const

{
  // use default copy constructor

  return newInstance<OrthotropicMaterial> ( *this );
}
  

//-----------------------------------------------------------------------
//   mGiveStressAtPoint 
//-----------------------------------------------------------------------

Vector OrthotropicMaterial::mGiveStressAtPoint 

 ( const idx_t   ipoint, 
   const Vector& mStrain,
   const double  he ) const

{
  return matmul ( materialStiffMat_, mStrain );
}

//-----------------------------------------------------------------------
//   giveStressAtPoint 
//-----------------------------------------------------------------------

Vector OrthotropicMaterial::giveStressAtPoint 

 ( const idx_t   ipoint, 
   const Vector& strain, 
   const double  he ) const

{
  MChain1    mc1;

  Vector       mStrain ( strain.size( ) );
  Vector       mStress ( strain.size( ) );

  transform( strain, mStrain );

  mStress = mGiveStressAtPoint ( ipoint , mStrain, he ) ;
  return  mc1.matmul ( transformMatInv_, mStress );
}

//-----------------------------------------------------------------------
//   getStiffMat
//-----------------------------------------------------------------------


Matrix OrthotropicMaterial::getStiffMat() const
{
  return stiffMat_;
}

//-----------------------------------------------------------------------
//   getTransformMat
//-----------------------------------------------------------------------


Matrix OrthotropicMaterial::getTransformMat() const
{
  return transformMat_;
}
  
//-----------------------------------------------------------------------
//   getTransformMatInv
//-----------------------------------------------------------------------


Matrix OrthotropicMaterial::getTransformMatInv() const
{
  return transformMatInv_;
}
  
//-----------------------------------------------------------------------
//   computeTransformMats_
//-----------------------------------------------------------------------


void   OrthotropicMaterial::computeTransformMats_ () 
{
  const double  pi = 3.1415926535897931;
  const double  c  = cos( theta_ * pi / 180.0 );
  const double  s  = sin( theta_ * pi / 180.0 );
  const double  sc = s*c;
  const double  c2 = c*c;
  const double  s2 = s*s;
  
  if ( rank_ == 3 ) 
  {
    transformMat_(0,0) = c2;
    transformMat_(0,1) = s2;
    transformMat_(0,3) = 2.0 * sc;

    transformMat_(1,0) = s2;
    transformMat_(1,1) = c2;
    transformMat_(1,3) = - 2.0 * sc;

    transformMat_(2,2) = 1.0;

    transformMat_(3,0) = - sc;
    transformMat_(3,1) = sc;
    transformMat_(3,3) = c2 - s2;

    transformMat_(4,4) = c;
    transformMat_(4,5) = - s;

    transformMat_(5,4) = s;
    transformMat_(5,5) = c;

    transformMatInv_(0,0) = c2;
    transformMatInv_(0,1) = s2;
    transformMatInv_(0,3) = - 2.0 * sc;

    transformMatInv_(1,0) = s2;
    transformMatInv_(1,1) = c2;
    transformMatInv_(1,3) = 2.0 * sc;

    transformMatInv_(2,2) = 1.0;

    transformMatInv_(3,0) = sc;
    transformMatInv_(3,1) = - sc;
    transformMatInv_(3,3) = c2 - s2;

    transformMatInv_(4,4) = c;
    transformMatInv_(4,5) = s;

    transformMatInv_(5,4) = - s;
    transformMatInv_(5,5) = c;
  }
  else if ( rank_ == 2 )
  {
    transformMat_(0,0) = c2;
    transformMat_(0,1) = s2;
    transformMat_(0,2) = 2.0 * sc;

    transformMat_(1,0) = s2;
    transformMat_(1,1) = c2;
    transformMat_(1,2) = - 2.0 * sc;

    transformMat_(2,0) = - sc;
    transformMat_(2,1) = sc;
    transformMat_(2,2) = c2 - s2;

    transformMatInv_(0,0) = c2;
    transformMatInv_(0,1) = s2;
    transformMatInv_(0,2) = - 2.0 * sc;

    transformMatInv_(1,0) = s2;
    transformMatInv_(1,1) = c2;
    transformMatInv_(1,2) = 2.0 * sc;

    transformMatInv_(2,0) = sc;
    transformMatInv_(2,1) = - sc;
    transformMatInv_(2,2) = c2 - s2;
  }
  else
  {
    throw Error ( JEM_FUNC, "unexpected rank: " + String ( rank_ ) );
  }
}


//-----------------------------------------------------------------------
//   computeStiffMat_
//-----------------------------------------------------------------------


void   OrthotropicMaterial::computeStiffMat_ () 

{
  const double  e1   = young1_;
  const double  e2   = young2_;
  const double  e3   = young3_;
  const double  nu12 = poisson12_;
  const double  nu23 = poisson23_;
  const double  nu31 = poisson31_;
  const double  g12  = shear12_;
  const double  g23  = shear23_;
  const double  g31  = shear31_;

  MChain3    mc3;

  if ( rank_ == 3 ) 
  {
    materialCompMat_(0,0) = 1.0 / e1;
    materialCompMat_(1,1) = 1.0 / e2;
    materialCompMat_(2,2) = 1.0 / e3;

    materialCompMat_(0,1) = materialCompMat_(1,0) = -nu12 / e1;
    materialCompMat_(0,2) = materialCompMat_(2,0) = -nu31 / e1;
    materialCompMat_(1,2) = materialCompMat_(2,1) = -nu23 / e2;

    materialCompMat_(3,3) = 1.0 / g12;
    materialCompMat_(4,4) = 1.0 / g23;
    materialCompMat_(5,5) = 1.0 / g31;

    materialStiffMat_ = materialCompMat_;

    invert( materialStiffMat_ );
  }
  else if ( state_ == "PLANE_STRAIN" )
  {
    // NB: the plane strain materialCompMat_ contains the 
    // slice(0,3)-part of the full 3D compliance matrix
    // This cannot be used to compute strain from stress but it can be
    // for update and inversion to compute damaged stiffness matrix

    materialCompMat_(0,0) = 1.0 / e1;
    materialCompMat_(1,1) = 1.0 / e2;
    materialCompMat_(2,2) = 1.0 / e3;

    materialCompMat_(0,1) = materialCompMat_(1,0) = -nu12 / e1;
    materialCompMat_(0,2) = materialCompMat_(2,0) = -nu31 / e1;
    materialCompMat_(1,2) = materialCompMat_(2,1) = -nu23 / e2;

    materialStiffMat_ = materialCompMat_;

    invert( materialStiffMat_ );

    materialStiffMat_(ALL,  2) = 0.;
    materialStiffMat_(  2,ALL) = 0.;
    materialStiffMat_(  2,  2)   = g12;
  }
  else if ( state_ == "PLANE_STRESS" )
  {
    materialCompMat_(0,0) = 1.0 / e1;
    materialCompMat_(1,1) = 1.0 / e2;
    materialCompMat_(2,2) = 1.0 / g12;
    materialCompMat_(0,1) = materialCompMat_(1,0) = -nu12 / e1;
    materialCompMat_(0,2) = materialCompMat_(2,0) = 0.0;
    materialCompMat_(1,2) = materialCompMat_(2,1) = 0.0;

    materialStiffMat_ = materialCompMat_;

    invert( materialStiffMat_ );
  }
  else
  {
    throw Error ( JEM_FUNC, "unexpected rank (or state): " + String ( rank_ ) );
  }
 
  // System::out() << "computed orthotropic stiffness matrix: " << endl <<
    // materialStiffMat_ << endl;
  
  stiffMat_ = mc3.matmul ( transformMatInv_, materialStiffMat_, tt_);

  // System::out() << "transformed orthotropic stiffness matrix: " << endl <<
    // stiffMat_ << endl;

}


//-----------------------------------------------------------------------
//   updateThermStrain_
//-----------------------------------------------------------------------


void   OrthotropicMaterial::updateThermStrain

  ( const Properties&  params ) 

{
  double dT = 0.;

  params.find ( dT, ThermNames::DELTA_T );

  Vector mStrain ( thermStrain_.size() );

  mStrain = 0.0;
 
  if ( dT )
  {
    mStrain[0] = alpha1_ * dT;
    mStrain[1] = alpha2_ * dT;

    if ( rank_ == 2 )
    {
      if (  state_ == "PLANE_STRAIN" )
      {
        System::warn() << "thermal expansion not correct for plane" << 
          " strain!\n";
      }
    }
    else if ( rank_ == 3 )
    {
      mStrain[2] = alpha3_ * dT;
    }

    matmul ( thermStrain_, transformMat_.transpose() , mStrain );
  }
  // System::out() << "thermal strain " << thermStrain_ << endl;
}


//-----------------------------------------------------------------------
//   giveStrainVector
//-----------------------------------------------------------------------

Vector OrthotropicMaterial:: giveStrainVector

    ( const Vector& strain ) const
   
{
  const idx_t strCount = strain.size ();

  JEM_PRECHECK ( strCount == 1 ||
                 strCount == 3 ||
                 strCount == 6 );

  if      ( strCount == 1 )
  {
    return strain;
  }
  else if ( strCount == 6 )
  {
    Vector ret ( 6 );

    ret[0] = strain[0];
    ret[1] = strain[1];
    ret[2] = strain[2];
    
    ret[3] = 0.5 * strain[3];
    ret[4] = 0.5 * strain[4];
    ret[5] = 0.5 * strain[5];

    return ret;
  }
  else if ( state_ == "PLANE_STRAIN" )
  {
    Vector ret ( 3 );

    ret[0] = strain[0];
    ret[1] = strain[1];
    ret[2] = 0.5 * strain[2];
    
    return ret;
  }

  // Plane stress, epsilon_zz put in the last position
  
  else 
  {
    Vector ret ( 4 );

    double epsXX = strain[0];
    double epsYY = strain[1];
    
    ret[0] = epsXX;
    ret[1] = epsYY;
    ret[2] = 0.5 * strain[2];
    ret[3] = - poisson12_ / ( 1.0 - poisson12_ ) * ( epsXX + epsYY );

    return ret;
  }

}


//-----------------------------------------------------------------------
//   giveStressVector
//-----------------------------------------------------------------------

Vector OrthotropicMaterial:: giveStressVector

    ( const Vector& stress ) const
   
{
  // Plane strain, sigma_zz put in last position

  if ( state_ == "PLANE_STRAIN" )
  {
    Vector ret ( 4 );

    double sigmaXX = stress[0];
    double sigmaYY = stress[1];

    ret[0]         = sigmaXX;
    ret[1]         = sigmaYY;
    ret[2]         = stress[2];
    ret[3]         = poisson12_ * ( sigmaXX + sigmaYY );

    return ret;    
  }
  else
  {
    return stress;
  }
}

//-----------------------------------------------------------------------
//   getNPrincipal
//-----------------------------------------------------------------------

idx_t     OrthotropicMaterial::getNPrincipal   () const
{
  return STRAIN_COUNTS [ rank_ ] ;
}

//-----------------------------------------------------------------------
//   getPrincipalStress
//-----------------------------------------------------------------------

Vector    OrthotropicMaterial::getPrincipalStress

    ( const Vector&      stress ) const
{
  Vector   matStress ( STRAIN_COUNTS [rank_] );

  matmul ( matStress, transformMat_ , stress );

  return matStress;
}

//-----------------------------------------------------------------------
//   getFailureStress
//-----------------------------------------------------------------------

Vector    OrthotropicMaterial::getFailureStress

    ( const Vector&      stress ) const

{
  Vector sigmaN ( rank_ );

  Matrix transN ( select ( transformMat_, iNormal_, ALL ) );

  matmul ( sigmaN, transN, stress );
//   Vector mStress ( getPrincipalStress ( stress ) );
// 
//   sigmaN = select ( mStress, iNormal_ );

  return sigmaN;
}


//-----------------------------------------------------------------------
//   getMaterialComplMat 
//-----------------------------------------------------------------------

void OrthotropicMaterial::getMaterialComplMat 
  
  ( const Matrix& compm ) const

{
  compm = materialCompMat_;
}

//-----------------------------------------------------------------------
//   getMaterialStiffMat 
//-----------------------------------------------------------------------

void OrthotropicMaterial::getMaterialStiffMat 

  ( const Matrix& stiff ) const

{
  stiff = materialStiffMat_;
}

//-----------------------------------------------------------------------
//   allocPoints 
//-----------------------------------------------------------------------

void     OrthotropicMaterial::allocPoints ( const idx_t count ) 
{
  pCount_ += count;
}

void     OrthotropicMaterial::allocPoints

  ( const idx_t      count,
    const Matrix&    transfer,
    const IntVector& oldPoints )

{
  allocPoints ( count );
}

//-----------------------------------------------------------------------
//   fill3DStress
//-----------------------------------------------------------------------

Tuple<double,6> OrthotropicMaterial::fill3DStress

  ( const Vector&    v3 ) const

{
  if ( v3.size() == 3 )
  {
    double sig_zz = 0.; // not correct for plane strain!

    return fillFrom2D_ ( v3, sig_zz );
  }
  else
  {
    return fillFrom3D_ ( v3 );
  }
}

//-----------------------------------------------------------------------
//   fill3DStrain
//-----------------------------------------------------------------------

Tuple<double,6> OrthotropicMaterial::fill3DStrain

  ( const Vector&    v3 ) const

{
  if ( v3.size() == 3 )
  {
    double eps_zz = 0.; // not correct for plane stress!

    return fillFrom2D_ ( v3, eps_zz );
  }
  else
  {
    return fillFrom3D_ ( v3 );
  }
}

//-----------------------------------------------------------------------
//   reduce3DStrainGrad
//-----------------------------------------------------------------------

void OrthotropicMaterial::reduce3DStrainGrad

  ( const Vector&          v3,
    const Tuple<double,6>& v6 ) const

{
  if ( v3.size() == 3 )
  {
    double zzContrib = 0.; // not correct for plane stress!

    reduceTo2D_ ( v3, zzContrib, v6 );
  }
  else
  {
    reduceTo3D_ ( v3, v6 );
  }
}

//-----------------------------------------------------------------------
//   reduce3DStressGrad
//-----------------------------------------------------------------------

void OrthotropicMaterial::reduce3DStressGrad

  ( const Vector&          v3,
    const Tuple<double,6>& v6 ) const

{
  if ( v3.size() == 3 )
  {
    double zzContrib = 0.; // not correct for plane strain!

    reduceTo2D_ ( v3, zzContrib, v6 );
  }
  else
  {
    reduceTo3D_ ( v3, v6 );
  }
}

//-----------------------------------------------------------------------
//   reduce3DMatrix
//-----------------------------------------------------------------------

void OrthotropicMaterial::reduce3DMatrix

  ( const Matrix&   m,
    const Matrix&   m66 ) const 

{
  if ( m.size(0) == 3 )
  {
    if ( state_ == "PLANE_STRAIN" )
    {
      select2DMatrix ( m, m66 );
    }
    else
    {
      double d;
      tmp66_ = m66;
      LUSolver::invert ( tmp66_, d );
      select2DMatrix ( m, tmp66_ );
      LUSolver::invert ( m, d );
    }
  }
  else
  {
    m = m66;
  }
}

//-----------------------------------------------------------------------
//   reduce3DVector
//-----------------------------------------------------------------------

void OrthotropicMaterial::reduce3DVector

  ( const Vector&   v,
    const Vector&   v6 ) const 

{
  // reduce a full 3D tuple to a 2D or 3D vector
  // (works the same for both stress and strain)

  JEM_ASSERT ( v6.size() == 6 );

  if ( v.size() == 3 )
  {
    v[0] = v6[0];
    v[1] = v6[1];
    v[2] = v6[3];
  }
  else
  {
    v[0] = v6[0];
    v[1] = v6[1];
    v[2] = v6[2];
    v[3] = v6[3];
    v[4] = v6[4];
    v[5] = v6[5];
  }
}

//-----------------------------------------------------------------------
//   select2DMatrix
//-----------------------------------------------------------------------

void OrthotropicMaterial::select2DMatrix

  ( const Matrix&  m,
    const Matrix&  m66 ) const 

{
  // selecting [xx, yy, xy] components from [xx, yy, zz, xy, yz, xz]

  for ( idx_t i = 0; i < 2; ++i )
  {
    m(i,2) = m66(i,3);
    m(2,i) = m66(3,i); 

    for ( idx_t j = 0; j < 2; ++j )
    {
      m(i,j) = m66(i,j);
    }
  }
  m(2,2) = m66(3,3);
}
