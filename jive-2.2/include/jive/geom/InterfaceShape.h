
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

#ifndef JIVE_GEOM_INTERFACESHAPE_H
#define JIVE_GEOM_INTERFACESHAPE_H

#include <jem/io/Serializable.h>
#include <jive/geom/BoundaryShape.h>
#include <jive/geom/import.h>


JIVE_BEGIN_PACKAGE( geom )


//-----------------------------------------------------------------------
//   class InterfaceShape
//-----------------------------------------------------------------------


class InterfaceShape : public Shape,
                       public Serializable
{
 public:

  JEM_DECLARE_CLASS       ( InterfaceShape, Shape );


                            InterfaceShape        ();

  explicit                  InterfaceShape

    ( const Ref<BShape>&      bshape );

                            InterfaceShape

    ( const String&           name,
      const Ref<BShape>&      bshape );

  virtual void              readFrom

    ( ObjectInput&            in );

  virtual void              writeTo

    ( ObjectOutput&           out )                 const;

  virtual idx_t             globalRank            () const;
  virtual idx_t             localRank             () const;
  virtual idx_t             nodeCount             () const;
  virtual idx_t             vertexCount           () const;
  virtual idx_t             ipointCount           () const;
  virtual idx_t             shapeFuncCount        () const;
  virtual String            getGeometry           () const;
  virtual Matrix            getVertexCoords       () const;
  virtual Matrix            getIntegrationScheme  () const;

  virtual void              getGlobalIntegrationPoints

    ( const Matrix&           x,
      const Matrix&           c )                    const;

  virtual void              getIntegrationWeights

    ( const Vector&           w,
      const Matrix&           c )                    const;

  virtual void              getGlobalPoint

    ( const Vector&           x,
      const Vector&           u,
      const Matrix&           c )                    const;

  virtual bool              getLocalPoint

    ( const Vector&           u,
      const Vector&           x,
      double                  eps,
      const Matrix&           c )                    const;

  virtual bool              containsLocalPoint

    ( const Vector&           u )                    const;

  virtual Matrix            getShapeFunctions     () const;
  virtual Matrix            getVertexFunctions    () const;

  virtual void              evalShapeFunctions

    ( const Vector&           h,
      const Vector&           u )                    const;

  void                      evalNormal

    ( const Vector&           q,
      const Vector&           u,
      const Matrix&           c )                    const;

  void                      getNormals

    ( const Matrix&           q,
      const Vector&           w,
      const Matrix&           c )                    const;

  void                      getVertexNormals

    ( const Matrix&           q,
      const Matrix&           c )                    const;

  inline BShape*            getBShape             () const;

  void                      getMidCoords

    ( const Matrix&           x,
      const Matrix&           c )                    const;

  inline const Matrix&      getMidCoords

    ( const Matrix&           c )                    const;

  virtual void*             getExtByID

    ( ExtensionID             extID )                const;



 protected:

  virtual                  ~InterfaceShape        ();


 private:

  void                      init_                 ();
  void                      shapeChanged_         ();


 private:

  Ref<BShape>               bshape_;

  Matrix                    midCoords_;

};


//-----------------------------------------------------------------------
//   related types
//-----------------------------------------------------------------------


typedef InterfaceShape      FShape;




//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getBShape
//-----------------------------------------------------------------------


inline BShape* InterfaceShape::getBShape () const
{
  return bshape_.get ();
}


//-----------------------------------------------------------------------
//   getMidCoords
//-----------------------------------------------------------------------


inline const Matrix& InterfaceShape::getMidCoords

  ( const Matrix&  c ) const

{
  getMidCoords ( midCoords_, c );

  return midCoords_;
}


JIVE_END_PACKAGE( geom )

#endif
