
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


#include <cmath>
#include <jem/base/assert.h>
#include <jem/base/limits.h>
#include <jem/base/System.h>
#include <jem/base/ClassTemplate.h>
#include <jem/base/array/operators.h>
#include <jem/base/array/utilities.h>
#include <jem/base/array/intrinsics.h>
#include <jem/util/Event.h>
#include <jem/util/Properties.h>
#include <jem/gl/byte.h>
#include <jem/gl/error.h>
#include <jem/gl/utilities.h>
#include <jem/gl/Box.h>
#include <jem/gl/Menu.h>
#include <jem/gl/Rect.h>
#include <jem/gl/Color.h>
#include <jem/gl/Commands.h>
#include <jem/gl/Vertex.h>
#include <jem/gl/VertexSet.h>
#include <jem/gl/ColorMap.h>
#include <jem/gl/Transform.h>
#include <jem/gl/TriangleSet.h>


JEM_DEFINE_CLASS( jem::gl::TriangleSet );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class TriangleSet::Work_
//=======================================================================


class TriangleSet::Work_
{
 public:

  explicit inline         Work_

    ( const TriangleSet&    ts );


 public:

  const idx_t             trCount;
  const idx_t*            triangs;
  const GLfloat*          vcoords;

};


//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------


inline TriangleSet::Work_::Work_ ( const TriangleSet& ts ) :

  trCount ( ts.triangles_.size(1) )

{
  triangs = ts.triangles_.addr    ();
  vcoords = ts.vertices_->getData ();
}


//=======================================================================
//   class TriangleSet::Work2D_
//=======================================================================


class TriangleSet::Work2D_ : public Work_
{
 public:

  explicit inline         Work2D_

    ( const TriangleSet&    ts );

  inline void             getTrCenter

    ( GLfloat*              c )       const;

  GLdouble                distanceTo

    ( const GLfloat*        x )       const;

};


//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------


inline TriangleSet::Work2D_::Work2D_ ( const TriangleSet& ts ) :

  Work_ ( ts )

{}


//-----------------------------------------------------------------------
//   getTrCenter
//-----------------------------------------------------------------------


inline void TriangleSet::Work2D_::getTrCenter ( GLfloat* c ) const
{
  idx_t  k = 2 * triangs[0];

  c[0]  = vcoords[k + 0];
  c[1]  = vcoords[k + 1];

  k     = 2 * triangs[1];

  c[0] += vcoords[k + 0];
  c[1] += vcoords[k + 1];

  k     = 2 * triangs[2];

  c[0] += vcoords[k + 0];
  c[1] += vcoords[k + 1];

  c[0] /= 3.0F;
  c[1] /= 3.0F;
}


//-----------------------------------------------------------------------
//   distanceTo
//-----------------------------------------------------------------------


GLdouble TriangleSet::Work2D_::distanceTo ( const GLfloat* x ) const
{
  GLdouble  gx[2][2];
  GLdouble  dx[2];
  GLdouble  u [2];
  GLdouble  tmp;
  GLdouble  det;
  idx_t      i, j, k;


  i        = 2 * triangs[0];
  j        = 2 * triangs[1];
  k        = 2 * triangs[2];

  tmp      = vcoords[i + 0];
  gx[0][0] = vcoords[j + 0] - tmp;
  gx[0][1] = vcoords[k + 0] - tmp;
  dx[0]    = x[0]           - tmp;

  tmp      = vcoords[i + 1];
  gx[1][0] = vcoords[j + 1] - tmp;
  gx[1][1] = vcoords[k + 1] - tmp;
  dx[1]    = x[1]           - tmp;


  det = gx[0][0] * gx[1][1] - gx[0][1] * gx[1][0];

  if ( isTiny( det ) )
  {
    return -1.0;
  }

  tmp  = 1.0 / det;

  u[0] = tmp * (gx[1][1] * dx[0] - gx[0][1] * dx[1]);
  u[1] = tmp * (gx[0][0] * dx[1] - gx[1][0] * dx[0]);

  if ( u[0] < 0.0 )
  {
    u[0] = 0.0;
  }
  if ( u[1] < 0.0 )
  {
    u[1] = 0.0;
  }

  tmp = u[0] + u[1];

  if ( tmp > 1.0 )
  {
    tmp   = 1.0 / tmp;
    u[0] *= tmp;
    u[1] *= tmp;
  }

  dx[0] = dx[0] - u[0] * gx[0][0] - u[1] * gx[0][1];
  dx[1] = dx[1] - u[0] * gx[1][0] - u[1] * gx[1][1];

  tmp   = dx[0] * dx[0] + dx[1] * dx[1];

  return tmp;
}


//=======================================================================
//   class TriangleSet::Work3D_
//=======================================================================


class TriangleSet::Work3D_ : public Work_
{
 public:

  explicit inline         Work3D_

    ( const TriangleSet&    ts );

  inline void             getTrCenter

    ( GLfloat*              c )              const;

  GLdouble                distanceTo

    ( const GLfloat*        x )              const;

  void                    getZOrder

    ( const IdxVector&     iperm );


 public:

  const idx_t             nrCount;
  const GLbyte*           normals;

};


//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------


inline TriangleSet::Work3D_::Work3D_ ( const TriangleSet& ts ) :

    Work_   ( ts ),
    nrCount ( ts.normals_.size(1) )

{
  normals = ts.normals_.addr ();
}


//-----------------------------------------------------------------------
//   getTrCenter
//-----------------------------------------------------------------------


inline void TriangleSet::Work3D_::getTrCenter ( GLfloat* c ) const
{
  idx_t  k = 3 * triangs[0];

  c[0]  = vcoords[k + 0];
  c[1]  = vcoords[k + 1];
  c[2]  = vcoords[k + 2];

  k     = 3 * triangs[1];

  c[0] += vcoords[k + 0];
  c[1] += vcoords[k + 1];
  c[2] += vcoords[k + 2];

  k     = 3 * triangs[2];

  c[0] += vcoords[k + 0];
  c[1] += vcoords[k + 1];
  c[2] += vcoords[k + 2];

  c[0] /= 3.0F;
  c[1] /= 3.0F;
  c[2] /= 3.0F;
}


//-----------------------------------------------------------------------
//   distanceTo
//-----------------------------------------------------------------------


