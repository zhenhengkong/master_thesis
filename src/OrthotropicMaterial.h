#ifndef ORTHOTROPICMATERIAL_H
#define ORTHOTROPICMATERIAL_H

#include <jem/base/String.h>
#include <jem/base/System.h>

#include "Material.h"


using jem::String;
using jem::System;
using jem::io::endl;



// =======================================================
//  class OrthotropicMaterial
// =======================================================

// This class implements an orthotropic or
// transversely isotropic elastic material
// Frans van der Meer, February 2008
//
// for transversely isotropic material, input:
//
//   - young1
//   - young2
//   - poisson12
//   - poisson23
//   - shear12
//   - theta
//
// for plane stress, input:
//
//   - young1
//   - young2
//   - poisson12
//   - shear12
//   - theta

class OrthotropicMaterial : public Material
{
 public:

  static const char*      YOUNG_1_PROP;
  static const char*      YOUNG_2_PROP;
  static const char*      YOUNG_3_PROP;
  static const char*      ALPHA_1_PROP;
  static const char*      ALPHA_2_PROP;
  static const char*      ALPHA_3_PROP;
  static const char*      POISSON_12_PROP;
  static const char*      POISSON_23_PROP;
  static const char*      POISSON_31_PROP;
  static const char*      SHEAR_12_PROP;
  static const char*      SHEAR_23_PROP;
  static const char*      SHEAR_31_PROP;
  static const char*      STATE_PROP;
  static const char*      THETA_PROP;


  explicit                OrthotropicMaterial

    ( const idx_t           rank,
      const Properties&     globdat );

                         ~OrthotropicMaterial ();

  virtual void            configure

    ( const Properties&     props );

  virtual void            getConfig

    ( const Properties&     conf ) const;

  virtual void            elasticUpdate

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain );

  void                    transform

    ( const Vector&         strain,
      Vector&               mStrain) const;

  void                    transformInv

    ( const Vector&         mStress,
      Vector&               stress) const;

  void                    changeFrame

    ( const Matrix&         mStiff,
      Matrix&               stiff) const;

  virtual  void           mUpdate

    ( Vector&               mStress,
      Matrix&               mStiff,
      const Vector&         mStrain,
      const idx_t           ipoint );

  virtual   void          update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain, 
      const idx_t           ipoint );

  Ref<Material>           clone           () const;

  virtual Vector          mGiveStressAtPoint 

    ( const idx_t           ipoint,
      const Vector&         mStrain,
      const double          he ) const;

  virtual    Vector       giveStressAtPoint 

    ( const idx_t           ipoint,
      const Vector&         strain,
      const double          he ) const;

  virtual void            allocPoints     
    
    ( const idx_t           count );

  virtual void            allocPoints

    ( const idx_t           count,
      const Matrix&         transfer,
      const IntVector&      oldPoints );

  inline virtual idx_t    pointCount      () const;

  Matrix                  getStiffMat     () const;
  Matrix                  getTransformMat () const;
  Matrix                  getTransformMatInv () const;

  inline double           giveYoung1      () const;
  inline double           giveYoung2      () const;
  inline double           giveYoung3      () const;
  inline double           givePoisson12   () const;
  inline double           givePoisson23   () const;
  inline double           givePoisson31   () const;
  inline double           giveShear12     () const;
  inline double           giveShear23     () const;
  inline double           giveShear31     () const;
  inline double           giveRho         () const;
  inline double           giveTheta       () const;
  inline idx_t            giveDim         () const;
  inline String           giveState       () const;

//   inline double           maxAllowedLength () const;

