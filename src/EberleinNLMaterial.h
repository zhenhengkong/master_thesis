/*
 * 
 *  Copyright (C) 2018 TU Delft. All rights reserved.
 *  
 *  This class implements an anisotropic Eberlein material.
 *  The isotropic NeoHookean part is implemented following
 *  Belytschko Sec. 5.4
 *  
 *  Author: F.P. van der Meer, f.p.vandermeer@tudelft.nl, Dean Howes
 *  Date: January 2018
 *
 */

#ifndef EBERLEINNLMATERIAL_H
#define EBERLEINNLMATERIAL_H

#include <jem/base/String.h>

#include "NLMaterial.h"


using jem::String;
using jem::Tuple;


// =======================================================
//  class EBERLEINNLMaterial
// =======================================================

class EberleinNLMaterial : public NLMaterial
{
 public:

  static const char*      LAMBDA_PROP;
  static const char*      MU_PROP;
  static const char*      BETA_PROP;
  static const char*      C_A_PROP;
  static const char*      K1_A_PROP;
  static const char*      K2_A_PROP;
  static const char*      C_10_PROP;
  static const char*      KAPPA_PROP;
  static const char*      RHO_PROP;
  static const char*      STATE_PROP;


  enum ProblemType {
    PlaneStrain,
    PlaneStress,
    AxiSymmetric
  };

  explicit                EberleinNLMaterial

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
      const Matrix&         defGrad,
      idx_t                 ipoint );

  Ref<NLMaterial>         clone           () const;

 protected:

  virtual                ~EberleinNLMaterial   ();

 protected:

  double                  mu_;
  double                  lambda_;
  double                  beta_;
  double                  c_a_;
  double                  k1_a_;
  double                  k2_a_;
  double                  c_10_;
  double				          kappa_;	

  String                  stateString_;
  ProblemType             state_;
};


#endif 
