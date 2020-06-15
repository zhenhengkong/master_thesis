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

#ifndef INVARIANTS_H
#define INVARIANTS_H

#include <jem/base/Tuple.h>
#include <jive/Array.h>

using jive::Vector;
using jive::Matrix;

using jem::Tuple;

typedef Tuple<double,6> Vec6;
typedef Tuple<double,3> Vec3;

class Invariants
{
 protected:
  // Invariants  ();
 public:
  Invariants  ( const Vec6&       vec );
  Invariants  ( const Vector&     vec );
  Invariants  ( const Matrix&     mat );

  // virtual  ~Invariants  ();

  //-----------------------------------------------------------------------
  //   compute the invariants of the strain tensor I1 I2 I3
  //-----------------------------------------------------------------------

  double                  getFirstInvariant () const;

  double                  getSecondInvariant () const;

  double                  getThirdInvariant () const;

  //-----------------------------------------------------------------------
  //   compute the second invariant of the deviatoric strain tensor J2
  //-----------------------------------------------------------------------

  double                  getJ2 () const;

  //-----------------------------------------------------------------------
  //   compute the derivatives w.r.t strain tensor  of
  //   invariants of the strain tensor
  //-----------------------------------------------------------------------

  virtual Vec6            getGradI1 () const;
  virtual Vec6            getGradI2 () const;
  virtual Vec6            getGradI3 () const;
  virtual Vec6            getGradJ2 () const;

  //-----------------------------------------------------------------------
  //   compute principal values (this version also returns invariants)
  //-----------------------------------------------------------------------

  Vec3                    getPrincipalValues

    ( Vec3&                 I ) const;

  //-----------------------------------------------------------------------
  //   inline functions for additional interface
  //-----------------------------------------------------------------------

  void                    getI1J2andGrads   

  ( double&                 I1,
    double&                 J2, 
    Vec6&                   dI1, 
    Vec6&                   dJ2 ) const;

  void                    getInvsAndGrads

  ( Vec3& I,   
    Vec3& princ,
    Vec6& dI1, 
    Vec6& dI2, 
    Vec6& dI3 ) const;

  void                    getI1andJ2  

  ( double&                 I1,
    double&                 J2 ) const;

  Vec3                    getSortedPrincipalValues () const;

  Vec3                    getPrincipalValues  () const;

  void                    getPrincipalValues

    ( const Vector&         p ) const;

 protected:

  Vec6                      v_;
};

class StrainInvariants : public Invariants
{
 public:
  StrainInvariants ( const Vector& vec );
  StrainInvariants ( const Vec6&   vec );

  virtual Vec6            getGradI1 () const;
  virtual Vec6            getGradI2 () const;
  virtual Vec6            getGradI3 () const;
  virtual Vec6            getGradJ2 () const;

 protected:

  Vec6                    fixStrainGrad_  
    
    ( const Vec6&           v6 ) const;
};

// define StressInvariants analogous to StrainInvariants 
// but with default behavior

typedef Invariants StressInvariants;  

Vec3 sort ( const Vec3& v );

Vec3 absSort ( const Vec3& v );

//-----------------------------------------------------------------------
//   getSortedPrincipalValues
//-----------------------------------------------------------------------

inline Vec3  Invariants::getSortedPrincipalValues () const
{
  return absSort(getPrincipalValues());
}

//-----------------------------------------------------------------------
//   getPrincipalValues (not returning invariants)
//-----------------------------------------------------------------------

inline Vec3  Invariants::getPrincipalValues () const
{
  Vec3 I;
  return getPrincipalValues ( I );
}

//-----------------------------------------------------------------------
//   getPrincipalValues (as Vector)
//-----------------------------------------------------------------------

inline void  Invariants::getPrincipalValues

  ( const Vector&  p ) const
  
{
  JEM_ASSERT ( p.size() == 3 );

  Vec3 tmp = getPrincipalValues();

  p[0] = tmp[0];
  p[1] = tmp[1];
  p[2] = tmp[2];
}

// ------------------------------------------------------------------
//  getI1andJ2
// ------------------------------------------------------------------

inline void  Invariants::getI1andJ2

  ( double&             I1,
    double&             J2 ) const

{
  I1 = getFirstInvariant();
  J2 = getJ2();
}

// ---------------------------------------------------------------
//   getInvsAndGrads
// ---------------------------------------------------------------

inline void  Invariants::getInvsAndGrads

  ( Vec3& I,   
    Vec3& princ,
    Vec6& dI1, 
    Vec6& dI2, 
    Vec6& dI3 ) const

{
  princ = getPrincipalValues ( I );
  dI1   = getGradI1();
  dI2   = getGradI2();
  dI3   = getGradI3();
}

// ------------------------------------------------------------------
//  getI1J2andGrads   
// ------------------------------------------------------------------

inline void  Invariants::getI1J2andGrads   

  ( double&  I1,
    double&  J2, 
    Vec6&    dI1, 
    Vec6&    dJ2 ) const

{
  I1 = getFirstInvariant();
  J2 = getJ2();

  dI1 = getGradI1();
  dJ2 = getGradJ2();
}

// ------------------------------------------------------------------
//  fixStrainGrad_   
// ------------------------------------------------------------------

inline Vec6  StrainInvariants::fixStrainGrad_

 ( const Vec6&  g ) const

{
  Vec6 ret;

  ret[0] = g[0];
  ret[1] = g[1];
  ret[2] = g[2];
  ret[3] = .5 * g[3];
  ret[4] = .5 * g[4];
  ret[5] = .5 * g[5];
 
  return ret;
}

#endif
