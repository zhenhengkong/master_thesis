/*
 * 
 *  Copyright (C) 2007 TU Delft. All rights reserved.
 *  
 *  This class implemens the isotropic elastic material
 *  This represents the material at a point in space.
 *  It is implemented in such a way that can be used for any
 *  discretisation methods, say finite elements, EFG and so on.
 *  
 *  Author: V.P. Nguyen, V.P.Nguyen@tudelft.nl
 *  Date: 30 November 2007
 *
 */


#include <jem/base/limits.h>
#include <jem/base/Error.h>
#include <jem/base/PrecheckException.h>
#include <jem/base/array/operators.h>
#include <jem/util/Properties.h>
#include <jem/numeric/algebra/matmul.h>
#include <jem/numeric/algebra/LUSolver.h>
#include <jem/numeric/utilities.h>
#include <jem/base/System.h>

#include "utilities.h"
#include "HookeMaterial.h"
#include "ThermNames.h"

using namespace jem;
using jem::numeric::matmul;
using jem::numeric::LUSolver;

const double one_third = 0.33333334;

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  HookeMaterial::YOUNG_PROP   = "young";
const char*  HookeMaterial::POISSON_PROP = "poisson";
const char*  HookeMaterial::RHO_PROP     = "rho";
const char*  HookeMaterial::STATE_PROP   = "state";
const char*  HookeMaterial::ALPHA_PROP   = "alpha";

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


HookeMaterial::HookeMaterial 

  ( idx_t rank, const Properties& globdat )
    : Material ( rank, globdat )
{
  JEM_PRECHECK ( rank >= 1 && rank <= 3 );

  young_   = 1.0;
  poisson_ = 1.0;
  rho_     = 1.0;
  alpha_   = 0.0;
}


HookeMaterial::~HookeMaterial ()
{}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void HookeMaterial::configure ( const Properties& props )
{
  props.get ( young_,   YOUNG_PROP,   0.0, maxOf( young_ ) );

  if ( rank_ > 1 )
  {
    props.get ( poisson_, POISSON_PROP, 0.0, 0.5 );
  }

  props.find ( alpha_, ALPHA_PROP,   0.0, maxOf( alpha_ ) );
  props.find ( rho_,   RHO_PROP,     0.0, maxOf( rho_   ) );

  idx_t strCount = STRAIN_COUNTS[rank_];

  // read problem type, plane stress ...

  if ( rank_ == 2  )
  {
    props.get( stateString_, STATE_PROP );

    if      ( stateString_ == "PLANE_STRAIN" )
    {
      state_ = PlaneStrain;
    }
    else if ( stateString_ == "PLANE_STRESS" )
    {
      state_ = PlaneStress;
    }
    else if ( stateString_ == "AXISYMMETRIC" )
    {
      state_ = AxiSymmetric;

      ++strCount;
    }
  }

  // compute the elastic moduli, only once time

  stiffMat_ .resize ( strCount, strCount );
  stiffMat_ = 0.0;

  computeStiffMat_ ();
  computeComplMat_ ();

  thermStrain_ . resize ( strCount );
  thermStrain_ = 0.0;
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void HookeMaterial::getConfig ( const Properties& conf ) const
{
  conf.set ( YOUNG_PROP,   young_   );

  if ( rank_ > 1 )
  {
    conf.set ( POISSON_PROP, poisson_ );
  }

  if ( rank_ == 2 )
  {
    conf.set ( STATE_PROP, stateString_ );
  }
}

//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------

void HookeMaterial::update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      idx_t                 ipoint )

{
  // compute the elastic moduli

  stiff = stiffMat_;

  // compute the stress vector

  matmul ( stress, stiffMat_, Vector(strain-thermStrain_) );
}

//-----------------------------------------------------------------------
//   updateThermStrain_
//-----------------------------------------------------------------------

void HookeMaterial::updateThermStrain

  ( const Properties&  params ) 

{
  double dT = 0.;

  params.find ( dT, ThermNames::DELTA_T );

  for ( idx_t i = 0; i < rank_; ++i )
  {
    thermStrain_[i] = alpha_ * dT;
  }
  if ( rank_ == 2 && state_ == AxiSymmetric )
  {
    thermStrain_[3] = alpha_ * dT;
  }
  System::out() << "thermStrain " << thermStrain_ << endl;
}

//-----------------------------------------------------------------------
//   clone
//-----------------------------------------------------------------------

