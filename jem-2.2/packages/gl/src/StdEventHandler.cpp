
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
#include <jem/base/Time.h>
#include <jem/base/ClassTemplate.h>
#include <jem/io/list.h>
#include <jem/util/Event.h>
#include <jem/util/Properties.h>
#include <jem/gl/input.h>
#include <jem/gl/utilities.h>
#include <jem/gl/Box.h>
#include <jem/gl/Font.h>
#include <jem/gl/Menu.h>
#include <jem/gl/Array.h>
#include <jem/gl/Actions.h>
#include <jem/gl/Commands.h>
#include <jem/gl/Camera.h>
#include <jem/gl/DummyNode.h>
#include <jem/gl/GfxContext.h>
#include <jem/gl/MessageBox.h>
#include <jem/gl/RubberBand.h>
#include <jem/gl/Transformer.h>
#include <jem/gl/Transformable.h>
#include <jem/gl/StdEventHandler.h>


JEM_DEFINE_CLASS( jem::gl::StdEventHandler );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class StdEventHandler
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char  StdEventHandler::NULL_MODE        = '\0';
const char  StdEventHandler::ZOOM_MODE        = 'Z';
const char  StdEventHandler::ROTATION_MODE    = 'R';
const char  StdEventHandler::TRANSLATION_MODE = 'T';


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


StdEventHandler::StdEventHandler

  ( const Ref<Camera>&   camera,
    const Ref<GfxNode>&  world ) :

    camera_ ( camera ),
    world_  (  world )

{
  JEM_PRECHECK ( camera != NIL && world != NIL );

  focusColor = Color::red ();
  noFocus_   = newInstance<DummyNode> ( "noFocus" );
  focus_     = noFocus_;
  mode_      = TRANSLATION_MODE;
}


StdEventHandler::~StdEventHandler ()
{}


//-----------------------------------------------------------------------
//   reset
//-----------------------------------------------------------------------


void StdEventHandler::reset ()
{
  camera_->reset ();
}


//-----------------------------------------------------------------------
//   redraw
//-----------------------------------------------------------------------


void StdEventHandler::redraw

  ( GfxContext&        gtx,
    int                action,
    const Properties&  params )

{
  bool  snap = false;

  params.find ( snap, GLActionParams::SNAPSHOT );

  if ( snap )
  {
    return;
  }

  DisableScope  lightScope ( GL_LIGHTING );

  Color         col0;
  GLfloat       lw0;


  glGetFloatv ( GL_LINE_WIDTH, &lw0 );
  getCurrent  ( col0 );

  if ( focus_ != noFocus_ && focus_ != world_ )
  {
    Transform  tr = getTransformOf ( focus_->getParent() );

    Box        box;

    focus_->getBBox ( box );

    if ( ! box.isEmpty() )
    {
      box.scale     ( 1.05 );
      setCurrent    ( focusColor );
      glLineWidth   ( 2.0 );
      glPushMatrix  ();
      setCurrent    ( tr );
      box.drawEdges ();
      glPopMatrix   ();
    }
  }

  if ( mode_ != NULL_MODE )
  {
    CanvasScope  canvas;

    Font         font = Font::getDefault ( MONOSPACE_FONT );

    GLsizei      w, h;
    GLint        x, y;

    font++;

    if ( canvas.width  < 400 ||
         canvas.height < 400 )
    {
      font--;
    }

    w = 2 + font.getWidthOf ( mode_ );
    h = 2 + font.getHeight  ();
    x = canvas.width - 5 - w;
    y = 5;

    glTranslatef ( (GLfloat) x, (GLfloat) y, 0.0F );
    glColor3f    ( 1.0F, 1.0F, 1.0F );
    glRecti      ( 0, 0, w,  h );
    glTranslatef ( 1.0F, 0.0F, 0.0F );
    glColor3f    ( 0.0F, 0.0F, 0.0F );

    font.begin   ( gtx );
    font.write   ( mode_ );
    font.end     ();
  }

  setCurrent  ( col0 );
  glLineWidth ( lw0  );
}


//-----------------------------------------------------------------------
//   newView
//-----------------------------------------------------------------------


void StdEventHandler::newView

  ( GfxContext&  gtx,
    const Rect&  vport )

{}


//-----------------------------------------------------------------------
//   keyPressed
//-----------------------------------------------------------------------


void StdEventHandler::keyPressed

  ( GfxContext&      gtx,
    const KeyPress&  key )

