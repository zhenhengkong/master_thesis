
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

#ifndef JIVE_GEOM_STDPOINT_H
#define JIVE_GEOM_STDPOINT_H

#include <jem/io/Serializable.h>
#include <jive/geom/import.h>
#include <jive/geom/StdShape.h>


JIVE_BEGIN_PACKAGE( geom )


//-----------------------------------------------------------------------
//   class StdPoint
//-----------------------------------------------------------------------


class StdPoint : public StdShape,
                 public Serializable
{
 public:

  JEM_DECLARE_CLASS     ( StdPoint, StdShape );

  static const int        RANK             = 0;
  static const int        VERTEX_COUNT     = 1;
  static const int        SHAPE_FUNC_COUNT = 1;

  static const char*      GEOMETRY;


                          StdPoint         ();

  virtual void            readFrom

    ( ObjectInput&          in );

  virtual void            writeTo

    ( ObjectOutput&         out )             const;

  virtual idx_t           rank             () const;
  virtual idx_t           shapeFuncCount   () const;
  virtual String          getGeometry      () const;
  virtual bool            isLinear         () const;
  virtual idx_t           vertexCount      () const;
  static  Matrix          vertexCoords     ();
  virtual Matrix          getVertexCoords  () const;

  virtual bool            containsPoint

    ( const Vector&         u )               const;

  virtual bool            clipPoint

    ( const Vector&         u )               const;

  virtual void            evalShapeFunctions

    ( const Vector&         f,
      const Vector&         u )               const;

  virtual void            evalShapeGradients

    ( const Vector&         f,
      const Matrix&         g,
      const Vector&         u )               const;

  virtual void            evalShapeGradGrads

    ( const Vector&         f,
      const Matrix&         g,
      const Matrix&         h,
      const Vector&         u )               const;

  static Ref<SShape>      makeNew

    ( const String&         geom,
      const Properties&     conf,
      const Properties&     props );

  static void             declare          ();


 protected:

  virtual                ~StdPoint         ();

};


JIVE_END_PACKAGE( geom )

#endif
