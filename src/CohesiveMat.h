#ifndef COHESIVE_MATERIAL_H
#define COHESIVE_MATERIAL_H

#include <jive/Array.h>
#include <jem/base/Error.h>
#include <jem/base/Object.h>
#include <jem/util/Properties.h>
#include "Material.h"

using jem::Ref;
using jem::String;
using jive::Vector;
using jive::Matrix;
using jem::Object;
using jem::util::Properties;


//-----------------------------------------------------------------------
//   class CohesiveMat
//-----------------------------------------------------------------------

// The CohesiveMat class represents a cohesive material model. 
// Its main tasks are to calculate the traction and the stiffness matrix.

class CohesiveMat : public Object
{
 public:

  explicit                CohesiveMat

    ( const idx_t           rank,
      const Properties&     globdat );

  virtual void            configure

    ( const Properties&     props,
      const Properties&     globdat );

  virtual void            getConfig

    ( const Properties&     conf,
      const Properties&     globdat )         const;

  virtual void            update

    ( Vector&             traction,
      Matrix&             stiff,
      const Vector&       jump,
      idx_t               mpoint )       = 0;

  virtual void            elasticUpdate

    ( Vector&             traction,
      Matrix&             stiff,
      const Vector&       jump )         = 0;

  virtual void            commit ();

  virtual void            cancel ();

  virtual bool            despair ();

  virtual bool            endDespair ();

  virtual void            allocPoints

    ( idx_t                 count,
      double                dam = 0. );

  virtual void            deallocPoints

    ( idx_t                 count );

  virtual double          giveHistory       ( idx_t point  ) const = 0;

  virtual inline double   giveDissipation   ( idx_t point  ) const;

  virtual inline idx_t    isLoading         ( idx_t point  ) const;

  virtual inline idx_t    wasLoading        ( idx_t point  ) const;

  virtual inline idx_t    loadingCount      ()             const;

  virtual inline idx_t    fullDamCount      ()             const;

//   virtual bool            isFailure
//     
//     ( const Vector&       stress,
//       const Ref<Material> bulkMat ) const;
// 
//   virtual double          evalFailure
//     
//     ( const Vector&       stress,
//       const Ref<Material> bulkMat ) const;

  virtual idx_t           ipointCount () const;

 protected:

  virtual                ~CohesiveMat      ();

 protected:

  idx_t                  rank_;

  bool                   desperateMode_;
  BoolVector             hasSwitched_;
  BoolVector             useSecant_;
};


//-----------------------------------------------------------------------
//   default implementation
//-----------------------------------------------------------------------

inline double  CohesiveMat::giveDissipation ( idx_t ipoint ) const
{
  return 0.;
}

inline idx_t CohesiveMat::isLoading ( idx_t ipoint ) const
{
  return false;
}

inline idx_t CohesiveMat::wasLoading ( idx_t ipoint ) const
{
  return false;
}

inline idx_t CohesiveMat::fullDamCount () const
{
  return -1;
}

inline idx_t CohesiveMat::loadingCount () const
{
  return -1;
}

//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


Ref<CohesiveMat>     newCohesiveMat

  ( const String&           name,
    const Properties&       conf,
    const Properties&       props,
    const Properties&       globdat );

#endif 
