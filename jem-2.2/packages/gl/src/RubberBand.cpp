
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


#include <cstdlib>
#include <jem/base/assert.h>
#include <jem/base/Time.h>
#include <jem/base/ClassTemplate.h>
#include <jem/util/Properties.h>
#include <jem/gl/utilities.h>
#include <jem/gl/Color.h>
#include <jem/gl/Actions.h>
#include <jem/gl/Compound.h>
#include <jem/gl/GfxContext.h>
#include <jem/gl/RedrawRoot.h>
#include <jem/gl/RubberBand.h>


JEM_DEFINE_CLASS( jem::gl::RubberBand );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class RubberBand
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  RubberBand::CANCEL_    = "_RUBBER_BAND_CANCEL";

const int    RubberBand::ACTIVE_    = 1 << 0;
const int    RubberBand::TRANSIENT_ = 1 << 1;


//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


RubberBand::RubberBand ( const String& name ) :

  Super ( name )

{
  state_   =  0;
  origin_  =  0;
  current_ =  0;
  aspect_  = -1.0;
}


RubberBand::~RubberBand ()
{}


//-----------------------------------------------------------------------
//   takeAction
//-----------------------------------------------------------------------


bool RubberBand::takeAction

  ( const String&      action,
    const Properties&  params )

{
  if ( (state_ & ACTIVE_) && action == CANCEL_ )
  {
    cancel ();

    return true;
  }

  return false;
}


//-----------------------------------------------------------------------
//   takeGLAction
//-----------------------------------------------------------------------


bool RubberBand::takeGLAction

  ( GfxContext&        gtx,
    int                action,
    const Properties&  params )

{
  typedef GLActionParams  Params;

  if ( ! (state_ & ACTIVE_) )
  {
    return false;
  }

  bool  result = true;

  switch ( action )
  {
  case GLActions::KEY_EVENT:
  {
    int  key;

    params.get ( key, Params::KEY_CODE );

    // Cancel when the ESC kay has been hit.

    if ( key == 27 )
    {
      cancel ( gtx );
    }

    break;
  }

  case GLActions::MOUSE_EVENT:
  {
    int   bttn;
    bool  down;

    params.get ( bttn, Params::MOUSE_BUTTON );
    params.get ( down, Params::MOUSE_DOWN   );

    if      ( bttn == mouse_.button && down == false )
    {
      finish_ ( gtx );
    }
    else if ( bttn != mouse_.button && down == true )
    {
      cancel  ( gtx );
    }

    break;
  }

  case GLActions::MOTION_EVENT:
  {
    int  x, y;

    params.get ( x, Params::MOUSE_XPOS );
    params.get ( y, Params::MOUSE_YPOS );

    if ( aspect_ > 0.0 )
    {
      int  w = x - (int) origin_[0];
      int  h = y - (int) origin_[1];

      if ( aspect_ > 1.0 )
      {
        int  sign = (h >= 0) ? 1 : -1;

        h = sign * (int) (::abs(w) * aspect_);
      }
      else
      {
        int  sign = (w >= 0) ? 1 : -1;

        w = sign * (int) (::abs(h) / aspect_);
      }

      x = w + (int) origin_[0];
      y = h + (int) origin_[1];
    }

    current_[0] = (GLint) x;
    current_[1] = (GLint) y;

    gtx.postFastRedraw ( this );

    break;
  }

  case GLActions::REDRAW:
  case GLActions::REDRAW_FAST:

    redraw_ ( gtx );

    break;

  case GLActions::INIT_VIEW:

    cancel ();

    break;

  default:

    result = false;
  }

  return result;
}


//-----------------------------------------------------------------------
//   start
//-----------------------------------------------------------------------


void RubberBand::start

  ( GfxContext&       gtx,
    const MouseDrag&  mouse )

{
  int  savedState = state_;

  state_     = 0;
  mouse_     = mouse;;
  origin_[0] = (GLint) (mouse.x - mouse.dx);
  origin_[1] = (GLint) (mouse.y - mouse.dy);
  current_   = origin_;

  // Cancel all currently active RubberBand objects.

  try
  {
    GfxNode*  root = findRedrawRoot ( getParent() );

    if ( root )
    {
      Properties  params;

      root->takeAction ( CANCEL_, params );
    }
  }
  catch ( ... )
  {
    state_ = savedState;
    throw;
  }

  state_ = savedState | ACTIVE_;

  gtx.postFastRedraw ( this );
}


void RubberBand::start

  ( GfxContext&          gtx,
    const Ref<GfxNode>&  scene,
    const MouseDrag&     mouse )

{
  if ( state_ & ACTIVE_ )
  {
    cancel ();
  }

  if ( ! getParent() && addToScene( scene, this ) )
  {
    state_ |= TRANSIENT_;
  }

  start ( gtx, mouse );
}


//-----------------------------------------------------------------------
//   cancel
//-----------------------------------------------------------------------


void RubberBand::cancel ()
{
  if ( state_ & TRANSIENT_ )
  {
    eraseFromParent ( this );
  }

  state_ = 0;
}


void RubberBand::cancel ( GfxContext& gtx )
{
  if ( state_ & ACTIVE_ )
  {
    // Redraw must be posted *before* deleting this node from its
    // parent.

    gtx.postRedraw   ( this );
    cancel           ();
    cancelEvent.emit ( gtx  );
  }
}


//-----------------------------------------------------------------------
//   finish_
//-----------------------------------------------------------------------


void RubberBand::finish_ ( GfxContext& gtx )
{
  Rect  r;

  if ( current_[0] > origin_[0] )
  {
    r.x      = origin_[0];
    r.width  = (GLsizei) (current_[0] - origin_[0]);
  }
  else
  {
    r.x      = current_[0];
    r.width  = (GLsizei) (origin_[0] - current_[0]);
  }

  if ( current_[1] > origin_[1] )
  {
    r.y      = origin_[1];
    r.height = (GLsizei) (current_[1] - origin_[1]);
  }
  else
  {
    r.y      = current_[1];
    r.height = (GLsizei) (origin_[1] - current_[1]);
  }

  cancel ();

  selectEvent.emit ( gtx, r, mouse_ );
}


//-----------------------------------------------------------------------
//   redraw_
//-----------------------------------------------------------------------


void RubberBand::redraw_ ( GfxContext& gtx )
{
  CanvasScope   canvasScope;

  GLfloat       lw;
  Color         col;


  getCurrent   ( col );
  glGetFloatv  ( GL_LINE_WIDTH, &lw );
  glLineWidth  ( 2.0F );
  glColor4f    ( 1.0F, 0.0F, 0.0F, 0.1F );
  glRecti      ( origin_ [0], origin_ [1],
                 current_[0], current_[1] );
  glColor3f    ( 1.0F, 0.0F, 0.0F );

  glBegin      ( GL_LINE_STRIP );
  {
    glVertex2i ( origin_ [0], origin_ [1] );
    glVertex2i ( origin_ [0], current_[1] );
    glVertex2i ( current_[0], current_[1] );
    glVertex2i ( current_[0], origin_ [1] );
    glVertex2i ( origin_ [0], origin_ [1] );
  }
  glEnd        ();

  glLineWidth  ( lw );
  setCurrent   ( col );
}


JEM_END_PACKAGE( gl )
