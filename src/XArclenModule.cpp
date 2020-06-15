
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
 *  F.P. van der Meer, April 2008 - Jan 2009
 *  - Added 1. as minimum residual scale factor, because i had some problems
 *    with immediately converging analyses.
 *  - Changed output format in reportProgress
 *  - When high residual scale factor: try to switch to load control
 *    (this is for use with EnergyArclenModel)
 *  - Included desperate mode switch when Newton-Raphson oscillates:
 *    - use secant stiffness in integration points that are switching 
 *      between loading/unloading 
 *    - turn of when residual decreases in a series of iterations
 *  - Added setLoadScale function (also added in canonical ArclenModule?)
 *
 */


#include <jem/base/limits.h>
#include <jem/base/Float.h>
#include <jem/base/Array.h>
#include <jem/base/System.h>
#include <jem/base/ClassTemplate.h>
#include <jem/base/ArithmeticException.h>
#include <jem/base/IllegalInputException.h>
#include <jem/base/IllegalOperationException.h>
#include <jem/base/array/operators.h>
#include <jem/io/Writer.h>
#include <jem/util/None.h>
#include <jem/util/Flex.h>
#include <jem/numeric/algebra/utilities.h>
#include <jive/util/utilities.h>
#include <jive/algebra/VectorSpace.h>
#include <jive/model/Actions.h>
#include <jive/model/StateVector.h>
#include <jive/model/ModelFactory.h>
#include <jive/app/ModuleFactory.h>
#include <jive/implict/Globdat.h>
#include <jive/implict/ConHandler.h>
#include <jive/implict/SolverInfo.h>
#include <jive/implict/NonlinRunData.h>
#include <jive/implict/ArclenActions.h>

#include "XArclenModule.h"
#include "SolverNames.h"

extern "C"
{
  #include <math.h>
}


JEM_DEFINE_CLASS( jive::implict::XArclenModule );


JIVE_BEGIN_PACKAGE( implict )


using jem::max;
using jem::isTiny;
using jem::newInstance;
using jem::Float;
using jem::System;
using jem::ArithmeticException;
using jem::IllegalInputException;
using jem::IllegalOperationException;
using jem::io::Writer;
using jem::io::endl;
using jem::numeric::axpy;
using jive::model::Actions;
using jive::model::StateVector;


//=======================================================================
//   class XArclenModule::RunData_
//=======================================================================


class XArclenModule::RunData_ : public NonlinRunData
{
 public:

  typedef NonlinRunData   Super;
  typedef RunData_        Self;


  explicit inline         RunData_

    ( const String&         context );

  void                    initArcModel

    ( const String&         name,
      const Properties&     conf,
      const Properties&     props,
      const Properties&     globdat );

  void                    commit

    ( const Properties&     globdat );

  void                    cancel

    ( const Properties&     globdat );


 public:

  Ref<Model>              arcModel;

  Matrix                  vbuf;

  double                  loadScale;
  double                  loadScale0;
  idx_t                   iterCount;

};


//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------


inline XArclenModule::RunData_::RunData_ ( const String& ctx ) :

  Super ( ctx )

{
  loadScale  = 0.0;
  loadScale0 = 0.0;
  iterCount  = 0;
}


//-----------------------------------------------------------------------
//   initArcModel
//-----------------------------------------------------------------------


void XArclenModule::RunData_::initArcModel

  ( const String&      name,
    const Properties&  conf,
    const Properties&  props,
    const Properties&  globdat )

{
  using jem::util::None;
  using jem::util::isNone;
  using jive::model::ModelFactory;
  using jive::model::ActionParams;

  if ( ! globdat.find( arcModel, Globdat::ARCLEN_MODEL ) )
  {
    Ref<Object>  obj;

    props.find ( obj, name );

    if ( obj == NIL || isNone( obj ) )
    {
      conf.set ( name, None::getInstance() );

      arcModel = model;
    }
    else
    {
      arcModel = ModelFactory::newInstance ( name,  conf,
                                             props, globdat );
    }
  }

  if ( arcModel != model )
  {
    Properties    params;

    arcModel->configure ( props, globdat );
    arcModel->getConfig ( conf,  globdat );
    arcModel->takeAction ( Actions::INIT, params, globdat );
  }
}


