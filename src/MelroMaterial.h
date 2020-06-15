/*
 * 
 *  Copyright (C) 2014 TU Delft. All rights reserved.
 *  
 *  This class implements the material model for polymers
 *  from Melro et al. (2013)
 *  
 *  Author: F.P. van der Meer, f.p.vandermeer@tudelft.nl
 *  Date: October 2014
 *
 */

#ifndef MELRO_MATERIAL_H
#define MELRO_MATERIAL_H

#include <jem/base/Exception.h>
#include "jem/numeric/func/Function.h"
#include "jem/util/Flex.h"

#include "Plasticity.h"
#include "HookeMaterial.h"
#include "Invariants.h"

using jem::Exception;
using jem::numeric::Function;
using jem::util::Flex;

// =======================================================
//  class MelroMaterial
// =======================================================

class MelroMaterial : public HookeMaterial,
                      public Plasticity
{
 public:

  typedef MelroMaterial   Self;
  typedef HookeMaterial   Super;

  explicit                MelroMaterial

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

  virtual void            getDissipationStress
    
    ( const Vector&         sstar,
      const Vector&         strain,
      const idx_t           ipoint ) const;

  void                    commit ();

  virtual Ref<Material>   clone () const;

  void                    setHistory

    ( const Vec6&           epsp,
      const double          epspeq,
      const idx_t           ipoint );

  virtual void            getHistory

    ( const Vector&         hvals,
      const idx_t           mpoint ) const;

  virtual double          giveHistory     ( const idx_t ipoint ) const;

  virtual double          giveDissipation ( const idx_t point  ) const;

  virtual void            allocPoints

    ( const idx_t           count );

  inline virtual idx_t    pointCount () const;

  inline virtual void     getThermStrain  
    
    ( const Vector&         eth ) const;

  inline virtual idx_t    isLoading         ( idx_t point  ) const;

  inline virtual idx_t    wasLoading        ( idx_t point  ) const;

  Vec6                    tmatmul

    ( const Matrix&         mat,
      const Vec6&           tuple ) const;

  inline void             t6_to_v6

    ( const Vector&         t6,
      const Vec6&           v6 ) const;

  inline void             v6_to_t6

    (       Vec6&           t6,
      const Vector&         v6 ) const;

  inline Vec6             v6_to_t6

    ( const Vector&         v6 ) const;

  Vec6                    deviatoric

    ( const Vec6&           full,
      const double          p ) const;

 protected:

  virtual                ~MelroMaterial   ();

  Ref<Function>           makeFunc_

    ( const Properties&     props,
      const String&         name )         const;

  void                    handleException_ 

    ( const Exception&      ex,
      const Vector&         strain,
      const idx_t           ipoint );

  Tuple<double,6,6>       aI4_plus_bII_ 

    ( const double          a,
      const double          b ) const;

 protected: 

  class                  Hist_
  {
   public:
    Hist_();
    void                   toVector ( const Vector& vec ) const;
    void                   print () const;

    Vec6                   epsp;    // plastic strain
    double                 epspeq;  // equivalent plastic strain
    double                 dissipation;
    bool                   loading;
  };

  class                  YieldFunc_ : public Object
  {
   public:

    YieldFunc_  
      
      ( const double       young,
        const double       poisson,
        const double       poissonP,
        const Ref<Function> sigC,
        const Ref<Function> sigT );

    void                 setRmSettings 

      ( const double       rmTolerance, 
        const idx_t        rmMaxIter );

    bool                 isPlastic

      ( const Invariants&  inv,
        const double       epspeq0);

    double               findRoot 
      
      ( const double       dgam0 );

    void                 findBounds

      (       double&      gmin,
              double&      gmax,
              double&      fmin,
              double&      fmax );

    double               findRoot

      (       double       gmin,
              double       gmax,
              double       fmin,
              double       fmax );

    void                 getTangentParameters 

      (       double&      betam, 
              double&      pbm, 
              double&      hfac, 
              double&      beta, 
              double&      phi, 
              double&      rho, 
              double&      chi, 
              double&      psi, 
              double&      xi, 
              double&      omega, 
        const double       dgam );

    void                 getTangentParameters 

      (       double&      beta, 
              double&      phi, 
              double&      rho, 
              double&      chi, 
              double&      psi ); 

    void                 getPGradientParameters

      (       double&      phiF,
              double&      rhoF,
              double&      chiF,
              double&      psiF ) const;

    void                 resetDGam ();

    inline Ref<Function> getSigmaCFunc () const;
    inline Ref<Function> getSigmaTFunc () const;

    inline double        getAlpha () const;
    inline double        getZetaP () const;
    inline double        getZetaS () const;
    inline double        getEpspeq () const;
    inline double        getK () const;
    inline double        getG () const;

   private:

    void                 setDGam_ 
      
      ( const double       dgam );

    double               estimateRoot_

      ( const double       gmin,
        const double       gmax,
        const double       fmin,
        const double       fmax ) const;

    void                 improveBounds_

      (       double&      gmin,
              double&      gmax,
              double&      fmin,
              double&      fmax );

    double               eval_

      ( const double       dgam );

    double               evalTrial_ () const;

   public:

   private:

    double                   G_;
    double                   K_;
    double                   alpha_;
    double                   alpha2_;
    double                   nuPFac_;
    double                   Ka_;

    Ref<Function>            sigmaC_;
    Ref<Function>            sigmaT_;
    double                   rmTol_;
    idx_t                    maxIter_;

    double                   epspeq0_;
    double                   j2tr_;
    double                   i1tr_;
    double                   j2fac_;
    double                   i1fac_;

    double                   zetaP_;
    double                   zetaS_;
    double                   zS2_;
    double                   zP2_;
    double                   depspeq_;
    double                   epspeq_;
    double                   sigC_;
    double                   sigT_;
    double                   sigct_;
    double                   HC_;
    double                   HT_;
    double                   gjzs3_;
    double                   kaizp2_;
    double                   dcdg_;
  };

  // need own rank, because base rank is always 3, for 3D stiffMat

  idx_t                    melroRank_;

  // input properties

  double                   rmTolerance_;
  double                   poissonP_;
  idx_t                    rmMaxIter_;

  // history 

  Flex<Hist_>              preHist_;
  Flex<Hist_>              newHist_;
  Flex<Hist_>*             latestHist_;

  // hardening functions

  Ref<YieldFunc_>          y_;
  Properties               globdat_;

  // preallocated arrays

  Vector                   v61_;
  Vector                   v62_;
};

