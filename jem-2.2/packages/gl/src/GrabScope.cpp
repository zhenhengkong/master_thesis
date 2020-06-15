
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


#include <jem/util/Properties.h>
#include <jem/gl/Rect.h>
#include <jem/gl/Actions.h>
#include <jem/gl/GfxNode.h>
#include <jem/gl/GrabScope.h>


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class GrabScope
//=======================================================================

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


GrabScope::GrabScope ()
{
  GLint  auxCount;

  glGetIntegerv ( GL_DRAW_BUFFER, &drawBuf_ );
  glGetIntegerv ( GL_READ_BUFFER, &readBuf_ );
  glGetIntegerv ( GL_AUX_BUFFERS, &auxCount );

  if ( auxCount > 0 )
  {
    grabBuf_ = GL_AUX0;
  }
  else
  {
    grabBuf_ = GL_BACK;
  }

  if ( drawBuf_ != grabBuf_ )
  {
    // Clear the current error state.

    glGetError   ();
    glDrawBuffer ( (GLenum) grabBuf_ );

    // Check whether the operation succeeded; a back buffer may not
    // always be available.

    if ( glGetError() != GL_NO_ERROR )
    {
      grabBuf_ = drawBuf_;
    }
  }

  if ( readBuf_ != grabBuf_ )
  {
    glReadBuffer ( (GLenum) grabBuf_ );
  }
}


GrabScope::~GrabScope ()
{
  if ( drawBuf_ != grabBuf_ )
  {
    glDrawBuffer ( (GLenum) drawBuf_ );
  }

  if ( readBuf_ != grabBuf_ )
  {
    glReadBuffer ( (GLenum) readBuf_ );
  }
}


//-----------------------------------------------------------------------
//   drawFrame
//-----------------------------------------------------------------------


void GrabScope::drawFrame

  ( GfxContext&  gtx,
    GfxNode&     scene )

{
  Properties  params;

  params.set         ( GLActionParams::SNAPSHOT, true );
  scene.takeGLAction ( gtx, GLActions::REDRAW, params );

  glFlush ();
}


//-----------------------------------------------------------------------
//   adjustRect
//-----------------------------------------------------------------------


void GrabScope::adjustRect ( Rect& rect )
{
  // Some (old) OpenGL implementations go beserk if the width is not
  // a multiple of four.

  rect.width = rect.width - (rect.width % 4);
}


JEM_END_PACKAGE( gl )
