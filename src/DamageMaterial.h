/*
 * 
 *  Copyright (C) 2007 TU Delft. All rights reserved.
 *  
 *  This class implemens the isotropic elastic-damage material
 *  This represents the material at a point in space.
 *  It is implemented in such a way that can be used for any
 *  discretisation methods, say finite elements, EFG and so on.
 *  
 *  Author: V.P. Nguyen, V.P.Nguyen@tudelft.nl
 *  Date: 30 November 2007
 *
 */

#ifndef DAMAGEMATERIAL_H
#define DAMAGEMATERIAL_H

#include <jive/Array.h>
#include <jem/base/String.h>
#include <jem/util/Flex.h>
#include <vector>

#include "HookeMaterial.h"
#include "Softening.h"
#include "VonMisesStrain.h"


using jem::String;
using jem::util::Flex;
using jive::Vector;
using jive::IntVector;
using std::vector;


enum SofteningLaw {
  Perfect,
  Linear,
  Exponential3Params,
  Exponential2Params,
  Exponential2ParamsReg,
  Hyperbolic,
  Power,
  ExpoEnergy
};

enum EquiStrainDef {
  Mazars,
  VonMises,
  Rankine,
  StrainEnergy,
};

//-----------------------------------------------------------------------
//   class ElastoDamageMaterial
//-----------------------------------------------------------------------

/*  The IsoDamageMaterial implements an isotropic linear elastic based damage
 *  material: sigma = (1-omega):D:epsilon
 *  
 * Start: 18 August 2007:
 * 
 * 1. Softening laws implemented:
 *  + Perfect softening (16/10/2007)
 *  + Linear softening
 *  + Modified power softening
 *  + Exponential softening
 *  + Exponential softening
 *  + Hyperbolic softening
 *  + Exponential softening
 *  + Exponential energy softening (German, IJNME 07)
 * 2. Equivalent strain definition
 *  + Positive strain or Mazars
 *  + Modified von Mises
 *  + Rankine-type (actually stress-based Mazars)
 * 3. Energy-based regularised softening law for exponential law done
 *  + Milan Jirasek's version with Gf
 *  + de Vree's version with crack band with (solution depends on this param!!!)
 */

class DamageMaterial : public Material, public Softening
{
 public:

  typedef  DamageMaterial Self;

  static const char*      SOFTENING_PROP;
  static const char*      EQUISTRAIN_PROP;

  static const char*      KAPPAI_PROP;
  static const char*      KAPPAC_PROP;
  static const char*      ALPHA_PROP;
  static const char*      BETA_PROP;
  static const char*      ETA_PROP;
  static const char*      B_PROP;
  static const char*      LENGTH_PROP;

  static const char*      TENSILE_PROP;
  static const char*      COMPRESSIVE_PROP;
  static const char*      FRACTURE_ENERGY_PROP;
  static const char*      RATE_FACTOR_PROP;

  static const char*      LAMBDA_PROP;

  static const char*      MAZARS_EQUI_STRAIN;
  static const char*      MISES_EQUI_STRAIN;
  static const char*      RANKINE_EQUI_STRAIN;
  static const char*      ENERGY_STRAIN;

  static const char*      PERFECT_SOFTENING;
  static const char*      LINEAR_SOFTENING;
  static const char*      EXPONENT1_SOFTENING; 
  static const char*      EXPONENT2_SOFTENING;
  static const char*      EXPONENT3_SOFTENING;
  static const char*      POWER_SOFTENING;
  static const char*      HYPERBOLIC_SOFTENING;
  static const char*      EXPOENERGY_SOFTENING;

  static const double     CRITICAL_DAMAGE;

  static vector<String>   equiStrainDefs;
  static vector<String>   softeningLawDefs;

  explicit                DamageMaterial

    ( idx_t                 rank,
      const Properties&     globdat );

  virtual void            configure

    ( const Properties&     props );

  virtual void            getConfig

    ( const Properties&     conf )         const;

  // update the stress, the reduced elastic modulii
  // given the updated strain at GP ipoint

