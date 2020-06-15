
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

#ifndef JIVE_APP_OUTPUTMODULE_H
#define JIVE_APP_OUTPUTMODULE_H

#include <jive/app/Module.h>


JIVE_BEGIN_PACKAGE( app )


class DataPrinter;


//-----------------------------------------------------------------------
//   class OutputModule
//-----------------------------------------------------------------------


class OutputModule : public Module
{
 public:

  JEM_DECLARE_CLASS       ( OutputModule, Module );

  static const char*        TYPE_NAME;


  explicit                  OutputModule

    ( const String&           name    = "output",
      Ref<DataPrinter>        printer = NIL );

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
      const Properties&       globdat )        const;


  static Ref<Module>        makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );

  static void               declare       ();


 protected:

  virtual                  ~OutputModule  ();

  void                      checkFile_

    ( const String&           name,
      int                     flags )        const;


 private:

  class                     Specs_;
  class                     RunData_;
  friend class              RunData_;


  Ref<DataPrinter>          printer_;

  String                    fileName_;
  int                       fflags_;
  int                       precision_;
  int                       pageWidth_;
  idx_t                     bufsize_;

  Ref<Specs_>               vecSpecs_;
  Ref<Specs_>               tabSpecs_;
  Ref<Function>             saveCond_;

  Ref<RunData_>             rundat_;

};


JIVE_END_PACKAGE( app )

#endif
