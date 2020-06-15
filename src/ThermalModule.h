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

#ifndef THERMAL_MODULE_H
#define THERMAL_MODULE_H


#include <jem/util/Properties.h>
#include <jive/app/Module.h>

using jem::Ref;
using jem::idx_t;
using jem::String;
using jem::util::Properties;
using jive::app::Module;


//-----------------------------------------------------------------------
//   class ThermalModule
//-----------------------------------------------------------------------


class ThermalModule : public Module
{
 public:

  typedef ThermalModule     Self;
  typedef Module            Super;

  static const char*        TYPE_NAME;
  static const char*        N_CURE_PROP;
  static const char*        DELTA_T_PROP;

  explicit                  ThermalModule

    ( const String&           name   = "thermal" );

  virtual Status            init

    ( const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );

  virtual Status            run

    ( const Properties&       globdat );

  virtual void              shutdown

    ( const Properties&       globdat );

  virtual void              configure

    ( const Properties&       props,
      const Properties&       globdat );

  virtual void              getConfig

    ( const Properties&       props,
      const Properties&       globdat )    const;

  static Ref<Module>        makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );

 protected:

  virtual                  ~ThermalModule  ();

 private:

  idx_t                     nCure_;
  double                    deltaT_;
};


#endif
