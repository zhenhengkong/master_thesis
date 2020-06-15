
/*
 *  Copyright (C) 2016 DRG. All rights reserved.
 *
 *  This file is part of Jive, an object oriented toolkit for solving
 *  partial differential equations.
 *
 *  Commercial License Usage
 *
 *  This file may be used under the terms of a commercial license
 *  provided with the software, or under the terms contained in a written
 *  agreement between you and DRG. For more information contact DRG at
 *  http://www.dynaflow.com.
 *
 *  GNU Lesser General Public License Usage
 *
 *  Alternatively, this file may be used under the terms of the GNU
 *  Lesser General Public License version 2.1 or version 3 as published
 *  by the Free Software Foundation and appearing in the file
 *  LICENSE.LGPLv21 and LICENSE.LGPLv3 included in the packaging of this
 *  file. Please review the following information to ensure the GNU
 *  Lesser General Public License requirements will be met:
 *  https://www.gnu.org/licenses/lgpl.html and
 *  http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 *  This file is part of Jive, an object oriented toolkit for
 *  solving partial differential equations.
 *
 *  Jive version: 2.2
 *  Date:         Thu 28 Jan 10:31:15 CET 2016
 */


#include <jem/base/assert.h>
#include <jem/base/Float.h>
#include <jem/base/System.h>
#include <jem/base/ClassTemplate.h>
#include <jem/base/CancelledException.h>
#include <jem/base/ArithmeticException.h>
#include <jem/base/IllegalArgumentException.h>
#include <jem/base/Thread.h>
#include <jem/base/array/operators.h>
#include <jem/numeric/algebra/utilities.h>
#include <jive/util/error.h>
#include <jive/util/utilities.h>
#include <jive/algebra/utilities.h>
#include <jive/algebra/VectorSpace.h>
#include <jive/algebra/ConstrainedMatrix.h>
#include <jive/solver/Names.h>
#include <jive/solver/SolverInfo.h>
#include <jive/solver/SolverParams.h>
#include <jive/solver/DummyConstrainer.h>
#include <jive/solver/GenericConstrainer.h>
#include <jive/solver/IterativeSolverException.h>
#include <jive/solver/StdIterativeSolver.h>


JEM_DEFINE_CLASS( jive::solver::StdIterativeSolver );


JIVE_BEGIN_PACKAGE( solver )


using jem::newInstance;
using jem::System;
using jem::IllegalArgumentException;
using jive::util::joinNames;


//=======================================================================
//   class StdIterativeSolver
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const double  StdIterativeSolver::MAX_RESIDUAL       = 1.0e5;

const char*   StdIterativeSolver::UPDATE_POLICIES[3] =
{
  "Auto",
  "Never",
  "Always"
};


//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


StdIterativeSolver::StdIterativeSolver

  ( const String&        name,
    Ref<AbstractMatrix>  matrix,
    Ref<VectorSpace>     vspace,
    Ref<Constraints>     cons ) :

    Super   (   name ),
    matrix_ ( matrix ),
    vspace_ ( vspace )

{
  JEM_PRECHECK ( matrix != NIL &&
                 vspace != NIL &&
                 matrix->isDistributed() ==
                 vspace->isDistributed() );

  const String  conName = joinNames ( myName_, "constrainer" );

  if ( cons != NIL &&
       cons->getDofSpace() != vspace->getDofSpace() )
  {
    throw IllegalArgumentException (
      getContext (),
      "vector space and constraints are associated with "
      "different DOF spaces"
    );
  }

  Ref<ConstrainedMatrix>  conmat =

    jem::dynamicCast<ConstrainedMatrix> ( matrix );

  if      ( conmat != NIL && cons == conmat->getConstraints() )
  {
    conman_ = newInstance<GenericConstrainer> ( conName, conmat );
  }
  else if ( cons == NIL )
  {
    conman_ = newInstance<DummyConstrainer>   ( conName, matrix );
  }
  else
  {
    conman_ = newInstance<GenericConstrainer> ( conName, cons,
                                                matrix_ );

    matrix_ = conman_->getOutputMatrix ();
  }

  mode_        = 0;
  maxIter_     = MAX_ITER;
  precision_   = PRECISION;
  policy_      = AUTO_UPDATE;
  iiter_       = 0;
  iiter0_      = JEM_IDX_C(-1);
  error_       = 0.0;
  cancelCount_ = 0;
}


StdIterativeSolver::~StdIterativeSolver ()
{}


//-----------------------------------------------------------------------
//   improve
//-----------------------------------------------------------------------


void StdIterativeSolver::improve

  ( const Vector&  lhs,
    const Vector&  rhs )

