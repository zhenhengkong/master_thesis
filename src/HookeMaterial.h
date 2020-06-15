/*
 * 
 *  Copyright (C) 2007 TU Delft. All rights reserved.
 *  
 *  This class implemens the isotropic linear elastic material
 *  
 *  Author: V.P. Nguyen, V.P.Nguyen@tudelft.nl
 *  Date: 30 November 2007
 *
 */

#ifndef HOOKEMATERIAL_H
#define HOOKEMATERIAL_H

#include <jem/base/String.h>

#include "Material.h"


using jem::String;
using jem::Tuple;


enum ProblemType {
  PlaneStrain,
  PlaneStress,
  AxiSymmetric
};

// =======================================================
//  class HookeMaterial
// =======================================================

// This class implements an isotropic elastic material

class HookeMaterial : public Material
{
 public:

  static const char*      YOUNG_PROP;
  static const char*      POISSON_PROP;
  static const char*      RHO_PROP;
  static const char*      STATE_PROP;
  static const char*      ALPHA_PROP;

  explicit                HookeMaterial

    ( idx_t                 rank,
      const Properties&     globdat );

  virtual void            configure

    ( const Properties&     props );

  virtual void            getConfig

    ( const Properties&     conf )         const;

  virtual void            update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      idx_t                 ipoint );

  virtual void            updateThermStrain

    ( const Properties&     conf );

  Ref<Material>           clone           () const;

  inline double           giveYoung       () const;
  inline double           givePoisson     () const;
  inline double           giveRho         () const;
  inline idx_t            giveDim         () const;
  inline ProblemType      giveState       () const;

  Tuple<double,6>         fill3DStrain

    ( const Vector&         v3 )             const;

  Tuple<double,6>         fill3DStress

    ( const Vector&         v3 )             const;

  void                    reduce3DStrainGrad

    ( const Vector&          v3,
      const Tuple<double,6>& v6 )            const;

  void                    reduce3DStressGrad

    ( const Vector&          v3,
      const Tuple<double,6>& v6 )            const;

  void                    reduce3DVector

    ( const Vector&          vector,
      const Tuple<double,6>& tuple )         const;

  void                    reduce3DMatrix

    ( const Matrix&            matrix,
      const Tuple<double,6,6>& tuple )       const;

  void                    select2DMatrix

    ( const Matrix&            matrix,
      const Tuple<double,6,6>& tuple )       const;

  void                    selectAxiMatrix

    ( const Matrix&            matrix,
      const Tuple<double,6,6>& tuple )       const;

 protected:

  virtual                ~HookeMaterial   ();

 protected:

  double                  young_;
  double                  poisson_;
  double                  rho_;
  double                  alpha_;

 private:

  void                    computeStiffMat_();
  void                    computeComplMat_();

 protected:

  String                  stateString_;
  ProblemType             state_;
};

//-----------------------------------------------------------------------
//   giveYoung
//-----------------------------------------------------------------------

inline double HookeMaterial::giveYoung  () const
{
  return young_;
}

//-----------------------------------------------------------------------
//   givePoisson
//-----------------------------------------------------------------------

inline double HookeMaterial::givePoisson() const
{
  return poisson_;
}

//-----------------------------------------------------------------------
//   giveRho
//-----------------------------------------------------------------------

inline double HookeMaterial::giveRho() const
{
  return rho_;
}

//-----------------------------------------------------------------------
//   giveDim
//-----------------------------------------------------------------------

inline idx_t  HookeMaterial::giveDim() const
{
  return rank_;
}

//-----------------------------------------------------------------------
//   giveState
//-----------------------------------------------------------------------

inline ProblemType   HookeMaterial::giveState() const
{
  return state_;
}

#endif 
