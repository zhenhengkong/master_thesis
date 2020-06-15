
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
#include <jem/gl/TextProjector.h>


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class TextProjector
//=======================================================================

//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------


TextProjector::TextProjector

  ( const Font&   font,
    Alignment     hAlign,
    Alignment     vAlign ) :

    Super ( font, hAlign, vAlign )

{
  proj_.setIdentity ();

  active_ = false;
}


//-----------------------------------------------------------------------
//   begin
//-----------------------------------------------------------------------


void TextProjector::begin ( GfxContext& gtx )
{
  Transform  p, t;

  if ( active_ )
  {
    end ();
  }

  getViewport    ( vport_ );
  glMatrixMode   ( GL_PROJECTION );
  getCurrent     ( p );
  glPushMatrix   ();
  loadOrtho      ( vport_, 0.0, -1.0 );
  glMatrixMode   ( GL_MODELVIEW );
  getCurrent     ( t );
  glPushMatrix   ();
  glLoadIdentity ();
  Super::begin   ( gtx );
  matmul         ( proj_, p, t );

  active_ = true;
}


void TextProjector::begin

  ( GfxContext&       gtx,
    const Transform&  proj )

{
  if ( active_ )
  {
    end ();
  }

  getViewport    ( vport_ );
  glMatrixMode   ( GL_PROJECTION );
  glPushMatrix   ();
  loadOrtho      ( vport_, 0.0, -1.0 );
  glMatrixMode   ( GL_MODELVIEW );
  glPushMatrix   ();
  glLoadIdentity ();
  Super::begin   ( gtx );

  proj_   = proj;
  active_ = true;
}


//-----------------------------------------------------------------------
//   end
//-----------------------------------------------------------------------


void TextProjector::end ()
{
  if ( active_ )
  {
    Super::end   ();
    glMatrixMode ( GL_PROJECTION );
    glPopMatrix  ();
    glMatrixMode ( GL_MODELVIEW );
    glPopMatrix  ();

    active_ = false;
  }
}


//-----------------------------------------------------------------------
//   setCursor
//-----------------------------------------------------------------------


void TextProjector::setCursor

  ( GLdouble  x,
    GLdouble  y,
    GLdouble  z )

{
  Vertex3d  v ( x, y, z );
  Vertex3d  w;

  // Project the cursor position to device independent coordinates.

  if ( project( w, proj_, v ) )
  {
    // Transform the cursor position to window coordinates.

    w[0] = 0.5 * (w[0] + 1.0) * vport_.width;
    w[1] = 0.5 * (w[1] + 1.0) * vport_.height;
    w[2] = 0.5 * (w[2] + 1.0);

    xPos = vport_.x + (GLint) w[0];
    yPos = vport_.y + (GLint) w[1];
    zPos = (GLfloat) w[2];
  }
  else
  {
    // Make the cursor invisible.

    xPos = -1;
    yPos = -1;
    zPos = -1.0;
  }
}


JEM_END_PACKAGE( gl )