inline idx_t MelroMaterial::pointCount () const
{
  return newHist_.size();
}

inline void MelroMaterial::getThermStrain 

  ( const Vector& eth ) const
{
  Vec6 tmp;

  for ( idx_t i = 0; i < 6; ++i ) 
  {
    tmp[i] = thermStrain_[i];
  }

  reduce3DVector ( eth, tmp );
}

inline idx_t MelroMaterial::isLoading ( idx_t ipoint ) const
{
  return newHist_[ipoint].loading;
}

inline idx_t MelroMaterial::wasLoading ( idx_t ipoint ) const
{
  return preHist_[ipoint].loading;
}

inline double MelroMaterial::YieldFunc_::getAlpha() const
{
  return alpha_;
}

inline Ref<Function> MelroMaterial::YieldFunc_::getSigmaCFunc() const
{
  return sigmaC_;
}

inline Ref<Function> MelroMaterial::YieldFunc_::getSigmaTFunc() const
{
  return sigmaT_;
}

inline double MelroMaterial::YieldFunc_::getZetaP() const
{
  return zetaP_;
}

inline double MelroMaterial::YieldFunc_::getZetaS() const
{
  return zetaS_;
}

inline double MelroMaterial::YieldFunc_::getEpspeq() const
{
  return epspeq_;
}

inline double MelroMaterial::YieldFunc_::getK() const
{
  return K_;
}

inline double MelroMaterial::YieldFunc_::getG() const
{
  return G_;
}

void MelroMaterial::v6_to_t6

  (       Vec6&   t6,
    const Vector& v6 ) const

{
  t6[0] = v6[0]; t6[1] = v6[1]; t6[2] = v6[2]; 
  t6[3] = v6[3]; t6[4] = v6[4]; t6[5] = v6[5]; 
}

Vec6 MelroMaterial::v6_to_t6

  ( const Vector& v6 ) const

{
  Vec6 t6;
  v6_to_t6 ( t6, v6 );
  return t6;
}

void MelroMaterial::t6_to_v6

  ( const Vector& v6,
    const Vec6&   t6 ) const

{
  v6[0] = t6[0]; v6[1] = t6[1]; v6[2] = t6[2]; 
  v6[3] = t6[3]; v6[4] = t6[4]; v6[5] = t6[5]; 
}

#endif 
