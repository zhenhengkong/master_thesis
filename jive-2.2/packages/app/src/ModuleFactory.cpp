
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
#include <jive/app/ModuleFactory.h>


JIVE_BEGIN_PACKAGE( app )


//=======================================================================
//   class ModuleFactory::CtorMap_
//=======================================================================


class ModuleFactory::CtorMap_ : public util::CtorMap<Constructor>
{};


//=======================================================================
//   class ModuleFactory
//=======================================================================

//-----------------------------------------------------------------------
//   declare
//-----------------------------------------------------------------------


void ModuleFactory::declare

  ( const String&  type,
    Constructor    ctor )

{
  JEM_PRECHECK ( ctor );

  CtorMap_::insert ( type, ctor );
}


//-----------------------------------------------------------------------
//   exists
//-----------------------------------------------------------------------


bool ModuleFactory::exists ( const String& type )
{
  return CtorMap_::contains ( type );
}


//-----------------------------------------------------------------------
//   listKnownTypes
//-----------------------------------------------------------------------


StringVector ModuleFactory::listKnownTypes ()
{
  return CtorMap_::listKnownTypes ();
}


//-----------------------------------------------------------------------
//   newInstance
//-----------------------------------------------------------------------


Ref<Module> ModuleFactory::newInstance

  ( const String&      type,
    const String&      name,
    const Properties&  conf,
    const Properties&  props,
    const Properties&  globdat )

{
  Constructor ctor = CtorMap_::find ( type );

  if ( ctor )
  {
    return ctor ( name, conf, props, globdat );
  }
  else
  {
    return NIL;
  }
}


Ref<Module> ModuleFactory::newInstance

  ( const String&      name,
    const Properties&  conf,
    const Properties&  props,
    const Properties&  globdat )

{
  Properties   subConf  = conf.makeProps ( name );
  Properties   subProps = props.getProps ( name );

  Ref<Module>  mod;
  String       type;


  subProps.get ( type, TYPE_PROP );
  subConf .set ( TYPE_PROP, type );

  mod = newInstance ( type, name, conf, props, globdat );

  if ( mod == NIL )
  {
    noSuchTypeError (
      subProps.getContext ( TYPE_PROP ),
      type, "module",
      CtorMap_::listKnownTypes ()
    );
  }

  return mod;
}


JIVE_END_PACKAGE( app )
