/*
 * 
 *  Copyright (C) 2015 TU Delft. All rights reserved.
 *  
 *  This class implements the material model for composites
 *  from Vogler et al. (2013)
 *  
 *  Author: F.P. van der Meer, f.p.vandermeer@tudelft.nl
 *  Date: April 2014
 *
 */

#ifndef VOGLER_MATERIAL_H
#define VOGLER_MATERIAL_H

#include <jem/base/Tuple.h>
#include <jem/numeric/func/Function.h>
#include <jem/util/Flex.h>
#include <jem/io/Writer.h>

#include "Plasticity.h"
#include "OrthotropicMaterial.h"

using jem::Tuple;
using jem::Array;
using jem::Exception;
using jem::io::Writer;
using jem::numeric::Function;
using jem::util::Flex;

// =======================================================
//  class VoglerMaterial
// =======================================================

class VoglerMaterial : public OrthotropicMaterial,
                       public Plasticity
{
 public:

  typedef VoglerMaterial      Self;
  typedef OrthotropicMaterial Super;
  typedef Tuple<double,3>     Vec3;
  typedef Tuple<double,6>     Vec6;
  typedef Tuple<double,6,6>   Mat66;
  typedef Tuple<double,3,3>   Mat33;

  explicit                VoglerMaterial

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

  virtual void            mUpdate

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

  inline virtual idx_t    isLoading         ( idx_t point  ) const;

  inline virtual idx_t    wasLoading        ( idx_t point  ) const;

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

  virtual                ~VoglerMaterial   ();


  Ref<Function>           makeFunc_

    ( const Properties&     props,
      const String&         name ) const;

  void                    handleException_ 

    ( const Exception&      ex,
      const Vector&         strain,
      const idx_t           ipoint );

  void                    reduce3DMatrix_

    ( const Matrix&         m,
      const Mat66&          t ) const;

  void                    reduce3DVector_

    ( const Vector&         v,
      const Vec6&           t ) const;


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

      ( const Mat66&       dmat,
        const Mat66&       mmat,
        const Array<Ref<Function> > alphas,
        const double       ffac );

    void                 setRmSettings

      ( const double       rmTolerance,
        const idx_t        rmMaxIter );

    bool                 isPlastic

      ( const Vec6&        sigtr,
        const double       epspeq0 );

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

    Mat66                getTangent 
      
      ( const double       dgam ) const;

    double               getEpspeq () const;
    Vec6                 getStress () const;
    Vec6                 getNg () const;

    static Mat66         fillKMat

      ( const double       a1,
        const double       a2,
        const double       a32,
        const double       mfac );

    static Vec6          fillLVec

      ( const double       a3,
        const double       mfac );

    void                 resetDGam ();

    Ref<Writer>          getDbgWriter () const;

   private:

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

    void                 updateDGamDependent_

      ( const double       dgam );

    double               evalTrial_ ();

    double               eval_ 
      
      ( const double       dgam );

    void                 getAlphas_

      (       double&      a1,
              double&      a2,
              double&      a3,
              double&      a32,
        const double       epspeq,
        const Vec6         stress ) const;

    void                 getDAlphas_

      (       double&      da1,
              double&      da2,
              double&      da3,
              double&      da32,
        const double       epspeq,
        const Vec6         stress ) const;

    Mat66                getFMat_

      ( const double       dgam ) const;

    double               vnorm_

      ( const Vec6&        v ) const;

    Vec6                 dvnorm_

      ( const Vec6&        v ) const;

   private:

    // constant members

    Mat66                  dmat_;
    Mat66                  mmat_;
    Mat66                  DM_;
    double                 rmTol_;
    idx_t                  maxIter_;
    double                 mfac_;  // 1.-VoglerMaterial::ffac_

    Ref<Writer>            dbgOut_;

    Ref<Function>          alpha1_;
    Ref<Function>          alpha2_;
    Ref<Function>          alpha3t_;
    Ref<Function>          alpha3c_;
    Ref<Function>          alpha32t_;
    Ref<Function>          alpha32c_;

    // constant inside return mapping

    double                 epspeq0_;
    Vec6                   sigtr_;

    // variables that are a function of dgam

    double                 epspeq_;
    double                 dfdg_;
    double                 dfdep_;
    Vec6                   sig_;
    Mat66                  fmat_;
    Mat66                  kmat_;
    Vec6                   lvec_;
    Vec6                   ng_;
  };

  // need own rank, because base rank is always 3, for 3D stiffMat

  idx_t                    localRank_;
  Matrix                   prealloc_;

  // input properties

  double                   poissonP_;
  double                   rmTolerance_;
  idx_t                    rmMaxIter_;
  double                   ffac_;  // not in Vogler's original model

  // dependent constants

  double                   beta1_;
  double                   beta2_;
  double                   beta32_;
  Mat66                    dmat_;

  // history 

  Flex<Hist_>              preHist_;
  Flex<Hist_>              newHist_;
  Flex<Hist_>*             latestHist_;

  // copy of globdat for hardening functions

  Ref<YieldFunc_>          y_;
  Properties               globdat_;
};

inline idx_t VoglerMaterial::pointCount () const
{
  return newHist_.size();
}

inline idx_t VoglerMaterial::isLoading ( idx_t ipoint ) const
{
  return newHist_[ipoint].loading;
}

inline idx_t VoglerMaterial::wasLoading ( idx_t ipoint ) const
{
  return preHist_[ipoint].loading;
}


void VoglerMaterial::v6_to_t6

  (       Vec6&   t6,
    const Vector& v6 ) const

{
  t6[0] = v6[0]; t6[1] = v6[1]; t6[2] = v6[2]; 
  t6[3] = v6[3]; t6[4] = v6[4]; t6[5] = v6[5]; 
}

Tuple<double,6> VoglerMaterial::v6_to_t6

  ( const Vector& v6 ) const

{
  Vec6 t6;
  v6_to_t6 ( t6, v6 );
  return t6;
}

void VoglerMaterial::t6_to_v6

  ( const Vector& v6,
    const Vec6&   t6 ) const

{
  v6[0] = t6[0]; v6[1] = t6[1]; v6[2] = t6[2]; 
  v6[3] = t6[3]; v6[4] = t6[4]; v6[5] = t6[5]; 
}

#endif 
