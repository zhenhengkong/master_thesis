
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


#include <jem/gl/utilities.h>
#include <jem/gl/GLSetup3D.h>


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class GLSetup3D
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


GLSetup3D::GLSetup3D ()
{}


GLSetup3D::~GLSetup3D ()
{}


//-----------------------------------------------------------------------
//   initGL
//-----------------------------------------------------------------------


void GLSetup3D::initGL ()
{
  Super::initGL   ();

  glPolygonOffset ( 1.0, 1.0 );

  glEnable        ( GL_DEPTH_TEST );

#ifdef JEM_OS_WINDOWS
  glEnable        ( GL_NORMALIZE );
#else
  glEnable        ( GL_RESCALE_NORMAL );
#endif

  checkGLError    ( JEM_FUNC );
}


JEM_END_PACKAGE( gl )