//-----------------------------------------------------------------------
//   commit
//-----------------------------------------------------------------------


void XArclenModule::RunData_::commit ( const Properties& globdat )
{
  Properties    params;

  params.set ( ArclenParams::IITER,          iterCount  );
  params.set ( ArclenParams::LOAD_SCALE,     loadScale  );
  params.set ( ArclenParams::OLD_LOAD_SCALE, loadScale0 );

  model->takeAction ( Actions::COMMIT, params, globdat );

  if ( arcModel != model )
  {
    arcModel->takeAction ( Actions::COMMIT, params, globdat );
  }

  loadScale0 = loadScale;

  StateVector::updateOld ( dofs, globdat );
}


//-----------------------------------------------------------------------
//   cancel
//-----------------------------------------------------------------------


void XArclenModule::RunData_::cancel ( const Properties& globdat )
{
  Properties      params;

  loadScale = loadScale0;
  iterCount = 0;   

  StateVector::restoreNew ( dofs, globdat );

  params.set ( ArclenParams::IITER,          iterCount  );
  params.set ( ArclenParams::LOAD_SCALE,     loadScale  );
  params.set ( ArclenParams::OLD_LOAD_SCALE, loadScale0 );

  model->takeAction ( Actions::CANCEL, params, globdat );

  if ( arcModel != model )
  {
    arcModel->takeAction ( Actions::CANCEL, params, globdat );
  }
}


//=======================================================================
//   class XArclenModule::Work_
//=======================================================================


class XArclenModule::Work_ : public ConHandler
{
 public:

  typedef ArclenParams    Params;


                          Work_

    ( XArclenModule&         module,
      const Properties&     globdat );

  inline                 ~Work_           ();

  void                    updateState     ();
  void                    updateResidual  ();

  inline void             updateModel

    ( const Properties&     globdat );

  void                    updateUnitLoad

    ( const Properties&     globdat );

  void                    updateArcFunc

    ( const Properties&     globdat );

  void                    reportProgress  ();


 public:

  Writer&                 out;
  RunData_&               rundat;

  Properties              params;

  Vector                  u;
  Vector                  d;
  Vector                  q;
  Vector                  r;
  Vector                  fint;
  Vector                  load;

  Vector                  jac10;
  double                  jac11;
  double                  arcFunc;
  double                  loadScale;

  double                  rnorm;
  double                  rscale;

  idx_t                   iiter;

};


//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


XArclenModule::Work_::Work_

  ( XArclenModule&      mod,
    const Properties&  globdat ) :

    out    ( System::info( mod.myName_ ) ),
    rundat ( *mod.rundat_ )

{
  idx_t  dofCount;
  idx_t  j;


  jac11     = 0.0;
  arcFunc   = 0.0;
  loadScale = rundat.loadScale;
  rnorm     = 0.0;
  rscale    = 0.0;
  iiter     = 0;

  rundat.updateConstraints ( globdat );
  rundat.dofs->resetEvents ();

  rundat.frozen = true;
  dofCount      = rundat.dofs->dofCount ();

  StateVector::get ( u, rundat.dofs, globdat );

  j = 0;

  rundat.vbuf.resize ( dofCount, 6 );

  d    .ref ( rundat.vbuf(ALL,j++) );
  q    .ref ( rundat.vbuf(ALL,j++) );
  r    .ref ( rundat.vbuf(ALL,j++) );
  fint .ref ( rundat.vbuf(ALL,j++) );
  load .ref ( rundat.vbuf(ALL,j++) );
  jac10.ref ( rundat.vbuf(ALL,j++) );

  params.set ( Params::IITER,          iiter );
  params.set ( Params::UNIT_LOAD,      load );
  params.set ( Params::LOAD_SCALE,     loadScale );
  params.set ( Params::OLD_LOAD_SCALE, rundat.loadScale0 );
  params.set ( Params::LOAD_RESPONSE,  q );
  params.set ( Params::JACOBIAN10,     jac10 );

  saveConstraints ( *rundat.cons );

  if ( ! (mod.options_ & DELTA_CONS) )
  {
    adjustConstraints ( *rundat.cons, u );
  }
}


