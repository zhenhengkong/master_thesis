#ifndef NLMATERIAL_H
#define NLMATERIAL_H

#include <jem/base/System.h>
#include <jem/util/Properties.h>
#include <jem/base/array/utilities.h>
#include <jem/base/array/intrinsics.h>
#include <jive/Array.h>
#include <jem/base/Object.h>

using jem::idx_t;
using jem::System;
using jem::Tuple;
using jem::io::endl;
using jem::Ref;
using jem::String;
using jem::Object;
using jem::util::Properties;
using jive::Vector;
using jive::Matrix;
using jive::IntVector;
using jive::BoolVector;
using jive::StringVector;


//-----------------------------------------------------------------------
//   class NLMaterial
//-----------------------------------------------------------------------

// The NLMaterial class represents a material model. Its main task is to
// calculate the strain-stress stiffness matrix.
// It is designed for geometrically nonlinear analysis where material 
// models may also need the deformation gradient
// At least update should be implemented by any derived class

class NLMaterial : public Object
{
 public:

  explicit                NLMaterial

    ( const idx_t           rank,
      const Properties&     globdat );

  virtual void            configure

    ( const Properties&     props );

  virtual void            getConfig

    ( const Properties&     conf )            const;

  virtual void            update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      const Matrix&         defGrad,
      const idx_t           ip ) = 0;

  virtual void            commit ();

  virtual void            cancel ();

  virtual Ref<NLMaterial>   clone () const = 0;

  virtual bool            despair ();

  virtual bool            endDespair ();

  virtual void            allocPoints

    ( const idx_t           count );

  virtual void            allocPoints

    ( const idx_t           count,
      const Matrix&         transfer,
      const IntVector&      oldPoints );

  virtual void            deallocPoints 
    
    ( const idx_t           count );

  idx_t                   getHistoryCount   () const;

  StringVector            getHistoryNames   () const;

  void                    getHistoryNames   
    
    ( const StringVector&   hnames ) const;

  virtual void            getHistory

    ( const Vector&         hvals,
      const idx_t           mpoint ) const;

  inline virtual idx_t    isLoading         ( idx_t point  ) const; 

  inline virtual idx_t    wasLoading        ( idx_t point  ) const;

  inline virtual idx_t    pointCount ()     const;

  bool                    isViscous ()        const;

  void                    setDT

    ( const double          dt );

  inline virtual void     getThermStrain  
    
    ( const Vector&         eth ) const;

  virtual void            updateThermStrain

    ( const Properties&     globdat );

  virtual double          giveDissipation

    ( const idx_t           ipoint ) const;

 protected:

  virtual                ~NLMaterial      ();

 protected:

  idx_t                  rank_;

  Vector                 thermStrain_;
  bool                   desperateMode_;
  BoolVector             hasSwitched_;
  BoolVector             useSecant_;

  bool                   viscous_;
  double                 dt_;

  StringVector           historyNames_;
};

//-----------------------------------------------------------------------
//   implementation
//-----------------------------------------------------------------------


inline idx_t NLMaterial::pointCount () const
{
  return 0;
}

inline idx_t NLMaterial::isLoading ( idx_t ipoint ) const
{
  return false;
}

inline idx_t NLMaterial::wasLoading ( idx_t ipoint ) const
{
  return false;
}

inline void NLMaterial::getThermStrain 

  ( const Vector& eth ) const
{
  eth = thermStrain_;
}

inline bool NLMaterial::isViscous () const
{
  return viscous_;
}

inline void NLMaterial::setDT ( const double dt ) 
{
  dt_ = dt;
}


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


Ref<NLMaterial>             newNLMaterial

  ( const String&           name,
    const Properties&       conf,
    const Properties&       props,
    const Properties&       globdat );

#endif 