{
  bool  handled = false;
  int   keyMods = key.mods & ~Keys::SHIFT;

  switch ( key.code )
  {
  case 'a':

    if ( keyMods == Keys::ALT )
    {
      Transformer::align ( *focus_, *camera_ );

      handled = true;
    }

    break;

  case 'c':

    if ( keyMods == Keys::ALT )
    {
      centerObj_ ();

      handled = true;
    }

    break;

  case 'r':

    if ( keyMods == Keys::ALT )
    {
      mode_   = ROTATION_MODE;
      handled = true;
    }

    break;

  case 't':

    if ( keyMods == Keys::ALT )
    {
      mode_   = TRANSLATION_MODE;
      handled = true;
    }

    break;

  case 'z':

    if ( keyMods == Keys::ALT )
    {
      mode_   = ZOOM_MODE;
      handled = true;
    }

    break;

  case '>':

    if ( keyMods == 0 )
    {
      scaleFocus_ ( gtx, 1 );

      handled = true;
    }

    break;

  case '<':

    if ( keyMods == 0 )
    {
      scaleFocus_ ( gtx, -1 );

      handled = true;
    }

    break;

  case '+':

    if ( keyMods == 0 )
    {
      zoomView_ ( gtx, 1 );

      handled = true;
    }

    break;

  case '-':

    if ( keyMods == 0 )
    {
      zoomView_ ( gtx, -1 );

      handled = true;
    }

    break;

  default:

    handled = false;
  }

  if ( handled )
  {
    gtx.postFastRedraw ( world_ );
  }
}


//-----------------------------------------------------------------------
//   mouseDragged
//-----------------------------------------------------------------------


void StdEventHandler::mouseDragged

  ( GfxContext&       gtx,
    const MouseDrag&  mouse )

{
  if ( mode_        == ZOOM_MODE &&
       mouse.seqnr  == 0         &&
       mouse.mods   == 0         &&
       mouse.button == Buttons::LEFT )
  {
    Ref<RubberBand>  band  = newInstance<RubberBand> ();
    Rect             vport = camera_->getViewport    ();

    connect ( band->selectEvent, camera_,
              & Camera::lookAtRect );

    band->setAspectRatio ( vport.aspectRatio() );
    band->start          ( gtx, world_, mouse );
  }
}


//-----------------------------------------------------------------------
//   exeCommand
//-----------------------------------------------------------------------


bool StdEventHandler::exeCommand

  ( GfxContext&    gtx,
    const String&  cmd )

{
  bool  result = true;

  if      ( cmd == Commands::TRANSLATE )
  {
    mode_ = TRANSLATION_MODE;
  }
  else if ( cmd == Commands::ROTATE )
  {
    mode_ = ROTATION_MODE;
  }
  else if ( cmd == Commands::ZOOM )
  {
    mode_ = ZOOM_MODE;
  }
  else if ( cmd == Commands::CENTER )
  {
    centerObj_ ();
  }
  else if ( cmd == Commands::ALIGN )
  {
    Transformer::align ( *focus_, *camera_ );
  }
  else
  {
    Properties  params;

    params.set ( GLActionParams::COMMAND, cmd );

    result =

      focus_->takeGLAction ( gtx, GLActions::EXE_COMMAND, params );

    if ( result && (cmd.startsWith( "Hide" ) ||
                    cmd.startsWith( "Show" )) )
    {
      camera_->updateZRange ();
    }
  }

  return result;
}


//-----------------------------------------------------------------------
//   printHelp
//-----------------------------------------------------------------------


void StdEventHandler::printHelp ( PrintWriter& pr )
{
  using jem::io::beginItem;
  using jem::io::endItem;

  if ( mode_ == ZOOM_MODE )
  {
    print ( pr, beginItem( "Left mouse drag" ) );
    print ( pr,   "Select the area to view." );
    print ( pr, endItem );

    print ( pr, beginItem( "Esc" ) );
    print ( pr,   "Cancel the current zoom operation." );
    print ( pr, endItem );
  }

  print ( pr, beginItem( "Alt+a" ) );
  print ( pr,   "Align the object axes with the view axes." );
  print ( pr, endItem );

  print ( pr, beginItem( "Alt+c" ) );
  print ( pr,   "Center object." );
  print ( pr, endItem );

  print ( pr, beginItem( "Alt+r" ) );
  print ( pr,   "Activate rotation mode." );
  print ( pr, endItem );

  print ( pr, beginItem( "Alt+t" ) );
  print ( pr,   "Activate translation mode." );
  print ( pr, endItem );

  print ( pr, beginItem( "Alt+z" ) );
  print ( pr,   "Activate zoom mode." );
  print ( pr, endItem );

  print ( pr, beginItem( ">" ) );
  print ( pr,   "Enlarge object by 10%." );
  print ( pr, endItem );

  print ( pr, beginItem( "<" ) );
  print ( pr,   "Shrink object by 10%." );
  print ( pr, endItem );

  print ( pr, beginItem( "+/-" ) );
  print ( pr,   "Zoom in/out." );
  print ( pr, endItem );
}