inline XArclenModule::Work_::~Work_ ()
{
  rundat.frozen    = false;
  rundat.loadScale = loadScale;
  rundat.iterCount = iiter;
}


//-----------------------------------------------------------------------
//   updateState
//-----------------------------------------------------------------------


void XArclenModule::Work_::updateState ()
{
  double  alpha, b, c;


  b = arcFunc + rundat.vspace->product ( jac10, d );
  c = jac11   + rundat.vspace->product ( jac10, q );

  if ( Float::isNaN( b + c ) )
  {
    throw ArithmeticException (
      rundat.context,
      "error evaluating the arc-length function: NaN"
    );
  }

  if ( isTiny( c ) )
  {
    throw ArithmeticException (
      rundat.context,
      "singular load increment"
    );
  }

  alpha = b / c;

  axpy ( u,    1.0, d );
  axpy ( u, -alpha, q );

  loadScale -= alpha;

  iiter++;

  params.set ( Params::IITER,      iiter     );
  params.set ( Params::LOAD_SCALE, loadScale );
}


//-----------------------------------------------------------------------
//   updateResidual
//-----------------------------------------------------------------------


void XArclenModule::Work_::updateResidual ()
{
  r = -fint;

  axpy           ( r, loadScale, load );
  evalMasterDofs ( r, *rundat.cons );

  rnorm = rscale * rundat.vspace->norm2 ( r );
}


//-----------------------------------------------------------------------
//   updateModel
//-----------------------------------------------------------------------


inline void XArclenModule::Work_::updateModel

  ( const Properties&  globdat )

{
  rundat.model->takeAction ( Actions::UPDATE,
			                       params, globdat );

  if ( rundat.arcModel != rundat.model )
  {
    rundat.arcModel->takeAction ( Actions::UPDATE,
				                          params, globdat );
  }
}


//-----------------------------------------------------------------------
//   updateUnitLoad
//-----------------------------------------------------------------------


void XArclenModule::Work_::updateUnitLoad

  ( const Properties&  globdat )

{
  double  tmp;
  bool    result;


  load   = 0.0;
  result = rundat.arcModel ->

    takeAction ( ArclenActions::GET_UNIT_LOAD,
		              params, globdat );

  if ( ! result )
  {
    String  name = rundat.arcModel->getName ();

    throw IllegalInputException (
      rundat.context,
      String::format (
        "no load returned by the model `%S\' "
        "(ignored action `%s\')",
        & name,
        ArclenActions::GET_UNIT_LOAD
      )
    );
  }

  tmp = rundat.vspace->norm2 ( load );

  if ( Float::isNaN( tmp ) )
  {
     throw ArithmeticException (
      rundat.context,
      "load vector contains invalid data: NaN"
     );
  }

  if ( isTiny( tmp ) )
  {
    System::warn() << rundat.context << " : zero load vector.\n";
  }
}


//-----------------------------------------------------------------------
//   updateArcFunc
//-----------------------------------------------------------------------


void XArclenModule::Work_::updateArcFunc

  ( const Properties&  globdat )

