
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

#ifndef JEM_GL_VERTEXSET_H
#define JEM_GL_VERTEXSET_H

#include <jem/base/Object.h>
#include <jem/base/Clonable.h>
#include <jem/util/event/Event.h>
#include <jem/gl/Array.h>


JEM_BEGIN_PACKAGE( gl )


class Box;


//-----------------------------------------------------------------------
//   class VertexSet
//-----------------------------------------------------------------------


class VertexSet : public Object, public Clonable
{
 public:

  JEM_DECLARE_CLASS       ( VertexSet, Object );

  typedef Tuple<idx_t,2>    Shape;

  util::Event< Self& >      newGeomEvent;
  util::Event< Shape,
               Self& >      newShapeEvent;


                            VertexSet     ();

  explicit                  VertexSet

    ( const GLfloatMatrix&    coords );

  virtual Ref<Object>       clone         () const;
  inline idx_t              size          () const;
  inline idx_t              rank          () const;
  inline Shape              shape         () const;

  void                      reshape

    ( idx_t                   m,
      idx_t                   n );

  inline GLfloat*           getData       () const;
  inline GLfloatMatrix      getCoords     () const;

  void                      setCoords

    ( const GLfloatMatrix&    coords );

  void                      newCoords     ();

  void                      getBBox

    ( Box&                    box )          const;

  void                      findVertex

    ( idx_t&                  index,
      GLfloat&                dist,
      GLfloat                 x,
      GLfloat                 y = 0.0F,
      GLfloat                 z = 0.0F )     const;

  void                      findVertex

    ( idx_t&                  index,
      GLfloat&                dist,
      const GLfloat*          pos )          const;


 protected:

  virtual                  ~VertexSet     ();


 private:

  void                      updateBBox_   ();

  void                      findVertex_

    ( idx_t&                  index,
      GLfloat&                dist,
      GLfloat                 xpos )         const;

  void                      findVertex_

    ( idx_t&                  index,
      GLfloat&                dist,
      GLfloat                 xpos,
      GLfloat                 ypos )         const;

  void                      findVertex_

    ( idx_t&                  index,
      GLfloat&                dist,
      GLfloat                 xpos,
      GLfloat                 ypos,
      GLfloat                 zpos )         const;

  void                      findVertex_

    ( idx_t&                  index,
      GLfloat&                dist,
      const GLfloat*          pos )          const;


 private:

  GLfloatMatrix             coords_;
  GLfloat                   bbox_[6];

  bool                      validBBox_;

};





//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   size
//-----------------------------------------------------------------------


inline idx_t VertexSet::size () const
{
  return coords_.size (1);
}


//-----------------------------------------------------------------------
//   rank
//-----------------------------------------------------------------------


inline idx_t VertexSet::rank () const
{
  return coords_.size (0);
}


//-----------------------------------------------------------------------
//   shape
//-----------------------------------------------------------------------


inline VertexSet::Shape VertexSet::shape () const
{
  return coords_.shape ();
}


//-----------------------------------------------------------------------
//   getData
//-----------------------------------------------------------------------


inline GLfloat* VertexSet::getData () const
{
  return coords_.addr ();
}


//-----------------------------------------------------------------------
//   getCoords
//-----------------------------------------------------------------------


inline GLfloatMatrix VertexSet::getCoords () const
{
  return coords_;
}


JEM_END_PACKAGE( gl )

#endif
