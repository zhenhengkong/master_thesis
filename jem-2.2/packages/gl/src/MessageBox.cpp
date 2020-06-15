
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


#include <jem/base/ClassTemplate.h>
#include <jem/gl/utilities.h>
#include <jem/gl/Rect.h>
#include <jem/gl/Color.h>
#include <jem/gl/TextPainter.h>
#include <jem/gl/MessageBox.h>


JEM_DEFINE_CLASS( jem::gl::MessageBox );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class MessageBox
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


MessageBox::MessageBox () : Super ( "messageBox" )
{
  textAlignment = CENTERED;
  boxHAlignment = CENTERED;
  boxVAlignment = CENTERED;
}


MessageBox::MessageBox

  ( const String&  txt,
    Alignment      hAlign,
    Alignment      vAlign ) :

    Super ( "messageBox" ),
    text  ( txt )

{
  textAlignment = hAlign;
  boxHAlignment = hAlign;
  boxVAlignment = vAlign;
}


MessageBox::~MessageBox ()
{}


//-----------------------------------------------------------------------
//   redraw
//-----------------------------------------------------------------------


void MessageBox::redraw ( GfxContext& gtx )
{
  const GLsizei  SPACE = 15;

  CanvasScope    canvas ( -1.0, 50.0 );

  TextPainter    txp    ( this->font );

  Rect           mbox;
  Rect           tbox;

  Color          col0;
  GLfloat        lw0;
  idx_t          i, k, n;


  n = text.size ();
  k = 1;

  for ( i = 0; i < n; i++ )
  {
    if ( text[i] == '\n' )
    {
      k++;
    }
  }

  while ( txp.font.getSize() != SMALL_FONT )
  {
    GLsizei  h = txp.font.getHeight ();

    if ( k * h > canvas.height / 2 )
    {
      txp.font--;
    }
    else
    {
      break;
    }
  }

  txp.hAlignment = textAlignment;

  tbox.resize ( txp.getSizeOf( text ) );

  switch ( boxHAlignment )
  {
  case LEFT_ALIGNED:

    tbox.x = 2 * SPACE;
    break;

  case CENTERED:

    tbox.x = (canvas.width - tbox.width) / 2;
    break;

  case RIGHT_ALIGNED:

    tbox.x = canvas.width - tbox.width - 2 * SPACE;
    break;
  }

  switch ( boxVAlignment )
  {
  case BOT_ALIGNED:

    tbox.y = 2 * SPACE;
    break;

  case CENTERED:

    tbox.y = (canvas.height - tbox.height) / 2;
    break;

  case TOP_ALIGNED:

    tbox.y = canvas.height - tbox.height - 2 * SPACE;
    break;
  }

  if ( tbox.x < 2 * SPACE )
  {
    tbox.x = 2 * SPACE;
  }

  if ( tbox.y < 2 * SPACE )
  {
    tbox.y = 2 * SPACE;
  }

  mbox.x      = tbox.x      -     SPACE;
  mbox.y      = tbox.y      -     SPACE;
  mbox.width  = tbox.width  + 2 * SPACE;
  mbox.height = tbox.height + 2 * SPACE;

  if ( textAlignment != LEFT_ALIGNED )
  {
    if ( textAlignment == CENTERED )
    {
      tbox.x = tbox.x + tbox.width / 2;
    }
    else
    {
      tbox.x = tbox.x + tbox.width;
    }

    if ( tbox.x > canvas.width - 2 * SPACE )
    {
      tbox.x = canvas.width - 2 * SPACE;
    }
  }

  getCurrent     ( col0 );
  glGetFloatv    ( GL_LINE_WIDTH, &lw0 );
  glLineWidth    ( 2.0 );

  renderShadow   ( (GLfloat)  mbox.x,
                   (GLfloat)  mbox.y,
                   (GLfloat) (mbox.x + mbox.width),
                   (GLfloat) (mbox.y + mbox.height), 8 );

  // A rendering problem has been experienced with some video drivers
  // (Intel) on Windows. A work-around is to enable/disable lighting.

#ifdef JEM_OS_WINDOWS

  glEnable       ( GL_LIGHTING );
  glDisable      ( GL_LIGHTING );

#endif

  glColor3f      ( 1.0F, 1.0F, 1.0F );
  mbox.draw      ();
  glColor3f      ( 0.0F, 0.0F, 0.0F );

  txp.begin      ( gtx );
  txp.setCursor  ( tbox.x, tbox.y );
  txp.writeBlock ( text,   tbox.size() );
  txp.end        ();

  // Unfortunately, GL_LINE_LOOP is broken with some Linux video
  // drivers. So use GL_LINE_STRIP instead.

  glBegin ( GL_LINE_STRIP );
  {
    glVertex2i ( mbox.x,              mbox.y );
    glVertex2i ( mbox.x + mbox.width, mbox.y );
    glVertex2i ( mbox.x + mbox.width, mbox.y + mbox.height );
    glVertex2i ( mbox.x,              mbox.y + mbox.height );
    glVertex2i ( mbox.x,              mbox.y );
  }
  glEnd ();

  glLineWidth ( lw0  );
  setCurrent  ( col0 );
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   showMessage
//-----------------------------------------------------------------------


Ref<MessageBox>         showMessage

  ( Viewer&               viewer,
    const Ref<GfxNode>&   node,
    const String&         text,
    Alignment             hAlign,
    Alignment             vAlign )

{
  Ref<MessageBox>  mbox =

    newInstance<MessageBox> ( text, hAlign, vAlign );

  mbox->show ( viewer, node );

  return mbox;
}


Ref<MessageBox>         showMessage

  ( GfxContext&           gtx,
    const Ref<GfxNode>&   node,
    const String&         text,
    Alignment             hAlign,
    Alignment             vAlign )

{
  Ref<MessageBox>  mbox =

    newInstance<MessageBox> ( text, hAlign, vAlign );

  mbox->show ( gtx, node );

  return mbox;
}


//-----------------------------------------------------------------------
//   showMessageUntil
//-----------------------------------------------------------------------


Ref<MessageBox>         showMessageUntil

  ( GfxContext&           gtx,
    const Ref<GfxNode>&   node,
    const String&         text,
    const Time&           deadline,
    Alignment             hAlign,
    Alignment             vAlign )

{
  Ref<MessageBox>  mbox =

    newInstance<MessageBox> ( text, hAlign, vAlign );

  mbox->show   ( gtx, node     );
  mbox->hideAt ( gtx, deadline );

  return mbox;
}


JEM_END_PACKAGE( gl )
