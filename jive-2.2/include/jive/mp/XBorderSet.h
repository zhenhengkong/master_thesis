
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

#ifndef JIVE_MP_XBORDERSET_H
#define JIVE_MP_XBORDERSET_H

#include <jive/util/StorageMode.h>
#include <jive/util/XGroupSet.h>
#include <jive/mp/BorderSet.h>


JIVE_BEGIN_PACKAGE( mp )


//-----------------------------------------------------------------------
//   class XBorderSet
//-----------------------------------------------------------------------


class XBorderSet : public BorderSet
{
 public:

  typedef XBorderSet        Self;
  typedef BorderSet         Super;
  typedef util::XGroupSet   Data;


  inline                    XBorderSet    ();

  explicit inline           XBorderSet

    ( const Ref<Data>&        borders );

  inline Data*              getData       () const;
  inline Self               clone         () const;
  inline void               clear         () const;

  inline void               reserve

    ( idx_t                   count )        const;

  inline void               trimToSize    () const;

  inline idx_t              addBorder

    ( const IdxVector&        inodes   )     const;

  inline idx_t              addBorder

    ( idx_t                   borderID,
      const IdxVector&        inodes   )     const;

  inline void               eraseBorders

    ( const IdxVector&        iborders )     const;

  inline void               reorderBorders

    ( const Reordering&       reord )        const;

  inline void               setBorderItems

    ( idx_t                   iborder,
      const IdxVector&        iitems )       const;

  static Self               find

    ( BorderKind              kind,
      const Ref<ItemSet>&     items,
      const Properties&       globdat );

  static Self               get

    ( BorderKind              kind,
      const Ref<ItemSet>&     items,
      const Properties&       globdat );


 private:

  inline Data&              xborders_       () const;

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


XBorderSet                  newXBorderSet

  ( BorderKind                kind,
    const Ref<ItemSet>&       items,
    util::StorageMode         mode = util::DEFAULT_STORAGE );

inline XBorderSet           newXBorderSet

  ( BorderKind                kind,
    const ItemSetSkin&        items,
    util::StorageMode         mode = util::DEFAULT_STORAGE );

XBorderSet                  newXBorderSet

  ( const String&             name,
    const Ref<ItemSet>&       items,
    util::StorageMode         mode = util::DEFAULT_STORAGE );

inline XBorderSet           newXBorderSet

  ( const String&             name,
    const ItemSetSkin&        items,
    util::StorageMode         mode = util::DEFAULT_STORAGE );




//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class XBorderSet
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


inline XBorderSet::XBorderSet ()
{}


inline XBorderSet::XBorderSet ( const Ref<Data>& borders ) :

    Super ( borders )

{}


//-----------------------------------------------------------------------
//   getData
//-----------------------------------------------------------------------


inline XBorderSet::Data* XBorderSet::getData () const
{
  return jem::staticCast<Data*> ( data_ );
}


//-----------------------------------------------------------------------
//   clone
//-----------------------------------------------------------------------


inline XBorderSet XBorderSet::clone () const
{
  return Self ( jem::checkedCast<Data>( data_->clone() ) );
}


//-----------------------------------------------------------------------
//   clear
//-----------------------------------------------------------------------


inline void XBorderSet::clear () const
{
  xborders_().clear ();
}


//-----------------------------------------------------------------------
//   reserve
//-----------------------------------------------------------------------


inline void XBorderSet::reserve ( idx_t n ) const
{
  xborders_().reserve ( n );
}


//-----------------------------------------------------------------------
//   trimToSize
//-----------------------------------------------------------------------


inline void XBorderSet::trimToSize () const
{
  xborders_().trimToSize ();
}


//-----------------------------------------------------------------------
//   addBorder
//-----------------------------------------------------------------------


inline idx_t XBorderSet::addBorder

  ( const IdxVector&  inodes ) const

{
  return xborders_().addGroup ( inodes );
}


inline idx_t XBorderSet::addBorder

  ( idx_t             borderID,
    const IdxVector&  inodes ) const

{
  return xborders_().addGroup ( borderID, inodes );
}


//-----------------------------------------------------------------------
//   eraseBorders
//-----------------------------------------------------------------------


inline void XBorderSet::eraseBorders

  ( const IdxVector&  iborders ) const

{
  xborders_().eraseGroups ( iborders );
}


//-----------------------------------------------------------------------
//   reorderBorders
//-----------------------------------------------------------------------


inline void XBorderSet::reorderBorders

  ( const Reordering&  reord ) const

{
  xborders_().reorderGroups ( reord );
}


//-----------------------------------------------------------------------
//   setBorderItems
//-----------------------------------------------------------------------


inline void XBorderSet::setBorderItems

  ( idx_t             iborder,
    const IdxVector&  iitems ) const

{
  xborders_().setGroupMembers ( iborder, iitems );
}


//-----------------------------------------------------------------------
//   xborders_
//-----------------------------------------------------------------------


inline XBorderSet::Data& XBorderSet::xborders_ () const
{
  return jem::staticCast<Data&> ( data_ );
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   newXBorderSet
//-----------------------------------------------------------------------


inline XBorderSet         newXBorderSet

  ( BorderKind              kind,
    const ItemSetSkin&      items,
    util::StorageMode       mode )

{
  return newXBorderSet ( kind, items.getData(), mode );
}


inline XBorderSet         newXBorderSet

  ( const jem::String&      name,
    const ItemSetSkin&      items,
    util::StorageMode       mode )

{
  return newXBorderSet ( name, items.getData(), mode );
}


JIVE_END_PACKAGE( mp )

#endif
