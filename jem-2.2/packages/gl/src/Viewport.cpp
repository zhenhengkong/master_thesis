
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
#include <jem/base/ClassTemplate.h>
#include <jem/base/IllegalArgumentException.h>
#include <jem/io/OutputStream.h>
#include <jem/util/Properties.h>
#include <jem/gl/utilities.h>
#include <jem/gl/Actions.h>
#include <jem/gl/PropertyNames.h>
#include <jem/gl/FrameGrabber.h>
#include <jem/gl/Viewport.h>


JEM_DEFINE_CLASS( jem::gl::Viewport );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class Viewport
//=======================================================================

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


Viewport::Viewport

  ( const String&        name,
    const Ref<GfxNode>&  child ) :

    Super ( name, child )

{
  bounds_[0] = 0.0;
  bounds_[1] = 0.0;
  bounds_[2] = 1.0;
  bounds_[3] = 1.0;

  bgColor_   = Color::white ();
  newVport_  = false;
  clearMask_ = GL_COLOR_BUFFER_BIT;
}


Viewport::~Viewport ()
{}


//-----------------------------------------------------------------------
//   takeGLAction
//-----------------------------------------------------------------------


bool Viewport::takeGLAction

  ( GfxContext&        gtx,
    int                action,
    const Properties&  params )

{
  bool  myResult    = true;
  bool  childResult = false;


  glScissor  ( vport_.x, vport_.y, vport_.width, vport_.height );
  glViewport ( vport_.x, vport_.y, vport_.width, vport_.height );

  if ( newVport_ && ! vport_.isEmpty() &&
       action != GLActions::INIT_VIEW )
  {
    resizeChild_ ( gtx );
  }

  switch ( action )
  {
  case GLActions::REDRAW:
  case GLActions::REDRAW_FAST:

    glClearColor ( bgColor_[0], bgColor_[1],
                   bgColor_[2], bgColor_[3] );

    glClear      ( clearMask_ );

    break;

  case GLActions::INIT_GL:

    glEnable ( GL_SCISSOR_TEST );

    clearMask_ = getClearMask ();

    break;

  case GLActions::INIT_VIEW:

    initView_    ( params );
    resizeChild_ ( gtx );

    break;

  case GLActions::GRAB_FRAME:

    {
      Ref<OutputStream>  output;
      Ref<FrameGrabber>  grabber;

      Rect               vport = vport_;

      params.get ( grabber, GLActionParams::FRAME_GRABBER );
      params.get ( output,  GLActionParams::FRAME_OUTPUT  );

      grabber->grabFrame ( *output, gtx, *this, vport );

      return true;
    }

    break;

  default:

    myResult = false;
  }

  childResult = passGLAction ( child_, gtx, action, params );

  checkGLError ( JEM_FUNC );

  return (myResult || childResult);
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void Viewport::configure ( const Properties& props )
{
  if ( props.contains( myName_ ) )
  {
    Properties  myProps = props.findProps ( myName_ );

    bgColor_.configure ( PropNames::BG_COLOR, myProps );
  }

  configChild ( child_, props );
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void Viewport::getConfig ( const Properties& conf ) const
{
  Properties  myConf = conf.makeProps ( myName_ );

  bgColor_.getConfig ( PropNames::BG_COLOR, myConf );

  if ( child_ != NIL )
  {
    child_->getConfig ( conf );
  }
}


//-----------------------------------------------------------------------
//   listProps
//-----------------------------------------------------------------------


void Viewport::listProps ( const Properties& conf ) const
{
  Properties  myConf = conf.makeProps ( myName_ );

  myConf.set ( PropNames::BG_COLOR,
               "The background color" );

  if ( child_ != NIL )
  {
    child_->listProps ( conf );
  }
}


//-----------------------------------------------------------------------
//   setChild
//-----------------------------------------------------------------------


void Viewport::setChild ( const Ref<GfxNode>& child )
{
  Super::setChild_ ( child );
}


//-----------------------------------------------------------------------
//   getChild
//-----------------------------------------------------------------------


Ref<GfxNode> Viewport::getChild () const
{
  return child_;
}


//-----------------------------------------------------------------------
//   setBgColor
//-----------------------------------------------------------------------


void Viewport::setBgColor ( const Color& bg )
{
  bgColor_ = clamped ( bg );
}


//-----------------------------------------------------------------------
//   setBounds
//-----------------------------------------------------------------------


void Viewport::setBounds ( const Vertex4d& b )
{
  if ( b[0] < 0.0 || b[0] > 1.0        ||
       b[1] < 0.0 || b[1] > 1.0        ||
       b[2] < 0.0 || b[0] + b[2] > 1.0 ||
       b[3] < 0.0 || b[1] + b[3] > 1.0 )
  {
    throw IllegalArgumentException (
      JEM_FUNC,
      "invalid viewport bounds"
    );
  }

  const GLsizei  w = vport0_.width;
  const GLsizei  h = vport0_.height;

  bounds_        = b;

  vport_ .x      = vport0_.x + (GLint) (b[0] * w);
  vport_ .y      = vport0_.y + (GLint) (b[1] * h);
  vport_ .width  =           (GLsizei) (b[2] * w);
  vport_ .height =           (GLsizei) (b[3] * h);

  newVport_      = true;
}


//-----------------------------------------------------------------------
//   initView_
//-----------------------------------------------------------------------


void Viewport::initView_ ( const Properties& params )
{
  typedef GLActionParams  Params;

  int  x, y, w, h;


  params.get ( x, Params::VIEW_XPOS   );
  params.get ( y, Params::VIEW_YPOS   );
  params.get ( w, Params::VIEW_WIDTH  );
  params.get ( h, Params::VIEW_HEIGHT );

  vport0_.x      = (GLint)   x;
  vport0_.y      = (GLint)   y;
  vport0_.width  = (GLsizei) w;
  vport0_.height = (GLsizei) h;

  vport_ .x      = vport0_.x + (GLint) (bounds_[0] * w);
  vport_ .y      = vport0_.y + (GLint) (bounds_[1] * h);
  vport_ .width  =           (GLsizei) (bounds_[2] * w);
  vport_ .height =           (GLsizei) (bounds_[3] * h);

  newVport_      = true;
}


//-----------------------------------------------------------------------
//   resizeChild_
//-----------------------------------------------------------------------


void Viewport::resizeChild_ ( GfxContext& gtx )
{
  typedef GLActionParams  Params;

  if ( child_ != NIL )
  {
    Properties  params ( "params" );

    params.set   ( Params::VIEW_XPOS,   (int) vport_.x      );
    params.set   ( Params::VIEW_YPOS,   (int) vport_.y      );
    params.set   ( Params::VIEW_WIDTH,  (int) vport_.width  );
    params.set   ( Params::VIEW_HEIGHT, (int) vport_.height );

    passGLAction ( child_, gtx, GLActions::INIT_VIEW, params );
  }

  newVport_ = false;
}


JEM_END_PACKAGE( gl )