GLdouble TriangleSet::Work3D_::distanceTo ( const GLfloat* x ) const
{
  GLdouble  px[2][2];
  GLdouble  dx[3];
  GLdouble  s [3];
  GLdouble  t [3];
  GLdouble  u [2];
  GLdouble  v [2];

  double    tmp;
  double    det;
  idx_t     i, j, k;


  i        = 3 * triangs[0];
  j        = 3 * triangs[1];
  k        = 3 * triangs[2];

  tmp      = vcoords[i + 0];
  s[0]     = vcoords[j + 0] - tmp;
  t[0]     = vcoords[k + 0] - tmp;
  dx[0]    = x[0]           - tmp;

  tmp      = vcoords[i + 1];
  s[1]     = vcoords[j + 1] - tmp;
  t[1]     = vcoords[k + 1] - tmp;
  dx[1]    = x[1]           - tmp;

  tmp      = vcoords[i + 2];
  s[2]     = vcoords[j + 2] - tmp;
  t[2]     = vcoords[k + 2] - tmp;
  dx[2]    = x[2]           - tmp;

  px[0][0] = s[0] * s[0] + s[1] * s[1] + s[2] * s[2];
  px[0][1] = s[0] * t[0] + s[1] * t[1] + s[2] * t[2];
  px[1][0] = px[0][1];
  px[1][1] = t[0] * t[0] + t[1] * t[1] + t[2] * t[2];

  v[0]     = s[0] * dx[0] + s[1] * dx[1] + s[2] * dx[2];
  v[1]     = t[0] * dx[0] + t[1] * dx[1] + t[2] * dx[2];

  det      = px[0][0] * px[1][1] - px[0][1] * px[1][0];

  if ( isTiny( det ) )
  {
    return -1.0;
  }

  tmp  = 1.0 / det;

  u[0] = tmp * (px[1][1] * v[0] - px[0][1] * v[1]);
  u[1] = tmp * (px[0][0] * v[1] - px[1][0] * v[0]);

  if ( u[0] < 0.0 )
  {
    u[0] = 0.0;
  }
  if ( u[1] < 0.0 )
  {
    u[1] = 0.0;
  }

  tmp = u[0] + u[1];

  if ( tmp > 1.0 )
  {
    tmp   = 1.0 / tmp;
    u[0] *= tmp;
    u[1] *= tmp;
  }

  dx[0] = dx[0] - u[0] * s[0] - u[1] * t[0];
  dx[1] = dx[1] - u[0] * s[1] - u[1] * t[1];
  dx[2] = dx[2] - u[0] * s[2] - u[1] * t[2];

  tmp   = dx[0] * dx[0] + dx[1] * dx[1] + dx[2] * dx[2];

  return tmp;
}


//-----------------------------------------------------------------------
//   getZOrder
//-----------------------------------------------------------------------


void TriangleSet::Work3D_::getZOrder ( const IdxVector& iperm )
{
  const idx_t*    triangs0 = triangs;

  GLfloatVector   z ( trCount );

  GLdouble        mx[16];
  GLfloat         t0, t1, t2;
  GLfloat         c[3];


  glGetDoublev ( GL_MODELVIEW_MATRIX, mx );

  t0 = (GLfloat) mx[ 2];
  t1 = (GLfloat) mx[ 6];
  t2 = (GLfloat) mx[10];

  for ( idx_t i = 0; i < trCount; i++ )
  {
    getTrCenter ( c );

    z[i]     = t0 * c[0] + t1 * c[1] + t2 * c[2];
    iperm[i] = i;

    triangs += 3;
  }

  triangs = triangs0;

  sort ( iperm, z );
}


//=======================================================================
//   class TriangleSet
//=======================================================================

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


TriangleSet::TriangleSet

  ( const String&          name,
    const Ref<VertexSet>&  vx ) :

    Super      ( name ),
    vertices_  (   vx ),
    triangles_ ( 3, 0 )

{
  JEM_PRECHECK ( vx != NIL );

  if ( vx->size() > 0 && (vx->rank() < 2 || vx->rank() > 3) )
  {
    shapeError ( JEM_FUNC, "vertex array", vx->shape() );
  }

  colorMode_  = NO_COLORS;
  normalMode_ = AUTO_NORMALS;
  opaque_     = true;
  checked_    = true;
  updated_    = false;

  connectHandlers_ ();
}


TriangleSet::~TriangleSet ()
{}


//-----------------------------------------------------------------------
//   getBBox
//-----------------------------------------------------------------------


void TriangleSet::getBBox ( Box& box ) const
{
  // TO DO: get actual bbox of triangles (not all vertices have to
  //        be used).

  vertices_->getBBox ( box );
}


//-----------------------------------------------------------------------
//   redraw
//-----------------------------------------------------------------------


void TriangleSet::redraw ( GfxContext& gtx )
{
  if ( this->size() == 0 )
  {
    return;
  }

  EnableScope  polyScope ( GL_POLYGON_OFFSET_FILL );

  const idx_t  rank = vertices_->rank ();

  GLfloat      col0[4];


  if ( ! checked_ )
  {
    checkData_ ( getContext() );
  }

  if ( normalMode_ == AUTO_NORMALS && ! updated_ )
  {
    updateNormals_ ();
  }

  glGetFloatv ( GL_CURRENT_COLOR, col0 );

  switch ( colorMode_ )
  {
  case NO_COLORS:

    if ( isOpaque( col0[3] ) )
    {
      DisableScope  blendScope ( GL_BLEND );

      if ( rank == 2 )
      {
        drawNoColors2D_ ();
      }
      else
      {
        drawNoColors3D_  ();
      }
    }
    else
    {
      if ( rank == 2 )
      {
        drawNoColors2D_  ();
      }
      else
      {
        blendNoColors3D_ ();
      }
    }

    break;

  case TRIANG_COLORS:

    if ( opaque_ )
    {
      DisableScope  blendScope ( GL_BLEND );

      if ( rank == 2 )
      {
        drawTrColors2D_ ();
      }
      else
      {
        drawTrColors3D_  ();
      }
    }
    else
    {
      if ( rank == 2 )
      {
        drawTrColors2D_  ();
      }
      else
      {
        blendTrColors3D_ ();
      }
    }

    glColor4fv ( col0 );

    break;

  case VERTEX_COLORS:

    colorMap_->enable  ( gtx );

    if ( rank == 2 )
    {
      drawVxColors2D_  ();
    }
    else
    {
      if ( opaque_ )
      {
        drawVxColors3D_  ();
      }
      else
      {
        blendVxColors3D_ ();
      }
    }

    colorMap_->disable ();
    glColor4fv         ( col0 );

    break;
  }
}


