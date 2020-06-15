
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


#include <jem/base/assert.h>
#include <jem/gl/utilities.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   renderShadow
//-----------------------------------------------------------------------


void      renderShadow

  ( GLfloat   x0,
    GLfloat   y0,
    GLfloat   x1,
    GLfloat   y1,
    GLfloat   w )

{
  const GLfloat  LIGHT[4] = { 0.0F, 0.0F, 0.0F, 0.0F };
  const GLfloat  DARK [4] = { 0.0F, 0.0F, 0.0F, 0.5F };

  const GLfloat  offset   = 0.4F * w;
  const GLfloat  radius   = 0.8F * w;

  const GLfloat  c1       = radius * 0.965926F;
  const GLfloat  c2       = radius * 0.866025F;
  const GLfloat  c3       = radius * 0.707107F;
  const GLfloat  c4       = radius * 0.500000F;
  const GLfloat  c5       = radius * 0.258819F;
  const GLfloat  c1i      = radius - c1;
  const GLfloat  c2i      = radius - c2;
  const GLfloat  c3i      = radius - c3;
  const GLfloat  c4i      = radius - c4;
  const GLfloat  c5i      = radius - c5;


  glNormal3f   ( 0.0F, 0.0F, 1.0F );

  glBegin      ( GL_QUADS );
  {
    glColor4fv ( DARK );
    glVertex2f ( x1 + offset,          y0          );
    glVertex2f ( x0 + offset + radius, y0          );
    glVertex2f ( x0 + offset + radius, y0 - offset );
    glVertex2f ( x1 + offset,          y0 - offset );
  }
  glEnd ();

  glBegin      ( GL_QUADS );
  {
    glColor4fv ( DARK  );
    glVertex2f ( x1,                   y1 - offset - radius );
    glVertex2f ( x1,                   y0                   );
    glVertex2f ( x1 + offset,          y0                   );
    glVertex2f ( x1 + offset,          y1 - offset - radius );
  }
  glEnd ();

  glBegin      ( GL_QUADS );
  {
    glColor4fv ( DARK  );
    glVertex2f ( x1 + offset,          y0 - offset          );
    glVertex2f ( x0 + offset + radius, y0 - offset          );
    glColor4fv ( LIGHT );
    glVertex2f ( x0 + offset + radius, y0 - offset - radius );
    glVertex2f ( x1 + offset,          y0 - offset - radius );
  }
  glEnd ();

  glBegin      ( GL_QUADS );
  {
    glColor4fv ( DARK  );
    glVertex2f ( x1 + offset,          y1 - offset - radius );
    glVertex2f ( x1 + offset,          y0 - offset          );
    glColor4fv ( LIGHT );
    glVertex2f ( x1 + offset + radius, y0 - offset          );
    glVertex2f ( x1 + offset + radius, y1 - offset - radius );
  }
  glEnd ();

  glBegin      ( GL_QUADS );
  {
    glColor4fv ( DARK  );
    glVertex2f ( x0 + offset + radius, y0 - offset );
    glVertex2f ( x0 + offset + radius, y0          );
    glColor4fv ( LIGHT );
    glVertex2f ( x0 + offset,          y0          );
    glVertex2f ( x0 + offset,          y0 - offset );
  }
  glEnd ();

  glBegin      ( GL_QUADS );
  {
    glColor4fv ( DARK  );
    glVertex2f ( x1,                   y1 - offset - radius );
    glVertex2f ( x1 + offset,          y1 - offset - radius );
    glColor4fv ( LIGHT );
    glVertex2f ( x1 + offset,          y1 - offset          );
    glVertex2f ( x1,                   y1 - offset          );
  }
  glEnd ();

  glBegin      ( GL_TRIANGLE_FAN );
  {
    glColor4fv ( DARK  );
    glVertex2f ( x0 + offset + radius, y0 - offset           );
    glColor4fv ( LIGHT );
    glVertex2f ( x0 + offset,          y0 - offset           );
    glVertex2f ( x0 + offset + c1i,    y0 - offset - c5      );
    glVertex2f ( x0 + offset + c2i,    y0 - offset - c4      );
    glVertex2f ( x0 + offset + c3i,    y0 - offset - c3      );
    glVertex2f ( x0 + offset + c4i,    y0 - offset - c2      );
    glVertex2f ( x0 + offset + c5i,    y0 - offset - c1      );
    glVertex2f ( x0 + offset + radius, y0 - offset - radius  );
  }
  glEnd ();

  glBegin      ( GL_TRIANGLE_FAN );
  {
    glColor4fv ( DARK  );
    glVertex2f ( x1 + offset,          y0 - offset          );
    glColor4fv ( LIGHT );
    glVertex2f ( x1 + offset,          y0 - offset - radius );
    glVertex2f ( x1 + offset + c5,     y0 - offset - c1     );
    glVertex2f ( x1 + offset + c4,     y0 - offset - c2     );
    glVertex2f ( x1 + offset + c3,     y0 - offset - c3     );
    glVertex2f ( x1 + offset + c2,     y0 - offset - c4     );
    glVertex2f ( x1 + offset + c1,     y0 - offset - c5     );
    glVertex2f ( x1 + offset + radius, y0 - offset          );
  }
  glEnd ();

  glBegin      ( GL_TRIANGLE_FAN );
  {
    glColor4fv ( DARK  );
    glVertex2f ( x1 + offset,          y1 - offset - radius );
    glColor4fv ( LIGHT );
    glVertex2f ( x1 + offset + radius, y1 - offset - radius );
    glVertex2f ( x1 + offset + c1,     y1 - offset - c5i    );
    glVertex2f ( x1 + offset + c2,     y1 - offset - c4i    );
    glVertex2f ( x1 + offset + c3,     y1 - offset - c3i    );
    glVertex2f ( x1 + offset + c4,     y1 - offset - c2i    );
    glVertex2f ( x1 + offset + c5,     y1 - offset - c1i    );
    glVertex2f ( x1 + offset,          y1 - offset          );
  }
  glEnd ();
}


JEM_END_PACKAGE( gl )
