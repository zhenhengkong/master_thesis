
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
#include <jem/util/Properties.h>
#include <jive/util/CtorMap.h>
#include <jive/geom/InternalShape.h>
#include <jive/geom/BoundaryShape.h>
#include <jive/geom/IShapeFactory.h>
#include <jive/geom/BShapeFactory.h>
#include <jive/geom/ShapeFactory.h>


JIVE_BEGIN_PACKAGE( geom )


using jive::util::CtorMap;


//=======================================================================
//   class ShapeFactory::CtorMap_
//=======================================================================


class ShapeFactory::CtorMap_ : public CtorMap<Constructor>
{};


//=======================================================================
//   class ShapeFactory
//=======================================================================

//-----------------------------------------------------------------------
//   declare
//-----------------------------------------------------------------------


void ShapeFactory::declare

  ( const String&  type,
    Constructor    ctor )

{
  JEM_PRECHECK2 ( ctor, "NULL construction function pointer" );

  CtorMap_::insert ( type, ctor );
}


//-----------------------------------------------------------------------
//   exists
//-----------------------------------------------------------------------


bool ShapeFactory::exists ( const String& type )
{
  return ( CtorMap_::contains    ( type ) ||
           IShapeFactory::exists ( type ) ||
           BShapeFactory::exists ( type ) );
}


//-----------------------------------------------------------------------
//   listKnownTypes
//-----------------------------------------------------------------------


StringVector ShapeFactory::listKnownTypes ()
{
  StringVector  shapeTypes  = CtorMap_     ::listKnownTypes ();
  StringVector  bshapeTypes = BShapeFactory::listKnownTypes ();
  StringVector  ishapeTypes = IShapeFactory::listKnownTypes ();

  const idx_t   k =     shapeTypes .size ();
  const idx_t   m = k + bshapeTypes.size ();
  const idx_t   n = m + ishapeTypes.size ();

  StringVector  typeList ( n );


  typeList[slice(BEGIN,k)] = shapeTypes;
  typeList[slice(k,m)]     = bshapeTypes;
  typeList[slice(m,END)]   = ishapeTypes;

  return typeList;
}


//-----------------------------------------------------------------------
//   newInstance
//-----------------------------------------------------------------------


Ref<Shape> ShapeFactory::newInstance

  ( const String&      type,
    const String&      name,
    const Properties&  conf,
    const Properties&  props )

{
  Constructor ctor = CtorMap_::find ( type );

  if ( ctor )
  {
    return ctor ( name, conf, props );
  }
  else
  {
    Ref<Shape> shape =

      IShapeFactory::newInstance ( type, name, conf, props );

    if ( shape == NIL )
    {
      shape = BShapeFactory::newInstance ( type, name,
                                           conf, props );
    }

    return shape;
  }
}


Ref<Shape> ShapeFactory::newInstance

  ( const String&      name,
    const Properties&  conf,
    const Properties&  props )

{
  Properties  subConf  = conf .makeProps ( name );
  Properties  subProps = props.getProps  ( name );

  Ref<Shape>  shape;
  String      type;


  subProps.get ( type, TYPE_PROP );
  subConf .set ( TYPE_PROP, type );

  shape = newInstance ( type, name, conf, props );

  if ( shape == NIL )
  {
    noSuchTypeError ( subProps.getContext ( TYPE_PROP ),
                      type,
                      "shape",
                      listKnownTypes() );
  }

  return shape;
}


JIVE_END_PACKAGE( geom )
