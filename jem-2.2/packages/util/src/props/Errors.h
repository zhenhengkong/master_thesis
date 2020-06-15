
/*
 *  Copyright (C) 2016 DRG. All rights reserved.
 *
 *  This file is part of Jem, a general purpose programming toolkit.
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
 *
 *  Jem version: 2.2
 *  Date:        Thu 28 Jan 10:36:48 CET 2016
 */

#ifndef JEM_UTIL_PROPS_ERRORS_H
#define JEM_UTIL_PROPS_ERRORS_H

#include <jem/base/String.h>
#include <jem/base/Exception.h>


JEM_BEGIN_PACKAGE( util )
JEM_BEGIN_NAMESPACE( props )


//-----------------------------------------------------------------------
//   class Errors
//-----------------------------------------------------------------------


class Errors
{
 public:

  static void                 typeMismatch

    ( const String&             where,
      const String&             name,
      const Ref<Object>&        obj,
      Class*                    target );

  static void                 valueOutOfRange

    ( const String&             where,
      const String&             name,
      lint                      lbound,
      lint                      ubound );

  static void                 valueOutOfRange

    ( const String&             where,
      const String&             name,
      double                    lbound,
      double                    ubound );

  static void                 noSuchProperty

    ( const String&             where,
      const String&             name );

  static void                 nonAggregateType

    ( const String&             where,
      const String&             name );

  static void                 arraySizeOverflow

    ( lint                      lineno );

  static void                 invalidOperands

    ( lint                      lineno );

  static void                 illegalInput

    ( lint                      lineno,
      const String&             source,
      const String&             what );

  static String               getContext

    ( lint                      lineno,
      const String&             source );

  static void                 setContext

    ( Exception&                ex,
      lint                      lineno,
      const String&             source );

};


JEM_END_NAMESPACE( props )
JEM_END_PACKAGE( util )

#endif