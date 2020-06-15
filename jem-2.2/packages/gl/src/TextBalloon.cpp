
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


#include <jem/base/Time.h>
#include <jem/base/ClassTemplate.h>
#include <jem/gl/utilities.h>
#include <jem/gl/Rect.h>
#include <jem/gl/Color.h>
#include <jem/gl/GfxContext.h>
#include <jem/gl/TextPainter.h>
#include <jem/gl/TextBalloon.h>


JEM_DEFINE_CLASS( jem::gl::TextBalloon );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class TextBalloon
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


TextBalloon::TextBalloon () : Super ( "balloon" )
{
  xPos = yPos = -1;
}


TextBalloon::TextBalloon

  ( const String&  txt,
    int            x,
    int            y ) :

    Super ( "balloon" ),
    text  ( txt )

{
  xPos = x;
  yPos = y;
}


TextBalloon::~TextBalloon ()
{}


//-----------------------------------------------------------------------
//   show
//-----------------------------------------------------------------------


void TextBalloon::show

  ( GfxContext&          gtx,
    const Ref<GfxNode>&  node )

{
  double  dt = 4.0 + (double) text.size() / 10.0;

  Super::show   ( gtx, node );
  Super::hideAt ( gtx, Time::now() + Time( dt ) );
}


//-----------------------------------------------------------------------
//   redraw
//-----------------------------------------------------------------------


void TextBalloon::redraw ( GfxContext& gtx )
{
  const GLsizei  SPACE = 4;

  CanvasScope    canvas ( -1.0, 50.0 );

  TextPainter    txp    ( this->font );

  Color          col;
  Size2i         tsz;
  GLfloat        lw;
  int            x0, x1;
  int            y0, y1;
  idx_t          i, k, n;


  if ( xPos < 0 || xPos > canvas.width ||
       yPos < 0 || yPos > canvas.height )
  {
    return;
  }

  n = text.size ();
  k = 1;

  for ( i = 0; i < n; i++ )
  {
    if ( text[i] == '\n' )
    {
      k++;
    }
  }

  txp.vAlignment = CENTERED;

  while ( txp.font.getSize() != SMALL_FONT )
  {
    GLsizei  h = txp.font.getHeight ();

    if ( k * h > canvas.height / 4 )
    {
      txp.font--;
    }
    else
    {
      break;
    }
  }

  tsz = txp.getSizeOf ( text );

  x0  = xPos + 2 * SPACE;
  x1  = xPos + 4 * SPACE + tsz.width;
  y0  = yPos + 2 * SPACE;
  y1  = yPos + 4 * SPACE + tsz.height;

  if ( x1 > canvas.width )
  {
    x0 = xPos - 4 * SPACE - tsz.width;
    x1 = xPos - 2 * SPACE;
  }

  if ( y1 > canvas.height )
  {
    y0 = yPos - 4 * SPACE - tsz.height;
    y1 = yPos - 2 * SPACE;
  }

  getCurrent     ( col );
  glGetFloatv    ( GL_LINE_WIDTH, &lw );
  glLineWidth    ( 1.0F );
  renderShadow   ( (GLfloat) x0, (GLfloat) y0,
                   (GLfloat) x1, (GLfloat) y1, (GLfloat) SPACE );
  glColor4f      ( 0.98F, 0.94F, 0.80F, 0.90F );
  glRecti        ( x0, y0, x1, y1 );

  // A rendering problem has been experienced with some video drivers
  // (Intel) on Windows. A work-around is to enable/disable lighting.

#ifdef JEM_OS_WINDOWS

  glEnable       ( GL_LIGHTING );
  glDisable      ( GL_LIGHTING );

#endif

  glTranslatef   ( 0.0F, 0.0F, -SPACE );
  glColor3f      ( 0.0F, 0.0F, 0.0F );
  txp.begin      ( gtx );
  txp.setCursor  ( x0 + SPACE, y0 + SPACE + tsz.height / 2 );
  txp.writeBlock ( text, tsz );
  txp.end        ();
  glColor3f      ( 0.8F, 0.8F, 0.0F );

  glBegin ( GL_LINE_STRIP );
  {
    glVertex2i ( x0, y0 );
    glVertex2i ( x1, y0 );
    glVertex2i ( x1, y1 );
    glVertex2i ( x0, y1 );
    glVertex2i ( x0, y0 );
  }
  glEnd ();

  glLineWidth ( lw );
  setCurrent  ( col );
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   showBalloon
//-----------------------------------------------------------------------


Ref<TextBalloon>        showBalloon

  ( GfxContext&           gtx,
    const Ref<GfxNode>&   node,
    const String&         text )

{
  Vertex2i  pos = gtx.getMousePos ();

  return showBalloon ( gtx, node, text, pos[0], pos[1] );
}


Ref<TextBalloon>         showBalloon

  ( GfxContext&           gtx,
    const Ref<GfxNode>&   node,
    const String&         text,
    int                   xPos,
    int                   yPos )

{
  Ref<TextBalloon>  balloon =

    newInstance<TextBalloon> ( text, xPos, yPos );

  balloon->show ( gtx, node );

  return balloon;
}


JEM_END_PACKAGE( gl )
