/*
 *  Copyright (C) 2013 TU Delft. All rights reserved.
 *  
 *  Object for computation of stress or strain invariants
 *  It works on a member vector vec_ in Voigt notation
 *  The derived class StrainInvariants removes the factor 2 from 
 *      engineering shear strain
 *
 *  Author: F.P. van der Meer
 *  Date: March 2013
 *
 */

// vector ordering 6: { a_xx, a_yy, a_zz, a_xy, a_yz, a_zx}^T   
// in 2d, first call fill3DStress or fill3DVec 

#include <jem/base/System.h>

#include "utilities.h"
#include "Invariants.h"
#include "voigtUtilities.h"

using jem::System;
using jem::io::endl;

static const double ONE_THIRD = 0.3333333333333333;

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------

Invariants::Invariants ( const Vec6& vec ) : v_(vec)
{
}

Invariants::Invariants ( const Vector& vec )
{
  JEM_ASSERT ( vec.size() == 6 );

  for ( idx_t i = 0; i < 6; ++i )
  {
    v_[i] = vec[i];
  }
}

Invariants::Invariants ( const Matrix& mat )
{
  JEM_ASSERT ( mat.size(0) == 3 );
  JEM_ASSERT ( mat.size(1) == 3 );

  voigtUtilities::tensor2VoigtStress3D ( v_, mat );
}

//-----------------------------------------------------------------------
//   getFirstInvariant
//-----------------------------------------------------------------------

double   Invariants::getFirstInvariant () const
{
  return v_[0] + v_[1] + v_[2];
}

//-----------------------------------------------------------------------
//   getSecondInvariant
//-----------------------------------------------------------------------

double  Invariants::getSecondInvariant () const
{
  double I2 = v_[0] * v_[1] + v_[1] * v_[2] + v_[2] * v_[0] - 
            ( v_[3] * v_[3] + v_[4] * v_[4] + v_[5] * v_[5] );

  return I2;
}

//-----------------------------------------------------------------------
//   getThirdInvariant
//-----------------------------------------------------------------------

double  Invariants::getThirdInvariant () const
{
  double I3 = v_[0] * v_[1] * v_[2] + 2.0 * v_[3] * v_[4] * v_[5] -
              v_[0] * v_[4] * v_[4] - 
              v_[1] * v_[5] * v_[5] - 
              v_[2] * v_[3] * v_[3];

  return I3;
}

//-----------------------------------------------------------------------
//   getJ2
//-----------------------------------------------------------------------

double  Invariants::getJ2 () const
{
  double J2 = ONE_THIRD * ( v_[0] * v_[0] + v_[1] * v_[1] + v_[2] * v_[2]   -
                     v_[0] * v_[1] - v_[1] * v_[2] - v_[2] * v_[0] ) +
                     v_[3] * v_[3] + v_[4] * v_[4] + v_[5] * v_[5];
  return J2;
}
  
// ---------------------------------------------------------------------
//  compute  the derivatives of first  invariant w.r.t a vector
// ---------------------------------------------------------------------

Vec6 Invariants::getGradI1 () const
  
{
  Vec6 ret;

  ret[0] = 1.0;
  ret[1] = 1.0;
  ret[2] = 1.0;
  
  ret[3] = 0.0;
  ret[4] = 0.0;
  ret[5] = 0.0;    

  return ret;
}

// ---------------------------------------------------------------------
//  compute  the derivatives of second  invariant w.r.t a vector
// ---------------------------------------------------------------------

Vec6  Invariants::getGradI2 () const

{
  Vec6 ret;

  ret[0] = v_[1] + v_[2];
  ret[1] = v_[2] + v_[0];
  ret[2] = v_[0] + v_[1];

  ret[3] = - 2. * v_[3];
  ret[4] = - 2. * v_[4];
  ret[5] = - 2. * v_[5];

  return ret;
}

// ---------------------------------------------------------------------
//  compute  the derivatives of third invariant w.r.t a vector
// ---------------------------------------------------------------------

Vec6  Invariants::getGradI3 () const
  
