
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

#ifndef JEM_GL_RECT_H
#define JEM_GL_RECT_H

#include <jem/gl/import.h>
#include <jem/gl/Size.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class Rect
//-----------------------------------------------------------------------


class Rect
{
 public:

  inline                    Rect          ();

  explicit inline           Rect

    ( GLsizei                 size );

  explicit inline           Rect

    ( const Size2i&           size );

  inline                    Rect

    ( GLsizei                 width,
      GLsizei                 height );

  inline                    Rect

    ( GLint                   x,
      GLint                   y,
      const Size2i&           size );

  inline                    Rect

    ( GLint                   x,
      GLint                   y,
      GLsizei                 width,
      GLsizei                 height );

  inline                    Rect

    ( const Rect&             rhs );

  inline Rect&              operator =

    ( const Rect&             rhs );

  void                      swap

    ( Rect&                   rhs );

  inline Size2i             size          () const;

  inline void               resize

    ( const Size2i&           s );

  inline bool               isValid       () const;
  inline bool               isEmpty       () const;
  inline void               makeEmpty     ();
  inline GLdouble           aspectRatio   () const;
  inline void               draw          () const;

  inline bool               contains

    ( GLint                   x,
      GLint                   y )            const;


 public:

  GLint                     x;
  GLint                     y;
  GLsizei                   width;
  GLsizei                   height;

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


inline void                 swap

  ( Rect&                     lhs,
    Rect&                     rhs );

void                        print

  ( TextOutput&               out,
    const Rect&               rect );

inline void                 getViewport

  ( Rect&                     vport );

inline Rect                 getViewport   ();

inline void                 loadOrtho

  ( const Rect&               vport,
    GLdouble                  clip0,
    GLdouble                  clip1 );




//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class Rect
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


inline Rect::Rect ()
{
  x = y  = 0;
  width  = 0;
  height = 0;
}


inline Rect::Rect ( GLsizei s )
{
  x = y  = 0;
  width  = s;
  height = s;
}


inline Rect::Rect ( const Size2i& s )
{
  x = y  = 0;
  width  = s.width;
  height = s.height;
}

inline Rect::Rect ( GLsizei w, GLsizei h )
{
  x = y  = 0;
  width  = w;
  height = h;
}


inline Rect::Rect ( GLint x, GLint y, const Size2i& s )
{
  this->x = x;
  this->y = y;
  width   = s.width;
  height  = s.height;
}


inline Rect::Rect ( GLint x, GLint y, GLsizei w, GLsizei h )
{
  this->x = x;
  this->y = y;
  width   = w;
  height  = h;
}


inline Rect::Rect ( const Rect& rhs )
{
  x      = rhs.x;
  y      = rhs.y;
  width  = rhs.width;
  height = rhs.height;
}


//-----------------------------------------------------------------------
//   assignment operator
//-----------------------------------------------------------------------


inline Rect& Rect::operator = ( const Rect& rhs )
{
  x      = rhs.x;
  y      = rhs.y;
  width  = rhs.width;
  height = rhs.height;

  return *this;
}


//-----------------------------------------------------------------------
//   size
//-----------------------------------------------------------------------


inline Size2i Rect::size () const
{
  return Size2i( width, height );
}


//-----------------------------------------------------------------------
//   resize
//-----------------------------------------------------------------------


inline void Rect::resize ( const Size2i& s )
{
  width  = s.width;
  height = s.height;
}


//-----------------------------------------------------------------------
//   isValid
//-----------------------------------------------------------------------


inline bool Rect::isValid () const
{
  return (width >= 0 && height >= 0);
}


//-----------------------------------------------------------------------
//   isEmpty
//-----------------------------------------------------------------------


inline bool Rect::isEmpty () const
{
  return (width <= 0 || height <= 0);
}


//-----------------------------------------------------------------------
//   makeEmpty
//-----------------------------------------------------------------------


inline void Rect::makeEmpty ()
{
  x = y  = 0;
  width  = 0;
  height = 0;
}


//-----------------------------------------------------------------------
//   aspectRatio
//-----------------------------------------------------------------------


inline GLdouble Rect::aspectRatio () const
{
  return (height + 0.001) / (width + 0.001);
}


//-----------------------------------------------------------------------
//   draw
//-----------------------------------------------------------------------


inline void Rect::draw () const
{
  glRecti ( x, y, x + width, y + height );
}


//-----------------------------------------------------------------------
//   contains
//-----------------------------------------------------------------------


inline bool Rect::contains

  ( GLint  x,
    GLint  y ) const

{
  GLint  xMin = this->x;
  GLint  yMin = this->y;
  GLint  xMax = xMin + width;
  GLint  yMax = yMin + height;

  return (x >= xMin && x < xMax &&
          y >= yMin && y < yMax);
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


inline void swap ( Rect& lhs, Rect& rhs )
{
  lhs.swap ( rhs );
}


//-----------------------------------------------------------------------
//   getViewport
//-----------------------------------------------------------------------


inline Rect getViewport ()
{
  Rect  vport;

  getViewport ( vport );

  return vport;
}


inline void getViewport ( Rect& vport )
{
  GLint  buf[4];

  glGetIntegerv ( GL_VIEWPORT, buf );

  vport.x      =           buf[0];
  vport.y      =           buf[1];
  vport.width  = (GLsizei) buf[2];
  vport.height = (GLsizei) buf[3];
}


//-----------------------------------------------------------------------
//   loadOrtho
//-----------------------------------------------------------------------


inline void     loadOrtho

  ( const Rect&   rect,
    GLdouble      clip0,
    GLdouble      clip1 )

{
  glLoadIdentity ();
  glOrtho        ( rect.x, rect.x + rect.width,
                   rect.y, rect.y + rect.height, clip0, clip1 );
}


JEM_END_PACKAGE( gl )

#endif
