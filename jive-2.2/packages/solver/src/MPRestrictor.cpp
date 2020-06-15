
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


#include <jem/base/ClassTemplate.h>
#include <jem/base/array/operators.h>
#include <jem/base/array/utilities.h>
#include <jem/base/IllegalArgumentException.h>
#include <jem/util/Event.h>
#include <jive/util/error.h>
#include <jive/util/DofSpace.h>
#include <jive/mp/VectorExchanger.h>
#include <jive/solver/MPRestrictor.h>


JEM_DEFINE_CLASS( jive::solver::MPRestrictor );


JIVE_BEGIN_PACKAGE( solver )


using jive::mp::SCATTER;
using jive::util::sizeError;


//=======================================================================
//   class MPRestrictor
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


MPRestrictor::MPRestrictor

  ( Ref<Restrictor>       rt,
    Ref<VectorExchanger>  vex ) :

  inner_     ( rt ),
  exchanger_ ( vex )

{
  JEM_PRECHECK ( rt != NIL );

  myName_ = rt->getName ();

  connect_ ();
}


MPRestrictor::~MPRestrictor ()
{}


//-----------------------------------------------------------------------
//   resetEvents
//-----------------------------------------------------------------------


void MPRestrictor::resetEvents ()
{
  inner_->resetEvents ();

  if ( exchanger_ != NIL )
  {
    exchanger_->getRecvBorders().resetEvents ();
    exchanger_->getSendBorders().resetEvents ();
  }
}


//-----------------------------------------------------------------------
//   shape
//-----------------------------------------------------------------------


AbstractMatrix::Shape MPRestrictor::shape () const
{
  return inner_->shape ();
}


//-----------------------------------------------------------------------
//   matmul
//-----------------------------------------------------------------------


void MPRestrictor::matmul

  ( const Vector&  lhs,
    const Vector&  rhs ) const

{
  if ( ! updated_ )
  {
    const_cast<Self*> ( this ) -> update_ ();
  }

  if ( exchanger_ == NIL )
  {
    inner_->matmul ( lhs, rhs );
  }
  else
  {
    const idx_t  dofCount = vbuffer_.size ();

    if ( rhs.size() != dofCount )
    {
      sizeError ( JEM_FUNC, "rhs vector", rhs.size(), dofCount );
    }

    vbuffer_ = rhs * weights_;

    inner_->matmul ( lhs, vbuffer_ );
  }
}


//-----------------------------------------------------------------------
//   transmul
//-----------------------------------------------------------------------


void MPRestrictor::transmul

  ( const Vector&  lhs,
    const Vector&  rhs ) const

{
  if ( ! updated_ )
  {
    const_cast<Self*> ( this ) -> update_ ();
  }

  if ( exchanger_ == NIL )
  {
    inner_->transmul ( lhs, rhs );
  }
  else
  {
    exchanger_->startOne ( SCATTER );
    inner_    ->transmul ( lhs, rhs );

    lhs *= weights_;

    exchanger_->endOne   ( lhs );
  }
}


//-----------------------------------------------------------------------
//   multiTransmul
//-----------------------------------------------------------------------


void MPRestrictor::multiTransmul

  ( Matrix&           lhsVecs,
    IdxVector&        lhsTags,
    const Matrix&     rhsVecs,
    const IdxVector&  rhsTags ) const

