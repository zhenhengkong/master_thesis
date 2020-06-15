
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
#include <jem/base/System.h>
#include <jem/base/ClassTemplate.h>
#include <jem/mp/utilities.h>
#include <jem/util/Event.h>
#include <jive/log.h>
#include <jive/util/error.h>
#include <jive/util/DummyItemSet.h>
#include <jive/mp/VectorExchanger.h>
#include <jive/solver/MPRestrictor.h>
#include <jive/solver/CoarseDofSpace.h>


JEM_DEFINE_CLASS( jive::solver::CoarseDofSpace );


JIVE_BEGIN_PACKAGE( solver )


using jem::checkedCast;
using jive::util::indexError;
using jive::util::DummyItemSet;


//=======================================================================
//   class CoarseDofSpace
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  CoarseDofSpace::DOF_TYPE_NAME = "v";

const int    CoarseDofSpace::UPDATED_      = 1 << 0;
const int    CoarseDofSpace::DISTRIBUTED_  = 1 << 1;


//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


CoarseDofSpace::CoarseDofSpace ( Ref<Restrictor> rt ) :

  rtor_ ( rt )

{
  using jive::util::newDummyItemSet;

  JEM_PRECHECK ( rt != NIL );

  status_   = 0;
  firstDof_ = 0;
  items_    = newDummyItemSet ( "coarseSpace", "row" );

  connect_ ();
}


CoarseDofSpace::~CoarseDofSpace ()
{}


//-----------------------------------------------------------------------
//   typeCount
//-----------------------------------------------------------------------


idx_t CoarseDofSpace::typeCount () const
{
  return 1;
}


//-----------------------------------------------------------------------
//   dofCount
//-----------------------------------------------------------------------


idx_t CoarseDofSpace::dofCount () const
{
  return items_->size ();
}


//-----------------------------------------------------------------------
//   globalDofCount
//-----------------------------------------------------------------------


idx_t CoarseDofSpace::globalDofCount () const
{
  using jem::mp::allsum;

  idx_t  count = items_->size ();

  if ( status_ & DISTRIBUTED_ )
  {
    MPRestrictor*  mpr = checkedCast<MPRestrictor*> ( rtor_ );
    MPContext*     mpx = mpr->getExchanger()->getMPContext ();

    count = allsum ( *mpx, count );
  }

  return count;
}


//-----------------------------------------------------------------------
//   resetEvents
//-----------------------------------------------------------------------


void CoarseDofSpace::resetEvents ()
{
  rtor_->resetEvents ();
}


//-----------------------------------------------------------------------
//   findType
//-----------------------------------------------------------------------


idx_t CoarseDofSpace::findType ( const String& name ) const
{
  if ( name == DOF_TYPE_NAME )
  {
    return 0;
  }
  else
  {
    return -1;
  }
}


//-----------------------------------------------------------------------
//   getTypeName
//-----------------------------------------------------------------------


String CoarseDofSpace::getTypeName ( idx_t itype ) const
{
  if ( itype < 0 || itype > 0 )
  {
    indexError ( JEM_FUNC, "DOF type", itype, 1 );
  }

  return DOF_TYPE_NAME;
}


//-----------------------------------------------------------------------
//   getTypeNames
//-----------------------------------------------------------------------


StringVector CoarseDofSpace::getTypeNames () const
{
  StringVector  names ( 1 );

  names[0] = DOF_TYPE_NAME;

  return names;
}


//-----------------------------------------------------------------------
//   findDofIndex
//-----------------------------------------------------------------------


idx_t CoarseDofSpace::findDofIndex

  ( idx_t  iitem,
    idx_t  itype ) const

{
  if ( status_ & UPDATED_ )
  {
    return iitem;
  }
  else
  {
    return -1;
  }
}


//-----------------------------------------------------------------------
//   findDofIndices
//-----------------------------------------------------------------------


idx_t CoarseDofSpace::findDofIndices

  ( const IdxVector&  idofs,
    const IdxVector&  iitems,
    const IdxVector&  itypes ) const

{
  JEM_PRECHECK ( idofs.size() >= iitems.size() * itypes.size() );

  idx_t  k = 0;

  if ( status_ & UPDATED_ )
  {
    const idx_t  m = iitems.size ();
    const idx_t  n = itypes.size ();

    if ( n == 1 )
    {
      if ( idofs.size() == m )
      {
        idofs = iitems;
      }
      else
      {
        idofs[slice(BEGIN,m)] = iitems;
      }

      k = m;
    }
    else
    {
      for ( idx_t i = 0; i < m; i++ )
      {
        idx_t  iitem = iitems[i];

        for ( idx_t j = 0; j < n; j++, k++ )
        {
          idofs[k] = iitem;
        }
      }
    }
  }

  return k;
}


//-----------------------------------------------------------------------
//   collectDofIndices
//-----------------------------------------------------------------------


idx_t CoarseDofSpace::collectDofIndices

  ( const IdxVector&  idofs,
    const IdxVector&  iitems,
    const IdxVector&  itypes ) const

{
  return Self::findDofIndices ( idofs, iitems, itypes );
}


//-----------------------------------------------------------------------
//   getDofsForItem
//-----------------------------------------------------------------------


idx_t CoarseDofSpace::getDofsForItem

  ( const IdxVector&  idofs,
    const IdxVector&  itypes,
    idx_t               iitem ) const

{
  JEM_PRECHECK ( idofs.size() >= itypes.size() );

  idx_t  k = 0;

  if ( (status_ & UPDATED_) && itypes.size() > 0 )
  {
    idofs [0] = iitem;
    itypes[0] = 0;

    k++;
  }

  return k;
}