//-----------------------------------------------------------------------
//   redrawFast
//-----------------------------------------------------------------------


void TriangleSet::redrawFast

  ( GfxContext&  gtx,
    double       lod )

{
  if ( lod <= 0.0 )
  {
    return;
  }

  if ( lod >= 1.0 )
  {
    Self::redraw ( gtx ); return;
  }


  DisableScope  blendScope ( GL_BLEND );

  const idx_t   rank = vertices_->rank ();

  GLfloat       col0[4];
  GLfloat       ps, ps0;


  if ( ! checked_ )
  {
    checkData_ ( getContext() );
  }

  if ( normalMode_ == AUTO_NORMALS && ! updated_ )
  {
    updateNormals_ ();
  }

  ps = getPointSize_ ( lod );

  if ( ps <= 0.0 )
  {
    return;
  }

  glGetFloatv ( GL_POINT_SIZE, &ps0 );
  glPointSize ( ps );

  switch ( colorMode_ )
  {
  case NO_COLORS:

    if ( rank == 2 )
    {
      plotNoColors2D_  ( lod );
    }
    else
    {
      plotNoColors3D_  ( lod );
    }

    break;

  case TRIANG_COLORS:

    glGetFloatv ( GL_CURRENT_COLOR, col0 );

    if ( rank == 2 )
    {
      plotTrColors2D_ ( lod );
    }
    else
    {
      plotTrColors3D_ ( lod );
    }

    glColor4fv ( col0 );

    break;

  case VERTEX_COLORS:

    glGetFloatv        ( GL_CURRENT_COLOR, col0 );
    colorMap_->enable  ( gtx );

    if ( rank == 2 )
    {
      plotVxColors2D_  ( lod );
    }
    else
    {
      plotVxColors3D_  ( lod );
    }

    colorMap_->disable ();
    glColor4fv         ( col0 );

    break;
  }

  glPointSize ( ps0 );
}


//-----------------------------------------------------------------------
//   exeCommand
//-----------------------------------------------------------------------


bool TriangleSet::exeCommand

  ( GfxContext&        gtx,
    const String&      cmd,
    const Properties&  params )

{
  if ( cmd == Commands::HIDE_FACES )
  {
    hidden = true;
    return   true;
  }

  if ( cmd == Commands::SHOW_FACES )
  {
    hidden = false;
    return   true;
  }

  return false;
}


//-----------------------------------------------------------------------
//   listOptions
//-----------------------------------------------------------------------


void TriangleSet::listOptions ( Menu& menu ) const
{
  menu.addOption ( Commands::HIDE_FACES,
                   Commands::SHOW_FACES, hidden );
}


//-----------------------------------------------------------------------
//   getTopology
//-----------------------------------------------------------------------


IdxMatrix TriangleSet::getTopology () const
{
  return triangles_;
}


//-----------------------------------------------------------------------
//   setTopology
//-----------------------------------------------------------------------


void TriangleSet::setTopology ( const IdxMatrix& topo )
{
  JEM_PRECHECK ( topo.size(0) == 3 );

  triangles_.ref ( makeContiguous( topo ) );

  setNoColors    ();
  setAutoNormals ();

  checked_ = false;
  updated_ = false;

  checkData_ ( JEM_FUNC );
}


//-----------------------------------------------------------------------
//   setNoColors
//-----------------------------------------------------------------------


void TriangleSet::setNoColors ()
{
  if ( colorMode_ != NO_COLORS )
  {
    trColors_.ref ( GLubyteMatrix() );
    vxColors_.ref ( GLfloatVector() );

    opaque_    = true;
    colorMap_  = NIL;
    colorMode_ = NO_COLORS;
  }
}


//-----------------------------------------------------------------------
//   setTriangColors
//-----------------------------------------------------------------------


void TriangleSet::setTriangColors ( const GLubyteMatrix& colors )
{
  JEM_PRECHECK ( colors.size(0) >= 3 &&
                 colors.size(0) <= 4 );

  const idx_t  n    = colors.size (1);

  GLubyte     alpha = toUbyte ( Color::OPAQUE_ALPHA );


  if ( n != triangles_.size(1) )
  {
    shapeError ( JEM_FUNC,
                 "triangle color matrix", colors.shape() );
  }

  trColors_.ref ( makeContiguous( colors ) );
  vxColors_.ref ( GLfloatVector() );

  if ( colors.size(0) == 4 && n > 0 )
  {
    opaque_ = testall ( colors(3,ALL) >= alpha );
  }
  else
  {
    opaque_ = false;
  }

  colorMap_  = NIL;
  colorMode_ = TRIANG_COLORS;
}


//-----------------------------------------------------------------------
//   setVertexColors
//-----------------------------------------------------------------------


void TriangleSet::setVertexColors

  ( const GLfloatVector&  colors,
    const Ref<ColorMap>&  colorMap )

{
  JEM_PRECHECK ( colorMap != NIL );

  if ( colors.size() != vertices_->size() )
  {
    sizeError ( JEM_FUNC,
                "vertex color vector", colors.size() );
  }

  vxColors_.ref ( makeContiguous( colors ) );
  trColors_.ref ( GLubyteMatrix() );

  opaque_    = isOpaque ( colorMap->getMinAlpha() );
  colorMap_  = colorMap;
  colorMode_ = VERTEX_COLORS;
}


//-----------------------------------------------------------------------
//   setAutoNormals
//-----------------------------------------------------------------------


void TriangleSet::setAutoNormals ()
{
  if ( normalMode_ != AUTO_NORMALS )
  {
    normals_.ref ( GLbyteMatrix() );

    normalMode_ = AUTO_NORMALS;
    updated_    = false;
  }
}


//-----------------------------------------------------------------------
//   setTriangNormals
//-----------------------------------------------------------------------


void TriangleSet::setTriangNormals ( const GLbyteMatrix& normals )
{
  const idx_t  k = triangles_.size (1);
  const idx_t  n = normals   .size (1);

  if ( normals.size(0) != 3 || (n != k && n != 3 * k) )
  {
    shapeError ( JEM_FUNC,
                 "triangle normal matrix", normals.shape() );
  }

  normals_.ref ( makeContiguous( normals ) );

  normalMode_ = TRIANG_NORMALS;
}


//-----------------------------------------------------------------------
//   setVertexNormals
//-----------------------------------------------------------------------


