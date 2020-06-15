
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
//   renderSlice
//-----------------------------------------------------------------------


void        renderSlice

  ( GLfloat   alpha,
    GLfloat   beta,
    GLfloat   radius,
    int       quality )

{
  JEM_ASSERT ( radius > 0.0F );

  GLfloat  x, y, t, dt;
  int      i, n, da;


  if ( beta <= alpha )
  {
    return;
  }

  da = 90 / (quality + 1);
  da = da - (90 % da);

  if ( da < 3 )
  {
    da = 3;
  }

  n  = (int) ((beta - alpha) / (GLfloat) da);

  if ( n < 1 )
  {
    n = 1;
  }

  dt    = (beta - alpha) / (GLfloat) n;
  dt    = (GLfloat) (dt    * RADS_PER_DEGREE);
  alpha = (GLfloat) (alpha * RADS_PER_DEGREE);

  glNormal3f ( 0.0F, 0.0F, 1.0F );
  glBegin    ( GL_TRIANGLE_FAN );
  glVertex2f ( 0.0F, 0.0F );

  for ( i = 0; i <= n; i++ )
  {
    t = alpha + (GLfloat) i * dt;
    x = (GLfloat) (radius * std::cos ( t ));
    y = (GLfloat) (radius * std::sin ( t ));

    glVertex2f ( x, y );
  }

  glEnd ();
}


JEM_END_PACKAGE( gl )
