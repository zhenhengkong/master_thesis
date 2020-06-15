
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
#include <jem/base/ClassTemplate.h>
#include <jem/base/IllegalArgumentException.h>
#include <jem/base/array/operators.h>
#include <jem/base/array/utilities.h>
#include <jem/util/Event.h>
#include <jem/util/Properties.h>
#include <jem/gl/byte.h>
#include <jem/gl/error.h>
#include <jem/gl/utilities.h>
#include <jem/gl/Menu.h>
#include <jem/gl/Color.h>
#include <jem/gl/Commands.h>
#include <jem/gl/VertexSet.h>
#include <jem/gl/ColorMap.h>
#include <jem/gl/LineSet.h>


JEM_DEFINE_CLASS( jem::gl::LineSet );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class LineSet::Work_
//=======================================================================


class LineSet::Work_
{
 public:

  explicit inline         Work_

    ( const LineSet&        ls );

  void                    getZOrder

    ( const IdxVector&      iperm );

  GLdouble                distanceTo2D

    ( const GLfloat*        x )        const;

  GLdouble                distanceTo3D

    ( const GLfloat*        x )        const;


 public:

  const idx_t             lnCount;
  const idx_t*            offsets;
  const idx_t*            indices;
  const GLfloat*          vcoords;

};


//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------


inline LineSet::Work_::Work_ ( const LineSet& ls ) :

  lnCount ( ls.lnOffsets_.size() - 1 )

{
  offsets = ls.lnOffsets_.addr    ();
  indices = ls.vxIndices_.addr    ();
  vcoords = ls.vertices_->getData ();
}


//-----------------------------------------------------------------------
//   getZOrder
//-----------------------------------------------------------------------


void LineSet::Work_::getZOrder ( const IdxVector& iperm )
{
  const GLfloat*  vx       = vcoords;

  const idx_t*    offsets0 = offsets;

  GLfloatVector   z ( lnCount );

  GLdouble        mx[16];
  GLfloat         t0, t1, t2;


  glGetDoublev ( GL_MODELVIEW_MATRIX, mx );

  t0 = (GLfloat) mx[ 2];
  t1 = (GLfloat) mx[ 6];
  t2 = (GLfloat) mx[10];

  for ( idx_t i = 0; i < lnCount; i++ )
  {
    if ( offsets[0] != offsets[1] )
    {
      idx_t  j = 3 * indices[offsets[0]];

      z[i] = t0 * vx[j + 0] + t1 * vx[j + 1] + t2 * vx[j + 2];
    }
    else
    {
      z[i] = 0.0;
    }

    iperm[i] = i;

    offsets++;
  }

  offsets = offsets0;

  sort ( iperm, z );
}


//-----------------------------------------------------------------------
//   distanceTo2D
//-----------------------------------------------------------------------


GLdouble LineSet::Work_::distanceTo2D ( const GLfloat* x ) const
{
  const idx_t  n = offsets[1] - 1;

  GLdouble     u[2];
  GLdouble     v[2];

  GLdouble     uu, uv, vv;
  GLdouble     d,  dmin;
  GLdouble     t;


  dmin = -1.0;

  for ( idx_t i = offsets[0]; i < n; i++ )
  {
    idx_t  iv0 = 2 * indices[i + 0];
    idx_t  iv1 = 2 * indices[i + 1];

    u[0] = vcoords[iv0 + 0] - x[0];
    u[1] = vcoords[iv0 + 1] - x[1];

    v[0] = vcoords[iv1 + 0] - vcoords[iv0 + 0];
    v[1] = vcoords[iv1 + 1] - vcoords[iv0 + 1];

    uu   = u[0] * u[0] + u[1] * u[1];
    uv   = u[0] * v[0] + u[1] * v[1];
    vv   = v[0] * v[0] + v[1] * v[1];

    if ( isTiny( vv ) )
    {
      d = uu;
    }
    else
    {
      t = clamped ( -uv / vv );
      d = uu + 2.0 * t * uv + t * t * vv;
    }

    if ( dmin < 0.0 || d < dmin )
    {
      dmin = d;
    }
  }

  return dmin;
}


//-----------------------------------------------------------------------
//   distanceTo3D
//-----------------------------------------------------------------------


