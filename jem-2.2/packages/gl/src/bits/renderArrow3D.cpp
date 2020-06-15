
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
//   renderArrow3D
//-----------------------------------------------------------------------


void renderArrow3D ( GLfloat size )
{
  JEM_ASSERT ( size > 0.0F );

  static const GLfloat  POINTS[9][3]  =

    { {  0.577350F,  0.000000F, -1.0F },
      {  0.408248F,  0.408248F, -1.0F },
      {  0.000000F,  0.577350F, -1.0F },
      { -0.408248F,  0.408248F, -1.0F },
      { -0.577350F,  0.000000F, -1.0F },
      { -0.408248F, -0.408248F, -1.0F },
      {  0.000000F, -0.577350F, -1.0F },
      {  0.408248F, -0.408248F, -1.0F },
      {  0.577350F,  0.000000F, -1.0F } };

  static const GLfloat  NORMALS[9][3] =

    { {  0.866025F,  0.000000F,  0.5F },
      {  0.612372F,  0.612372F,  0.5F },
      {  0.000000F,  0.866025F,  0.5F },
      { -0.612372F,  0.612372F,  0.5F },
      { -0.866025F,  0.000000F,  0.5F },
      { -0.612372F, -0.612372F,  0.5F },
      {  0.000000F, -0.866025F,  0.5F },
      {  0.612372F, -0.612372F,  0.5F },
      {  0.866025F,  0.000000F,  0.5F } };

  const GLfloat  COLOR_SCALE = 0.4F;

  const GLfloat  z = 1.0F - size;

  GLfloat        c [4];
  GLfloat        c0[4];


  // The arrow head

  {
    glBegin ( GL_TRIANGLES );

    for ( int i = 0; i < 8; i++ )
    {
      glNormal3fv ( NORMALS[i] );

      glVertex3f  ( 0.0, 0.0, 1.0 );

      glVertex3f  ( size * POINTS[i][0],
                    size * POINTS[i][1], z );

      glNormal3fv ( NORMALS[i + 1] );

      glVertex3f  ( size * POINTS[i + 1][0],
                    size * POINTS[i + 1][1], z );
    }

    glEnd ();

    glGetFloatv ( GL_CURRENT_COLOR, c0 );

    c[0] = COLOR_SCALE * c0[0];
    c[1] = COLOR_SCALE * c0[1];
    c[2] = COLOR_SCALE * c0[2];
    c[3] =               c0[3];

    glBegin ( GL_POLYGON );

    glColor4fv ( c );
    glNormal3f ( 0.0, 0.0, -1.0 );

    for ( int i = 7; i >= 0; i-- )
    {
      glVertex3f ( size * POINTS[i][0],
                   size * POINTS[i][1], z );
    }

    glEnd ();

    glColor4fv ( c0 );
  }

  // The arrow line

  {
    DisableScope  lightScope ( GL_LIGHTING );

    glBegin    ( GL_LINES );

    glVertex3f ( 0.0, 0.0, 0.0 );
    glVertex3f ( 0.0, 0.0,   z );

    glEnd      ();
  }
}


JEM_END_PACKAGE( gl )
