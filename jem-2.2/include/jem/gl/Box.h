
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

#ifndef JEM_GL_BOX_H
#define JEM_GL_BOX_H

#include <jem/gl/Vertex.h>


JEM_BEGIN_PACKAGE( gl )


class Rect;
class Transform;


//-----------------------------------------------------------------------
//   class Box
//-----------------------------------------------------------------------


class Box
{
 public:

  inline                    Box         ();

  inline                    Box

    ( GLdouble                a,
      GLdouble                b );

  inline                    Box

    ( const Vertex2d&         a,
      const Vertex2d&         b );

  inline                    Box

    ( const Vertex3d&         a,
      const Vertex3d&         b );

                            Box

    ( const Box&              rhs );

                           ~Box         ();

  Box&                      operator =

    ( const Box&              rhs );

  void                      swap

    ( Box&                    rhs );

  inline int                rank        () const;
  inline bool               isEmpty     () const;
  double                    size        () const;
  Vertex3d                  center      () const;
  void                      drawEdges   () const;
  void                      drawFaces   () const;
  void                      align       ();
  void                      makeEmpty   ();

  void                      getCorners

    ( Vertex3d&               a,
      Vertex3d&               b )          const;

  inline void               setCorners

    ( const Vertex2d&         a,
      const Vertex2d&         b );

  inline void               setCorners

    ( const Vertex3d&         a,
      const Vertex3d&         b );

  void                      setCorners1D

    ( GLdouble                a,
      GLdouble                b );

  void                      setCorners2D

    ( const GLfloat*          a,
      const GLfloat*          b );

  void                      setCorners2D

    ( const GLdouble*         a,
      const GLdouble*         b );

  void                      setCorners3D

    ( const GLfloat*          a,
      const GLfloat*          b );

  void                      setCorners3D

    ( const GLdouble*         a,
      const GLdouble*         b );

  void                      scale

    ( GLdouble                s );

  void                      enlargeWith

    ( const Box&              box );

  void                      transform

    ( const Transform&        tr );

  void                      transform2D

    ( const Transform&        tr );

  void                      project

    ( const Transform&        p );

  void                      project

    ( const Transform&        p,
      const Rect&             vport );


 private:

  inline void               delTransfm_ ();

  void                      getCorners2D_

    ( Vertex3d&               a,
      Vertex3d&               b )          const;

  void                      getCorners3D_

    ( Vertex3d&               a,
      Vertex3d&               b )          const;

  void                      project_

    ( const Transform&        p );


 private:

  int                       rank_;
  Vertex3d                  corners_[2];
  Transform*                transfm_;

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


inline void                 swap

  ( Box&                      lhs,
    Box&                      rhs );

bool                        isVisible

  ( const Box&                box,
    const Transform&          proj );

double                      distance

  ( const Box&                first,
    const Box&                second );

bool                        overlap

  ( const Box&                first,
    const Box&                second );



//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class Box
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


inline Box::Box ()
{
  transfm_ = 0;

  makeEmpty ();
}


inline Box::Box ( GLdouble a, GLdouble b )
{
  transfm_ = 0;

  setCorners1D ( a, b );
}


inline Box::Box ( const Vertex2d& a, const Vertex2d& b )
{
  transfm_ = 0;

  setCorners2D ( a.addr(), b.addr() );
}


inline Box::Box ( const Vertex3d& a, const Vertex3d& b )
{
  transfm_ = 0;

  setCorners3D ( a.addr(), b.addr() );
}


//-----------------------------------------------------------------------
//   rank
//-----------------------------------------------------------------------


inline int Box::rank () const
{
  return rank_;
}


//-----------------------------------------------------------------------
//   isEmpty
//-----------------------------------------------------------------------


inline bool Box::isEmpty () const
{
  return (rank_ == 0);
}


//-----------------------------------------------------------------------
//   setCorners
//-----------------------------------------------------------------------


inline void Box::setCorners

  ( const Vertex2d&  a,
    const Vertex2d&  b )

{
  setCorners2D ( a.addr(), b.addr() );
}


inline void Box::setCorners

  ( const Vertex3d&  a,
    const Vertex3d&  b )

{
  setCorners3D ( a.addr(), b.addr() );
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


inline void swap ( Box& lhs, Box& rhs )
{
  lhs.swap ( rhs );
}


JEM_END_PACKAGE( gl )

#endif