void TriangleSet::setVertexNormals ( const GLbyteMatrix& normals )
{
  if ( normals.size(0) != 3 ||
       normals.size(1) != vertices_->size() )
  {
    shapeError ( JEM_FUNC,
                 "vertex normal matrix", normals.shape() );
  }

  normals_.ref ( makeContiguous( normals ) );

  normalMode_ = VERTEX_NORMALS;
}


//-----------------------------------------------------------------------
//   findTriangle
//-----------------------------------------------------------------------


void TriangleSet::findTriangle

  ( idx_t&     index,
    GLfloat&  dist,
    GLfloat   x,
    GLfloat   y,
    GLfloat   z ) const

{
  if ( ! checked_ )
  {
    const_cast<Self*>(this)->checkData_ ( getContext() );
  }

  GLfloat  pos[3] = { x, y, z };


  index = JEM_IDX_C(-1);
  dist  = 0.0F;

  if ( vertices_->rank() == 2 )
  {
    findTriangle2D_ ( index, dist, pos );
  }
  else
  {
    findTriangle3D_ ( index, dist, pos );
  }
}


//-----------------------------------------------------------------------
//   drawNoColors2D_
//-----------------------------------------------------------------------


void TriangleSet::drawNoColors2D_ () const
{
  Work2D_  w ( *this );


  glBegin    ( GL_TRIANGLES  );
  glNormal3f ( 0.0, 0.0, 1.0 );

  for ( idx_t i = w.trCount; i > 0; i-- )
  {
    glVertex2fv ( w.vcoords + (2 * w.triangs[0]) );
    glVertex2fv ( w.vcoords + (2 * w.triangs[1]) );
    glVertex2fv ( w.vcoords + (2 * w.triangs[2]) );

    w.triangs  += 3;
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   drawNoColors3D_
//-----------------------------------------------------------------------


void TriangleSet::drawNoColors3D_ () const
{
  Work3D_  w ( *this );


  glBegin ( GL_TRIANGLES );

  if ( normalMode_ == VERTEX_NORMALS )
  {
    for ( idx_t i = w.trCount; i > 0; i-- )
    {
      glNormal3bv ( w.normals + (3 * w.triangs[0]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[0]) );
      glNormal3bv ( w.normals + (3 * w.triangs[1]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[1]) );
      glNormal3bv ( w.normals + (3 * w.triangs[2]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[2]) );

      w.triangs  += 3;
    }
  }
  else if ( w.nrCount == w.trCount )
  {
    for ( idx_t i = w.trCount; i > 0; i-- )
    {
      glNormal3bv ( w.normals );
      glVertex3fv ( w.vcoords + (3 * w.triangs[0]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[1]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[2]) );

      w.triangs  += 3;
      w.normals  += 3;
    }
  }
  else
  {
    for ( idx_t i = w.trCount; i > 0; i-- )
    {
      glNormal3bv ( w.normals );
      glVertex3fv ( w.vcoords + (3 * w.triangs[0]) );
      glNormal3bv ( w.normals +  3 );
      glVertex3fv ( w.vcoords + (3 * w.triangs[1]) );
      glNormal3bv ( w.normals +  6);
      glVertex3fv ( w.vcoords + (3 * w.triangs[2]) );

      w.triangs  += 3;
      w.normals  += 9;
    }
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   drawTrColors2D_
//-----------------------------------------------------------------------


void TriangleSet::drawTrColors2D_ () const
{
  const GLubyte*  colors = trColors_.addr ();
  const idx_t     cst    = trColors_.size (0);

  Work2D_         w ( *this );


  glBegin    ( GL_TRIANGLES  );
  glNormal3f ( 0.0, 0.0, 1.0 );

  for ( idx_t i = w.trCount; i > 0; i-- )
  {
    if ( cst == 3 )
    {
      glColor3ubv ( colors );
    }
    else
    {
      glColor4ubv ( colors );
    }

    glVertex2fv ( w.vcoords + (2 * w.triangs[0]) );
    glVertex2fv ( w.vcoords + (2 * w.triangs[1]) );
    glVertex2fv ( w.vcoords + (2 * w.triangs[2]) );

    w.triangs  += 3;
    colors     += cst;
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   drawTrColors3D_
//-----------------------------------------------------------------------


void TriangleSet::drawTrColors3D_ () const
{
  const GLubyte*  colors = trColors_.addr ();
  const idx_t     cst    = trColors_.size (0);

  Work3D_         w ( *this );


  glBegin ( GL_TRIANGLES );

  if ( normalMode_ == VERTEX_NORMALS )
  {
    for ( idx_t i = w.trCount; i > 0; i-- )
    {
      if ( cst == 3 )
      {
        glColor3ubv ( colors );
      }
      else
      {
        glColor4ubv ( colors );
      }

      glNormal3bv ( w.normals + (3 * w.triangs[0]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[0]) );
      glNormal3bv ( w.normals + (3 * w.triangs[1]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[1]) );
      glNormal3bv ( w.normals + (3 * w.triangs[2]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[2]) );

      w.triangs  += 3;
      colors     += cst;
    }
  }
  else if ( w.nrCount == w.trCount )
  {
    for ( idx_t i = w.trCount; i > 0; i-- )
    {
      if ( cst == 3 )
      {
        glColor3ubv ( colors );
      }
      else
      {
        glColor4ubv ( colors );
      }

      glNormal3bv ( w.normals );
      glVertex3fv ( w.vcoords + (3 * w.triangs[0]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[1]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[2]) );

      w.triangs  += 3;
      w.normals  += 3;
      colors     += cst;
    }
  }
  else
  {
    for ( idx_t i = w.trCount; i > 0; i-- )
    {
      if ( cst == 3 )
      {
        glColor3ubv ( colors );
      }
      else
      {
        glColor4ubv ( colors );
      }

      glNormal3bv ( w.normals );
      glVertex3fv ( w.vcoords + (3 * w.triangs[0]) );
      glNormal3bv ( w.normals +  3 );
      glVertex3fv ( w.vcoords + (3 * w.triangs[1]) );
      glNormal3bv ( w.normals +  6 );
      glVertex3fv ( w.vcoords + (3 * w.triangs[2]) );

      w.triangs  += 3;
      w.normals  += 9;
      colors     += cst;
    }
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   drawVxColors2D_
//-----------------------------------------------------------------------


void TriangleSet::drawVxColors2D_ () const
{
  const GLfloat*  colors = vxColors_.addr ();

  Work2D_         w ( *this );


  glBegin    ( GL_TRIANGLES  );
  glNormal3f ( 0.0, 0.0, 1.0 );

  for ( idx_t i = w.trCount; i > 0; i-- )
  {
    idx_t  iv0 = w.triangs[0];
    idx_t  iv1 = w.triangs[1];
    idx_t  iv2 = w.triangs[2];

    glTexCoord1f ( colors[iv0] );
    glVertex2fv  ( w.vcoords + (2 * iv0) );
    glTexCoord1f ( colors[iv1] );
    glVertex2fv  ( w.vcoords + (2 * iv1) );
    glTexCoord1f ( colors[iv2] );
    glVertex2fv  ( w.vcoords + (2 * iv2) );

    w.triangs   += 3;
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   drawVxColors3D_
//-----------------------------------------------------------------------


void TriangleSet::drawVxColors3D_ () const
{
  const GLfloat*  colors = vxColors_.addr ();

  Work3D_         w ( *this );


  glBegin ( GL_TRIANGLES  );

  if ( normalMode_ == VERTEX_NORMALS )
  {
    for ( idx_t i = w.trCount; i > 0; i-- )
    {
      idx_t  iv0 = w.triangs[0];
      idx_t  iv1 = w.triangs[1];
      idx_t  iv2 = w.triangs[2];

      glTexCoord1f ( colors[iv0] );
      glNormal3bv  ( w.normals + (3 * iv0) );
      glVertex3fv  ( w.vcoords + (3 * iv0) );
      glTexCoord1f ( colors[iv1] );
      glNormal3bv  ( w.normals + (3 * iv1) );
      glVertex3fv  ( w.vcoords + (3 * iv1) );
      glTexCoord1f ( colors[iv2] );
      glNormal3bv  ( w.normals + (3 * iv2) );
      glVertex3fv  ( w.vcoords + (3 * iv2) );

      w.triangs   += 3;
    }
  }
  else if ( w.nrCount == w.trCount )
  {
    for ( idx_t i = w.trCount; i > 0; i-- )
    {
      idx_t  iv0 = w.triangs[0];
      idx_t  iv1 = w.triangs[1];
      idx_t  iv2 = w.triangs[2];

      glNormal3bv  ( w.normals );
      glTexCoord1f ( colors[iv0] );
      glVertex3fv  ( w.vcoords + (3 * iv0) );
      glTexCoord1f ( colors[iv1] );
      glVertex3fv  ( w.vcoords + (3 * iv1) );
      glTexCoord1f ( colors[iv2] );
      glVertex3fv  ( w.vcoords + (3 * iv2) );

      w.triangs   += 3;
      w.normals   += 3;
    }
  }
  else
  {
    for ( idx_t i = w.trCount; i > 0; i-- )
    {
      idx_t  iv0 = w.triangs[0];
      idx_t  iv1 = w.triangs[1];
      idx_t  iv2 = w.triangs[2];

      glNormal3bv  ( w.normals );
      glTexCoord1f ( colors[iv0] );
      glVertex3fv  ( w.vcoords + (3 * iv0) );
      glNormal3bv  ( w.normals +  3 );
      glTexCoord1f ( colors[iv1] );
      glVertex3fv  ( w.vcoords + (3 * iv1) );
      glNormal3bv  ( w.normals +  6 );
      glTexCoord1f ( colors[iv2] );
      glVertex3fv  ( w.vcoords + (3 * iv2) );

      w.triangs   += 3;
      w.normals   += 9;
    }
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   blendNoColors3D_
//-----------------------------------------------------------------------


void TriangleSet::blendNoColors3D_ () const
{
  Work3D_    w     ( *this );
  IdxVector  iperm ( w.trCount );

  GLboolean  zmask;


  beginBlend_ ( zmask );
  w.getZOrder ( iperm );

  glBegin     ( GL_TRIANGLES );

  if ( normalMode_ == VERTEX_NORMALS )
  {
    for ( idx_t i = 0; i < w.trCount; i++ )
    {
      idx_t  j = 3 * iperm[i];

      glNormal3bv ( w.normals + (3 * w.triangs[j + 0]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 0]) );
      glNormal3bv ( w.normals + (3 * w.triangs[j + 1]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 1]) );
      glNormal3bv ( w.normals + (3 * w.triangs[j + 2]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 2]) );
    }
  }
  else if ( w.nrCount == w.trCount )
  {
    for ( idx_t i = 0; i < w.trCount; i++ )
    {
      idx_t  j = 3 * iperm[i];

      glNormal3bv ( w.normals +  j );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 0]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 1]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 2]) );
    }
  }
  else
  {
    for ( idx_t i = 0; i < w.trCount; i++ )
    {
      idx_t  j = 3 * iperm[i];

      glNormal3bv ( w.normals + (3 * j + 0) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 0]) );
      glNormal3bv ( w.normals + (3 * j + 3) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 1]) );
      glNormal3bv ( w.normals + (3 * j + 6) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 2]) );
    }
  }

  glEnd     ();
  endBlend_ ( zmask );
}


