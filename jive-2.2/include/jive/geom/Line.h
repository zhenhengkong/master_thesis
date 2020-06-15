
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

#ifndef JIVE_GEOM_LINE_H
#define JIVE_GEOM_LINE_H

#include <jive/util/Topology.h>
#include <jive/geom/typedefs.h>
#include <jive/geom/InternalShape.h>


JIVE_BEGIN_PACKAGE( geom )


//-----------------------------------------------------------------------
//   class Line
//-----------------------------------------------------------------------


class Line
{
 public:

  static const char*      GEOMETRY;
  static const char*      TYPE_NAME;

  static const int        BOUNDARY_COUNT = 2;


  static Ref<IShape>      getShape

    ( const String&         name,
      String&               ischeme,
      const Ref<SShape>&    xfuncs,
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<IShape>      getShape

    ( const String&         name,
      const Matrix&         ischeme,
      const Ref<SShape>&    xfuncs,
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<IShape>      getShape

    ( const String&         name,
      const Properties&     conf,
      const Properties&     props );

  static void             declare             ();


 protected:

  class                   Utils_;

};

//-----------------------------------------------------------------------
//   class Line2
//-----------------------------------------------------------------------


class Line2 : public Line
{
 public:

  static const char*      TYPE_NAME;

  static const int        NODE_COUNT         = 2;
  static const int        NODES_PER_BOUNDARY = 1;

  static const char*      ISCHEME;


  static Ref<IShape>      getShape

    ( const String&         name    = "line2",
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<IShape>      getShape

    ( const String&         name,
      String&               ischeme,
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<IShape>      getShape

    ( const String&         name,
      const Matrix&         ischeme,
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<IShape>      getShape

    ( const String&         name,
      const Properties&     conf,
      const Properties&     props );

  static util::Topology   getBoundaryTopology ();

  static void             declare             ();

};


//-----------------------------------------------------------------------
//   class Line3
//-----------------------------------------------------------------------


class Line3 : public Line
{
 public:

  static const char*      TYPE_NAME;

  static const int        NODE_COUNT         = 3;
  static const int        NODES_PER_BOUNDARY = 1;

  static const char*      ISCHEME;


  static Ref<IShape>      getShape

    ( const String&         name    = "line3",
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<IShape>      getShape

    ( const String&         name,
      String&               ischeme,
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<IShape>      getShape

    ( const String&         name,
      const Matrix&         ischeme,
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<IShape>      getShape

    ( const String&         name,
      const Properties&     conf,
      const Properties&     props );

  static util::Topology   getBoundaryTopology ();

  static void             declare             ();

};


//-----------------------------------------------------------------------
//   class Line4
//-----------------------------------------------------------------------


class Line4 : public Line
{
 public:

  static const char*      TYPE_NAME;

  static const int        NODE_COUNT         = 4;
  static const int        NODES_PER_BOUNDARY = 1;

  static const char*      ISCHEME;


  static Ref<IShape>      getShape

    ( const String&         name    = "line4",
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<IShape>      getShape

    ( const String&         name,
      String&               ischeme,
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<IShape>      getShape

    ( const String&         name,
      const Matrix&         ischeme,
      const Ref<SShape>&    sfuncs  = NIL );

  static Ref<IShape>      getShape

    ( const String&         name,
      const Properties&     conf,
      const Properties&     props );

  static util::Topology   getBoundaryTopology ();

  static void             declare             ();

};


//-----------------------------------------------------------------------
//   compatibility typedefs
//-----------------------------------------------------------------------


typedef Line2             LinearLine;
typedef Line3             QuadraticLine;


JIVE_END_PACKAGE( geom )

#endif
