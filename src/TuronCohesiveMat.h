/*
 *  Copyright (C) 2008 TU Delft. All rights reserved.
 *  
 *  This class implements a mixed mode traction-seperation law
 *  for cohesive crack models with dummy stiffness (Turon et al. 2006). 
 *  For use in InterfaceModel.
 *  
 *  Author: F.P. van der Meer
 *  Date: June 2008
 *
 */


#ifndef TURON_COHESIVE_MATERIAL_H 
#define TURON_COHESIVE_MATERIAL_H

#include <jem/base/String.h>
#include <jem/util/Flex.h>

#include "CohesiveMat.h"

using jem::idx_t;
using jem::String;
using jem::util::Flex;


// =======================================================
//  class TuronCohesiveMat
// =======================================================


class TuronCohesiveMat : public virtual CohesiveMat
{
 public:

  static const char*      F2T_PROP;
  static const char*      F6_PROP;
  static const char*      G_I_PROP;
  static const char*      G_II_PROP;
  static const char*      DUMMY_PROP;
  static const char*      ETA_PROP;
  static const double     MIN_SECANT;
  static const double     OMEGA_MAX;
  static const double     EPS;

  /*
   *  constructor
   */

  explicit                TuronCohesiveMat

    ( const idx_t           rank,
      const Properties&     globdat );

  /*
   *  configure and getConfig (from input file)
   */

  virtual void            configure

    ( const Properties&     props,
      const Properties&     globdat );

  virtual void            getConfig

    ( const Properties&     conf,
      const Properties&     globdat )         const;

  /*
   *  compute the traction (t) and cohesive tangent stiffness matrix
   *  (stiff) at material point mpoint given the displacement jump (jump)
   *   jump[0] = crack opening displacement
   *   jump[1] = crack sliding displacement
   */

  virtual void            update

    ( Vector&               traction,
      Matrix&               stiff,
      const Vector&         jump,
      idx_t                 mpoint );

  virtual void            elasticUpdate

    ( Vector&               traction,
      Matrix&               stiff,
      const Vector&         jump );

  /*
   *  Called when the Newton Raphson converged
   *  Swap newHist_ to oldHist_ 
   */

  virtual void            commit           ();


  /**
   *  Allocate for history variables if it is not yet 
   *  initialized. Otherwise, extend it by appending at the end.
   *  Initial value of damage is optional
   */

  virtual void            allocPoints

    ( idx_t                 count,
      double                dam = 0. );

  virtual void            deallocPoints

    ( idx_t                 count );

  /*
   *  Return history variables at material point ipoint
   */

  inline double           giveDissipation   ( idx_t ipoint ) const;

  inline double           giveHistory       ( idx_t ipoint ) const;

  inline double           getModeIIDiss     ( idx_t ipoint ) const;

  inline idx_t            isLoading         ( idx_t ipoint ) const;

  virtual idx_t           wasLoading        ( idx_t ipoint ) const;

  inline idx_t            loadingCount      ()             const;

  /*
   * Return number of integration points
   */

  virtual idx_t           ipointCount  (  ) const
  {
    return preHist_.size ( ) ;
  }

  /*
   * Get dummy stiffness
   */

  inline double           getDummy () const;

 protected:

  virtual                ~TuronCohesiveMat   ();

  double                  getDelta0_ ( const double beta ) const;

  void                    allocPoints_

    ( const idx_t           count,
      const double          dam,
      const idx_t           loading );

 protected:

 /*
  * Compute omega from effective traction
  */

  double                  f2t_;
  double                  f6_;
  double                  gI_;
  double                  gII_;
  double                  eta_;

  double                  f2t2_;
  double                  f62_;
  double                  deltaN02_;
  double                  deltaS02_;
  double                  deltaN0F_;
  double                  deltaS0F_;

  double                  dummy_;

  // history variable (equivalent crack opening), involve in time

  class                   Hist_
  {
   public:
                            Hist_ 

      ( const double          dam = 0.,
        const idx_t           loading = 0 );

    double                  damage;     
    idx_t                   loading;     
    double                  dissipation;
    double                  modeIIDiss;
  };

  Flex<Hist_>             preHist_;      // history of the previous load step
  Flex<Hist_>             newHist_;      // history of the current iteration
  Flex<Hist_>*            latestHist_;   // points to latest hist
};

// -------------------------------------------------------------------
//  giveHistory
// -------------------------------------------------------------------

inline double  TuronCohesiveMat::giveHistory ( idx_t ipoint ) const
{
  return (*latestHist_)[ipoint].damage;
}

// -------------------------------------------------------------------
//  isLoading
// -------------------------------------------------------------------

inline idx_t   TuronCohesiveMat::isLoading ( idx_t ipoint ) const
{
  return (*latestHist_)[ipoint].loading;
}

// -------------------------------------------------------------------
//  wasLoading
// -------------------------------------------------------------------

inline idx_t   TuronCohesiveMat::wasLoading ( idx_t ipoint )  const
{
  return preHist_[ipoint].loading;
}

// -------------------------------------------------------------------
//  getModeIIDiss     
// -------------------------------------------------------------------

inline double  TuronCohesiveMat::getModeIIDiss ( idx_t ipoint ) const
{
  return 0.;
  // return (*latestHist_)[ipoint].modeIIDiss;
}

// -------------------------------------------------------------------
//  giveDissipation
// -------------------------------------------------------------------

inline double  TuronCohesiveMat::giveDissipation ( idx_t ipoint ) const
{
  return (*latestHist_)[ipoint].dissipation;
}

// -------------------------------------------------------------------
//  getDummy
// -------------------------------------------------------------------

inline double  TuronCohesiveMat::getDummy ( ) const
{
  return dummy_;
}

// -------------------------------------------------------------------
//  loadingCount
// -------------------------------------------------------------------

inline idx_t TuronCohesiveMat::loadingCount () const
{
  idx_t ret = 0;

  for ( idx_t i = 0; i < preHist_.size(); ++i )
  {
    ret += (*latestHist_)[i].loading;
  }

  return ret;
}

#endif 
