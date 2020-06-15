
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


#include <jem/base/limits.h>
#include <jem/gl/utilities.h>
#include <jem/gl/Rect.h>
#include <jem/gl/StdGLSetup.h>


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class StdGLSetup
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


StdGLSetup::StdGLSetup ()
{
  zscaleSet_ = false;
}


StdGLSetup::~StdGLSetup ()
{}


//-----------------------------------------------------------------------
//   initGL
//-----------------------------------------------------------------------


void StdGLSetup::initGL ()
{
  // Clear the current error flag. If it has been set, some external
  // code must responsible.

  for ( int i = 0; i < 10; i++ )
  {
    GLenum  err = glGetError ();

    if ( err == GL_NO_ERROR )
    {
      break;
    }
  }

  glEnable        ( GL_BLEND );
  glBlendFunc     ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
  glPolygonMode   ( GL_FRONT_AND_BACK, GL_FILL );
  glEnable        ( GL_COLOR_MATERIAL );
  glAlphaFunc     ( GL_GREATER, 0.05F );
  checkGLError    ( JEM_FUNC );

  zscaleSet_ = false;
}


//-----------------------------------------------------------------------
//   initView
//-----------------------------------------------------------------------


void StdGLSetup::initView ( const Rect& r )
{
  if ( ! zscaleSet_ && r.width > 0 && r.height > 0 )
  {
    GLboolean  ztest = glIsEnabled ( GL_DEPTH_TEST );

    if ( ztest == GL_TRUE )
    {
      // This is a work-around for a stupid bug in ATI's implementation
      // of glReadPixels.

      GLfloat  d;

      glClear      ( GL_DEPTH_BUFFER_BIT );
      glFlush      ();
      glReadPixels ( 0, 0, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, & d );

      if ( ! isTiny( d - 1.0 ) )
      {
        glPixelTransferf ( GL_DEPTH_SCALE, 1.0F / d );
      }

      zscaleSet_ = true;
    }
  }

  checkGLError ( JEM_FUNC );
}


JEM_END_PACKAGE( gl )
