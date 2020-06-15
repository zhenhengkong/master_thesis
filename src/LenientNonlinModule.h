/*
 * 
 *  Copyright (C) 2014 TU Delft. All rights reserved.
 *  
 *  F.P. van der Meer, April 2014
 *
 *  Iterative solver that optionally continues with non-converged solution
 *
 */

#ifndef LENIENT_NONLIN_MODULE_H
#define LENIENT_NONLIN_MODULE_H

#include <jem/util/Properties.h>

#include <jive/implict/NonlinModule.h>
#include <jive/model/Model.h>

using jem::String;
using jem::Ref;

using jem::util::Properties;
using jive::app::Module;
using jive::model::Model;
using jive::implict::NonlinModule;

//-----------------------------------------------------------------------
//   class NonlinModule
//-----------------------------------------------------------------------


class LenientNonlinModule : public NonlinModule
{
 public:

  typedef LenientNonlinModule  Self;
  typedef NonlinModule         Super;

  static const char*           TYPE_NAME;

   // public member functions 

  explicit                  LenientNonlinModule

    ( const String&           name    = "lenient" );

  static Ref<Module>        makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );

  virtual Status            init

    ( const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );

  virtual Status            run

    ( const Properties&       globdat );

  virtual void              configure

    ( const Properties&       props,
      const Properties&       globdat );

  virtual void              getConfig

    ( const Properties&       conf,
      const Properties&       globdat )      const;

 protected:

  virtual                  ~LenientNonlinModule  ();

 private:

};


#endif
