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

#ifndef NEOHOOKEMATERIAL_H
#define NEOHOOKEMATERIAL_H

#include <jem/base/String.h>
#include <jem/util/Flex.h>

#include "NLMaterial.h"


using jem::String;
using jem::Tuple;
using jem::util::Flex;


// =======================================================
//  class NeoHookeNLMaterial
// =======================================================

class NeoHookeNLMaterial : public NLMaterial
{
 public:

  static const char*      LAMBDA_PROP;
  static const char*      MU_PROP;
  static const char*      RHO_PROP;
  static const char*      STATE_PROP;
  static const char*      KAPPA0_PROP;
  static const char*      KAPPAU_PROP;

  enum ProblemType {
    PlaneStrain,
    PlaneStress,
    AxiSymmetric
  };

  explicit                NeoHookeNLMaterial

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

  virtual                ~NeoHookeNLMaterial   ();

 protected:

  double                  mu_;
  double                  lambda_;
  double                  kappa0_; 
  double                  kappau_;
 
  String                  stateString_;
  ProblemType             state_;

  class                 Hist_
  {
   public:
    Hist_();

    double                damagem;
    idx_t                 loadingm;

  };
  
  Flex<Hist_>             preHist_;
  Flex<Hist_>             newHist_;

};


#endif 