GLdouble LineSet::Work_::distanceTo3D ( const GLfloat* x ) const
{
  const idx_t  n = offsets[1] - 1;

  GLdouble     u[3];
  GLdouble     v[3];

  GLdouble     uu, uv, vv;
  GLdouble     d,  dmin;
  GLdouble     t;


  dmin = -1.0;

  for ( idx_t i = offsets[0]; i < n; i++ )
  {
    idx_t  iv0 = 3 * indices[i + 0];
    idx_t  iv1 = 3 * indices[i + 1];

    u[0] = vcoords[iv0 + 0] - x[0];
    u[1] = vcoords[iv0 + 1] - x[1];
    u[2] = vcoords[iv0 + 2] - x[2];

    v[0] = vcoords[iv1 + 0] - vcoords[iv0 + 0];
    v[1] = vcoords[iv1 + 1] - vcoords[iv0 + 1];
    v[2] = vcoords[iv1 + 2] - vcoords[iv0 + 2];

    uu   = u[0] * u[0] + u[1] * u[1] + u[2] * u[2];
    uv   = u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
    vv   = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];

    if ( isTiny( vv ) )
    {
      d = uu;
    }
    else
    {
      t = clamped ( -uv / vv );
      d = uu + 2.0 * t * uv + t * t * vv;
    }

    if ( dmin < 0.0 || d < dmin )
    {
      dmin = d;
    }
  }

  return dmin;
}


//=======================================================================
//   class LineSet
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const int  LineSet::SAMPLE_COUNT = 16 * 1024;


//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


LineSet::LineSet

  ( const String&          name,
    const Ref<VertexSet>&  vx ) :

    Super      ( name ),
    vertices_  (   vx ),
    lnOffsets_ (    1 )

{
  JEM_PRECHECK ( vx != NIL );

  if ( vx->size() > 0 && (vx->rank() < 2 || vx->rank() > 3) )
  {
    shapeError ( JEM_FUNC, "vertex array", vx->shape() );
  }

  lnOffsets_  = 0;
  colorMode_  = NO_COLORS;
  opaque_     = true;
  noStrips_   = false;
  checked_    = true;

  connectHandlers_ ();
}


LineSet::~LineSet ()
{}


//-----------------------------------------------------------------------
//   getBBox
//-----------------------------------------------------------------------


void LineSet::getBBox ( Box& box ) const
{
  // TO DO: get actual bbox of lines (not all vertices have to
  //        be used).

  vertices_->getBBox ( box );
}


//-----------------------------------------------------------------------
//   redraw
//-----------------------------------------------------------------------