//-----------------------------------------------------------------------
//   blendTrColors3D_
//-----------------------------------------------------------------------


void TriangleSet::blendTrColors3D_ () const
{
  const GLubyte*  colors = trColors_.addr ();
  const idx_t     cst    = trColors_.size (0);

  Work3D_         w      ( *this );
  IdxVector       iperm  ( w.trCount );

  GLboolean       zmask;


  beginBlend_ ( zmask );
  w.getZOrder ( iperm );

  glBegin ( GL_TRIANGLES );

  if ( normalMode_ == VERTEX_NORMALS )
  {
    for ( idx_t i = 0; i < w.trCount; i++ )
    {
      idx_t  j = iperm[i];

      if ( cst == 3 )
      {
        glColor3ubv ( colors + (j * JEM_IDX_C(3)) );
      }
      else
      {
        glColor4ubv ( colors + (j * JEM_IDX_C(4)) );
      }

      j *= 3;

      glNormal3bv ( w.normals + (3 * w.triangs[j + 0]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 0]) );
      glNormal3bv ( w.normals + (3 * w.triangs[j + 1]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 1]) );
      glNormal3bv ( w.normals + (3 * w.triangs[j + 2]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 2]) );
    }
  }
  else if ( w.nrCount == w.trCount )
  {
    for ( idx_t i = 0; i < w.trCount; i++ )
    {
      idx_t  j = iperm[i];

      if ( cst == 3 )
      {
        glColor3ubv ( colors + (j * JEM_IDX_C(3)) );
      }
      else
      {
        glColor4ubv ( colors + (j * JEM_IDX_C(4)) );
      }

      j *= 3;

      glNormal3bv ( w.normals +  j );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 0]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 1]) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 2]) );
    }
  }
  else
  {
    for ( idx_t i = 0; i < w.trCount; i++ )
    {
      idx_t  j = iperm[i];

      if ( cst == 3 )
      {
        glColor3ubv ( colors + (j * JEM_IDX_C(3)) );
      }
      else
      {
        glColor4ubv ( colors + (j * JEM_IDX_C(4)) );
      }

      j *= 3;

      glNormal3bv ( w.normals + (3 * j + 0) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 0]) );
      glNormal3bv ( w.normals + (3 * j + 3) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 1]) );
      glNormal3bv ( w.normals + (3 * j + 6) );
      glVertex3fv ( w.vcoords + (3 * w.triangs[j + 2]) );
    }
  }

  glEnd     ();
  endBlend_ ( zmask );
}


