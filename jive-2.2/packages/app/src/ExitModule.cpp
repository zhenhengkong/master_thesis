
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
#include <jive/app/ModuleFactory.h>
#include <jive/app/ExitModule.h>


JEM_DEFINE_CLASS( jive::app::ExitModule );


JIVE_BEGIN_PACKAGE( app )


//=======================================================================
//   class ExitModule
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  ExitModule::TYPE_NAME = "Exit";


//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


ExitModule::ExitModule ( const String& name ) :

  Super ( name )

{}


ExitModule::~ExitModule ()
{}


//-----------------------------------------------------------------------
//   run
//-----------------------------------------------------------------------


Module::Status ExitModule::run ( const Properties& )
{
  return EXIT;
}


//-----------------------------------------------------------------------
//   makeNew
//-----------------------------------------------------------------------


Ref<Module> ExitModule::makeNew

  ( const String&      name,
    const Properties&  conf,
    const Properties&  props,
    const Properties&  globdat )

{
  return jem::newInstance<Self> ( name );
}


//-----------------------------------------------------------------------
//   declare
//-----------------------------------------------------------------------


void ExitModule::declare ()
{
  ModuleFactory::declare ( TYPE_NAME,  & makeNew );
  ModuleFactory::declare ( CLASS_NAME, & makeNew );
}


JIVE_END_PACKAGE( app )