{
  Vec6 ret;

  ret[0] = v_[1] * v_[2] - v_[4] * v_[4];
  ret[1] = v_[2] * v_[0] - v_[5] * v_[5];
  ret[2] = v_[0] * v_[1] - v_[3] * v_[3];

  ret[3] = 2.0 * ( v_[4] * v_[5] - v_[2] * v_[3] );
  ret[4] = 2.0 * ( v_[5] * v_[3] - v_[0] * v_[4] );
  ret[5] = 2.0 * ( v_[3] * v_[4] - v_[1] * v_[5] );

  return ret;
}

// ---------------------------------------------------------------------
//  compute  the derivatives of second deviatoric invariant
// ---------------------------------------------------------------------

Vec6  Invariants::getGradJ2 () const
  
{
  Vec6 ret;

  ret[0] = ONE_THIRD * ( 2.0 * v_[0] - v_[1] - v_[2] );
  ret[1] = ONE_THIRD * ( 2.0 * v_[1] - v_[2] - v_[0] );
  ret[2] = ONE_THIRD * ( 2.0 * v_[2] - v_[0] - v_[1] );

  ret[3] = 2.0 * v_[3];
  ret[4] = 2.0 * v_[4];
  ret[5] = 2.0 * v_[5];

  return ret;
}


//-----------------------------------------------------------------------
//   getPrincipalValues + return invariants
//-----------------------------------------------------------------------

Vec3  Invariants::getPrincipalValues

  ( Vec3& I ) const
  
{
  I[0] = getFirstInvariant  ();
  I[1] = getSecondInvariant ();
  I[2] = getThirdInvariant  ();
  
  Vec3 roots;
  
  const idx_t  n  = solveCubicEqua ( roots, 1.0, -I[0], I[1], -I[2] );

  for ( idx_t i = n; i < 3; ++i ) roots[i] = 0.0;

  return roots;
}

//=======================================================================
//   StrainInvariants
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------

StrainInvariants::StrainInvariants ( const Vector& vec ) : Invariants ( vec )
{
  // remove engineering shear strain in order to be able to use 
  // generic Invariant member functions on stress and strain vectors

  v_[3] *= 0.5;
  v_[4] *= 0.5;
  v_[5] *= 0.5;
}

StrainInvariants::StrainInvariants ( const Vec6& vec ) : Invariants ( vec )
{
  // remove engineering shear strain in order to be able to use 
  // generic Invariant member functions on stress and strain vectors

  v_[3] *= 0.5;
  v_[4] *= 0.5;
  v_[5] *= 0.5;
}

//-----------------------------------------------------------------------
//   gradients
//-----------------------------------------------------------------------

Vec6 StrainInvariants::getGradI1() const
{
  return fixStrainGrad_ ( Invariants::getGradI1() );
}

Vec6 StrainInvariants::getGradI2() const
{
  return fixStrainGrad_ ( Invariants::getGradI2() );
}

Vec6 StrainInvariants::getGradI3() const
{
  return fixStrainGrad_ ( Invariants::getGradI3() );
}

Vec6 StrainInvariants::getGradJ2() const
{
  return fixStrainGrad_ ( Invariants::getGradJ2() );
}

//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   simple sort for Vec3
//-----------------------------------------------------------------------

Vec3 sort ( const Vec3& v )
{
  Tuple<idx_t,3> ii  ( 0, 0, 0 );

  v[0] >= v[1] ? ++ii[1] : ++ii[0];
  v[1] >= v[2] ? ++ii[2] : ++ii[1];
  v[0] >= v[2] ? ++ii[2] : ++ii[0];

  return Vec3 ( v[ii[0]], v[ii[1]], v[ii[2]] );
}

//-----------------------------------------------------------------------
//   simple sort of absolute values (maintaining sign) for Vec3
//-----------------------------------------------------------------------

Vec3 absSort ( const Vec3& v )
{
  Vec3           av  ( abs(v)  );
  Tuple<idx_t,3> ii  ( 0, 0, 0 );

  av[0] >= av[1] ? ++ii[1] : ++ii[0];
  av[1] >= av[2] ? ++ii[2] : ++ii[1];
  av[0] >= av[2] ? ++ii[2] : ++ii[0];

  return Vec3 ( v[ii[0]], v[ii[1]], v[ii[2]] );
}