{
  bool  result;


  jac10  = 0.0;
  jac11  = 0.0;
  result = rundat.arcModel ->

    takeAction ( ArclenActions::GET_ARC_FUNC,
		             params, globdat );

  if ( ! result )
  {
    String  name = rundat.arcModel->getName ();

    throw jem::Exception (
      rundat.context,
      String::format (
        "no arc-length function returned by the model `%S\' "
        "(ignored action `%s\')",
        & name,
        ArclenActions::GET_ARC_FUNC
      )
    );
  }

  params.get  ( arcFunc, ArclenParams::ARC_FUNC   );
  params.find ( jac11,   ArclenParams::JACOBIAN11 );
}


//-----------------------------------------------------------------------
//   reportProgress
//-----------------------------------------------------------------------


void XArclenModule::Work_::reportProgress ()
{
  NumberFormat&  nf = rundat.nformat;

  // double  loadIncr = loadScale - rundat.loadScale0;

  // out << rundat.context << ", iteration " << iiter << '\n';

  // out << "  load scale      = " << nf.print( loadScale ) << '\n';
  // out << "  load increment  = " << nf.print( loadIncr )  << '\n';
  // out << "  arc-length func = " << nf.print( arcFunc )   << '\n';
  // out << "  scaled residual = " << nf.print( rnorm )     << '\n';

  out << "module `xarclen' : iter = " << iiter << 
    "  scaled residual = " << nf.print( rnorm )     << //'\n'; // fpm
    " load " << nf.print ( loadScale ) << '\n';

  out.flush ();
}


//=======================================================================
//   class XArclenModule
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const idx_t  XArclenModule::DELTA_CONS      = 1 << 0;

const char*  XArclenModule::TYPE_NAME       = "Arclen";
const char*  XArclenModule::MODEL_PROP      = "model";
const char*  XArclenModule::MAX_ITER_PROP   = "maxIter";
const char*  XArclenModule::PRECISION_PROP  = "precision";
const char*  XArclenModule::DELTA_CONS_PROP = "deltaCons";
const char*  XArclenModule::SOLVER_PROP     = "solver";
const char*  XArclenModule::DESPAIR_PROP    = "allowDespair";
const char*  XArclenModule::MAX_ITER2_PROP  = "maxIter2";
const char*  XArclenModule::NHOPE_PROP      = "nHope";
const char*  XArclenModule::SWT_RESIDUAL_PROP = "swtResidual";


//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


XArclenModule::XArclenModule ( const String& name ) :

  Super ( name )

{
  maxIter_     = 20;
  options_     = 0;
  precision_   = 1.0e-3;
  swtResidual_ = 10.;

  nDespair_    = 0;
  nDSucces_    = 0;
  nDFailIn_    = 0;
  nDFailAfter_ = 0;
  nDMaxIter2_  = 0;

  nIterD_      = 0;
  nIterTot_    = 0;
 
  nLoadSwitch_ = 0;
}


XArclenModule::~XArclenModule ()
{}


//-----------------------------------------------------------------------
//   init
//-----------------------------------------------------------------------


Module::Status XArclenModule::init

  ( const Properties&  conf,
    const Properties&  props,
    const Properties&  globdat )

{
  using jive::util::joinNames;

  rundat_ = NIL;

  Ref<RunData_>  newdat = newInstance<RunData_> ( getContext() );
  String         name   = joinNames ( myName_, SOLVER_PROP );

  newdat->init         ( globdat );
  newdat->initSolver   ( name, precision_, conf, props, globdat );

  name = joinNames     ( myName_, MODEL_PROP );

  newdat->initArcModel ( name, conf, props, globdat );

  // Everything OK, so commit the changes

  rundat_.swap ( newdat );

  return OK;
}


//-----------------------------------------------------------------------
//   shutdown
//-----------------------------------------------------------------------