Ref<Material> HookeMaterial::clone () const

{
  // use default copy constructor

  return newInstance<HookeMaterial> ( *this );
}
  
//-----------------------------------------------------------------------
//   computeStiffMat_
//-----------------------------------------------------------------------

void   HookeMaterial::computeStiffMat_ () 
{
  const idx_t   n  = STRAIN_COUNTS[rank_];

  const double  e  = young_;
  const double  nu = poisson_;


  if      ( rank_ == 1 )
  {
    stiffMat_(0,0) = e;
  }
  else if ( rank_ == 3 )
  {
    const double  a = e / ((1.0 + nu) * (1.0 - 2.0 * nu));
    const double  b = 0.5 * (1.0 - 2.0 * nu);
    const double  c = 1.0 - nu;

    stiffMat_(0,0) = a * c;
    stiffMat_(0,1) = stiffMat_(0,2) = a * nu;
    stiffMat_(1,1) = stiffMat_(0,0);
    stiffMat_(1,2) = stiffMat_(0,1);
    stiffMat_(2,2) = stiffMat_(0,0);
    stiffMat_(3,3) = a * b;
    stiffMat_(4,4) = stiffMat_(3,3);
    stiffMat_(5,5) = stiffMat_(3,3);

    // Copy lower triangle of the stress-strain matrix.

    for ( idx_t i = 0; i < n; i++ )
    {
      for ( idx_t j = 0; j < i; j++ )
      {
        stiffMat_(i,j) = stiffMat_(j,i);
      }
    }
  }
  else if ( state_ == PlaneStrain )
  {
    const double  a = e / ((1.0 + nu) * (1.0 - 2.0 * nu));
    const double  b = 0.5 * (1.0 - 2.0 * nu);
    const double  c = 1.0 - nu;

    stiffMat_(0,0) = a * c;
    stiffMat_(0,1) = stiffMat_(1,0) = a * nu;
    stiffMat_(1,1) = a * c;
    stiffMat_(2,2) = a * b;
  }
  else if ( state_ == PlaneStress )
  {
    const double  a = e / (1.0 - nu * nu);

    stiffMat_(0,0) = a;
    stiffMat_(0,1) = stiffMat_(1,0) = a * nu;
    stiffMat_(1,1) = a;
    stiffMat_(2,2) = a * 0.5 * (1.0 - nu);
  }
  else if ( state_ == AxiSymmetric )
  {
    const double  a = e / ((1.0 + nu) * (1.0 - 2.0 * nu));
    const double  b = 0.5 * (1.0 - 2.0 * nu);
    const double  c = 1.0 - nu;

    stiffMat_(0,0) = a * c;
    stiffMat_(0,1) = stiffMat_(0,3) = a * nu;
    stiffMat_(1,1) = stiffMat_(3,3) = stiffMat_(0,0);
    stiffMat_(1,3) = stiffMat_(0,1);
    stiffMat_(2,2) = a * b;

    // Copy lower triangle of the stress-strain matrix.

    for ( idx_t i = 0; i < n+1; i++ )
    {
      for ( idx_t j = 0; j < i; j++ )
      {
        stiffMat_(i,j) = stiffMat_(j,i);
      }
    }
  }
  else
  {
    throw Error ( JEM_FUNC, "unexpected rank: " + String ( rank_ ) );
  }
}

//-----------------------------------------------------------------------
//   computeComplMat_
//-----------------------------------------------------------------------


void   HookeMaterial::computeComplMat_ ()
{
  double d;
  complMat_ . resize ( stiffMat_.size(0), stiffMat_.size(1) );
  complMat_ = stiffMat_;
  LUSolver::invert ( complMat_, d );
}

//-----------------------------------------------------------------------
//   fill3DStress
//-----------------------------------------------------------------------

Tuple<double,6> HookeMaterial::fill3DStress

  ( const Vector&    v3 ) const

{
  if ( v3.size() == 3 )
  {
    double sig_zz = state_ == PlaneStress
                  ? 0.
                  : poisson_ * ( v3[0] + v3[1] );

    return fillFrom2D_ ( v3, sig_zz );
  }
  else if ( v3.size() == 4 )
  {
    return fillFrom2D_ ( v3[slice(0,3)], v3[3] );
  }
  else
  {
    return fillFrom3D_ ( v3 );
  }
}

//-----------------------------------------------------------------------
//   fill3DStrain
//-----------------------------------------------------------------------

