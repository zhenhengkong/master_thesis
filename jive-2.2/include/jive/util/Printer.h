
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

#ifndef JIVE_UTIL_PRINTER_H
#define JIVE_UTIL_PRINTER_H

#include <jem/base/Object.h>
#include <jive/util/import.h>


JIVE_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class Printer
//-----------------------------------------------------------------------


class Printer
{
 public:

  static PrintWriter&     get     ();

  static void             set

    ( Ref<PrintWriter>      out );

  static void             flush   ();


 private:

  class                   Flusher_;
  friend class            Flusher_;


  static int              getKey_ ();
  static void             newKey_ ();


 private:

  static int              key_;

};


JIVE_END_PACKAGE( util )

#endif