//   double                  maxLe_;     // maximum characteristic length

  // Given the engineering strain vector (compute from FE)
  // compute the normal strain vector (divide by two shear
  // strains) including add eps_zz
  // in case of plane stress problem
  
  Vector                  giveStrainVector

    ( const Vector&         strain )          const;


  // Given the stress vector computed from FE, build the
  // new one, need in case of plane strain problem (sigma_zz != 0)
  
  Vector                  giveStressVector

    ( const Vector&         strain )          const;

  //-----------------------------------------------------------------------
  //   give number of components returned by getPrincipalStress
  //-----------------------------------------------------------------------

  virtual idx_t           getNPrincipal   () const;

  //-----------------------------------------------------------------------
  //   compute stress in material axes
  //-----------------------------------------------------------------------

  Vector                  getPrincipalStress

    ( const Vector&      stress )            const;

  //-----------------------------------------------------------------------
  //   compute sigma*n where n is perpendicular to fiber
  //-----------------------------------------------------------------------

  Vector                  getFailureStress

    ( const Vector&      stress )            const;

  //-----------------------------------------------------------------------
  //   get stiffness matrix in material frame
  //-----------------------------------------------------------------------
  
  void                    getMaterialStiffMat     
    
    ( const Matrix&      stiff ) const;

  //-----------------------------------------------------------------------
  //   get compliance matrix in material frame
  //-----------------------------------------------------------------------
  
  void                    getMaterialComplMat

    ( const Matrix&      compm ) const;

  //-----------------------------------------------------------------------
  //   update thermal strain
  //-----------------------------------------------------------------------
  
  virtual void            updateThermStrain

   ( const Properties&   params );

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
      const Vector&          vec6 )          const;

  void                    reduce3DMatrix

    ( const Matrix&            matrix,
      const Matrix&            mat66 )       const;

  void                    select2DMatrix

    ( const Matrix&            matrix,
      const Matrix&            mat66 )       const;

 protected:

  double                  young1_;
  double                  young2_;
  double                  young3_;
  double                  alpha1_;
  double                  alpha2_;
  double                  alpha3_;
  double                  poisson12_;
  double                  poisson23_;
  double                  poisson31_;
  double                  shear12_;
  double                  shear23_;
  double                  shear31_;
  double                  theta_;
  Matrix                  stiffMat_;
  Matrix                  materialStiffMat_;
  Matrix                  materialCompMat_;
  Matrix                  transformMat_;
  Matrix                  transformMatInv_;
  Matrix                  tt_;
  String                  state_;
  IntVector               iNormal_;
  Matrix                  preallocated_;
  Matrix                  tmp66_;

  
 private:

  void                    computeStiffMat_();
  void                    computeTransformMats_();
  
 private:

  idx_t                   pCount_;

};

//-----------------------------------------------------------------------
//   pointCount
//-----------------------------------------------------------------------

inline idx_t  OrthotropicMaterial::pointCount  () const

{
  return pCount_;
}

//-----------------------------------------------------------------------
//   giveYoung1
//-----------------------------------------------------------------------

inline double OrthotropicMaterial::giveYoung1  () const
{
  return young1_;
}

//-----------------------------------------------------------------------
//   giveYoung2
//-----------------------------------------------------------------------

inline double OrthotropicMaterial::giveYoung2  () const
{
  return young2_;
}

//-----------------------------------------------------------------------
//   giveYoung3
//-----------------------------------------------------------------------

inline double OrthotropicMaterial::giveYoung3  () const
{
  return young3_;
}

//-----------------------------------------------------------------------
//   givePoisson12
//-----------------------------------------------------------------------

inline double OrthotropicMaterial::givePoisson12() const
{
  return poisson12_;
}

//-----------------------------------------------------------------------
//   givePoisson23
//-----------------------------------------------------------------------

inline double OrthotropicMaterial::givePoisson23() const
{
  return poisson23_;
}

//-----------------------------------------------------------------------
//   givePoisson31
//-----------------------------------------------------------------------

inline double OrthotropicMaterial::givePoisson31() const
{
  return poisson31_;
}

//-----------------------------------------------------------------------
//   giveShear12
//-----------------------------------------------------------------------

inline double OrthotropicMaterial::giveShear12() const
{
  return shear12_;
}

//-----------------------------------------------------------------------
//   giveShear23
//-----------------------------------------------------------------------

inline double OrthotropicMaterial::giveShear23() const
{
  return shear23_;
}

//-----------------------------------------------------------------------
//   giveShear31
//-----------------------------------------------------------------------

inline double OrthotropicMaterial::giveShear31() const
{
  return shear31_;
}

//-----------------------------------------------------------------------
//   giveTheta
//-----------------------------------------------------------------------

inline double OrthotropicMaterial::giveTheta() const
{
  return theta_;
}

//-----------------------------------------------------------------------
//   giveDim
//-----------------------------------------------------------------------

inline idx_t  OrthotropicMaterial::giveDim() const
{
  return rank_;
}

//-----------------------------------------------------------------------
//   giveState
//-----------------------------------------------------------------------

inline String    OrthotropicMaterial::giveState() const
{
  return state_;
}

//-----------------------------------------------------------------------
//   maxAllowedLength 
//-----------------------------------------------------------------------

// inline double OrthotropicMaterial::maxAllowedLength () const

// {
  // NB: this is the same for linear and exponential softening! 

//   return 0.99 * maxLe_;
// }  

#endif 