{
  using jem::reshape;

  JEM_PRECHECK ( rhsVecs.size(1) == rhsTags.size() );

  if ( ! updated_ )
  {
    const_cast<Self*> ( this ) -> update_ ();
  }

  const idx_t  rhsCount = rhsTags .size ();
  const idx_t  dofCount = vbuffer_.size ();

  if ( exchanger_ == NIL )
  {
    lhsVecs.resize ( dofCount, rhsCount );
    lhsTags.resize ( rhsCount );

    lhsTags = rhsTags;

    for ( idx_t j = 0; j < rhsCount; j++ )
    {
      inner_->transmul ( lhsVecs[j], rhsVecs[j] );
    }
  }
  else
  {
    Shape   tmpShape ( dofCount, rhsCount );
    Matrix  tmpVecs;

    exchanger_->startSome ( SCATTER, rhsTags );

    if ( rhsCount == 1 )
    {
      tmpVecs.ref    ( reshape( vbuffer_, tmpShape ) );
    }
    else
    {
      tmpVecs.resize ( tmpShape );
    }

    for ( idx_t j = 0; j < rhsCount; j++ )
    {
      inner_->transmul ( tmpVecs[j], rhsVecs[j] );

      tmpVecs[j] *= weights_;
    }

    exchanger_->endSome ( lhsVecs, lhsTags, tmpVecs, rhsTags );
  }
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void MPRestrictor::configure ( const Properties& props )
{
  inner_->configure ( props );
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void MPRestrictor::getConfig ( const Properties& conf ) const
{
  inner_->getConfig ( conf );
}


//-----------------------------------------------------------------------
//   hasTrait
//-----------------------------------------------------------------------


bool MPRestrictor::hasTrait ( const String& trait ) const
{
  using jive::algebra::MatrixTraits;

  bool  result = false;

  if      ( trait == MatrixTraits::SYMMETRIC )
  {
    result = inner_->hasTrait ( trait );
  }
  else if ( trait == MatrixTraits::DISTRIBUTED )
  {
    if ( exchanger_ == NIL )
    {
      result = false;
    }
    else
    {
      result = exchanger_->isDistributed ();
    }
  }

  return result;
}


//-----------------------------------------------------------------------
//   getExtByID
//-----------------------------------------------------------------------


void* MPRestrictor::getExtByID ( ExtensionID extID ) const
{
  if ( extID == MPMatrixExt::ID )
  {
    MPMatrixExt*  ext = const_cast<Self*> ( this );

    return ext;
  }
  else
  {
    return inner_->getExtByID ( extID );
  }
}


//-----------------------------------------------------------------------
//   getExchanger
//-----------------------------------------------------------------------


VectorExchanger* MPRestrictor::getExchanger () const
{
  return exchanger_.get ();
}


//-----------------------------------------------------------------------
//   getExchangeMode
//-----------------------------------------------------------------------


int MPRestrictor::getExchangeMode () const
{
  return SCATTER;
}


//-----------------------------------------------------------------------
//   getWeights
//-----------------------------------------------------------------------


Vector MPRestrictor::getWeights () const
{
  if ( ! updated_ )
  {
    const_cast<Self*> ( this ) -> update_ ();
  }

  return weights_;
}


//-----------------------------------------------------------------------
//   connect_
//-----------------------------------------------------------------------


void MPRestrictor::connect_ ()
{
  using jem::util::connect;

  updated_ = false;

  connect ( inner_->newValuesEvent, this, & Self::valuesChanged_ );
  connect ( inner_->newStructEvent, this, & Self::structChanged_ );

  if ( exchanger_ != NIL )
  {
    connect ( exchanger_->getRecvBorders().changeEvent(),
              this, & Self::valuesChanged_ );

    if ( exchanger_->hasOverlap() )
    {
      connect ( exchanger_->getSendBorders().changeEvent(),
                this, & Self::valuesChanged_ );
    }
  }
}


//-----------------------------------------------------------------------
//   update_
//-----------------------------------------------------------------------


void MPRestrictor::update_ ()
{
  // This is to avoid infinite recursions:

  if ( updated_ )
  {
    return;
  }

  updated_ = true;

  if ( exchanger_ != NIL )
  {
    const DofSpace*  dofs     = exchanger_->getDofSpace ();
    const idx_t        dofCount = dofs      ->dofCount    ();

    if ( dofCount != inner_->size(1) )
    {
      throw jem::IllegalArgumentException (
        getContext (),
        "dof space incompatible with restriction operator "
        "(size mismatch)"
      );
    }

    vbuffer_.resize ( dofCount );
    weights_.resize ( dofCount );

    vbuffer_ = 0.0;
    weights_ = 1.0;

    exchanger_->scatter ( weights_ );

    weights_ = 1.0 / weights_;
  }

  updated_ = true;

  Self::resetEvents ();
}


//-----------------------------------------------------------------------
//   invalidate_
//-----------------------------------------------------------------------


void MPRestrictor::invalidate_ ()
{
  Vector  empty;

  updated_ = false;

  vbuffer_.ref ( empty );
  weights_.ref ( empty );
}


//-----------------------------------------------------------------------
//   valuesChanged_
//-----------------------------------------------------------------------


void MPRestrictor::valuesChanged_ ()
{
  newValuesEvent.emit ( *this );
}


//-----------------------------------------------------------------------
//   structChanged_
//-----------------------------------------------------------------------


void MPRestrictor::structChanged_ ()
{
  invalidate_ ();

  newStructEvent.emit ( *this );
}


JIVE_END_PACKAGE( solver )
