
/*
 * 
 *  Copyright (C) 2007 Habanera. All rights reserved.
 *  
 *  This file is part of Jive, an object oriented toolkit for
 *  solving partial differential equations.
 *  
 *  Please read the file LICENSE included in this distribution for
 *  usage and distribution terms.
 *  
 *  For more information contact Habanera at http://www.habanera.nl.
 * 
 *  Frans van der Meer, March 2008
 *
 */

#ifndef JIVE_IMPLICT_XARCLENMODULE_H
#define JIVE_IMPLICT_XARCLENMODULE_H

#include <jive/implict/SolverModule.h>


JIVE_BEGIN_PACKAGE( implict )


//-----------------------------------------------------------------------
//   class XArclenModule
//-----------------------------------------------------------------------


class XArclenModule : public SolverModule
{
 public:

  JEM_DECLARE_CLASS       ( XArclenModule, SolverModule );

  static const idx_t        DELTA_CONS;

  static const char*        TYPE_NAME;
  static const char*        MODEL_PROP;
  static const char*        MAX_ITER_PROP;
  static const char*        PRECISION_PROP;
  static const char*        DELTA_CONS_PROP;
  static const char*        SOLVER_PROP;
  static const char*        DESPAIR_PROP;
  static const char*        MAX_ITER2_PROP;
  static const char*        NHOPE_PROP;
  static const char*        SWT_RESIDUAL_PROP;


  explicit                  XArclenModule

    ( const String&           name = "arclen" );

  virtual Status            init

    ( const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );

  virtual void              shutdown

    ( const Properties&       globdat );

  virtual void              configure

    ( const Properties&       props,
      const Properties&       globdat );

  virtual void              getConfig

    ( const Properties&       props,
      const Properties&       globdat )      const;

  virtual void              advance

    ( const Properties&       globdat );

  virtual void              solve

    ( const Properties&       info,
      const Properties&       globdat );

  virtual void              cancel

    ( const Properties&       globdat );

  virtual bool              commit

    ( const Properties&       globdat );

  virtual void              setPrecision

    ( double                  eps );

  virtual double            getPrecision  () const;

  void                      setOptions

    ( idx_t                   options );

  idx_t                     getOptions    () const;

  inline void              setSwtResidual

    ( double                  val );

  virtual void              setLoadScale  

    ( double                  val );

  inline double             getSwtResidual () const;
  
  static Ref<Module>        makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat);

  static void               declare       ();


 protected:

  virtual                  ~XArclenModule  ();


 private:

  class                     Work_;
  class                     RunData_;

  friend class              Work_;
  friend class              RunData_;


  bool                      solve_

    ( Work_&                  work,
      const Properties&       globdat );


 private:

  idx_t                     maxIter_;
  idx_t                     options_;
  double                    precision_;

  Ref<RunData_>             rundat_;

  // flag that has to be turned on to use desperateMode

  bool                      allowDespair_;

  // for statistics 

  idx_t                     nDespair_;
  idx_t                     nDSucces_;
  idx_t                     nDFailIn_;
  idx_t                     nDFailAfter_;
  idx_t                     nDMaxIter2_;
  idx_t                     nDDirect_;

  idx_t                     nIterD_;
  idx_t                     nIterTot_;

  idx_t                     nLoadSwitch_;

  // number of iterations in desperateMode

  idx_t                     maxIter2_;

  // number of converging iterations to leave desperateMode
  
  idx_t                     nHope_;

  // if residual scale factor > swtResidual * old factor, 
  // try to switch to load/disp control
  
  double                    swtResidual_;
};


//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getSwtResidual 
//-----------------------------------------------------------------------

inline double XArclenModule::getSwtResidual () const
{
  return swtResidual_;
}

//-----------------------------------------------------------------------
//   setSwtResidual 
//-----------------------------------------------------------------------

inline void   XArclenModule::setSwtResidual 

  ( double val ) 

{
  swtResidual_ = val;
}


JIVE_END_PACKAGE( implict )

#endif
