
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
#include <jive/algebra/MBuilderFactory.h>


JIVE_BEGIN_PACKAGE( algebra )


//=======================================================================
//   class MBuilderFactory::CtorMap_
//=======================================================================


class MBuilderFactory::CtorMap_ : public util::CtorMap<Constructor>
{};


//=======================================================================
//   class MBuilderFactory
//=======================================================================

//-----------------------------------------------------------------------
//   declare
//-----------------------------------------------------------------------


void MBuilderFactory::declare

  ( const String&  type,
    Constructor    ctor )

{
  JEM_PRECHECK2 ( ctor, "NULL constructor function pointer" );

  CtorMap_::insert ( type, ctor );
}


//-----------------------------------------------------------------------
//   exists
//-----------------------------------------------------------------------


bool MBuilderFactory::exists ( const String& type )
{
  return CtorMap_::contains ( type );
}


//-----------------------------------------------------------------------
//   listKnownTypes
//-----------------------------------------------------------------------


StringVector MBuilderFactory::listKnownTypes ()
{
  return CtorMap_::listKnownTypes ();
}


//-----------------------------------------------------------------------
//   newInstance
//-----------------------------------------------------------------------


Ref<MatrixBuilder> MBuilderFactory::newInstance

  ( const String&      type,
    const String&      name,
    const Properties&  conf,
    const Properties&  props,
    const Properties&  params,
    const Properties&  globdat )

{
  Constructor ctor = CtorMap_::find ( type );

  if ( ctor )
  {
    return ctor ( name, conf, props, params, globdat );
  }
  else
  {
    return NIL;
  }
}


Ref<MatrixBuilder> MBuilderFactory::newInstance

  ( const String&      name,
    const Properties&  conf,
    const Properties&  props,
    const Properties&  params,
    const Properties&  globdat )

{
  Properties     subConf  = conf .makeProps ( name );
  Properties     subProps = props.getProps  ( name );

  Ref<MBuilder>  mbuilder;
  String         type;


  subProps.get ( type, TYPE_PROP );
  subConf .set ( TYPE_PROP, type );

  mbuilder = newInstance ( type, name, conf, props, params, globdat );

  if ( mbuilder == NIL )
  {
    if ( CtorMap_::contains( type ) )
    {
      subProps.propertyError (
        TYPE_PROP,
        String::format (
          "invalid matrix type: `%s\' (invalid parameters)", type
        )
      );
    }
    else
    {
      noSuchTypeError (
        subProps.getContext ( TYPE_PROP ),
        type, "matrix",
        CtorMap_::listKnownTypes ()
      );
    }
  }

  return mbuilder;
}


JIVE_END_PACKAGE( algebra )
