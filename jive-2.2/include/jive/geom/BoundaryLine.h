
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

#ifndef JIVE_GEOM_BOUNDARYLINE_H
#define JIVE_GEOM_BOUNDARYLINE_H

#include <jive/geom/typedefs.h>
#include <jive/geom/BoundaryShape.h>


JIVE_BEGIN_PACKAGE( geom )


//-----------------------------------------------------------------------
//   class BoundaryLine2
//-----------------------------------------------------------------------


class BoundaryLine2
{
 public:

  static const char*      TYPE_NAME;
  static const char*      ISCHEME;


  static Ref<BShape>      getShape

    ( const String&         name    = "bline2",
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<BShape>      getShape

    ( const String&         name,
      String&               ischeme,
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<BShape>      getShape

    ( const String&         name,
      const Matrix&         ischeme,
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<BShape>      getShape

    ( const String&         name,
      const Properties&     conf,
      const Properties&     props );


  static void             declare ();

};


//-----------------------------------------------------------------------
//   class BoundaryLine3
//-----------------------------------------------------------------------


class BoundaryLine3
{
 public:

  static const char*      TYPE_NAME;
  static const char*      ISCHEME;


  static Ref<BShape>      getShape

    ( const String&         name    = "bline3",
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<BShape>      getShape

    ( const String&         name,
      String&               ischeme,
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<BShape>      getShape

    ( const String&         name,
      const Matrix&         ischeme,
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<BShape>      getShape

    ( const String&         name,
      const Properties&     conf,
      const Properties&     props );


  static void             declare ();

};


//-----------------------------------------------------------------------
//   class BoundaryLine4
//-----------------------------------------------------------------------


class BoundaryLine4
{
 public:

  static const char*      TYPE_NAME;
  static const char*      ISCHEME;


  static Ref<BShape>      getShape

    ( const String&         name    = "bline4",
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<BShape>      getShape

    ( const String&         name,
      String&               ischeme,
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<BShape>      getShape

    ( const String&         name,
      const Matrix&         ischeme,
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<BShape>      getShape

    ( const String&         name,
      const Properties&     conf,
      const Properties&     props );


  static void             declare ();

};


//-----------------------------------------------------------------------
//   compatibility typedefs
//-----------------------------------------------------------------------


typedef BoundaryLine2     LinearBoundaryLine;
typedef BoundaryLine3     QuadraticBoundaryLine;


JIVE_END_PACKAGE( geom )

#endif