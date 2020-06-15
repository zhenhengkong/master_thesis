
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


#include <jem/base/Float.h>
#include <jem/base/System.h>
#include <jem/base/ClassTemplate.h>
#include <jem/base/array/operators.h>
#include <jem/mp/Context.h>
#include <jem/util/Event.h>
#include <jem/util/Properties.h>
#include <jem/numeric/algebra/matmul.h>
#include <jive/util/utilities.h>
#include <jive/util/Constraints.h>
#include <jive/mp/VectorExchanger.h>
#include <jive/algebra/GramSchmidt.h>
#include <jive/algebra/MPVectorSpace.h>
#include <jive/algebra/StdVectorSpace.h>
#include <jive/algebra/ConstrainedMatrix.h>
#include <jive/algebra/MPMatrixExtension.h>
#include <jive/solver/Names.h>
#include <jive/solver/Solver.h>
#include <jive/solver/Restrictor.h>
#include <jive/solver/SolverParams.h>
#include <jive/solver/PreconFactory.h>
#include <jive/solver/CoarseMatrix.h>
#include <jive/solver/CoarseDofSpace.h>
#include <jive/solver/CoarsePrecon.h>


JEM_DEFINE_CLASS( jive::solver::CoarsePrecon );


JIVE_BEGIN_PACKAGE( solver )


using jem::newInstance;
using jive::util::joinNames;


//=======================================================================
//   class CoarsePrecon
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  CoarsePrecon::TYPE_NAME   = "Coarse";

const int    CoarsePrecon::NEW_VALUES_ = 1 << 0;
const int    CoarsePrecon::NEW_STRUCT_ = 1 << 1;


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


CoarsePrecon::CoarsePrecon

  ( const String&      name,
    Ref<CoarseMatrix>  cmat ) :

    Super      ( name ),
    coarseMat_ ( cmat )

{
  using jive::algebra::MPVectorSpace;
  using jive::algebra::StdVectorSpace;

  JEM_PRECHECK ( cmat != NIL );

  Ref<CoarseDofSpace>  dofs   = cmat->getCoarseDofs ();
  Ref<VectorSpace>     vspace = newInstance<StdVectorSpace> ( dofs );

  if ( dofs->isDistributed() )
  {
    vspace = newInstance<MPVectorSpace> ( vspace,
                                          dofs->getMPContext() );
  }

  grams_ = newGramSchmidt ( joinNames  ( myName_, "grams" ),
                            Properties (),
                            vspace );

  connect_ ();
}


CoarsePrecon::~CoarsePrecon ()
{}


//-----------------------------------------------------------------------
//   resetEvents
//-----------------------------------------------------------------------


void CoarsePrecon::resetEvents ()
{
  coarseMat_->resetEvents ();
}


//-----------------------------------------------------------------------
//   shape
//-----------------------------------------------------------------------


AbstractMatrix::Shape CoarsePrecon::shape () const
{
  return coarseMat_->getInner()->shape ();
}


//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------


void CoarsePrecon::update ()
{
  if ( events_ )
  {
    update_ ();
  }
}


//-----------------------------------------------------------------------
//   matmul
//-----------------------------------------------------------------------


void CoarsePrecon::matmul

  ( const Vector&  lhs,
    const Vector&  rhs ) const

