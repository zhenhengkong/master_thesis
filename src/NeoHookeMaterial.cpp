/*
 * 
 *  Copyright (C) 2018 TU Delft. All rights reserved.
 *  
 *  This class implements an isotropic NeoHookean material.
 *  Its update function computes PK2 stress from the Green-Lagrange
 *  strain tensor and evaluates the associated gradient
 *  Following Belytschko Sec. 5.4
 *  
 *  Author: F.P. van der Meer, f.p.vandermeer@tudelft.nl
 *  Date: January 2018
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
#include "voigtUtilities.h"
#include "NeoHookeMaterial.h"

using namespace jem;
using namespace voigtUtilities;
using jem::numeric::matmul;
using jem::numeric::LUSolver;

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  NeoHookeMaterial::LAMBDA_PROP  = "lambda";
const char*  NeoHookeMaterial::MU_PROP      = "mu";
const char*  NeoHookeMaterial::STATE_PROP   = "state";

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


NeoHookeMaterial::NeoHookeMaterial 

  ( idx_t rank, const Properties& globdat )
    : Material ( rank, globdat )
{
  JEM_PRECHECK ( rank >= 1 && rank <= 3 );

  lambda_ = 1.0;
  mu_     = 1.0;
}


NeoHookeMaterial::~NeoHookeMaterial ()
{}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void NeoHookeMaterial::configure ( const Properties& props )
{
  props.get ( lambda_, LAMBDA_PROP );
  props.get ( mu_, MU_PROP, 0.0, maxOf( mu_ ) );

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
      throw Error ( JEM_FUNC, "Plane stress not supported" );
    }
    else if ( stateString_ == "AXISYMMETRIC" )
    {
      state_ = AxiSymmetric;

      ++strCount;
    }
  }
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void NeoHookeMaterial::getConfig ( const Properties& conf ) const
{
  conf.set ( LAMBDA_PROP, lambda_ );
  conf.set ( MU_PROP, mu_ );

  if ( rank_ == 2 )
  {
    conf.set ( STATE_PROP, stateString_ );
  }
}

//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------

void NeoHookeMaterial::update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      idx_t                 ipoint )

{
  idx_t strCount = STRAIN_COUNTS[rank_];

  Matrix  C ( rank_, rank_ );  
  Matrix  S ( rank_, rank_ );

  voigt2TensorStrain ( C, strain );

  C *= 2.;
  for ( idx_t i = 0; i < rank_; ++i )
  {
    C(i,i) += 1;
  }

  double d;

  LUSolver::invert ( C, d );

  double jac = sqrt(d);

  double lnJ = log(jac);

  S = ( lambda_ * lnJ - mu_ ) * C;

  for ( idx_t i = 0; i < rank_; ++i )
  {
    S(i,i) += mu_;
  }
  tensor2VoigtStress ( stress, S );

  double muTangent = mu_ - lambda_ * lnJ;

  for ( idx_t iv = 0; iv < strCount; ++iv )
  {
    for ( idx_t jv = 0; jv < strCount; ++jv )
    {
      idx_t i, j, k, l;

      tensorIndex ( i, j, iv, rank_ );
      tensorIndex ( k, l, jv, rank_ );

      stiff(iv,jv)  = lambda_ * C(i,j) * C(k,l);
      stiff(iv,jv) += muTangent * ( C(i,k) * C(j,l) + C(i,l) * C(k,j) );
    }
  }


//   if ( rank_ == 2 )
//   {
//     double l2m = lambda_ + 2. * muTangent;
//     for ( idx_t i = 0; i < rank_; ++i )
//     {
//       for ( idx_t j = 0; j < rank_; ++j )
//       {
//         stiff(i,j)  = lambda_ * C(i,i) * C(j,j) ;
//         stiff(i,j) += 2. * muTangent * ( C(i,j) * C(i,j) );
//       }
//     }
// 
//     stiff(2,2)  = lambda_ * C(0,1) * C(0,1);
//     stiff(2,2) += muTangent * ( C(0,0) * C(1,1) + C(0,1) * C(0,1) );
//     stiff(0,2)  = l2m * C(0,0) * C(0,1);
//     stiff(1,2)  = l2m * C(1,1) * C(0,1);
//   }
//   else if ( rank_ == 3 )
//   {
//     Tuple<idx_t,6> I ( 0, 1, 2, 0, 1, 2 );
//     Tuple<idx_t,6> J ( 0, 1, 2, 1, 2, 0 );
// 
//     for ( idx_t iv = 0; iv < 6; ++iv )
//     {
//       for ( idx_t jv = iv; jv < 6; ++jv )
//       {
//         idx_t i = I[iv];
//         idx_t j = J[iv];
//         idx_t k = I[jv];
//         idx_t l = J[jv];
// 
//         stiff(iv,jv)  = lambda_ * C(i,j) * C(k,l);
//         stiff(iv,jv) += muTangent * ( C(i,k)*C(j,l) * C(i,l) * C(k,j) );
// 
//         stiff(jv,iv)  = stiff(iv,jv);
//       }
//       for ( idx_t jv = 3; jv < 6; ++jv )
//       {
//         idx_t i = I[iv];
//         idx_t j = J[iv];
//         idx_t k = I[jv];
//         idx_t l = J[jv];
// 
//         stiff(iv,jv)  = lambda_ * C(i,j) * C(k,l);
//         stiff(iv,jv) += muTangent * ( C(i,k)*C(j,l) * C(i,l) * C(k,j) );
//       }
//     }
//   }
}

//-----------------------------------------------------------------------
//   clone
//-----------------------------------------------------------------------

Ref<Material> NeoHookeMaterial::clone () const

{
  // use default copy constructor

  return newInstance<NeoHookeMaterial> ( *this );
}

//-----------------------------------------------------------------------
//   fill3DStress
//-----------------------------------------------------------------------

Tuple<double,6> NeoHookeMaterial::fill3DStress

  ( const Vector&    v3 ) const

{
  Error ( JEM_FUNC, "Not implemented" );
  Tuple<double,6> tmp;
  return tmp;
}

//-----------------------------------------------------------------------
//   fill3DStrain
//-----------------------------------------------------------------------

Tuple<double,6> NeoHookeMaterial::fill3DStrain

  ( const Vector&    v3 ) const

{
  Error ( JEM_FUNC, "Not implemented" );
  Tuple<double,6> tmp;
  return tmp;
}