//-----------------------------------------------------------------------
//   initMenu
//-----------------------------------------------------------------------


void StdEventHandler::initMenu ( Menu& menu ) const
{
  menu.addEntry ( Commands::TRANSLATE );
  menu.addEntry ( Commands::ROTATE    );
  menu.addEntry ( Commands::ZOOM      );
  menu.addEntry ( Commands::CENTER    );
  menu.addEntry ( Commands::ALIGN     );
}


//-----------------------------------------------------------------------
//   listOptions
//-----------------------------------------------------------------------


void StdEventHandler::listOptions ( Menu& menu ) const
{
  Properties  params;

  params.set         ( ActionParams::OPTIONS, &menu );
  focus_->takeAction ( Actions::LIST_OPTIONS, params );
}


//-----------------------------------------------------------------------
//   setFocus
//-----------------------------------------------------------------------


void StdEventHandler::setFocus ( const Ref<GfxNode>& node )
{
  if ( node != focus_ )
  {
    Properties  params;

    if ( focus_ != noFocus_ )
    {
      focus_->takeAction ( Actions::GIVE_FOCUS, params );
    }

    if ( node == NIL )
    {
      focus_ = noFocus_;
    }
    else
    {
      focus_ = node;
      focus_ ->takeAction ( Actions::TAKE_FOCUS, params );
    }
  }
}


//-----------------------------------------------------------------------
//   setMode
//-----------------------------------------------------------------------


void StdEventHandler::setMode ( char mode )
{
  mode_ = mode;
}


//-----------------------------------------------------------------------
//   zoomView_
//-----------------------------------------------------------------------


void StdEventHandler::zoomView_

  ( GfxContext&  gtx,
    int          step )

{
  int  z;

  if ( step < 0 )
  {
    z = (int) camera_->zoomOut ();
  }
  else
  {
    z = (int) camera_->zoomIn  ();
  }

  showMessageUntil ( gtx, world_,
                     String::format ( "Zoom factor: %d", z ),
                     Time::now() + Time( JEM_LINT_C(2) ) );
}


//-----------------------------------------------------------------------
//   scaleFocus_
//-----------------------------------------------------------------------


void StdEventHandler::scaleFocus_

  ( GfxContext&  gtx,
    int          step )

{
  Transformable*  tf = toTransformable ( focus_ );

  Transform       t, u;
  Vertex3d        c;
  Box             box;
  GLdouble        s, x;
  int             n;


  if ( ! tf )
  {
    return;
  }

  tf->getBareBBox ( box );

  t = tf->getTransform ();
  u = getTransformOf   ( focus_->getParent() );

  t.transform ( u );

  s = t.getScale ();

  if ( s <= 0.0 )
  {
    return;
  }

  x = ::log( s ) / ::log( 1.1 );

  if ( x < 0.0 )
  {
    n = -(int) (0.5 - x);
  }
  else
  {
    n =  (int) (0.5 + x);
  }

  n += step;

  clamp ( n, -100, 100 );

  x  = ::pow ( 1.1, n );

  box.transform ( t );

  c = box.center ();

  t.translate ( -c[0], -c[1], -c[2] );
  t.scale     ( x / s );
  t.translate (  c[0],  c[1],  c[2] );

  if ( ! invert( u ) )
  {
    return;
  }

  t  .transform    ( u );
  tf->setTransform ( t );

  camera_->updateZRange ();

  showMessageUntil ( gtx, world_,
                     String::format ( "Scale factor: %#.2g", x ),
                     Time::now() + Time( JEM_LINT_C(2) ) );
}


//-----------------------------------------------------------------------
//   centerObj_
//-----------------------------------------------------------------------


void StdEventHandler::centerObj_ ()
{
  camera_->setZoom      ( 0.0 );
  camera_->lookAtObj    ( *focus_ );
  camera_->updateZRange ();
}


JEM_END_PACKAGE( gl )