{
  using jem::max;
  using jem::Float;
  using jem::ArithmeticException;
  using jem::numeric::axpy;

  JEM_PRECHECK ( lhs.size() == rhs.size() );

  const idx_t  dofCount = vspace_->size ();
  const bool   strict   = ! (mode_ & LENIENT_MODE);

  Vector       du;
  Vector       r;
  Vector       u;
  Vector       f;

  double       rscale;


  if ( lhs.size() != dofCount )
  {
    util::sizeError ( getContext (),
                      "lhs vector", lhs.size(), dofCount );
  }

  conman_->update ();
  checkMatrix_    ();

  if ( mode_ & PRECON_MODE )
  {
    Matrix  vbuf ( dofCount, 2 );

    r .ref ( vbuf[0] );
    du.ref ( vbuf[1] );
    u .ref ( lhs );
    f .ref ( rhs );
  }
  else
  {
    Matrix  vbuf ( dofCount, 4 );

    r .ref ( vbuf[0] );
    du.ref ( vbuf[1] );
    u .ref ( vbuf[2] );
    f .ref ( vbuf[3] );

    conman_->getRhs  ( f, rhs );
    conman_->initLhs ( u, lhs );
  }

  rscale = vspace_->norm2 ( f );

  if ( Float::isNaN( rscale ) )
  {
    throw ArithmeticException (
      getContext (),
      "invalid norm of right-hand vector: NaN"
    );
  }

  if ( Float::isTiny( rscale ) )
  {
    u = 0.0;

    if ( ! (mode_ & PRECON_MODE) )
    {
      conman_->getLhs ( lhs, u );
    }

    return;
  }

  rscale = 1.0 / rscale;
  error_ = 1.0;
  iiter_ = 0;

  if ( (policy_ == ALWAYS_UPDATE) ||
       (policy_ == AUTO_UPDATE    && iiter0_ < 0) )
  {
    updatePrecon_ ();
  }

  while ( iiter_ < maxIter_ )
  {
    getResidual ( r, u, f );

    error_ = rscale * vspace_->norm2 ( r );

    if ( Float::isNaN( error_ ) )
    {
      throw ArithmeticException (
        getContext (),
        "invalid norm of residual vector: NaN"
      );
    }

    if ( iiter_ > 0 )
    {
      nextIterEvent.emit ( iiter_, error_, *this );
    }

    if ( error_ <= precision_ || error_ > MAX_RESIDUAL )
    {
      break;
    }

    if ( iiter_ > 0 )
    {
      restartEvent.emit ( iiter_, error_, *this );
    }

    if ( strict )
    {
      solve_ ( iiter_, error_, rscale, du, r );
    }
    else
    {
      try
      {
        solve_ ( iiter_, error_, rscale, du, r );
      }
      catch ( const SolverException& e )
      {
        axpy ( u, 1.0, du );

        if ( iiter_ == 0 )
        {
          print ( System::warn(), myName_, " : ", e.what(), '\n' );
        }

        break;
      }
    }

    axpy ( u, 1.0, du );
  }

  if ( error_ > max( 1.0, precision_ )  )
  {
    throw IterativeSolverException (
      getContext (),
      String::format ( "residual too large: %e", error_ ),
      iiter_,
      error_
    );
  }

  if ( iiter_ >= maxIter_ && strict )
  {
    throw IterativeSolverException (
      getContext (),
      String::format (
        "maximum number of iterations (%d) exceeded",
        maxIter_
      ),
      iiter_,
      error_
    );
  }

  if ( ! (mode_ & PRECON_MODE) )
  {
    conman_->getLhs ( lhs, u );
  }

  if ( policy_ == AUTO_UPDATE )
  {
    if      ( iiter0_ < 0 )
    {
      iiter0_ = iiter_;
    }
    else if ( iiter_ > (iiter0_ + iiter0_ / 5) )
    {
      iiter0_ = JEM_IDX_C(-1);
    }
  }
}


//-----------------------------------------------------------------------
//   getInfo
//-----------------------------------------------------------------------


