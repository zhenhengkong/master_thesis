
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

#ifndef JIVE_GEOM_PARAMETRICEDGE_H
#define JIVE_GEOM_PARAMETRICEDGE_H

#include <jive/geom/ParametricBoundary.h>


JIVE_BEGIN_PACKAGE( geom )


//-----------------------------------------------------------------------
//   class ParametricEdge
//-----------------------------------------------------------------------


class ParametricEdge : public ParametricBoundary
{
 public:

  JEM_DECLARE_CLASS       ( ParametricEdge, ParametricBoundary );


                            ParametricEdge  ();

                            ParametricEdge

    ( const String&           name,
      const Matrix&           ischeme,
      const Ref<StdShape>&    xshape,
      const Ref<StdShape>&    sshape = NIL );

  virtual void              getGlobalIntegrationPoints

    ( const Matrix&           x,
      const Matrix&           c )              const;

  virtual void              getIntegrationWeights

    ( const Vector&           w,
      const Matrix&           c )              const;

  virtual void              getGlobalPoint

    ( const Vector&           x,
      const Vector&           u,
      const Matrix&           c )              const;

  virtual bool              getLocalPoint

    ( const Vector&           u,
      const Vector&           x,
      double                  eps,
      const Matrix&           c )              const;

  virtual void              evalNormal

    ( const Vector&           q,
      const Vector&           u,
      const Matrix&           c )              const;

  virtual void              calcNormals

    ( const Matrix&           q,
      const Vector*           w,
      const Matrix&           c,
      const PointSet&         points )         const;


 protected:

  virtual                  ~ParametricEdge  ();

};


JIVE_END_PACKAGE( geom )

#endif
