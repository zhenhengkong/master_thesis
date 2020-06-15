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

#ifndef NEOHOOKENLMATERIAL_H
#define NEOHOOKENLMATERIAL_H

#include <jem/base/String.h>

#include "Material.h"


using jem::String;
using jem::Tuple;


// =======================================================
//  class NeoHookeMaterial
// =======================================================

// This class implements an isotropic elastic material

class NeoHookeMaterial : public Material
{
 public:

  static const char*      LAMBDA_PROP;
  static const char*      MU_PROP;
  static const char*      RHO_PROP;
  static const char*      STATE_PROP;

  enum ProblemType {
    PlaneStrain,
    PlaneStress,
    AxiSymmetric
  };

  explicit                NeoHookeMaterial

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

  Ref<Material>           clone           () const;

  Tuple<double,6>         fill3DStrain

    ( const Vector&         v3 )             const;

  Tuple<double,6>         fill3DStress

    ( const Vector&         v3 )             const;

 protected:

  virtual                ~NeoHookeMaterial   ();

 protected:

  double                  mu_;
  double                  lambda_;

 private:

  void                    computeStiffMat_();
  void                    computeComplMat_();

 protected:

  String                  stateString_;
  ProblemType             state_;
};


#endif 