//-----------------------------------------------------------------------
//   getDofsForType
//-----------------------------------------------------------------------


idx_t CoarseDofSpace::getDofsForType

  ( const IdxVector&  idofs,
    const IdxVector&  iitems,
    idx_t               itype ) const

{
  JEM_PRECHECK ( idofs.size() >= iitems.size() );

  idx_t  k = 0;

  if ( itype != 0 )
  {
    indexError ( JEM_FUNC, "DOF type", itype, 1 );
  }

  if ( status_ & UPDATED_ )
  {
    k = jem::min ( iitems.size(), items_->size() );

    for ( idx_t i = 0; i < k; i++ )
    {
      idofs[k] = k;
    }
  }

  return k;
}


//-----------------------------------------------------------------------
//   decodeDofIndex
//-----------------------------------------------------------------------


void CoarseDofSpace::decodeDofIndex

  ( idx_t&  iitem,
    idx_t&  itype,
    idx_t   idof ) const

{
  iitem = idof;
  itype = 0;
}


//-----------------------------------------------------------------------
//   decodeDofIndices
//-----------------------------------------------------------------------


void CoarseDofSpace::decodeDofIndices

  ( const IdxVector&  iitems,
    const IdxVector&  itypes,
    const IdxVector&  idofs ) const

{
  JEM_PRECHECK ( iitems.size() >= idofs.size() &&
                 itypes.size() >= idofs.size() );

  const idx_t  n = idofs.size ();

  if ( iitems.size() == n )
  {
    iitems = idofs;
  }
  else
  {
    iitems[slice(BEGIN,n)] = idofs;
  }

  if ( itypes.size() == n )
  {
    itypes = 0;
  }
  else
  {
    itypes[slice(BEGIN,n)] = 0;
  }
}


//-----------------------------------------------------------------------
//   toMatrix
//-----------------------------------------------------------------------


SparseIdxMatrix CoarseDofSpace::toMatrix () const
{
  using jem::shape;

  const idx_t  rowCount = items_->size ();

  IdxVector    offsets  ( rowCount + 1 );
  IdxVector    itypes   ( rowCount );
  IdxVector    idofs    ( rowCount );


  itypes = JEM_IDX_C(0);

  for ( idx_t i = 0; i < rowCount; i++ )
  {
    offsets[i] = idofs[i] = i;
  }

  offsets[rowCount] = rowCount;

  return SparseIdxMatrix ( shape( rowCount, 1 ),
                           offsets, itypes, idofs );
}


//-----------------------------------------------------------------------
//   getItems
//-----------------------------------------------------------------------


ItemSet* CoarseDofSpace::getItems () const
{
  return items_.get ();
}


//-----------------------------------------------------------------------
//   getMPContext
//-----------------------------------------------------------------------


MPContext* CoarseDofSpace::getMPContext () const
{
  MPContext*  mpx = 0;

  if ( status_ & DISTRIBUTED_ )
  {
    MPRestrictor*  mpr = checkedCast<MPRestrictor*> ( rtor_ );

    mpx = mpr->getExchanger()->getMPContext ();
  }

  return mpx;
}


//-----------------------------------------------------------------------
//   emitEvents_
//-----------------------------------------------------------------------


void CoarseDofSpace::emitEvents_ ( int events )
{}


//-----------------------------------------------------------------------
//   connect_
//-----------------------------------------------------------------------


void CoarseDofSpace::connect_ ()
{
  using jem::dynamicCast;
  using jem::util::connect;

  status_ &= ~UPDATED_;

  if ( dynamicCast<MPRestrictor*>( rtor_ ) )
  {
    status_ |=  DISTRIBUTED_;
  }
  else
  {
    status_ &= ~DISTRIBUTED_;
  }

  rtor_->resetEvents ();

  connect ( rtor_->newStructEvent, this, & Self::invalidate_ );
}


//-----------------------------------------------------------------------
//   update_
//-----------------------------------------------------------------------


void CoarseDofSpace::update_ ()
{
  using jem::mp::recv;
  using jem::mp::send;
  using jive::util::newDummyItemSet;

  const idx_t    localSize = rtor_->size (0);

  DummyItemSet*  items  = checkedCast<DummyItemSet*> ( items_ );


  firstDof_ = 0;

  if ( status_ & DISTRIBUTED_ )
  {
    MPContext*  mpx    = getMPContext ();
    const int   myRank = mpx->myRank  ();

    idx_t       lastDof;


    print ( JIVE_LOG_DEBUG, "Numbering coarse DOFs ...\n" );

    if ( myRank > 0 )
    {
      recv ( *mpx, firstDof_, myRank - 1 );
    }

    lastDof = firstDof_ + localSize;

    if ( myRank < mpx->size() - 1 )
    {
      send ( *mpx, lastDof, myRank + 1 );
    }
  }

  for ( idx_t i = 0; i < localSize; i++ )
  {
    items->addItem ( firstDof_ + i );
  }

  Self::resetEvents ();

  status_ |= UPDATED_;

  newSizeEvent.emit ( localSize, *this );
}


//-----------------------------------------------------------------------
//   invalidate_
//-----------------------------------------------------------------------


void CoarseDofSpace::invalidate_ ()
{
  if ( status_ & UPDATED_ )
  {
    DummyItemSet*  items = checkedCast<DummyItemSet*> ( items_ );

    status_  &= ~UPDATED_;
    firstDof_ =  0;

    items->clear ();

    newSizeEvent.emit ( 0, *this );
  }
}


JIVE_END_PACKAGE( solver )