void LineSet::redraw ( GfxContext& gtx )
{
  if ( this->size() == 0 )
  {
    return;
  }

  DisableScope  lightScope ( GL_LIGHTING );

  const idx_t   rank = vertices_->rank ();

  GLfloat       col0[4];


  if ( ! checked_ )
  {
    checkData_ ( getContext() );
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
        drawNoColors2D_  ();
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

  case LINE_COLORS:

    if ( opaque_ )
    {
      DisableScope  blendScope ( GL_BLEND );

      if ( rank == 2 )
      {
        drawLnColors2D_ ();
      }
      else
      {
        drawLnColors3D_  ();
      }
    }
    else
    {
      if ( rank == 2 )
      {
        drawLnColors2D_  ();
      }
      else
      {
        blendLnColors3D_ ();
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

    glColor4fv         ( col0 );
    colorMap_->disable ();

    break;
  }
}


//-----------------------------------------------------------------------
//   redrawFast
//-----------------------------------------------------------------------


void LineSet::redrawFast

  ( GfxContext&  gtx,
    double       lod )

{
  if ( lod <= 0.0 )
  {
    return;
  }

  if ( this->size() < SAMPLE_COUNT || lod >= 1.0 )
  {
    Self::redraw ( gtx ); return;
  }

  DisableScope  blendScope ( GL_BLEND    );
  DisableScope  lightScope ( GL_LIGHTING );

  const idx_t   rank = vertices_->rank ();

  GLfloat       col0[4];
  GLfloat       ps0;


  if ( ! checked_ )
  {
    checkData_ ( getContext() );
  }

  glGetFloatv ( GL_POINT_SIZE, &ps0 );
  glPointSize ( 2.0 );

  switch ( colorMode_ )
  {
  case NO_COLORS:

    if ( rank == 2 )
    {
      plotNoColors2D_ ( lod );
    }
    else
    {
      plotNoColors3D_ ( lod );
    }

    break;

  case LINE_COLORS:

    glGetFloatv ( GL_CURRENT_COLOR, col0 );

    if ( rank == 2 )
    {
      plotLnColors2D_ ( lod );
    }
    else
    {
      plotLnColors3D_ ( lod );
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


bool LineSet::exeCommand

  ( GfxContext&        gtx,
    const String&      cmd,
    const Properties&  params )

{
  if ( cmd == Commands::HIDE_LINES )
  {
    hidden = true;
    return   true;
  }

  if ( cmd == Commands::SHOW_LINES )
  {
    hidden = false;
    return   true;
  }

  return false;
}


//-----------------------------------------------------------------------
//   listOptions
//-----------------------------------------------------------------------


void LineSet::listOptions ( Menu& menu ) const
{
  menu.addOption ( Commands::HIDE_LINES,
                   Commands::SHOW_LINES, hidden );
}


//-----------------------------------------------------------------------
//   setTopology
//-----------------------------------------------------------------------


void LineSet::setTopology

  ( const IdxVector&  offsets,
    const IdxVector&  indices )

{
  JEM_PRECHECK ( offsets.size() > 0 );

  const idx_t  n = offsets.size() - 1;


  lnOffsets_.ref ( makeContiguous( offsets ) );
  vxIndices_.ref ( makeContiguous( indices ) );

  setNoColors ();

  noStrips_ = false;
  checked_  = false;

  checkData_ ( JEM_FUNC );

  noStrips_ = true;

  for ( idx_t i = 0; i < n; i++ )
  {
    if ( offsets[i + 1] - offsets[i] != 2 )
    {
      noStrips_ = false;
      break;
    }
  }
}


//-----------------------------------------------------------------------
//   getLineOffsets
//-----------------------------------------------------------------------


IdxVector LineSet::getLineOffsets () const
{
  return lnOffsets_;
}


//-----------------------------------------------------------------------
//   getVertexIndices
//-----------------------------------------------------------------------


IdxVector LineSet::getVertexIndices () const
{
  return vxIndices_;
}


//-----------------------------------------------------------------------
//   setNoColors
//-----------------------------------------------------------------------


void LineSet::setNoColors ()
{
  if ( colorMode_ != NO_COLORS )
  {
    lnColors_.ref ( GLubyteMatrix() );
    vxColors_.ref ( GLfloatVector() );

    opaque_    = true;
    colorMap_  = NIL;
    colorMode_ = NO_COLORS;
  }
}


//-----------------------------------------------------------------------
//   setLineColors
//-----------------------------------------------------------------------


void LineSet::setLineColors ( const GLubyteMatrix& colors )
{
  JEM_PRECHECK ( colors.size(0) >= 3 &&
                 colors.size(0) <= 4 );

  const idx_t  n     = colors.size (1);

  GLubyte      alpha = toUbyte ( Color::OPAQUE_ALPHA );


  if ( n != lnOffsets_.size() - 1 )
  {
    shapeError ( JEM_FUNC,
                 "line color matrix", colors.shape() );
  }

  lnColors_.ref ( makeContiguous( colors ) );
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
  colorMode_ = LINE_COLORS;
}


//-----------------------------------------------------------------------
//   setVertexColors
//-----------------------------------------------------------------------


void LineSet::setVertexColors

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
  lnColors_.ref ( GLubyteMatrix() );

  colorMap_  = colorMap;
  colorMode_ = VERTEX_COLORS;
}


//-----------------------------------------------------------------------
//   findLine
//-----------------------------------------------------------------------


void LineSet::findLine

  ( idx_t&    index,
    GLfloat&  dist,
    GLfloat   x,
    GLfloat   y,
    GLfloat   z ) const

{
  if ( ! checked_ )
  {
    const_cast<Self*>(this)->checkData_ ( getContext() );
  }

  GLfloat   pos[3] = { x, y, z };

  Work_     w ( *this );

  GLdouble  minDist2;
  GLdouble  dist2;


  index    = -1;
  dist     =  0.0F;
  minDist2 =  0.0;

  if ( vertices_->rank() == 2 )
  {
    for ( idx_t i = 0; i < w.lnCount; i++ )
    {
      dist2 = w.distanceTo2D ( pos );

      w.offsets++;

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
    }
  }
  else
  {
    for ( idx_t i = 0; i < w.lnCount; i++ )
    {
      dist2 = w.distanceTo3D ( pos );

      w.offsets++;

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
  }

  if ( index >= 0 )
  {
    dist = (GLfloat) std::sqrt ( minDist2 );
  }
}


//-----------------------------------------------------------------------
//   drawNoColors2D_
//-----------------------------------------------------------------------


void LineSet::drawNoColors2D_ () const
{
  Work_  w ( *this );

  if ( noStrips_ )
  {
    glBegin ( GL_LINES );

    for ( idx_t i = w.lnCount; i > 0; i-- )
    {
      glVertex2fv ( w.vcoords + (2 * w.indices[0] ) );
      glVertex2fv ( w.vcoords + (2 * w.indices[1] ) );

      w.indices  += 2;
    }

    glEnd ();
  }
  else
  {
    for ( idx_t i = w.lnCount; i > 0; i-- )
    {
      idx_t  n = w.offsets[1];

      glBegin ( GL_LINE_STRIP );

      for ( idx_t j = w.offsets[0]; j < n; j++ )
      {
        glVertex2fv ( w.vcoords + (2 * w.indices[j]) );
      }

      glEnd ();

      w.offsets++;
    }
  }
}


//-----------------------------------------------------------------------
//   drawNoColors3D_
//-----------------------------------------------------------------------


void LineSet::drawNoColors3D_ () const
{
  Work_  w ( *this );

  if ( noStrips_ )
  {
    glBegin ( GL_LINES );

    for ( idx_t i = w.lnCount; i > 0; i-- )
    {
      glVertex3fv ( w.vcoords + (3 * w.indices[0] ) );
      glVertex3fv ( w.vcoords + (3 * w.indices[1] ) );

      w.indices  += 2;
    }

    glEnd ();
  }
  else
  {
    for ( idx_t i = w.lnCount; i > 0; i-- )
    {
      idx_t  n = w.offsets[1];

      glBegin ( GL_LINE_STRIP );

      for ( idx_t j = w.offsets[0]; j < n; j++ )
      {
        glVertex3fv ( w.vcoords + (3 * w.indices[j]) );
      }

      glEnd ();

      w.offsets++;
    }
  }
}


//-----------------------------------------------------------------------
//   drawLnColors2D_
//-----------------------------------------------------------------------


void LineSet::drawLnColors2D_ () const
{
  const GLubyte*  colors = lnColors_.addr ();
  const idx_t     cst    = lnColors_.size (0);

  Work_           w ( *this );


  if ( noStrips_ )
  {
    glBegin ( GL_LINES );

    for ( idx_t i = w.lnCount; i > 0; i-- )
    {
      if ( cst == 3 )
      {
        glColor3ubv ( colors );
      }
      else
      {
        glColor4ubv ( colors );
      }

      glVertex2fv ( w.vcoords + (2 * w.indices[0] ) );
      glVertex2fv ( w.vcoords + (2 * w.indices[1] ) );

      w.indices  += 2;
      colors     += cst;
    }

    glEnd ();
  }
  else
  {
    for ( idx_t i = w.lnCount; i > 0; i-- )
    {
      idx_t  n = w.offsets[1];

      glBegin ( GL_LINE_STRIP );

      if ( cst == 3 )
      {
        glColor3ubv ( colors );
      }
      else
      {
        glColor4ubv ( colors );
      }

      for ( idx_t j = w.offsets[0]; j < n; j++ )
      {
        glVertex2fv ( w.vcoords + (2 * w.indices[j]) );
      }

      glEnd ();

      w.offsets++;
      colors += cst;
    }
  }
}


//-----------------------------------------------------------------------
//   drawLnColors3D_
//-----------------------------------------------------------------------


void LineSet::drawLnColors3D_ () const
{
  const GLubyte*  colors = lnColors_.addr ();
  const idx_t     cst    = lnColors_.size (0);

  Work_           w ( *this );


  if ( noStrips_ )
  {
    glBegin ( GL_LINES );

    for ( idx_t i = w.lnCount; i > 0; i-- )
    {
      if ( cst == 3 )
      {
        glColor3ubv ( colors );
      }
      else
      {
        glColor4ubv ( colors );
      }

      glVertex3fv ( w.vcoords + (3 * w.indices[0] ) );
      glVertex3fv ( w.vcoords + (3 * w.indices[1] ) );

      w.indices  += 2;
      colors     += cst;
    }

    glEnd ();
  }
  else
  {
    for ( idx_t i = w.lnCount; i > 0; i-- )
    {
      idx_t  n = w.offsets[1];

      glBegin ( GL_LINE_STRIP );

      if ( cst == 3 )
      {
        glColor3ubv ( colors );
      }
      else
      {
        glColor4ubv ( colors );
      }

      for ( idx_t j = w.offsets[0]; j < n; j++ )
      {
        glVertex3fv ( w.vcoords + (3 * w.indices[j]) );
      }

      glEnd ();

      w.offsets++;
      colors += cst;
    }
  }
}


//-----------------------------------------------------------------------
//   drawVxColors2D_
//-----------------------------------------------------------------------


void LineSet::drawVxColors2D_ () const
{
  const GLfloat*  colors = vxColors_.addr ();

  Work_           w ( *this );


  if ( noStrips_ )
  {
    glBegin ( GL_LINES );

    for ( idx_t i = w.lnCount; i > 0; i-- )
    {
      idx_t  iv0 = w.indices[0];
      idx_t  iv1 = w.indices[1];

      glTexCoord1f ( colors[iv0] );
      glVertex2fv  ( w.vcoords + (2 * iv0) );
      glTexCoord1f ( colors[iv1] );
      glVertex2fv  ( w.vcoords + (2 * iv1) );

      w.indices   += 2;
    }

    glEnd ();
  }
  else
  {
    for ( idx_t i = w.lnCount; i > 0; i-- )
    {
      idx_t n = w.offsets[1];

      glBegin ( GL_LINE_STRIP );

      for ( idx_t j = w.offsets[0]; j < n; j++ )
      {
        idx_t  iv0 = w.indices[j];

        glTexCoord1f ( colors[iv0] );
        glVertex2fv  ( w.vcoords + (2 * iv0) );
      }

      glEnd ();

      w.offsets++;
    }
  }
}


//-----------------------------------------------------------------------
//   drawVxColors3D_
//-----------------------------------------------------------------------


void LineSet::drawVxColors3D_ () const
{
  const GLfloat*  colors = vxColors_.addr ();

  Work_           w ( *this );


  if ( noStrips_ )
  {
    glBegin ( GL_LINES );

    for ( idx_t i = w.lnCount; i > 0; i-- )
    {
      idx_t  iv0 = w.indices[0];
      idx_t  iv1 = w.indices[1];

      glTexCoord1f ( colors[iv0] );
      glVertex3fv  ( w.vcoords + (3 * iv0) );
      glTexCoord1f ( colors[iv1] );
      glVertex3fv  ( w.vcoords + (3 * iv1) );

      w.indices   += 2;
    }

    glEnd ();
  }
  else
  {
    for ( idx_t i = w.lnCount; i > 0; i-- )
    {
      idx_t  n = w.offsets[1];

      glBegin ( GL_LINE_STRIP );

      for ( idx_t j = w.offsets[0]; j < n; j++ )
      {
        idx_t  iv0 = w.indices[j];

        glTexCoord1f ( colors[iv0] );
        glVertex3fv  ( w.vcoords + (3 * iv0) );
      }

      glEnd ();

      w.offsets++;
    }
  }
}


//-----------------------------------------------------------------------
//   blendNoColors3D_
//-----------------------------------------------------------------------


void LineSet::blendNoColors3D_ () const
{
  Work_      w     ( *this );

  IdxVector  iperm ( w.lnCount );

  GLboolean  zmask;


  beginBlend_ ( zmask );
  w.getZOrder ( iperm );

  if ( noStrips_ )
  {
    glBegin ( GL_LINES );

    for ( idx_t i = 0; i < w.lnCount; i++ )
    {
      idx_t  k = 2 * iperm[i];

      glVertex3fv ( w.vcoords + (3 * w.indices[k + 0] ) );
      glVertex3fv ( w.vcoords + (3 * w.indices[k + 1] ) );
    }

    glEnd ();
  }
  else
  {
    for ( idx_t i = 0; i < w.lnCount; i++ )
    {
      idx_t   k = iperm[i];
      idx_t  n = w.offsets[k + 1];

      glBegin ( GL_LINE_STRIP );

      for ( idx_t j = w.offsets[k]; j < n; j++ )
      {
        glVertex3fv ( w.vcoords + (3 * w.indices[j]) );
      }

      glEnd ();
    }
  }

  endBlend_ ( zmask );
}


//-----------------------------------------------------------------------
//   blendLnColors3D_
//-----------------------------------------------------------------------


void LineSet::blendLnColors3D_ () const
{
  const GLubyte*  colors = lnColors_.addr ();
  const idx_t     cst    = lnColors_.size (0);

  Work_           w      ( *this );
  IdxVector       iperm  ( w.lnCount );

  GLboolean       zmask;


  beginBlend_ ( zmask );
  w.getZOrder ( iperm );

  if ( noStrips_ )
  {
    glBegin ( GL_LINES );

    for ( idx_t i = 0; i < w.lnCount; i++ )
    {
      idx_t  k  = iperm[i];

      if ( cst == 3 )
      {
        glColor3ubv ( colors + (k * JEM_IDX_C(3)) );
      }
      else
      {
        glColor4ubv ( colors + (k * JEM_IDX_C(4)) );
      }

      k *= 2;

      glVertex3fv ( w.vcoords + (3 * w.indices[k + 0] ) );
      glVertex3fv ( w.vcoords + (3 * w.indices[k + 1] ) );
    }

    glEnd ();
  }
  else
  {
    for ( idx_t i = 0; i < w.lnCount; i++ )
    {
      idx_t  k = iperm[i];
      idx_t  n = w.offsets[k + 1];

      glBegin ( GL_LINE_STRIP );

      if ( cst == 3 )
      {
        glColor3ubv ( colors + (k * JEM_IDX_C(3)) );
      }
      else
      {
        glColor4ubv ( colors + (k * JEM_IDX_C(4)) );
      }

      for ( idx_t j = w.offsets[k]; j < n; j++ )
      {
        glVertex3fv ( w.vcoords + (3 * w.indices[j]) );
      }

      glEnd ();
    }
  }

  endBlend_ ( zmask );
}


//-----------------------------------------------------------------------
//   blendVxColors3D_
//-----------------------------------------------------------------------


void LineSet::blendVxColors3D_ () const
{
  const GLfloat*  colors = vxColors_.addr ();

  Work_           w      ( *this );
  IdxVector       iperm  ( w.lnCount );

  GLboolean       zmask;


  beginBlend_ ( zmask );
  w.getZOrder ( iperm );

  if ( noStrips_ )
  {
    glBegin ( GL_LINES );

    for ( idx_t i = 0; i < w.lnCount; i++ )
    {
      idx_t  k   = 2 * iperm[i];
      idx_t  iv0 = w.indices[k + 0];
      idx_t  iv1 = w.indices[k + 1];

      glTexCoord1f ( colors[iv0] );
      glVertex3fv  ( w.vcoords + (3 * iv0) );
      glTexCoord1f ( colors[iv1] );
      glVertex3fv  ( w.vcoords + (3 * iv1) );
    }

    glEnd ();
  }
  else
  {
    for ( idx_t i = 0; i < w.lnCount; i++ )
    {
      idx_t  k = iperm[i];
      idx_t  n = w.offsets[k + 1];

      glBegin ( GL_LINE_STRIP );

      for ( idx_t j = w.offsets[k]; j < n; j++ )
      {
        idx_t  iv0 = w.indices[j];

        glTexCoord1f ( colors[iv0] );
        glVertex3fv  ( w.vcoords + (3 * iv0) );
      }

      glEnd ();
    }
  }

  endBlend_ ( zmask );
}


//-----------------------------------------------------------------------
//   plotNoColors2D_
//-----------------------------------------------------------------------


void LineSet::plotNoColors2D_ ( double lod ) const
{
  const double  st = 1.0 / lod;

  Work_         w  ( *this );


  glBegin ( GL_POINTS );

  if ( noStrips_ )
  {
    for ( idx_t i = 0, j = 0; j < w.lnCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      glVertex2fv ( w.vcoords + (2 * w.indices[2 * j + 0] ) );
      glVertex2fv ( w.vcoords + (2 * w.indices[2 * j + 1] ) );
    }
  }
  else
  {
    for ( idx_t i = 0, j = 0; j < w.lnCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      idx_t  n = w.offsets[j + 1];

      for ( idx_t k = w.offsets[j]; k < n; k++ )
      {
        glVertex2fv ( w.vcoords + (2 * w.indices[k]) );
      }
    }
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   plotNoColors3D_
//-----------------------------------------------------------------------


void LineSet::plotNoColors3D_ ( double lod ) const
{
  const double  st = 1.0 / lod;

  Work_         w  ( *this );


  glBegin ( GL_POINTS );

  if ( noStrips_ )
  {
    for ( idx_t i = 0, j = 0; j < w.lnCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      glVertex3fv ( w.vcoords + (3 * w.indices[2 * j + 0] ) );
      glVertex3fv ( w.vcoords + (3 * w.indices[2 * j + 1] ) );
    }
  }
  else
  {
    for ( idx_t i = 0, j = 0; j < w.lnCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      idx_t  n = w.offsets[j + 1];

      for ( idx_t k = w.offsets[j]; k < n; k++ )
      {
        glVertex3fv ( w.vcoords + (3 * w.indices[k]) );
      }
    }
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   plotLnColors2D_
//-----------------------------------------------------------------------


void LineSet::plotLnColors2D_ ( double lod ) const
{
  const GLubyte*  colors = lnColors_.addr ();
  const idx_t     cst    = lnColors_.size (0);

  const double    st     = 1.0 / lod;

  Work_           w      ( *this );


  glBegin ( GL_POINTS );

  if ( noStrips_ )
  {
    for ( idx_t i = 0, j = 0; j < w.lnCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      if ( cst == 3 )
      {
        glColor3ubv ( colors + (j * JEM_IDX_C(3)) );
      }
      else
      {
        glColor4ubv ( colors + (j * JEM_IDX_C(4)) );
      }

      glVertex2fv ( w.vcoords + (2 * w.indices[2 * j + 0] ) );
      glVertex2fv ( w.vcoords + (2 * w.indices[2 * j + 1] ) );
    }
  }
  else
  {
    for ( idx_t i = 0, j = 0; j < w.lnCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      idx_t  n = w.offsets[j + 1];

      if ( cst == 3 )
      {
        glColor3ubv ( colors + (j * JEM_IDX_C(3)) );
      }
      else
      {
        glColor4ubv ( colors + (j * JEM_IDX_C(4)) );
      }

      for ( idx_t k = w.offsets[j]; k < n; k++ )
      {
        glVertex2fv ( w.vcoords + (2 * w.indices[k]) );
      }
    }
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   plotLnColors3D_
//-----------------------------------------------------------------------


void LineSet::plotLnColors3D_ ( double lod ) const
{
  const GLubyte*  colors = lnColors_.addr ();
  const idx_t     cst    = lnColors_.size (0);

  const double    st     = 1.0 / lod;

  Work_           w ( *this );


  glBegin ( GL_POINTS );

  if ( noStrips_ )
  {
    for ( idx_t i = 0, j = 0; j < w.lnCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      if ( cst == 3 )
      {
        glColor3ubv ( colors + (j * JEM_IDX_C(3)) );
      }
      else
      {
        glColor4ubv ( colors + (j * JEM_IDX_C(4)) );
      }

      glVertex3fv ( w.vcoords + (3 * w.indices[2 * j + 0] ) );
      glVertex3fv ( w.vcoords + (3 * w.indices[2 * j + 1] ) );
    }
  }
  else
  {
    for ( idx_t i = 0, j = 0; j < w.lnCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      idx_t  n = w.offsets[j + 1];

      if ( cst == 3 )
      {
        glColor3ubv ( colors + (j * JEM_IDX_C(3)) );
      }
      else
      {
        glColor4ubv ( colors + (j * JEM_IDX_C(4)) );
      }

      for ( idx_t k = w.offsets[j]; k < n; k++ )
      {
        glVertex3fv ( w.vcoords + (3 * w.indices[k]) );
      }
    }
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   plotVxColors2D_
//-----------------------------------------------------------------------


void LineSet::plotVxColors2D_ ( double lod ) const
{
  const GLfloat*  colors = vxColors_.addr ();

  const double    st     = 1.0 / lod;

  Work_           w      ( *this );


  glBegin ( GL_POINTS );

  if ( noStrips_ )
  {
    for ( idx_t i = 0, j = 0; j < w.lnCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      idx_t  iv0 = w.indices[2 * j + 0];
      idx_t  iv1 = w.indices[2 * j + 1];

      glTexCoord1f ( colors[iv0] );
      glVertex2fv  ( w.vcoords + (2 * iv0) );
      glTexCoord1f ( colors[iv1] );
      glVertex2fv  ( w.vcoords + (2 * iv1) );
    }
  }
  else
  {
    for ( idx_t i = 0, j = 0; j < w.lnCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      idx_t  n = w.offsets[j + 1];

      for ( idx_t k = w.offsets[j]; k < n; k++ )
      {
        idx_t  iv0 = w.indices[k];

        glTexCoord1f ( colors[iv0] );
        glVertex2fv  ( w.vcoords + (2 * iv0) );
      }
    }
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   plotVxColors3D_
//-----------------------------------------------------------------------


void LineSet::plotVxColors3D_ ( double lod ) const
{
  const GLfloat*  colors = vxColors_.addr ();

  const double    st     = 1.0 / lod;

  Work_           w      ( *this );


  glBegin ( GL_POINTS );

  if ( noStrips_ )
  {
    for ( idx_t i = 0, j = 0; j < w.lnCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      idx_t  iv0 = w.indices[2 * j + 0];
      idx_t  iv1 = w.indices[2 * j + 1];

      glTexCoord1f ( colors[iv0] );
      glVertex3fv  ( w.vcoords + (3 * iv0) );
      glTexCoord1f ( colors[iv1] );
      glVertex3fv  ( w.vcoords + (3 * iv1) );
    }
  }
  else
  {
    for ( idx_t i = 0, j = 0; j < w.lnCount;
          i++, j = (idx_t) (st * (double) i) )
    {
      idx_t  n = w.offsets[j + 1];

      for ( idx_t k = w.offsets[j]; k < n; k++ )
      {
        idx_t  iv0 = w.indices[k];

        glTexCoord1f ( colors[iv0] );
        glVertex3fv  ( w.vcoords + (3 * iv0) );
      }
    }
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   beginBlend_
//-----------------------------------------------------------------------


inline void LineSet::beginBlend_ ( GLboolean& zmask ) const
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


void LineSet::endBlend_ ( GLboolean zmask ) const
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
//   checkData_
//-----------------------------------------------------------------------


void LineSet::checkData_ ( const String& context )
{
  const idx_t  rank    = vertices_->rank ();
  const idx_t  vxCount = vertices_->size ();
  const idx_t  lnCount = lnOffsets_.size () - 1;
  const idx_t  ixCount = vxIndices_.size ();


  if ( vxCount > 0 && (rank < 2 || rank > 3) )
  {
    shapeError ( context, "vertex array", vertices_->shape() );
  }

  for ( idx_t i = 0; i < lnCount; i++ )
  {
    idx_t  j = lnOffsets_[i];
    idx_t  k = lnOffsets_[i + 1];

    if ( k > j && (j < 0 || j >= ixCount || k > ixCount) )
    {
      throw IllegalArgumentException (
        context,
        "invalid line offset array"
      );
    }
  }

  if ( ixCount > 0 )
  {
    idx_t  imin = min ( vxIndices_ );
    idx_t  imax = max ( vxIndices_ );

    if ( imin < 0 )
    {
      indexError ( context, "vertex", imin );
    }
    if ( imax >= vxCount )
    {
      indexError ( context, "vertex", imax, vxCount );
    }
  }

  if ( colorMode_       == VERTEX_COLORS &&
       vxColors_.size() != vxCount )
  {
    sizeError ( context,
                "vertex color vector", vxColors_.size() );
  }

  checked_ = true;
}


//-----------------------------------------------------------------------
//   connectHandlers_
//-----------------------------------------------------------------------


void LineSet::connectHandlers_ ()
{
  util::connect ( vertices_->newShapeEvent,
                  this, & Self::newShapeHandler_ );
}


//-----------------------------------------------------------------------
//   newShapeHandler_
//-----------------------------------------------------------------------


void LineSet::newShapeHandler_ ()
{
  checked_  = false;
}


JEM_END_PACKAGE( gl )
