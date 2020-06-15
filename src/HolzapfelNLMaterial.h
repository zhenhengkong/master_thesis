/*
 * 
 *  Copyright (C) 2018 TU Delft. All rights reserved.
 *  
 *  This class implements an anisotropic Holzapfel material.
 *  The isotropic NeoHookean part is implemented following
 *  Belytschko Sec. 5.4
 *  
 *  Author: F.P. van der Meer, f.p.vandermeer@tudelft.nl, Dean Howes
 *  Date: January 2018
 *
 */

#ifndef HOLZAPFELNLMATERIAL_H
#define HOLZAPFELNLMATERIAL_H

#include <jem/base/String.h>
#include <jem/util/Flex.h>

#include "NLMaterial.h"


using jem::String;
using jem::Tuple;
using jem::util::Flex;


// =======================================================
//  class HolzapfelNLMaterial
// =======================================================

class HolzapfelNLMaterial : public NLMaterial
{
 public:

  static const char*      LAMBDA_PROP;
  static const char*      MU_PROP;
  static const char*      BETA_PROP;

  static const char*      K1_A_PROP;
  static const char*      K2_A_PROP;


  static const char*      RHO_PROP;
  static const char*      STATE_PROP;

  static const char*      KAPPAM0_PROP;
  static const char*      KAPPAMU_PROP;

  static const char*      KAPPAF0_PROP;
  static const char*      KAPPAFU_PROP;


  enum ProblemType {
    PlaneStrain,
    PlaneStress,
    AxiSymmetric
  };

  explicit                HolzapfelNLMaterial

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

  virtual void            allocPoints

    ( const idx_t           count );

  Ref<NLMaterial>         clone           () const;

  virtual void            commit ();

 protected:

  virtual                ~HolzapfelNLMaterial   ();

 protected:

  double                  mu_;
  double                  lambda_;
  double                  beta_;

  double                  k1_a_;
  double                  k2_a_;

  double                  kappam0_;
  double                  kappamu_;
 
  double                  kappaf0_;
  double                  kappafu_;

  String                  stateString_;
  ProblemType             state_;

  class                 Hist_
  {
   public:
    Hist_();

    double                damagem;
    double                damagef;
    idx_t                 loadingm;
    idx_t                 loadingf;
  };
  
  Flex<Hist_>             preHist_;
  Flex<Hist_>             newHist_;
};


#endif 