Tuple<double,6> HookeMaterial::fill3DStrain

  ( const Vector&    v3 ) const

{
  if ( v3.size() == 3 )
  {
    double eps_zz = state_ == PlaneStress
                  ? -poisson_ / (1.-poisson_) * (v3[0]+v3[1])
                  : 0.;

    return fillFrom2D_ ( v3, eps_zz );
  }
  else if ( v3.size() == 4 )
  {
    return fillFrom2D_ ( v3[slice(0,3)], v3[3] );
  }
  else
  {
    return fillFrom3D_ ( v3 );
  }
}

//-----------------------------------------------------------------------
//   reduce3DStrainGrad
//-----------------------------------------------------------------------

void HookeMaterial::reduce3DStrainGrad

  ( const Vector&          v3,
    const Tuple<double,6>& v6 ) const 

{
  // reduce a full 3D tuple with gradients wrt strain
  // to a vector that is either 2D or 3D

  if ( v3.size() == 3 )
  {
    double zzContrib = 0.;

    if ( state_ == PlaneStress )
    {
      zzContrib = -poisson_ / (1.-poisson_) * v6[2];
    }
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

void HookeMaterial::reduce3DStressGrad

  ( const Vector&          v3,
    const Tuple<double,6>& v6 ) const 

{
  // reduce a full 3D tuple with gradients wrt stress
  // to a vector that is either 2D or 3D

  if ( v3.size() == 3 )
  {
    double zzContrib = 0.;

    if ( state_ == PlaneStrain )
    {
      zzContrib = -poisson_ / (1.-poisson_) * v6[2];
    }
    reduceTo2D_ ( v3, zzContrib, v6 );
  }
  else
  {
    reduceTo3D_ ( v3, v6 );
  }
}

//-----------------------------------------------------------------------
//   reduce3DVector
//-----------------------------------------------------------------------

void HookeMaterial::reduce3DVector

  ( const Vector&          v,
    const Tuple<double,6>& t ) const 

{
  // reduce a full 3D tuple to a 2D or 3D vector
  // (works the same for both stress and strain)

  if ( v.size() == 3 )
  {
    v[0] = t[0];
    v[1] = t[1];
    v[2] = t[3];
  }
  else if ( v.size() == 4 )
  {
    Tuple<int,4> aximap ( 0, 1, 3, 2 );

    for ( idx_t i = 0; i < 4; ++i )
    {
      v[i] = t[aximap[i]];
    }
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
//   reduce3DMatrix
//-----------------------------------------------------------------------

void HookeMaterial::reduce3DMatrix

  ( const Matrix&            m,
    const Tuple<double,6,6>& t ) const 

{
  if ( m.size(0) == 3 )
  {
    if ( state_ == PlaneStrain )
    {
      select2DMatrix ( m, t );
    }
    else
    {
      double d;
      Tuple<double,6,6> tmp66 = t;
      LUSolver::invert ( tmp66, d );
      select2DMatrix ( m, tmp66 );
      LUSolver::invert ( m, d );
    }
  }
  else if ( m.size(0) == 4 )
  {
    selectAxiMatrix ( m, t );
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
//   select2DMatrix
//-----------------------------------------------------------------------

void HookeMaterial::select2DMatrix

  ( const Matrix&            m,
    const Tuple<double,6,6>& t ) const 

{
  // selecting [xx, yy, xy] components from [xx, yy, zz, xy, yz, xz]

  for ( idx_t i = 0; i < 2; ++i )
  {
    m(i,2) = t(i,3);
    m(2,i) = t(3,i); 

    for ( idx_t j = 0; j < 2; ++j )
    {
      m(i,j) = t(i,j);
    }
  }
  m(2,2) = t(3,3);
}

//-----------------------------------------------------------------------
//   selectAxiMatrix
//-----------------------------------------------------------------------

void HookeMaterial::selectAxiMatrix

  ( const Matrix&            m,
    const Tuple<double,6,6>& t ) const 

{
  // selecting [xx, yy, xy, zz] components from [xx, yy, zz, xy, yz, xz]

  Tuple<int,4> aximap ( 0, 1, 3, 2 );

  for ( idx_t i = 0; i < 4; ++i )
  {
    for ( idx_t j = 0; j < 4; ++j )
    {
      m(i,j) = t ( aximap[i], aximap[j] );
    }
  }
}