//-----------------------------------------------------------------------
//   blendVxColors3D_
//-----------------------------------------------------------------------


void TriangleSet::blendVxColors3D_ () const
{
  const GLfloat*  colors = vxColors_.addr ();

  Work3D_         w      ( *this );
  IdxVector       iperm  ( w.trCount );

  GLboolean       zmask;


  beginBlend_ ( zmask );
  w.getZOrder ( iperm );

  glBegin ( GL_TRIANGLES  );

  if ( normalMode_ == VERTEX_NORMALS )
  {
    for ( idx_t i = 0; i < w.trCount; i++ )
    {
      idx_t  j   = 3 * iperm[i];
      idx_t  iv0 = w.triangs[j + 0];
      idx_t  iv1 = w.triangs[j + 1];
      idx_t  iv2 = w.triangs[j + 2];

      glTexCoord1f ( colors[iv0] );
      glNormal3bv  ( w.normals + (3 * iv0) );
      glVertex3fv  ( w.vcoords + (3 * iv0) );
      glTexCoord1f ( colors[iv1] );
      glNormal3bv  ( w.normals + (3 * iv1) );
      glVertex3fv  ( w.vcoords + (3 * iv1) );
      glTexCoord1f ( colors[iv2] );
      glNormal3bv  ( w.normals + (3 * iv2) );
      glVertex3fv  ( w.vcoords + (3 * iv2) );
    }
  }
  else if ( w.nrCount == w.trCount )
  {
    for ( idx_t i = 0; i < w.trCount; i++ )
    {
      idx_t  j   = 3 * iperm[i];
      idx_t  iv0 = w.triangs[j + 0];
      idx_t  iv1 = w.triangs[j + 1];
      idx_t  iv2 = w.triangs[j + 2];

      glNormal3bv  ( w.normals + j );
      glTexCoord1f ( colors[iv0] );
      glVertex3fv  ( w.vcoords + (3 * iv0) );
      glTexCoord1f ( colors[iv1] );
      glVertex3fv  ( w.vcoords + (3 * iv1) );
      glTexCoord1f ( colors[iv2] );
      glVertex3fv  ( w.vcoords + (3 * iv2) );
    }
  }
  else
  {
    for ( idx_t i = 0; i < w.trCount; i++ )
    {
      idx_t  j   = 3 * iperm[i];
      idx_t  iv0 = w.triangs[j + 0];
      idx_t  iv1 = w.triangs[j + 1];
      idx_t  iv2 = w.triangs[j + 2];

      glNormal3bv  ( w.normals + (3 * j + 0) );
      glTexCoord1f ( colors[iv0] );
      glVertex3fv  ( w.vcoords + (3 * iv0) );
      glNormal3bv  ( w.normals + (3 * j + 3) );
      glTexCoord1f ( colors[iv1] );
      glVertex3fv  ( w.vcoords + (3 * iv1) );
      glNormal3bv  ( w.normals + (3 * j + 6) );
      glTexCoord1f ( colors[iv2] );
      glVertex3fv  ( w.vcoords + (3 * iv2) );
    }
  }

  glEnd     ();
  endBlend_ ( zmask );
}


//-----------------------------------------------------------------------
//   plotNoColors2D_
//-----------------------------------------------------------------------