void StdIterativeSolver::getInfo ( const Properties& info ) const
{
  info.set ( SolverInfo::RESIDUAL,   error_ );
  info.set ( SolverInfo::ITER_COUNT, iiter_ );
  info.set ( SolverInfo::DOF_COUNT,  vspace_->globalSize() );
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void StdIterativeSolver::configure ( const Properties& props )
{
  Super::configure ( props );

  if ( props.contains( myName_ ) )
  {
    Properties   myProps = props.findProps ( myName_ );

    String       policy;

    if ( myProps.find( policy, PropNames::UPDATE_POLICY ) )
    {
      int  i;

      for ( i = 0; i < 3; i++ )
      {
        if ( policy.equalsIgnoreCase( UPDATE_POLICIES[i] ) )
        {
          break;
        }
      }

      if ( i >= 3 )
      {
        myProps.propertyError (
          PropNames::UPDATE_POLICY,
          String::format (
            "invalid update policy: %s; "
            "should be one of `%s\', `%s\' or `%s\'",
            policy,
            UPDATE_POLICIES[0],
            UPDATE_POLICIES[1],
            UPDATE_POLICIES[2]
          )
        );
      }

      policy_ = (UpdatePolicy) i;
      iiter0_ = JEM_IDX_C(-1);
    }
  }
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void StdIterativeSolver::getConfig ( const Properties& props ) const
{
  Properties  myProps = props.makeProps ( myName_ );

  Super::getConfig ( props );

  myProps.set ( PropNames::UPDATE_POLICY,
                UPDATE_POLICIES[(int) policy_] );
}

//-----------------------------------------------------------------------
//   setMode
//-----------------------------------------------------------------------


void StdIterativeSolver::setMode ( int mode )
{
  mode_ = mode;
}


//-----------------------------------------------------------------------
//   getMode
//-----------------------------------------------------------------------


int StdIterativeSolver::getMode () const
{
  return mode_;
}


//-----------------------------------------------------------------------
//   setPrecision
//-----------------------------------------------------------------------


void StdIterativeSolver::setPrecision ( double eps )
{
  JEM_PRECHECK ( eps > 0.0 );

  precision_ = eps;
}


//-----------------------------------------------------------------------
//   getPrecision
//-----------------------------------------------------------------------


double StdIterativeSolver::getPrecision () const
{
  return precision_;
}


//-----------------------------------------------------------------------
//   getMatrix
//-----------------------------------------------------------------------


AbstractMatrix* StdIterativeSolver::getMatrix () const
{
  return conman_->getInputMatrix ();
}


//-----------------------------------------------------------------------
//   getConstraints
//-----------------------------------------------------------------------


Constraints* StdIterativeSolver::getConstraints () const
{
  return conman_->getConstraints ();
}


//-----------------------------------------------------------------------
//   setMaxIterCount
//-----------------------------------------------------------------------


void StdIterativeSolver::setMaxIterCount ( idx_t n )
{
  JEM_PRECHECK ( n >= 0 );

  maxIter_ = n;
}


//-----------------------------------------------------------------------
//   getMaxIterCount
//-----------------------------------------------------------------------


idx_t StdIterativeSolver::getMaxIterCount () const
{
  return maxIter_;
}


//-----------------------------------------------------------------------
//   getResidual
//-----------------------------------------------------------------------


void StdIterativeSolver::getResidual

  ( const Vector&  r,
    const Vector&  lhs,
    const Vector&  rhs )

{
  matrix_->matmul ( r, lhs );

  r = rhs - r;
}


//-----------------------------------------------------------------------
//   decodeParams
//-----------------------------------------------------------------------


bool StdIterativeSolver::decodeParams

  ( Ref<AbstractMatrix>&  matrix,
    Ref<VectorSpace>&     vspace,
    Ref<Constraints>&     cons,
    const Properties&     params )

{
  if ( ! params.find( matrix, SolverParams::MATRIX ) )
  {
    return false;
  }

  if ( ! params.find( vspace, SolverParams::VECTOR_SPACE ) )
  {
    return false;
  }

  if ( params.find( cons, SolverParams::CONSTRAINTS ) )
  {
    Ref<ConstrainedMatrix>  conmat;

    params.find ( conmat, SolverParams::CONSTRAINED_MATRIX );

    if ( conmat == NIL || cons != conmat->getConstraints() )
    {
      conmat = newInstance<ConstrainedMatrix> ( matrix, cons );

      params.set ( SolverParams::CONSTRAINED_MATRIX, conmat );
    }

    matrix = conmat;
  }

  return true;
}


//-----------------------------------------------------------------------
//   testCancelled_
//-----------------------------------------------------------------------


void StdIterativeSolver::testCancelled_ ()
{
  cancelCount_ += vspace_->size();

  if ( cancelCount_ > JEM_IDX_C(10000) )
  {
    if ( jem::Thread::cancelled() )
    {
      throw jem::CancelledException (
        getContext(), "solver cancelled"
      );
    }

    cancelCount_ = 0;
  }
}


//-----------------------------------------------------------------------
//   checkMatrix_
//-----------------------------------------------------------------------


void StdIterativeSolver::checkMatrix_ ()
{
  AbstractMatrix::Shape  sh = matrix_->shape ();

  if ( sh[0] != sh[1] )
  {
    util::nonSquareMatrixError ( getContext(), "system", sh );
  }

  if ( vspace_->size() != sh[0] )
  {
    throw IllegalArgumentException (
      getContext (),
      String::format (
        "vector space size (%d) and matrix size (%d) "
        "do not match",
        vspace_->size (),
        sh[0]
      )
    );
  }
}


JIVE_END_PACKAGE( solver )
