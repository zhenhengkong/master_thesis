
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

#ifndef JIVE_FEM_ELEMENTSET_H
#define JIVE_FEM_ELEMENTSET_H

#include <jive/util/GroupSet.h>
#include <jive/fem/NodeSet.h>


JIVE_BEGIN_PACKAGE( fem )


using jem::util::Event;


//-----------------------------------------------------------------------
//   class ElementSet
//-----------------------------------------------------------------------


class ElementSet : public ItemSetSkin
{
 public:

  typedef ElementSet          Self;
  typedef ItemSetSkin         Super;

  typedef util::GroupSet      Data;
  typedef util::Topology      Topology;


  inline                      ElementSet        ();

  explicit inline             ElementSet

    ( const Ref<Data>&          elems );

  inline Event
    < Data& >&                newTopoEvent      () const;

  inline Data*                getData           () const;
  inline NodeSet              getNodes          () const;

  inline String               getElemPath

    ( idx_t                     elemID )           const;

  inline idx_t                findElement

    ( idx_t                     elemID )           const;

  inline idx_t                getElemID

    ( idx_t                     ielem )            const;

  inline idx_t                maxElemNodeCount  () const;

  inline idx_t                maxElemNodeCountOf

    ( const IdxVector&          ielems )           const;

  inline idx_t                getElemNodeCount

    ( idx_t                     ielem )            const;

  inline idx_t                getElemNodes

    ( const IdxVector&          inodes,
      idx_t                     ielem )            const;

  inline void                 getSomeElemNodes

    ( const IdxVector&          inodes,
      const IdxVector&          index,
      idx_t                     ielem  )           const;

  inline IdxVector            toVector          () const;
  inline Topology             toMatrix          () const;

  inline IdxVector            getNodesOf

    ( const IdxVector&          ielems )           const;

  inline IdxVector            getUniqueNodesOf

    ( const IdxVector&          ielems )           const;

  void                        checkElements

    ( const String&             context,
      idx_t                     nodeCount )        const;

  void                        checkSomeElements

    ( const String&             context,
      const IdxVector&          ielems,
      idx_t                     nodeCount )        const;

  void                        getElemBoxes

    ( const Matrix&             boxes )           const;

  void                        getSomeElemBoxes

    ( const Matrix&             boxes,
      const IdxVector&          ielems )           const;

  void                        store

    ( const Properties&         globdat )          const;

  void                        store

    ( const String&             name,
      const Properties&         globdat )          const;

  static Self                 find

    ( const Properties&         globdat );

  static Self                 find

    ( const String&             name,
      const Properties&         globdat );

  static Self                 get

    ( const Properties&         globdat,
      const String&             context );

  static Self                 get

    ( const String&             name,
      const Properties&         globdat,
      const String&             context );

  static bool                 isValidData

    ( const Ref<Data>&          data );

  static inline void          checkData

    ( const Ref<Data>&          data,
      const String&             context );


 protected:

  inline Data&                elems_            () const;


 private:

  static const char*          CLASS_NAME_;

};





//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class ElementSet
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


inline ElementSet::ElementSet ()
{}


inline ElementSet::ElementSet ( const Ref<Data>& elems ) :

  Super ( elems )

{}


//-----------------------------------------------------------------------
//   newTopoEvent
//-----------------------------------------------------------------------


inline Event < util::GroupSet& >& ElementSet::newTopoEvent () const
{
  return elems_().newTopoEvent;
}


//-----------------------------------------------------------------------
//   getData
//-----------------------------------------------------------------------


inline ElementSet::Data* ElementSet::getData () const
{
  return jem::staticCast<Data*> ( data_ );
}


//-----------------------------------------------------------------------
//   getNodes
//-----------------------------------------------------------------------


inline NodeSet ElementSet::getNodes () const
{
  return NodeSet

    ( static_cast<NodeSet::Data*> ( elems_().getGroupedItems() ) );
}


//-----------------------------------------------------------------------
//   getElemPath
//-----------------------------------------------------------------------


inline String ElementSet::getElemPath ( idx_t elemID ) const
{
  return elems_().getItemPath ( elemID );
}


//-----------------------------------------------------------------------
//   findElement
//-----------------------------------------------------------------------


inline idx_t ElementSet::findElement ( idx_t elemID ) const
{
  return elems_().findItem ( elemID );
}


//-----------------------------------------------------------------------
//   getElemID
//-----------------------------------------------------------------------


inline idx_t ElementSet::getElemID ( idx_t ielem  ) const
{
  return elems_().getItemID ( ielem );
}


//-----------------------------------------------------------------------
//   maxElemNodeCount
//-----------------------------------------------------------------------


inline idx_t ElementSet::maxElemNodeCount () const
{
  return elems_().maxGroupSize ();
}


//-----------------------------------------------------------------------
//   maxElemNodeCountOf
//-----------------------------------------------------------------------


inline idx_t ElementSet::maxElemNodeCountOf

  ( const IdxVector& ielems ) const

{
  return elems_().maxGroupSizeOf ( ielems );
}

//-----------------------------------------------------------------------
//   getElemNodeCount
//-----------------------------------------------------------------------


inline idx_t ElementSet::getElemNodeCount

  ( idx_t ielem ) const

{
  return elems_().getGroupSize ( ielem );
}


//-----------------------------------------------------------------------
//   getElemNodes
//-----------------------------------------------------------------------


inline idx_t ElementSet::getElemNodes

  ( const IdxVector&  inodes,
    idx_t               ielem ) const

{
  return elems_().getGroupMembers ( inodes, ielem );
}


//-----------------------------------------------------------------------
//   getSomeElemNodes
//-----------------------------------------------------------------------


inline void ElementSet::getSomeElemNodes

  ( const IdxVector&  inodes,
    const IdxVector&  index,
    idx_t               ielem  ) const

{
  elems_().getSomeGroupMembers ( inodes, index, ielem );
}


//-----------------------------------------------------------------------
//   toVector
//-----------------------------------------------------------------------


inline IdxVector ElementSet::toVector () const
{
  return elems_().toVector ();
}


//-----------------------------------------------------------------------
//   toMatrix
//-----------------------------------------------------------------------


inline ElementSet::Topology ElementSet::toMatrix () const
{
  return elems_().toMatrix ();
}


//-----------------------------------------------------------------------
//   getNodesOf
//-----------------------------------------------------------------------


inline IdxVector ElementSet::getNodesOf

  ( const IdxVector&  ielems ) const

{
  return elems_().getMembersOf ( ielems );
}


//-----------------------------------------------------------------------
//   getUniqueNodesOf
//-----------------------------------------------------------------------


inline IdxVector ElementSet::getUniqueNodesOf

  ( const IdxVector&  ielems ) const

{
  return elems_().getUniqueMembersOf ( ielems );
}


//-----------------------------------------------------------------------
//   checkData
//-----------------------------------------------------------------------


inline void ElementSet::checkData

  ( const Ref<Data>&  data,
    const String&     context )

{
  if ( ! isValidData( data ) )
  {
    dataError ( context, "element", data );
  }
}


//-----------------------------------------------------------------------
//   elems_
//-----------------------------------------------------------------------


inline ElementSet::Data& ElementSet::elems_ () const
{
  return jem::staticCast<Data&> ( data_ );
}


JIVE_END_PACKAGE( fem )

#endif
