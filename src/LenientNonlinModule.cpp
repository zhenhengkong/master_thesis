/*
 * 
 *  Copyright (C) 2014 TU Delft. All rights reserved.
 *  
 *  F.P. van der Meer, April 2014
 *
 *  Iterative solver that optionally continues with non-converged solution
 *
 */

#include <jem/base/Exception.h>
#include <jem/base/System.h>
#include <jive/app/ModuleFactory.h>
#include <jive/implict/SolverInfo.h>

#include "LenientNonlinModule.h"

using jem::newInstance;
using jem::slice;
using jive::implict::SolverInfo;

//=======================================================================
//   class LenientNonlinModule
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------

const char*  LenientNonlinModule::TYPE_NAME = "LNonlin";

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


LenientNonlinModule::LenientNonlinModule ( const String&  name ) :
  Super ( name )

{ 
}


LenientNonlinModule::~LenientNonlinModule ()

{
}


//-----------------------------------------------------------------------
//   init
//-----------------------------------------------------------------------


Module::Status LenientNonlinModule::init

  ( const Properties&  conf,
    const Properties&  props,
    const Properties&  globdat )

{
  return Super::init ( conf, props, globdat );
}


//-----------------------------------------------------------------------
//   run
//-----------------------------------------------------------------------


Module::Status LenientNonlinModule::run 

  ( const Properties& globdat )

{
  using jem::System;

  Properties info = SolverInfo::get ( globdat );
  double backup = getPrecision();

  Status ret = OK;

  while ( true )
  {
    info.clear ();

    advance ( globdat );

    try
    {
      solve ( info, globdat );
    }
    catch ( const jem::Exception& ex )
    {
      if ( ex.what()[slice(0,14)].equals("no convergence") )
      {
        setPrecision ( 10.*getPrecision() );
        cancel ( globdat );
        System::out() << myName_ << 
          " continues with lower precision " << getPrecision() << "\n";
        continue;
      }
      else
      {
        cancel ( globdat );
        throw;
      }
    }

    setPrecision ( backup );

    if ( commit ( globdat ) )
    {
      break;
    }

    print ( System::info ( myName_ ), 
            "Solution rejected; re-trying\n" );

    cancel ( globdat );
  }

  return ret;
}

//-----------------------------------------------------------------------
//   configure()
//-----------------------------------------------------------------------


void LenientNonlinModule::configure

  ( const Properties&  props,
    const Properties&  globdat )

{
  Super::configure ( props, globdat );
}


//-----------------------------------------------------------------------
//   getConfig()
//-----------------------------------------------------------------------


void LenientNonlinModule::getConfig

  ( const Properties&  conf,
    const Properties&  globdat ) const

{
  Super::getConfig ( conf, globdat );
}


//-----------------------------------------------------------------------
//   makeNew
//-----------------------------------------------------------------------

Ref<Module>  LenientNonlinModule::makeNew

  ( const String&           name,
    const Properties&       conf,
    const Properties&       props,
    const Properties&       globdat )

{
  return newInstance<Self> ( name );
}

//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   declareLenientNonlinModule
//-----------------------------------------------------------------------

void declareLenientNonlinModule ()
{
  using jive::app::ModuleFactory;

  ModuleFactory::declare ( LenientNonlinModule::TYPE_NAME,
                         & LenientNonlinModule::makeNew );
}
