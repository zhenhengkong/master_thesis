
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


#include <jem/base/Class.h>
#include <jem/base/IllegalInputException.h>
#include <jem/util/Properties.h>
#include <jive/util/AllItemGroup.h>
#include <jive/util/ArrayItemGroup.h>
#include <jive/util/EmptyItemGroup.h>
#include <jive/fem/import.h>
#include <jive/fem/Globdat.h>
#include <jive/fem/ElementGroup.h>


JIVE_BEGIN_PACKAGE( fem )


using jem::newInstance;


//=======================================================================
//   class ElementGroup
//=======================================================================

//-----------------------------------------------------------------------
//   get
//-----------------------------------------------------------------------


ElementGroup ElementGroup::get

  ( const Properties&  conf,
    const Properties&  props,
    const Properties&  globdat,
    const String&      context )

{
  String  name;

  props.get ( name, "elements" );
  conf .set ( "elements", name );

  Ref<Data>  data =

    Data::find ( name, Globdat::ELEMENTS, globdat );

  if ( data == NIL || ! isValidData( data ) )
  {
    throw jem::IllegalInputException (
      context,
      String::format (
        "undefined element group: `%s\'", name
      )
    );
  }

  return ElementGroup ( data );
}


//-----------------------------------------------------------------------
//   isValidData
//-----------------------------------------------------------------------


bool ElementGroup::isValidData ( const Ref<Data>& data )
{
  using jem::Class;

  typedef NodeSet   ::Data  NodeData;
  typedef ElementSet::Data  ElemData;

  if ( data == NIL )
  {
    return true;
  }

  ItemSet*  items = data ->getItems ();
  Class*    type  = items->getClass ();

  if ( ! ElemData::getType()->isBaseOf( type ) )
  {
    return false;
  }

  items = static_cast<ElemData*>( items )->getGroupedItems ();
  type  = items->getClass ();

  return NodeData::getType()->isBaseOf ( type );
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   newElementGroup
//-----------------------------------------------------------------------


ElementGroup          newElementGroup

  ( const ElementSet&   elems )

{
  using jive::util::EmptyItemGroup;

  JEM_PRECHECK ( elems != NIL );

  return ElementGroup

    ( newInstance<EmptyItemGroup>( elems.getData() ) );
}


ElementGroup          newElementGroup

  ( const SliceAll&     all,
    const ElementSet&   elems )

{
  using jive::util::AllItemGroup;

  JEM_PRECHECK ( elems != NIL );

  return ElementGroup

    ( newInstance<AllItemGroup>( elems.getData() ) );
}


ElementGroup          newElementGroup

  ( const IdxVector&    ielems,
    const ElementSet&   elems )

{
  using jive::util::ArrayItemGroup;

  JEM_PRECHECK ( elems != NIL );

  return ElementGroup

    ( newInstance<ArrayItemGroup>( ielems, elems.getData() ) );
}


JIVE_END_PACKAGE( fem )