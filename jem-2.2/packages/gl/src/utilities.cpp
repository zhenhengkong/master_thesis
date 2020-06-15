
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
#include <jem/base/System.h>
#include <jem/gl/glu.h>
#include <jem/gl/Color.h>
#include <jem/gl/GLException.h>
#include <jem/gl/utilities.h>


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   constants
//=======================================================================


const GLdouble  RADS_PER_DEGREE =

  (GLdouble) (3.14159265358979323846 / 180.0);

const GLdouble  DEGREES_PER_RAD =

  (GLdouble) (180.0 / 3.14159265358979323846);


//=======================================================================
//   class CanvasScope
//=======================================================================

//-----------------------------------------------------------------------
//   destructor
//-----------------------------------------------------------------------


CanvasScope::~CanvasScope ()
{
  glMatrixMode  ( GL_PROJECTION );
  glPopMatrix   ();
  glMatrixMode  ( GL_MODELVIEW  );
  glPopMatrix   ();

  if ( lighting_ == GL_TRUE )
  {
    glEnable ( GL_LIGHTING );
  }

  glDepthFunc ( depthFunc_ );
}


//-----------------------------------------------------------------------
//   init_
//-----------------------------------------------------------------------


void CanvasScope::init_ ( GLfloat clip0, GLfloat clip1 )
{
  GLint  vport[4];
  GLint  zfunc;

  glGetIntegerv  ( GL_VIEWPORT, vport );

  width    = (GLsizei) vport[2];
  height   = (GLsizei) vport[3];
  nearClip = clip0;
  farClip  = clip1;

  glMatrixMode   ( GL_PROJECTION );
  glPushMatrix   ();
  glLoadIdentity ();

  glOrtho        ( vport[0], vport[0] + vport[2],
                   vport[1], vport[1] + vport[3], clip0, clip1 );

  glMatrixMode   ( GL_MODELVIEW );
  glPushMatrix   ();
  glLoadIdentity ();

  lighting_ = glIsEnabled ( GL_LIGHTING );

  if ( lighting_ == GL_TRUE )
  {
    glDisable ( GL_LIGHTING );
  }

  glGetIntegerv ( GL_DEPTH_FUNC, &zfunc );
  glDepthFunc   ( GL_ALWAYS );

  depthFunc_ = (GLenum) zfunc;
}


//=======================================================================
//   utility functions
//=======================================================================

//-----------------------------------------------------------------------
//   checkGLError
//-----------------------------------------------------------------------


void checkGLError ( const String& where )
{
  static int  count = 0;

  for ( int i = 0; i < 10; i++ )
  {
    GLenum  err = glGetError ();

    if ( err == GL_NO_ERROR )
    {
      break;
    }

    if      ( err == GL_OUT_OF_MEMORY )
    {
      if ( where.size() > 0 )
      {
        throw GLException ( where, err );
      }
      else
      {
        throw GLException ( err );
      }
    }
    else if ( count < 20 )
    {
      print ( System::err(), "OpenGL error:  ",
              (const char*) gluErrorString( err ), '\n' );

      if ( where.size() )
      {
        print ( System::err(), "Error context: ", where, '\n' );
      }

      count++;
    }
  }
}


//-----------------------------------------------------------------------
//   getClearMask
//-----------------------------------------------------------------------


GLbitfield getClearMask ()
{
  GLbitfield  mask = GL_COLOR_BUFFER_BIT;

  if ( glIsEnabled( GL_DEPTH_TEST ) == GL_TRUE )
  {
    mask |= GL_DEPTH_BUFFER_BIT;
  }

  if ( glIsEnabled( GL_STENCIL_TEST ) == GL_TRUE )
  {
    mask |= GL_STENCIL_BUFFER_BIT;
  }

  return mask;
}


//-----------------------------------------------------------------------
//   adjustTickCount
//-----------------------------------------------------------------------


void adjustTickCount ( int& n )
{
  if      ( n < 2 )
  {
    n = 0;
  }
  else if ( n < 3 )
  {
    n = 2;
  }
  else if ( n < 5 )
  {
    n = 3;
  }
  else if ( n < 6 )
  {
    n = 5;
  }
  else if ( n < 11 )
  {
    n = 6;
  }
  else if ( n < 21 )
  {
    n = 11;
  }
  else
  {
    n = 21;
  }
}


//-----------------------------------------------------------------------
//   round
//-----------------------------------------------------------------------


void        round

  ( float&    val,
    float     step )

{
  double  v = val;
  double  s = step;

  if ( v < 0.0 )
  {
    v = ::floor ( (v + 0.5 * s) / s );
  }
  else
  {
    v = ::ceil  ( (v - 0.5 * s) / s );
  }

  val = (float) (v * s);
}


void        round

  ( double&   val,
    double    step )

{
  if ( val < 0.0 )
  {
    val = ::floor ( (val + 0.5 * step) / step );
  }
  else
  {
    val = ::ceil  ( (val - 0.5 * step) / step );
  }

  val *= step;
}


JEM_END_PACKAGE( gl )