  virtual void            update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      idx_t                 ipoint );

  // the same as above but overloaded for mesh adjusted damage model
  // he is the element characteristic element length 

  virtual void            update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      idx_t                 ipoint,
      double                he );

  /*
   *  Called when the Newton Raphson converged
   *  Swap newHist_ to oldHist_ 
   */

  virtual void            commit           ();

  virtual Ref<Material>   clone () const;

  virtual void            allocPoints

    ( idx_t                 ipCount );

  // ----------------------------------------------------------
  //   return the history variables
  // ----------------------------------------------------------
  
  inline double           giveHistory      ( idx_t ipoint ) const;
  virtual void            getHistory

    ( const Vector&         hvals,
      const idx_t           mpoint ) const;

  inline double           giveDissipation  ( idx_t ipoint ) const;

  inline double           giveLengthScale  ()             const;
  inline double           giveRho          ()             const;

  // compute the equivalent strain given a strain vector

  double                  getEquiStrain

    ( const Vector& strain );

  // compute derivatives of equivalent strain w.r.t strain
  // vector

  void                    getdEquidEps

    ( const Vector& debde,
      const Vector& strain );

  Matrix                  giveElasticMatrix ()const;

  idx_t                   isLoading         ( idx_t ip )    const;

  // compute the derivative of Omega w.r.t kappa

  double                  getdOmegadKappa

    ( double kappa )                                      const;

  double                  getdOmegadKappa

    ( double kappa, double he )                           const;

  /*
   * return true if the material point mpoint is completely damaged
   */
 
  bool                    isFullyDamaged     ( idx_t mpoint ) const;
 
  Tuple<double,6>         fill3DStrain

    ( const Vector&         v3 )              const;

  Tuple<double,6>         fill3DStress

    ( const Vector&         v3 )              const;

  double                  maxAllowedLength () const;

 protected:

  virtual                ~DamageMaterial   ();

 private:

  void                    setMaxLe_        ();

  double                  getKappa_

    (       double&         dkde,
            idx_t&          load,
      const double          eqstrain,
      const idx_t           ipoint ) const;

  double                  updateKappa_ 

    (       double&         dkde,
      const double          eqstrain,
      const double          kappa0 ) const; 

  // compute the damage variable Omega from kappa

  double                  damageEvolution_

    ( double kappa )                 const;

  // overloaded version for mesh adjusted damage model

  double                  damageEvolution_

    ( double kappa,
      double he )                    const;

  // ---------------------------------------------------------------
  //  compute the derivatives of equivalent strain
  //  w.r.t the strain vector
  //  Mazars formulas
  // ---------------------------------------------------------------

  void                    getdEquidEpsMazars_

    ( const Vector&   debde,
      const Vector&   strain );


  // ---------------------------------------------------------------
  //  compute the derivatives of equivalent strain
  //  w.r.t the strain vector
  //  Rankine formulas
  // ---------------------------------------------------------------

  void                    getdEquidEpsRankine_

   ( const Vector&   debde,
     const Vector&   stress,
     const double    young,
     const Matrix&   De );

  
 private:

  // parameters for softening and equivalent strain def

  SofteningLaw            softening_;
  EquiStrainDef           equiStrain_;

  double                  kappaI_;    // damage threshold
  double                  kappaC_;    // softening modulus
  double                  alpha_;
  double                  beta_;
  double                  b_;
  double                  s_;

  double                  eta_;

  double                  c_;

  double                  ft_;        // tensile strength
  double                  fc_;        // compressive strength
  double                  gf_;        // fracture energy

  double                  lambda_;    // crack band width 
  double                  rateFac_;   // rate-dependent damage
  double                  irfac_;     // rateFac_^{-1}

  Ref<HookeMaterial>      elasticMat_;
  Matrix                  elasticMod_;

  VonMisesStrain          vonMisesEqv_;

  // history variables

  class                  Hist_
  {
   public:
    Hist_();
    void toVector ( const Vector& vec ) const;

    double                kappa  ;     // non-local equivalent strain
    idx_t                 loading;     // dk/deqveps: loading/unloading index 
    double                dissipation; // dissipation
  };

  Flex<Hist_>             preHist_;    // history of the previous load step
  Flex<Hist_>             newHist_;    // history of the current iteration
  Flex<Hist_>*            latestHist_;
};

// -------------------------------------------------------------------
//  giveHistory
// -------------------------------------------------------------------

inline double  DamageMaterial::giveHistory ( idx_t ipoint ) const
{
  return newHist_[ipoint].kappa;
}

// -------------------------------------------------------------------
//  giveDissipation
// -------------------------------------------------------------------

inline double  DamageMaterial::giveDissipation ( idx_t ipoint ) const
{
  return (*latestHist_)[ipoint].dissipation;
}

// -------------------------------------------------------------------
//  isLoading
// -------------------------------------------------------------------

inline idx_t  DamageMaterial::isLoading ( idx_t ipoint ) const
{
  return newHist_[ipoint].loading;
}

// -------------------------------------------------------------------
//  giveLengthScale
// -------------------------------------------------------------------

inline double  DamageMaterial::giveLengthScale   ( ) const
{
  return c_;
}