void TriangleSet::plotNoColors2D_ ( double lod ) const
{
  const double  st = 1.0 / lod;

  Work2D_       w  ( *this );

  const idx_t*  tr = w.triangs;

  GLfloat       c[2];


  glBegin    ( GL_POINTS );
  glNormal3f ( 0.0, 0.0, 1.0 );

  for ( idx_t i = 0, j = 0; j < w.trCount;
        i++, j = (idx_t) (st * (double) i) )
  {
    w.triangs     = tr + (3 * j);

    w.getTrCenter ( c );
    glVertex2fv   ( c );
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   plotNoColors3D_
//-----------------------------------------------------------------------


void TriangleSet::plotNoColors3D_ ( double lod ) const
{
  const double  st = 1.0 / lod;

  Work3D_       w  ( *this );

  const idx_t*  tr = w.triangs;

  GLfloat       c[3];


  glBegin ( GL_POINTS );

  if ( normalMode_ == VERTEX_NORMALS )
  {
    for ( idx_t i = 0, j = 0; j < w.trCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      w.triangs     = tr + (3 * j);

      glNormal3bv   ( w.normals + (3 * w.triangs[0]) );
      w.getTrCenter ( c );
      glVertex3fv   ( c );
    }
  }
  else if ( w.nrCount == w.trCount )
  {
    for ( idx_t i = 0, j = 0; j < w.trCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      w.triangs     = tr + (3 * j);

      glNormal3bv   ( w.normals + (3 * j) );
      w.getTrCenter ( c );
      glVertex3fv   ( c );
    }
  }
  else
  {
    for ( idx_t i = 0, j = 0; j < w.trCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      w.triangs     = tr + (3 * j);

      glNormal3bv   ( w.normals + (9 * j) );
      w.getTrCenter ( c );
      glVertex3fv   ( c );
    }
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   plotTrColors2D_
//-----------------------------------------------------------------------


void TriangleSet::plotTrColors2D_ ( double lod ) const
{
  const GLubyte*  colors = trColors_.addr ();
  const idx_t     cst    = trColors_.size (0);

  const double    st     = 1.0 / lod;

  Work2D_         w      ( *this );

  const idx_t*    tr     = w.triangs;

  GLfloat         c[2];


  glBegin    ( GL_POINTS );
  glNormal3f ( 0.0, 0.0, 1.0 );

  for ( idx_t i = 0, j = 0; j < w.trCount;
        i++, j = (idx_t) (st * (double) i) )
  {
    w.triangs = tr + (3 * j);

    if ( cst == 3 )
    {
      glColor3ubv ( colors + (j * JEM_IDX_C(3)) );
    }
    else
    {
      glColor4ubv ( colors + (j * JEM_IDX_C(4)) );
    }

    w.getTrCenter ( c );
    glVertex2fv   ( c );
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   plotTrColors3D_
//-----------------------------------------------------------------------


void TriangleSet::plotTrColors3D_ ( double lod ) const
{
  const GLubyte*  colors = trColors_.addr ();
  const idx_t     cst    = trColors_.size (0);

  const double    st     = 1.0 / lod;

  Work3D_         w      ( *this );

  const idx_t*    tr     = w.triangs;

  GLfloat         c[3];


  glBegin ( GL_POINTS );

  if ( normalMode_ == VERTEX_NORMALS )
  {
    for ( idx_t i = 0, j = 0; j < w.trCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      w.triangs = tr + (3 * j);

      if ( cst == 3 )
      {
        glColor3ubv ( colors + (j * JEM_IDX_C(3)) );
      }
      else
      {
        glColor4ubv ( colors + (j * JEM_IDX_C(4)) );
      }

      glNormal3bv   ( w.normals + (3 * w.triangs[0]) );
      w.getTrCenter ( c );
      glVertex3fv   ( c );
    }
  }
  else if ( w.nrCount == w.trCount )
  {
    for ( idx_t i = 0, j = 0; j < w.trCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      w.triangs = tr + (3 * j);

      if ( cst == 3 )
      {
        glColor3ubv ( colors + (j * JEM_IDX_C(3)) );
      }
      else
      {
        glColor4ubv ( colors + (j * JEM_IDX_C(4)) );
      }

      glNormal3bv   ( w.normals + (j * 3)   );
      w.getTrCenter ( c );
      glVertex3fv   ( c );
    }
  }
  else
  {
    for ( idx_t i = 0, j = 0; j < w.trCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      w.triangs = tr + (3 * j);

      if ( cst == 3 )
      {
        glColor3ubv ( colors + (j * JEM_IDX_C(3)) );
      }
      else
      {
        glColor4ubv ( colors + (j * JEM_IDX_C(4)) );
      }

      glNormal3bv   ( w.normals + (j * 9)   );
      w.getTrCenter ( c );
      glVertex3fv   ( c );
    }
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   plotVxColors2D_
//-----------------------------------------------------------------------


void TriangleSet::plotVxColors2D_ ( double lod ) const
{
  const GLfloat*  colors = vxColors_.addr ();

  const double    st     = 1.0 / lod;

  Work2D_         w      ( *this );

  const idx_t*    tr     = w.triangs;

  GLfloat         c[2];


  glBegin    ( GL_POINTS     );
  glNormal3f ( 0.0, 0.0, 1.0 );

  for ( idx_t i = 0, j = 0; j < w.trCount;
        i++, j = (idx_t) (st * (double) i) )
  {
    w.triangs     = tr + (3 * j);

    glTexCoord1f  ( colors[w.triangs[0]] );
    w.getTrCenter ( c );
    glVertex2fv   ( c );
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   plotVxColors3D_
//-----------------------------------------------------------------------


void TriangleSet::plotVxColors3D_ ( double lod ) const
{
  const GLfloat*  colors = vxColors_.addr ();

  const double    st     = 1.0 / lod;

  Work3D_         w      ( *this );

  const idx_t*    tr     = w.triangs;

  GLfloat         c[3];


  glBegin ( GL_POINTS );

  if ( normalMode_ == VERTEX_NORMALS )
  {
    for ( idx_t i = 0, j = 0; j < w.trCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      w.triangs     = tr + (3 * j);

      glTexCoord1f  ( colors[w.triangs[0]] );
      glNormal3bv   ( w.normals + (3 * w.triangs[0]) );
      w.getTrCenter ( c );
      glVertex3fv   ( c );
    }
  }
  else if ( w.nrCount == w.trCount )
  {
    for ( idx_t i = 0, j = 0; j < w.trCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      w.triangs     = tr + (3 * j);

      glNormal3bv   ( w.normals + (3 * j)  );
      glTexCoord1f  ( colors[w.triangs[0]] );
      w.getTrCenter ( c );
      glVertex3fv   ( c );
    }
  }
  else
  {
    for ( idx_t i = 0, j = 0; j < w.trCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      w.triangs     = tr + (3 * j);

      glNormal3bv   ( w.normals + (9 * j)  );
      glTexCoord1f  ( colors[w.triangs[0]] );
      w.getTrCenter ( c );
      glVertex3fv   ( c );
    }
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   beginBlend_
//-----------------------------------------------------------------------


inline void TriangleSet::beginBlend_ ( GLboolean& zmask ) const
{
  glGetBooleanv ( GL_DEPTH_WRITEMASK, & zmask );

  if ( zmask == GL_TRUE )
  {
    glDepthMask ( GL_FALSE );
  }
}


//-----------------------------------------------------------------------
//   endBlend_
//-----------------------------------------------------------------------


void TriangleSet::endBlend_ ( GLboolean zmask ) const
{
  if ( zmask == GL_TRUE )
  {
    DisableScope  lightScope ( GL_LIGHTING );

    glDepthMask ( zmask );
    glColor4f   ( 0.0F, 0.0F, 0.0F, 0.0F );

    drawNoColors3D_ ();
  }
}


//-----------------------------------------------------------------------
//   findTriangle2D_
//-----------------------------------------------------------------------


void TriangleSet::findTriangle2D_

  ( idx_t&         index,
    GLfloat&       dist,
    const GLfloat  pos[3] ) const

{
  Work2D_   w ( *this );

  GLdouble  minDist2;
  GLdouble  dist2;


  minDist2 = 0.0;

  for ( idx_t i = 0; i < w.trCount; i++ )
  {
    dist2      = w.distanceTo ( pos );
    w.triangs += 3;

    if ( dist2 < 0.0 )
    {
      continue;
    }

    if ( index < 0 || dist2 < minDist2 )
    {
      minDist2 = dist2;
      index    = i;
    }
  }

  if ( index >= 0 )
  {
    minDist2 += pos[2] * pos[2];
    dist      = (GLfloat) std::sqrt ( minDist2 );
  }
}


//-----------------------------------------------------------------------
//   findTriangle3D_
//-----------------------------------------------------------------------


void TriangleSet::findTriangle3D_

  ( idx_t&          index,
    GLfloat&       dist,
    const GLfloat  pos[3] ) const

{
  Work3D_   w ( *this );

  GLdouble  minDist2;
  GLdouble  dist2;


  minDist2 = 0.0;

  for ( idx_t i = 0; i < w.trCount; i++ )
  {
    dist2      = w.distanceTo ( pos );
    w.triangs += 3;

    if ( dist2 < 0.0 )
    {
      continue;
    }

    if ( index < 0 || dist2 < minDist2 )
    {
      minDist2 = dist2;
      index    = i;
    }
  }

  if ( index >= 0 )
  {
    dist = (GLfloat) std::sqrt ( minDist2 );
  }
}


//-----------------------------------------------------------------------
//   checkData_
//-----------------------------------------------------------------------


void TriangleSet::checkData_ ( const String& context )
{
  const idx_t  rank    = vertices_->rank ();
  const idx_t  vxCount = vertices_->size ();
  const idx_t  trCount = triangles_.size (1);


  if ( vxCount > 0 && (rank < 2 || rank > 3) )
  {
    shapeError ( context, "vertex array", vertices_->shape() );
  }

  if ( trCount > 0 )
  {
    idx_t  imin = jem::min ( triangles_ );
    idx_t  imax = jem::max ( triangles_ );

    if ( imin < 0 )
    {
      indexError ( context, "vertex", imin );
    }
    if ( imax >= vxCount )
    {
      indexError ( context, "vertex", imax, vxCount );
    }
  }

  if ( colorMode_       == VERTEX_COLORS  &&
       vxColors_.size() != vxCount )
  {
    sizeError ( context,
                "vertex color vector", vxColors_.size() );
  }

  if ( normalMode_      == VERTEX_NORMALS &&
       normals_.size(1) != vxCount )
  {
    shapeError ( context,
                 "vertex normal matrix", normals_.shape() );
  }

  checked_ = true;
}


//-----------------------------------------------------------------------
//   updateNormals_
//-----------------------------------------------------------------------


void TriangleSet::updateNormals_ ()
{
  using jem::io::Writer;

  if ( vertices_->rank() == 3 )
  {
    GLfloatMatrix   vertices = vertices_->getCoords ();

    const idx_t     trCount  = triangles_.size (1);
    const idx_t*    triangs  = triangles_.addr ();
    const GLfloat*  vcoords  = vertices  .addr ();

    GLbyte*         normals;
    Vertex3d        a, b, c;
    idx_t           errCount;


    normals_.resize ( 3, trCount );

    errCount = 0;
    normals  = normals_.addr ();

    for ( idx_t i = 0; i < trCount; i++ )
    {
      idx_t  k = 3 * triangs[0];

      c[0] = vcoords[k + 0];
      c[1] = vcoords[k + 1];
      c[2] = vcoords[k + 2];

      k    = 3 * triangs[1];

      a[0] = vcoords[k + 0] - c[0];
      a[1] = vcoords[k + 1] - c[1];
      a[2] = vcoords[k + 2] - c[2];

      k    = 3 * triangs[2];

      b[0] = vcoords[k + 0] - c[0];
      b[1] = vcoords[k + 1] - c[1];
      b[2] = vcoords[k + 2] - c[2];

      if ( isTiny( getNormal( c, a, b ) ) )
      {
        errCount++;

        c[0] = 0.0;
        c[1] = 0.0;
        c[2] = 1.0;
      }

      normals[0] = toByte ( c[0] );
      normals[1] = toByte ( c[1] );
      normals[2] = toByte ( c[2] );

      triangs   += 3;
      normals   += 3;
    }

    if ( errCount )
    {
      Writer&  warn = System::warn ();

      print ( warn, myName_ );

      if ( errCount == 1 )
      {
        print ( warn, " : singular triangle (ignored)\n\n" );
      }
      else
      {
        print ( warn, " : ", errCount,
                " singular triangles (ignored)\n\n" );
      }
    }
  }
  else
  {
    normals_.ref ( GLbyteMatrix() );
  }

  updated_ = true;
}


//-----------------------------------------------------------------------
//   connectHandlers_
//-----------------------------------------------------------------------


void TriangleSet::connectHandlers_ ()
{
  util::connect ( vertices_->newGeomEvent,
                  this, & Self::newGeomHandler_ );

  util::connect ( vertices_->newShapeEvent,
                  this, & Self::newShapeHandler_ );
}


//-----------------------------------------------------------------------
//   newGeomHandler_
//-----------------------------------------------------------------------


void TriangleSet::newGeomHandler_ ()
{
  if ( normalMode_ == AUTO_NORMALS )
  {
    updated_ = false;
  }
}


//-----------------------------------------------------------------------
//   newShapeHandler_
//-----------------------------------------------------------------------


void TriangleSet::newShapeHandler_ ()
{
  checked_ = false;

  if ( normalMode_ == AUTO_NORMALS )
  {
    updated_ = false;
  }
}


//-----------------------------------------------------------------------
//   getPointSize_
//-----------------------------------------------------------------------


GLfloat TriangleSet::getPointSize_ ( double lod ) const
{
  Transform  t, p;
  Rect       vport;
  Box        box;
  Vertex3d   a, b;
  double     w, h;
  GLfloat    ps;


  vertices_->getBBox ( box );

  glGetDoublev   ( GL_MODELVIEW_MATRIX,  t.addr() );
  glGetDoublev   ( GL_PROJECTION_MATRIX, p.addr() );

  box.transform  ( t );
  box.project    ( p );
  box.getCorners ( a, b );

  // Forget it if this triangle set is not visible

  if ( a[0] > 1.0 || b[0] < -1.0 ||
       a[1] > 1.0 || b[1] < -1.0 ||
       a[2] > 1.0 || b[2] < -1.0 )
  {
    return -1.0;
  }

  getViewport ( vport );

  toWindow    ( a, vport );
  toWindow    ( b, vport );

  w  = b[0] - a[0];
  h  = b[1] - a[1];

  ps = (GLfloat)
    (1.2 * std::sqrt ( (w * h) / (lod * (double) this->size()) ));

  return clamped ( ps, (GLfloat) 1.0, (GLfloat) 20.0 );
}


JEM_END_PACKAGE( gl )
