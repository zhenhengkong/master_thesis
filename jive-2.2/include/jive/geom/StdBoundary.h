
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

#ifndef JIVE_GEOM_STDBOUNDARY_H
#define JIVE_GEOM_STDBOUNDARY_H

#include <jem/io/Serializable.h>
#include <jive/geom/import.h>
#include <jive/geom/StdShape.h>


JIVE_BEGIN_PACKAGE( geom )


//-----------------------------------------------------------------------
//   class StdBoundary
//-----------------------------------------------------------------------


class StdBoundary : public StdShape,
                    public Serializable
{
 public:

  JEM_DECLARE_CLASS       ( StdBoundary, StdShape );


                            StdBoundary      ();

                            StdBoundary

    ( const Matrix&           xnodes,
      const Ref<StdShape>&    xshape,
      const Ref<StdShape>&    sshape );

                            StdBoundary

    ( const Ref<Self>&        parent,
      const Matrix&           xnodes );

  virtual void              readFrom

    ( ObjectInput&            in );

  virtual void              writeTo

    ( ObjectOutput&           out )             const;

  virtual idx_t             rank             () const;
  virtual idx_t             shapeFuncCount   () const;
  virtual String            getGeometry      () const;
  virtual idx_t             vertexCount      () const;
  virtual Matrix            getVertexCoords  () const;

  virtual bool              containsPoint

    ( const Vector&           u )               const;

  virtual bool              clipPoint

    ( const Vector&           u )               const;

  virtual void              evalShapeFunctions

    ( const Vector&           f,
      const Vector&           u )               const;

  virtual void              evalShapeGradients

    ( const Vector&           f,
      const Matrix&           g,
      const Vector&           u )               const;

  virtual void              evalShapeGradGrads

    ( const Vector&           f,
      const Matrix&           g,
      const Matrix&           h,
      const Vector&           u )               const;

  virtual void*             getExtByID

    ( ExtensionID             extID )           const;

  virtual void              mapBoundaryPoint

    ( const Vector&           v,
      const Vector&           u )               const;


 protected:

  virtual                  ~StdBoundary      ();


 protected:

  Ref<Self>                 parent_;
  Ref<StdShape>             xshape_;
  Ref<StdShape>             sshape_;

  idx_t                     rank_;
  idx_t                     nodeCount_;
  idx_t                     funcCount_;

  Matrix                    xnodeCoords_;
  Vector                    xshapeFuncs_;
  Matrix                    xshapeGrads_;
  Matrix                    sshapeGrads_;
  Matrix                    xshapeGrads2_;
  Matrix                    sshapeGrads2_;

  Matrix                    jacobi_;
  Matrix                    qujobi_;
  Matrix                    jacobi2_;

  Vector                    point_;


 private:

  void                      init_           ();
  void                      shapeChanged_   ();

};


JIVE_END_PACKAGE( geom )

#endif