// -------------------------------------------------------------------
//  giveRho
// -------------------------------------------------------------------

inline double  DamageMaterial::giveRho           ( ) const
{
  return elasticMat_->giveRho ();
}

// -------------------------------------------------------------------
//  Hist_::toVector
// -------------------------------------------------------------------

inline void DamageMaterial::Hist_::toVector

 ( const Vector&  vec ) const

{
  vec[0] = kappa;
  vec[1] = double(loading);
  vec[2] = dissipation;
}

inline Tuple<double,6> DamageMaterial::fill3DStrain

  ( const Vector&         v3 ) const

{
  return elasticMat_->fill3DStrain ( v3 );
}

inline Tuple<double,6> DamageMaterial::fill3DStress

  ( const Vector&         v3 ) const

{
  return elasticMat_->fill3DStress ( v3 );
}

// *********************************************
//    
//    definition of some helper functions
//    
// ********************************************

// ----------------------------------------------------------------------
//  perfect softening 
// ----------------------------------------------------------------------

double                  perfectSoftening

  ( const double Ki,
    const double K );

double                  getDerivPerfectSoftening

    ( const double Ki,
      const double K );

// ----------------------------------------------------------------------
//  linear softening 
// ----------------------------------------------------------------------

double                  linearSoftening

  ( const double Ki,
    const double Kc,
    const double K );

double                  getDerivLinearSoftening

    ( const double Ki,
      const double Kc,
      const double K );
// ----------------------------------------------------------------------
//  linear softening (overloaded)
// ----------------------------------------------------------------------

double                  linearSoftening

  ( const double Ki,
    const double Gf,
    const double K,
    const double Le );

double                  getDerivLinearSoftening

    ( const double Ki,
      const double Gf,
      const double K,
      const double Le );

// ----------------------------------------------------------------------
//  exponential softening1 ( Ki, alpha, beta, K) 
// ----------------------------------------------------------------------

double                  exponentialSoftening1

  ( const double Ki,
    const double alpha,
    const double beta,
    const double K );

double                  getDerivExponentialSoftening1

  ( const double Ki,
    const double alpha,
    const double beta,
    const double K );

// overloaded for mesh adjusted softenting 

double                  exponentialSoftening1

  ( const double Ki,
    const double alpha,
    const double gf, 
    const double ft,
    const double K, 
    const double he );

double                  getDerivExponentialSoftening1

  ( const double Ki,
    const double alpha,
    const double gf, 
    const double ft,
    const double K,
    const double he );

// ----------------------------------------------------------------------
//  exponential softening2 
// ----------------------------------------------------------------------2

double                  exponentialSoftening2

  ( const double Ki,
    const double Kc,
    const double K );

double                  getDerivExponentialSoftening2

    ( const double Ki,
      const double Kc,
      const double K );

// overloaded for mesh adjusted softening modulus

double                  exponentialSoftening2

  ( const double Ki, const double Kc,
    const double K, const double lamda,
    const double he );

double                  getDerivExponentialSoftening2

    ( const double Ki, const double Kc,
      const double K,  const double lamda,
      const double he );

// Jirasek regularised exponential softening law

double                  exponentialSoftening3

  ( const double Ki, const double K,
    const double gf, const double ft, 
    const double he );

double                  getDerivExponentialSoftening3

   ( const double Ki, const double K,
     const double gf, const double ft, 
     const double he );


// ----------------------------------------------------------------------
//  power softening 
// ----------------------------------------------------------------------

double                  powerSoftening

  ( const double Ki,
    const double Kc,
    const double alpha,
    const double beta,
    const double K );

double                  getDerivPowerSoftening

  ( const double Ki,
    const double Kc,
    const double alpha,
    const double beta,
    const double K );

// ----------------------------------------------------------------------
//  hyperbolic softening 
// ----------------------------------------------------------------------

double                  hyperbolicSoftening

  ( const double Ki,
    const double b,
    const double K );

double                  getDerivHyperbolicSoftening

  ( const double Ki,
    const double b,
    const double K );


// ----------------------------------------------------------------------
//  exponential strain energy softening 
// ----------------------------------------------------------------------

double                     expoEnergySoftening

 ( const double kappa, const double kappaI,
   const double kappaC, const double s );

double                     getDerivExpoEnergySoftening

 ( const double kappa, const double kappaI,
   const double kappaC, const double s );

// ----------------------------------------------------------------------
//  equivalent strain definition of Rankine
// ----------------------------------------------------------------------

double                  equiStrainRankine

  ( const double           young,
    const Tuple<double,3>& princ );

#endif
