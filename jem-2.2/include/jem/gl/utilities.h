
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

#ifndef JEM_GL_UTILITIES_H
#define JEM_GL_UTILITIES_H

#include <jem/base/String.h>
#include <jem/util/utilities.h>
#include <jem/gl/gl.h>


JEM_BEGIN_PACKAGE( gl )


using jem::util::clamp;
using jem::util::clamped;


//-----------------------------------------------------------------------
//   global constants
//-----------------------------------------------------------------------


extern const GLdouble     RADS_PER_DEGREE;
extern const GLdouble     DEGREES_PER_RAD;


//-----------------------------------------------------------------------
//   class EnableScope
//-----------------------------------------------------------------------


class EnableScope
{
 public:

  explicit inline         EnableScope

    ( GLenum                cap );

  inline                  EnableScope

    ( GLenum                cap,
      bool                  choice );

  inline                 ~EnableScope ();


 private:

  GLenum                  cap_;
  GLboolean               state_;

};


//-----------------------------------------------------------------------
//   class DisableScope
//-----------------------------------------------------------------------


class DisableScope
{
 public:

  explicit inline         DisableScope

    ( GLenum                cap );

  inline                  DisableScope

    ( GLenum                cap,
      bool                  choice );

  inline                 ~DisableScope ();


 private:

  GLenum                  cap_;
  GLboolean               state_;

};


//-----------------------------------------------------------------------
//   ZMaskScope
//-----------------------------------------------------------------------


class ZMaskScope
{
 public:

  inline                  ZMaskScope      ();
  inline                 ~ZMaskScope      ();


 private:

  GLboolean               mask_;

};


//-----------------------------------------------------------------------
//   class DrawBufferScope
//-----------------------------------------------------------------------


class DrawBufferScope
{
 public:

  explicit inline         DrawBufferScope

    ( GLenum                mode );

  inline                 ~DrawBufferScope ();


 private:

  GLint                   mode_;

};


//-----------------------------------------------------------------------
//   class CanvasScope
//-----------------------------------------------------------------------


class CanvasScope
{
 public:

  inline                  CanvasScope   ();

  inline                  CanvasScope

    ( GLfloat               clip0,
      GLfloat               clip1 );

                         ~CanvasScope   ();


 public:

  GLsizei                 width;
  GLsizei                 height;
  GLfloat                 nearClip;
  GLfloat                 farClip;


 private:

  void                    init_

    ( GLfloat               clip0,
      GLfloat               clip1 );


 private:

  GLboolean               lighting_;
  GLenum                  depthFunc_;

};


//-----------------------------------------------------------------------
//   utility functions
//-----------------------------------------------------------------------


void                      checkGLError

  ( const String&           where = "" );

GLbitfield                getClearMask  ();

void                      adjustTickCount

  ( int&                    nticks );

void                      renderDisk

  ( GLfloat                 radius  = 1.0F,
    int                     quality = 1 );

void                      renderSlice

  ( GLfloat                 alpha,
    GLfloat                 beta,
    GLfloat                 radius  = 1.0F,
    int                     quality = 1 );

void                      renderArrow3D

  ( GLfloat                 size    = 0.2F );

void                      renderShadow

  ( GLfloat                 x0,
    GLfloat                 y0,
    GLfloat                 x1,
    GLfloat                 y1,
    GLfloat                 w );

void                      round

  ( float&                  val,
    float                   step );

void                      round

  ( double&                 val,
    double                  step );

inline float              rounded

  ( float                   val,
    float                   step );

inline double             rounded

  ( double                  val,
    double                  step );

inline void               clamp

  ( float&                  t );

inline void               clamp

  ( double&                 t );

inline float              clamped

  ( float                   t );

inline double             clamped

  ( double                  t );




//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class EnableScope
//=======================================================================


inline EnableScope::EnableScope ( GLenum cap ) : cap_ ( cap )
{
  state_ = glIsEnabled ( cap );

  if ( state_ == GL_FALSE )
  {
    glEnable ( cap_ );
  }
}


inline EnableScope::EnableScope

  ( GLenum  cap,
    bool    choice ) : cap_ ( cap )

{
  if ( choice )
  {
    state_ = glIsEnabled ( cap );

    if ( state_ == GL_FALSE )
    {
      glEnable ( cap );
    }
  }
  else
  {
    state_ = GL_TRUE;
  }
}


inline EnableScope::~EnableScope ()
{
  if ( state_ == GL_FALSE )
  {
    glDisable ( cap_ );
  }
}


//=======================================================================
//   class DisableScope
//=======================================================================


inline DisableScope::DisableScope ( GLenum cap ) : cap_ ( cap )
{
  state_ = glIsEnabled ( cap );

  if ( state_ == GL_TRUE )
  {
    glDisable ( cap_ );
  }
}


inline DisableScope::DisableScope

  ( GLenum  cap,
    bool    choice ) : cap_ ( cap )

{
  if ( choice )
  {
    state_ = glIsEnabled ( cap );

    if ( state_ == GL_TRUE )
    {
      glDisable ( cap );
    }
  }
  else
  {
    state_ = GL_FALSE;
  }
}


inline DisableScope::~DisableScope ()
{
  if ( state_ == GL_TRUE )
  {
    glEnable ( cap_ );
  }
}


//=======================================================================
//   class ZMaskScope
//=======================================================================


inline ZMaskScope::ZMaskScope ()
{
  glGetBooleanv ( GL_DEPTH_WRITEMASK, & mask_ );

  if ( mask_ == GL_TRUE )
  {
    glDepthMask ( GL_FALSE );
  }
}


inline ZMaskScope::~ZMaskScope ()
{
  if ( mask_ == GL_TRUE )
  {
    glDepthMask ( GL_TRUE );
  }
}


//=======================================================================
//   class DrawBufferScope
//=======================================================================


inline DrawBufferScope::DrawBufferScope ( GLenum mode )
{
  glGetIntegerv ( GL_DRAW_BUFFER, & mode_ );
  glDrawBuffer  ( mode );
}


inline DrawBufferScope::~DrawBufferScope ()
{
  glDrawBuffer   ( (GLenum) mode_ );
}


//=======================================================================
//   class CanvasScope
//=======================================================================


inline CanvasScope::CanvasScope ()
{
  init_ ( -1.0, 40.0 );
}


inline CanvasScope::CanvasScope ( GLfloat clip0, GLfloat clip1 )
{
  init_ ( clip0, clip1 );
}


//=======================================================================
//   utility functions
//=======================================================================

//-----------------------------------------------------------------------
//   rounded
//-----------------------------------------------------------------------


inline float rounded ( float val, float step )
{
  round ( val, step );

  return val;
}


inline double rounded ( double val, double step )
{
  round ( val, step );

  return val;
}


//-----------------------------------------------------------------------
//   clamp
//-----------------------------------------------------------------------


inline void clamp ( float& t )
{
  clamp ( t, 0.0F, 1.0F );
}


inline void clamp ( double& t )
{
  clamp ( t, 0.0, 1.0 );
}


//-----------------------------------------------------------------------
//   clamped
//-----------------------------------------------------------------------


inline float clamped ( float t )
{
  return clamped ( t, 0.0F, 1.0F );
}


inline double clamped ( double t )
{
  return clamped ( t, 0.0, 1.0 );
}


JEM_END_PACKAGE( gl )

#endif