void XArclenModule::shutdown ( const Properties& globdat )
{
  rundat_ = NIL;

  System::out() << "XArclenModule statistics ..."
    << "\n... total # of iterations:       " << nIterTot_ 
    << "\n... times aborted after switch:  " << nLoadSwitch_;

  if ( allowDespair_ )
  {
    System::out() << "\n..."
    << "\n... # of desperateModes:         " << nDespair_;

    if ( nDespair_ > 0 )
    {
    System::out() 
    << "\n-------- ( of which  " << nDFailIn_ << " kept oscillating.)"
    << "\n-------- (           " << nDMaxIter2_ << " didn't converge.)"
    << "\n-------- (      and  " << nDSucces_ << " were succesful,"
    << "\n------------  although " << nDFailAfter_ << " re-oscillated.)"
    << "\n... # of iters in desperateMode  " << nIterD_;
    }
  }
  System::out() << "\n\n";
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void XArclenModule::configure

  ( const Properties&  props,
    const Properties&  globdat )

{
  using jem::maxOf;

  if ( props.contains( myName_ ) )
  {
    Properties  myProps = props.findProps ( myName_ );

    bool        option;


    if ( myProps.find( option, DELTA_CONS_PROP ) )
    {
      if ( option )
      {
        options_ |=  DELTA_CONS;
      }
      else
      {
        options_ &= ~DELTA_CONS;
      }
    }

    myProps.find ( maxIter_,   MAX_ITER_PROP,
                   0,          maxOf( maxIter_ ) );

    myProps.find ( precision_, PRECISION_PROP,
                   0.0,        1.0e20 );

    allowDespair_ = false;

    myProps.find ( allowDespair_, DESPAIR_PROP );

    if ( allowDespair_ )
    {
      maxIter2_ = 100;
      
      myProps.find ( maxIter2_, MAX_ITER2_PROP,
                     maxIter_,  maxOf( maxIter2_ ) );

      nHope_    = 10;

      myProps.find ( nHope_,    NHOPE_PROP,
                     0,         maxIter2_ - maxIter_ );
    }

    myProps.find ( swtResidual_, SWT_RESIDUAL_PROP,
                     1.0,        maxOf( swtResidual_ ) );
  }

  if ( rundat_ != NIL )
  {
    RunData_&  d = * rundat_;

    d.solver->configure ( props );

    if ( d.arcModel != d.model )
    {
      d.arcModel->configure ( props, globdat );
    }
  }
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void XArclenModule::getConfig

  ( const Properties&  props,
    const Properties&  globdat ) const

{
  Properties  myProps = props.makeProps ( myName_ );


  myProps.set ( DELTA_CONS_PROP, (bool) (options_ & DELTA_CONS) );

  myProps.set ( MAX_ITER_PROP,  maxIter_   );
  myProps.set ( PRECISION_PROP, precision_ );

  myProps.set ( DESPAIR_PROP, allowDespair_ );

  if ( allowDespair_ )
  {
    myProps.set ( NHOPE_PROP,     nHope_    );
    myProps.set ( MAX_ITER2_PROP, maxIter2_ );
  }

  myProps.set ( SWT_RESIDUAL_PROP, swtResidual_ );

  if ( rundat_ != NIL )
  {
    RunData_&  d = * rundat_;

    d.solver->getConfig ( props );

    if ( d.arcModel != d.model )
    {
      d.arcModel->getConfig ( props, globdat );
    }
  }
}


//-----------------------------------------------------------------------
//   advance
//-----------------------------------------------------------------------


void XArclenModule::advance ( const Properties& globdat )
{
  if ( rundat_ == NIL )
  {
    notAliveError ( JEM_FUNC );
  }

  rundat_->advance ( globdat );
}


//-----------------------------------------------------------------------
//   solve
//-----------------------------------------------------------------------


void XArclenModule::solve

  ( const Properties&  info,
    const Properties&  globdat )

{
  if ( rundat_ == NIL )
  {
    notAliveError ( JEM_FUNC );
  }

  RunData_&  d = * rundat_;

  Work_      w ( *this, globdat );

  double     loadIncr;
  bool       result;


  w.out << "Starting the generalized arc-length solver `"
        << "xarclen"
        << "\' ...\n";

  w.updateUnitLoad ( globdat );
  d.updateMatrix   ( w.fint, globdat );

  // Calculate the initial residual and the residual scale factor,
  // including the slave DOFs.

  w.r      = w.loadScale * w.load - w.fint;

  w.rscale = ::sqrt( Float::EPSILON ) *

    max ( d.vspace->norm2( w.fint ),
          d.vspace->norm2( w.load ) * ::fabs( w.loadScale ) );

  w.rscale = max ( w.rscale, d.vspace->norm2( w.r ) );

  d.solver->solve   ( w.q, w.load );
  w.updateArcFunc   ( globdat );
  w.zeroConstraints ( *d.cons );
  d.solver->solve   ( w.d, w.r );

  result = solve_   ( w, globdat );

  w.restoreConstraints ( *d.cons );

  loadIncr = w.loadScale - d.loadScale0;

  info.set ( SolverInfo::CONVERGED,  result      );
  info.set ( SolverInfo::ITER_COUNT, w.iiter     );
  info.set ( SolverInfo::RESIDUAL,   w.rnorm     );
  info.set ( SolverInfo::LOAD_INCR,  loadIncr    );
  info.set ( SolverInfo::LOAD_SCALE, w.loadScale );

  if ( Globdat::hasVariable( myName_, globdat ) )
  {
    Properties  vars = Globdat::getVariables ( myName_, globdat );

    vars.set ( SolverInfo::CONVERGED,  result      );
    vars.set ( SolverInfo::ITER_COUNT, w.iiter     );
    vars.set ( SolverInfo::RESIDUAL,   w.rnorm     );
    vars.set ( SolverInfo::LOAD_INCR,  loadIncr    );
    vars.set ( SolverInfo::LOAD_SCALE, w.loadScale );
  }

  if ( ! result )
  {
    throw jem::Exception (
      getContext (),
      String::format (
        "no convergence achieved in %d iterations; "
        "final residual: %e",
        w.iiter,
        w.rnorm
      )
    );
  }

  w.out << "The generalized arc-length solver converged in "
        << w.iiter
        << " iterations\n\n";
}


//-----------------------------------------------------------------------
//   cancel
//-----------------------------------------------------------------------


void XArclenModule::cancel ( const Properties& globdat )
{
  if ( rundat_ == NIL )
  {
    notAliveError ( JEM_FUNC );
  }

  rundat_->cancel ( globdat );
}


//-----------------------------------------------------------------------
//   commit
//-----------------------------------------------------------------------


bool XArclenModule::commit ( const Properties& globdat )
{
  if ( rundat_ == NIL )
  {
    notAliveError ( JEM_FUNC );
  }

  rundat_->commit ( globdat );

  return true;
}


//-----------------------------------------------------------------------
//   setPrecision
//-----------------------------------------------------------------------


void XArclenModule::setPrecision ( double eps )
{
  JEM_PRECHECK ( eps > 0.0 );

  precision_ = eps;
}


//-----------------------------------------------------------------------
//   getPrecision
//-----------------------------------------------------------------------


double XArclenModule::getPrecision () const
{
  return precision_;
}


//-----------------------------------------------------------------------
//   setOptions
//-----------------------------------------------------------------------


void XArclenModule::setOptions ( idx_t options )
{
  options_ = options;
}


//-----------------------------------------------------------------------
//   getOptions
//-----------------------------------------------------------------------


idx_t XArclenModule::getOptions () const
{
  return options_;
}


//-----------------------------------------------------------------------
//   setLoadScale [fpm]
//-----------------------------------------------------------------------


void XArclenModule::setLoadScale ( double val )
{
  rundat_->loadScale0 = rundat_->loadScale = val;
}


//-----------------------------------------------------------------------
//   makeNew
//-----------------------------------------------------------------------


Ref<Module> XArclenModule::makeNew

  ( const String&      name,
    const Properties&  conf,
    const Properties&  props,
    const Properties&  globdat)

{
  return newInstance<Self> ( name );
}


//-----------------------------------------------------------------------
//   declare
//-----------------------------------------------------------------------


void XArclenModule::declare ()
{
  using jive::app::ModuleFactory;

  ModuleFactory::declare ( TYPE_NAME,  & makeNew );
  ModuleFactory::declare ( CLASS_NAME, & makeNew );
}


//-----------------------------------------------------------------------
//   solve_
//-----------------------------------------------------------------------


bool XArclenModule::solve_

  ( Work_&             w,
    const Properties&  globdat )

{
  using jem::util::Flex;

  RunData_&  d = w.rundat;

  ++nIterTot_;
  w.updateState   ();
  w.updateModel   ( globdat );
  w.updateArcFunc ( globdat );
  d.updateMatrix  ( w.fint, globdat );

  // fpm: initialize stuff for desperateMode

  Flex<double>   oldRs;
  idx_t          descending = 0;
  idx_t          dIterCount = 0;
  double         switchRate;
  bool           kinked;
  Properties     params;

  oldRs.pushBack ( 1. );

  idx_t  maxIter       = maxIter_;
  bool   desperateMode = false;

  d.model->takeAction ( "END_DESPAIR", params, globdat );

  // and for divergence check

  const double loadScale0 = w.loadScale;

  // endfpm

  // Update the residual and the residual scale factor, including the
  // slave DOFs.

  w.r = w.loadScale * w.load - w.fint;

  if ( 1 )
  {
    // fpm

    double oldScale = w.rscale;

    w.rscale = max ( w.rscale, d.vspace->norm2( w.r ), 1. ); // added 1. [fpm]
    w.rscale = max ( w.rscale, ::sqrt( Float::EPSILON ) *
        d.vspace->norm2( w.fint ) );

    if ( oldScale > 0. && w.rscale > swtResidual_ * oldScale )
    {
      params.clear();

      // try to switch to load control, if that succeeds restart solver,
      // otherwise, reduce scale factor to reasonable value so that 
      // non-equilibrium solution will not wrongly be considered converged

      d.model->takeAction ( SolverNames::STOP_ARCL,  params, globdat );

      bool done = false;

      params.find ( done, SolverNames::DONE );

      if ( done )
      {
        w.out << "arclen solver says: try to switch, secant system ";
        w.out << ( w.rscale / oldScale ) << endl;
        return false;
      }
      else
      {
        w.out << "reduced residual factor from, "
          << d.nformat.print( w.rscale ) << '\n';

        w.rscale = 10.*oldScale;
      }
    }
  }
  else
  {
    // jive standard
    w.rscale = max ( w.rscale, d.vspace->norm2( w.r ) ); 
    w.rscale = max ( w.rscale, ::sqrt( Float::EPSILON ) *
        d.vspace->norm2( w.fint ) );
  }

  w.out << "module `xarclen'"
        << " : residual scale factor = "
        << d.nformat.print( w.rscale )
	<< '\n';

  if ( w.rscale > 0.0 )
  {
    w.rscale = 1.0 / w.rscale;
  }

  // Update the scaled residual norm, excluding the slave DOFs.

  w.updateResidual ();

  do
  {
    if ( desperateMode )
    {
      ++dIterCount;
      ++nIterD_;

      if ( w.rnorm < oldRs.back() )
      {
        ++descending;
      }
      else
      {
        descending = 0;
      }
    }

    oldRs.pushBack ( w.rnorm );

    w.reportProgress ();

    if ( desperateMode  
         ? dIterCount > 3 
         : w.iiter + maxIter_ - maxIter > 3 )
         
    {
      // difference in residuals must be small ...

      switchRate = max ( ::fabs ( oldRs[w.iiter  ] - oldRs[w.iiter-2] ),
                         ::fabs ( oldRs[w.iiter-1] - oldRs[w.iiter-3] ) );

      // and residual must be going up and down

      kinked = ( ( oldRs[w.iiter  ] - oldRs[w.iiter-1] ) * 
                 ( oldRs[w.iiter-1] - oldRs[w.iiter-2] ) < 0. )
               &&
               ( ( oldRs[w.iiter-1] - oldRs[w.iiter-2] ) * 
                 ( oldRs[w.iiter-2] - oldRs[w.iiter-3] ) < 0. );

      if ( desperateMode 
           ? dIterCount > 5
           : w.iiter + maxIter_ - maxIter > 5 )
      {
        // also ABCABC stepped loop

        double rate3 = max ( ::fabs ( oldRs[w.iiter  ] - oldRs[w.iiter-3] ),
                             ::fabs ( oldRs[w.iiter-1] - oldRs[w.iiter-4] ),
                             ::fabs ( oldRs[w.iiter-2] - oldRs[w.iiter-5] ) );

        if ( rate3 < .1 * switchRate )
        {
          switchRate = rate3;
          kinked = true;
        }
      }
    }
    else
    {
      switchRate = 10.;
      kinked = false;
    }

    if ( allowDespair_ )
    {
      if ( ( ! desperateMode ) && switchRate < precision_ )
      {
        // move to desperateMode (unless that has been tried already)

        if ( dIterCount > 0 ) 
        {
          w.out << "  -- too bad, desperateMode didn't help\n";

          ++nDFailAfter_;

          return false;
        }

        w.out << "  -- desperateMode\n";

        desperateMode = true;
        descending    = 0;
        dIterCount    = 0;
        maxIter       = maxIter2_;
        ++nDespair_;

        d.model->takeAction ( "DESPAIR", params, globdat );
      }
    }
    else if ( switchRate < precision_ * .5 )
    {
      w.out << "  -- oscillating, while allowDespair = false" << endl;
     
      return false;
    }

    // fpm 
    if ( w.loadScale * loadScale0 < 0. )
    {
      w.out << "  -- stop trying after load switch \n";

      ++nLoadSwitch_;

      return false;
    }

    if ( w.rnorm <= precision_ && ::fabs( w.arcFunc ) <= precision_ )
    {
      if ( desperateMode )
      {
        ++nDSucces_;
        d.model->takeAction ( "END_DESPAIR", params, globdat );
      }
      return true;
    }

    if ( desperateMode )
    {
      maxIter = maxIter2_ - nHope_ + descending;

      kinked &= ( dIterCount > 3 );

      if ( descending == nHope_ )
      {
        w.out << "  -- this ought to do it, end desperateMode \n";

        desperateMode = false;
        descending    = 0;
        ++nDSucces_;

        d.model->takeAction ( "END_DESPAIR", params, globdat );

        maxIter = w.iiter + maxIter_;
      }

      else if ( switchRate < precision_ * .01 && kinked )
      {
        w.out << "  -- oscillating, while in desperateMode\n";

        ++nDFailIn_;

        return false;
      }
    }
    if ( w.iiter > maxIter )
    {
      // [fpm]: don't quit if one more iteration like this gives convergence

      double rf = w.rnorm / oldRs[w.iiter-1];

      if ( rf * w.rnorm < precision_ )
      {
        w.out << "  -- continue even though ( iiter > maxIter )\n";
      }
      else
      {
        nDMaxIter2_ += idx_t ( desperateMode );

        return false;
      }
    }

    if ( w.rnorm > 1.0e4 || ::fabs( w.arcFunc ) > 1.0e4 )
    {
      return false;
    }

    d.solver->solve  ( w.d, w.r    );
    d.solver->solve  ( w.q, w.load );

    ++nIterTot_;
    w.updateState    ();
    w.updateModel    ( globdat );
    w.updateArcFunc  ( globdat );
    d.updateMatrix   ( w.fint, globdat );
    w.updateResidual ();
  }
  while ( true );

  return false;
}



JIVE_END_PACKAGE( implict )
