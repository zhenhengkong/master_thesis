
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
#include <jem/gl/utilities.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   renderDisk
//-----------------------------------------------------------------------


void        renderDisk

  ( GLfloat   radius,
    int       quality )

{
  JEM_ASSERT ( radius > 0.0F );

  glNormal3f ( 0.0F, 0.0F, 1.0F );
  glBegin    ( GL_TRIANGLE_FAN );
  glVertex2f (   0.0F, 0.0F );
  glVertex2f ( radius, 0.0F );

  if ( quality <= 1 )
  {
    const GLfloat  a = radius * 0.866025F;
    const GLfloat  b = radius * 0.500000F;

    glVertex2f (       a,       b );
    glVertex2f (       b,       a );
    glVertex2f (    0.0F,  radius );
    glVertex2f (      -b,       a );
    glVertex2f (      -a,       b );
    glVertex2f ( -radius,    0.0F );
    glVertex2f (      -a,      -b );
    glVertex2f (      -b,      -a );
    glVertex2f (    0.0F, -radius );
    glVertex2f (       b,      -a );
    glVertex2f (       a,      -b );
  }
  else
  {
    GLfloat  x, y, t;
    int      a, da;

    da = 90 / (quality + 1);
    da = da - (90 % da);

    if ( da < 3 )
    {
      da = 3;
    }

    for ( a = da; a < 360; a += da )
    {
      t = (GLfloat) (RADS_PER_DEGREE * a);
      x = (GLfloat) (radius * std::cos ( t ));
      y = (GLfloat) (radius * std::sin ( t ));

      glVertex2f ( x, y );
    }
  }

  glVertex2f ( radius, 0.0F );

  glEnd ();
}


JEM_END_PACKAGE( gl )
