
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

#ifndef JIVE_GL_VIEWMODULE_H
#define JIVE_GL_VIEWMODULE_H

#include <jive/app/Module.h>
#include <jive/gl/import.h>


JIVE_BEGIN_PACKAGE( gl )


class Plugin;
class ChainPlugin;


//-----------------------------------------------------------------------
//   class ViewModule
//-----------------------------------------------------------------------


class ViewModule : public Module
{
 public:

  JEM_DECLARE_CLASS       ( ViewModule, Module );

  static const char*        TYPE_NAME;

  static const int          MODE_2D;
  static const int          FG_MODE;
  static const int          OFF_SCREEN_MODE;


  explicit                  ViewModule

    ( const String&           name = "view",
      int                     mode = 0 );

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

    ( const Properties&       conf,
      const Properties&       globdat )      const;

  virtual bool              takeAction

    ( const String&           action,
      const Properties&       params,
      const Properties&       globdat );

  void                      runForever

    ( const Properties&       globdat );

  void                      setMode

    ( int                     mode );

  int                       getMode       () const;

  void                      addPlugin

    ( const Ref<Plugin>&      plugin );

  void                      addPlugin

    ( const Ref<Plugin>&      plugin,
      const Properties&       conf,
      const Properties&       props );

  Ref<Plugin>               addUserPlugin ();

  void                      erasePlugin

    ( const Ref<Plugin>&      plugin );

  void                      printHelp

    ( PrintWriter&            out )          const;

  static Ref<Module>        makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );

  static void               declare       ();


 protected:

  virtual                  ~ViewModule    ();


 private:

  class                     RunData_;


  void                      loadConfig0_

    ( const Properties&       conf );

  bool                      exeCommand_

    ( const Properties&       params,
      const Properties&       globdat );

  void                      printHelp_

    ( PrintWriter&            out );


 private:

  int                       mode_;

  String                    snapFile_;
  String                    confFile_;
  bool                      autoSnap_;

  Ref<ChainPlugin>          plugins_;
  Ref<Function>             updCond_;
  Ref<Function>             snapCond_;
  Ref<RunData_>             rundat_;

};


JIVE_END_PACKAGE( gl )

#endif