/*
 * 
 *  Copyright (C) 2009 TU Delft. All rights reserved.
 *  
 *  This class implements a module for computations with thermal strain.
 *  The ThermalModule should be placed before the SolverModule in the 
 *  ModuleChain
 *
 *  A temperature jump is applied in one or a number of initial time steps.
 *  The value of the temperature jump is stored in globdat, to be accessed 
 *  in the constitutive models. 
 *
 *  During the thermal process, takeAction_->DISINCREMENT is called to tell 
 *  the model not to apply an increment during this time step
 *
 *  In each regular time step the takeAction_->GET_FORCE is called to 
 *  assemble the thermal force vector which is required for the energy 
 *  arclength method. 
 *
 *
 *  Author: Frans van der Meer, F.P.vanderMeer@tudelft.nl
 *  Date: February 2009
 *
 */

#include <jem/base/System.h>
#include <jive/util/Globdat.h>
#include <jive/model/Model.h>
#include <jive/implict/SolverInfo.h>
#include <jive/app/ModuleFactory.h>

#include "ThermalModule.h"
#include "SolverNames.h"
#include "ThermNames.h"


using jem::System;
using jem::newInstance;
using jem::io::endl;
using jive::Vector;
using jive::model::Model;
using jive::util::Globdat;


//=======================================================================
//   class ThermalModule
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------

const char* ThermalModule::TYPE_NAME    = "Thermal";
const char* ThermalModule::N_CURE_PROP  = "nCure";
const char* ThermalModule::DELTA_T_PROP = "deltaT";

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------

ThermalModule::ThermalModule 

  ( const String&  name ) :

      Super   ( name   )

{
  nCure_   = 1;
  deltaT_  = 0.;
}

ThermalModule::~ThermalModule ()
{}


//-----------------------------------------------------------------------
//   init
//-----------------------------------------------------------------------


Module::Status ThermalModule::init

  ( const Properties&  conf,
    const Properties&  props,
    const Properties&  globdat )

{
  Properties  myConf  = conf .makeProps ( myName_ );
  Properties  myProps = props.findProps ( myName_ );

  myConf.set   ( "type" , TYPE_NAME );

  myProps.find ( nCure_, N_CURE_PROP, 1, 100 );
  myConf.set   ( N_CURE_PROP, nCure_         );

  myProps.get  ( deltaT_, DELTA_T_PROP );
  myConf.set   ( DELTA_T_PROP, deltaT_ );

  return OK;
}


//-----------------------------------------------------------------------
//   run
//-----------------------------------------------------------------------


Module::Status ThermalModule::run ( const Properties& globdat )
{
  Ref<Model>  model = Model::get ( globdat, getContext() );
  Properties  params;

  idx_t       it;

  globdat.get ( it, Globdat::TIME_STEP );

  if ( it < nCure_ )
  {
    // apply temperature increment
    // and cancel load/disp increment during curing steps
    // [actually apply negative increment, not tested against adaptive
    //  strategies and switches etc]

    double dT = ( deltaT_ * ( it + 1 ) ) / nCure_;

    params.set ( ThermNames::DELTA_T,  dT );

    System::out() << "This is a curing time step, DeltaT set to: "
                  << dT << "\n\n";

    model->takeAction ( ThermNames::APPLY_TEMP, params, globdat );
  }
  else
  {
    // update thermal force vector (needed to compute energy dissipation)
    // [better not do this during curing because fth0 causes nonzero
    // energy dissipation, which may trigger a switch, undesirably!]
    // This is only done when the force vector has been made by the
    // FlexArclenModule. It is also set to zero there.
    
    Vector fDiss;

    if ( globdat.find ( fDiss, SolverNames::DISSIPATION_FORCE ) )
    {
      model->takeAction ( ThermNames::GET_FORCE, params, globdat );
    }
  }

  return OK;
}


//-----------------------------------------------------------------------
//   shutdown
//-----------------------------------------------------------------------


void ThermalModule::shutdown ( const Properties& globdat )
{
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void ThermalModule::configure

  ( const Properties&  props,
    const Properties&  globdat )

{
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void ThermalModule::getConfig

  ( const Properties&  conf,
    const Properties&  globdat ) const

{
}


//-----------------------------------------------------------------------
//   makeNew
//-----------------------------------------------------------------------

Ref<Module>  ThermalModule::makeNew

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
//   declareThermalModule
//-----------------------------------------------------------------------

void declareThermalModule ()
{
  using jive::app::ModuleFactory;

  ModuleFactory::declare ( ThermalModule::TYPE_NAME,  
                         & ThermalModule::makeNew );
}