{
  JEM_PRECHECK ( lhs.size() == rhs.size() );

  coarseMat_->update ();

  if ( events_ & (~NEW_VALUES_) )
  {
    const_cast<Self*> ( this ) -> update_ ();
  }

  Restrictor*  rtor = coarseMat_->getRestrictor ();

  Vector       a ( qFactor_.size(0) );
  Vector       b ( qFactor_.size(1) );

  rtor  ->matmul        ( a, rhs );
  grams_->orthogonalize ( b, a, qFactor_ );
  jem::numeric::matmul  ( a, rFactor_, b );
  rtor  ->transmul      ( lhs, a );
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void CoarsePrecon::configure ( const Properties& props )
{
  Restrictor*  rtor = coarseMat_->getRestrictor ();

  Super::configure ( props );
  rtor ->configure ( props );

  newValuesEvent.emit ( *this );
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void CoarsePrecon::getConfig ( const Properties& props ) const
{
  Restrictor*  rtor = coarseMat_->getRestrictor ();

  Super::getConfig ( props );
  rtor ->getConfig ( props );
}


//-----------------------------------------------------------------------
//   hasTrait
//-----------------------------------------------------------------------


bool CoarsePrecon::hasTrait ( const String& trait ) const
{
  using jive::algebra::MatrixTraits;

  if ( trait == MatrixTraits::SYMMETRIC ||
       trait == MatrixTraits::DISTRIBUTED )
  {
    return coarseMat_->hasTrait ( trait );
  }
  else
  {
    return false;
  }
}


//-----------------------------------------------------------------------
//   getConstraints
//-----------------------------------------------------------------------


Constraints* CoarsePrecon::getConstraints () const
{
  ConstrainedMatrix*  conmat =

    dynamic_cast<ConstrainedMatrix*> ( coarseMat_->getInner() );

  if ( conmat )
  {
    return conmat->getConstraints ();
  }
  else
  {
    return (Constraints*) 0;
  }
}


//-----------------------------------------------------------------------
//   makeNew
//-----------------------------------------------------------------------


Ref<Precon> CoarsePrecon::makeNew

  ( const String&      name,
    const Properties&  conf,
    const Properties&  props,
    const Properties&  params,
    const Properties&  globdat )

{
  using jem::System;
  using jive::algebra::MPMatrixExt;

  Ref<AbstractMatrix>   matrix;
  Ref<VectorExchanger>  exchanger;
  Ref<Constraints>      cons;
  Ref<Restrictor>       rt;


  // Get the matrix.

  if ( ! params.find( matrix, SolverParams::MATRIX ) )
  {
    return NIL;
  }

  // Get the vector exchanger if the matrix is distributed.

  if ( matrix->isDistributed() )
  {
    MPMatrixExt*  mpExt = matrix->getExtension<MPMatrixExt> ();

    if ( mpExt )
    {
      exchanger = mpExt->getExchanger ();
    }
  }

  // Apply the constraints, if necessary.

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

  // Get the restriction operator.

  if ( ! params.find( rt, SolverParams::RESTRICTOR ) )
  {
    rt = newRestrictor (
      joinNames ( name, PropNames::RESTRICTOR ),
      conf, props, params, globdat
    );
  }

  if ( rt == NIL )
  {
    return NIL;
  }

  if ( matrix->isDistributed() != rt->isDistributed() )
  {
    print ( System::warn(), name,
            " : incompatible matrix and restriction operator" );

    return NIL;
  }

  Ref<CoarseDofSpace>  coarseDofs =

    newInstance<CoarseDofSpace> ( rt );

  Ref<CoarseMatrix>    coarseMat  =

    newInstance<CoarseMatrix> (
      joinNames ( name, PropNames::MATRIX ),
      matrix, coarseDofs
    );

  return newInstance<Self> ( name, coarseMat );
}


//-----------------------------------------------------------------------
//   declare
//-----------------------------------------------------------------------


void CoarsePrecon::declare ()
{
  PreconFactory::declare ( TYPE_NAME,  & Self::makeNew );
  PreconFactory::declare ( CLASS_NAME, & Self::makeNew );
}


//-----------------------------------------------------------------------
//   connect_
//-----------------------------------------------------------------------


void CoarsePrecon::connect_ ()
{
  using jem::util::connect;

  events_ = ~0x0;

  connect ( coarseMat_->newValuesEvent,
            this, & Self::valuesChanged_ );

  connect ( coarseMat_->newStructEvent,
            this, & Self::structChanged_ );
}


//-----------------------------------------------------------------------
//   update_
//-----------------------------------------------------------------------


void CoarsePrecon::update_ ()
{
  using jem::Float;
  using jem::System;
  using jem::io::endl;
  using jem::io::Writer;

  coarseMat_->update ();

  Writer&          out        = System::info ( myName_ );

  CoarseDofSpace*  coarseDofs = coarseMat_->getCoarseDofs  ();

  const idx_t      localSize  = coarseDofs->dofCount       ();
  const idx_t      globalSize = coarseDofs->globalDofCount ();
  const idx_t      firstDof   = coarseDofs->getMyFirstDof  ();

  Vector           a ( globalSize + 2 );
  Vector           r ( localSize );

  double           scale;
  idx_t            n, nzp;


  print ( out, myName_, " : assembling the coarse matrix ...\n" );

  n = coarseMat_->assemble ( qFactor_ );

  print ( out, myName_, " : ready in ", n, " iterations.\n" );
  print ( out, myName_, " : coarse matrix size = ",
          globalSize, endl );

  // Initialize the R-factor to the identity matrix.

  rFactor_.resize ( localSize, globalSize );

  rFactor_ = 0.0;

  for ( idx_t i = 0; i < localSize; i++ )
  {
    idx_t  j = i + firstDof;

    rFactor_(i,j) = 1.0;
  }

  // Apply a parallel version of the QR decomposition algorithm.

  print ( out, myName_, " : factoring the coarse matrix ...\n" );

  nzp = 0;

  for ( idx_t j = 0; j < globalSize; j++ )
  {
    grams_->orthogonalize ( a[slice(BEGIN,j + 2)],
                            qFactor_[j],
                            qFactor_[slice(BEGIN,j)] );

    // Check for a zero pivot.

    if ( 0.1 * a[j + 1] <= Float::EPSILON * a[j] )
    {
      scale = 0.0;
      nzp++;
    }
    else
    {
      scale = 1.0 / a[j + 1];

      jem::numeric::matmul ( r,
                             rFactor_[slice(BEGIN,j)],
                             a[slice(BEGIN,j)] );

      rFactor_[j] -= r;
    }

    qFactor_[j] *= scale;
    rFactor_[j] *= scale;
  }

  if ( nzp == 0 )
  {
    print ( out, myName_, " : factorization complete.\n" );
  }
  else
  {
    print ( System::warn(), myName_,
            " : singular coarse matrix.\n" );
  }

  coarseMat_->resetEvents ();

  events_ = 0;
}


//-----------------------------------------------------------------------
//   valuesChanged_
//-----------------------------------------------------------------------


void CoarsePrecon::valuesChanged_ ()
{
  events_ |= NEW_VALUES_;

  newValuesEvent.emit ( *this );
}


//-----------------------------------------------------------------------
//   structChanged_
//-----------------------------------------------------------------------


void CoarsePrecon::structChanged_ ()
{
  events_ |= NEW_STRUCT_;

  newStructEvent.emit ( *this );
}


JIVE_END_PACKAGE( solver )
