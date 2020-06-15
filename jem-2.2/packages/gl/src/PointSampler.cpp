
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
#include <jem/base/ClassTemplate.h>
#include <jem/base/tuple/operators.h>
#include <jem/base/tuple/intrinsics.h>
#include <jem/util/Properties.h>
#include <jem/gl/Box.h>
#include <jem/gl/Rect.h>
#include <jem/gl/Actions.h>
#include <jem/gl/GfxContext.h>
#include <jem/gl/RedrawRoot.h>
#include <jem/gl/Transformable.h>
#include <jem/gl/PointSampler.h>


JEM_DEFINE_CLASS( jem::gl::PointSampler );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class PointSampler
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const double  PointSampler::MIN_INTERVAL  = 0.5;

const char*   PointSampler::TARGET_       = "target";
const char*   PointSampler::SAMPLER_      = "sampler";
const char*   PointSampler::GET_SAMPLE_   = "getPointSample";
const char*   PointSampler::FIND_SAMPLER_ = "findPointSampler";


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


PointSampler::PointSampler

  ( const String&        name,
    const Ref<GfxNode>&  obj ) :

    Super   ( name ),
    target_ ( obj  )

{
  JEM_PRECHECK ( obj != NIL );

  btnMask_  = Buttons::LEFT;
  focus_    = false;
  lastTime_ = Time::zero ();

  reset_ ();
}


PointSampler::~PointSampler ()
{}


//-----------------------------------------------------------------------
//   takeAction
//-----------------------------------------------------------------------


bool PointSampler::takeAction

  ( const String&      action,
    const Properties&  params )

{
  MouseClick&  mouse = sample_.mouse;


  if ( action == Actions::TAKE_FOCUS )
  {
    mouse.button = 0;
    button_      = 0;
    focus_       = true;

    return true;
  }

  if ( action == Actions::GIVE_FOCUS )
  {
    mouse.button = 0;
    button_      = 0;
    focus_       = false;

    params.set ( ActionParams::FOCUS, this );

    return true;
  }

  if ( action == Actions::GIVE_INPUT )
  {
    mouse.button = 0;
    button_      = 0;

    return true;
  }

  if ( action == FIND_SAMPLER_ )
  {
    Ref<GfxNode>  t;

    params.get ( t, TARGET_ );

    if ( t == target_ )
    {
      params.set ( SAMPLER_, this );
      return true;
    }

    return false;
  }

  if ( action == GET_SAMPLE_ && mouse.button )
  {
    Ref<Self>  ps;

    params.get ( ps, SAMPLER_ );

    if ( ps == this )
    {
      return true;
    }

    if ( depth_ < ps->depth_ )
    {
      ps->reset_ ();

      params.set ( SAMPLER_, this );
    }
    else
    {
      reset_ ();
    }

    return true;
  }

  return false;
}


//-----------------------------------------------------------------------
//   takeGLAction
//-----------------------------------------------------------------------


bool PointSampler::takeGLAction

  ( GfxContext&        gtx,
    int                action,
    const Properties&  params )

{
  typedef GLActionParams  Params;


  if ( ! focus_ )
  {
    return false;
  }

  MouseClick&  mouse = sample_.mouse;


  if ( action == GLActions::MOUSE_EVENT )
  {
    bool  down;
    int   bttn;

    params.get ( down, Params::MOUSE_DOWN );

    if ( down && ! button_ )
    {
      Time  delta = Time::now() - lastTime_;

      // This is to ignore the second click if the mouse button
      // is clicked multiple times in quick succession (a double
      // click, for instance).

      if ( delta.toDouble() < MIN_INTERVAL )
      {
        return false;
      }

      reset_     ();

      params.get ( button_,    Params::MOUSE_BUTTON );
      params.get ( mouse.mods, Params::MOUSE_MODS   );
      params.get ( mouse.x,    Params::MOUSE_XPOS   );
      params.get ( mouse.y,    Params::MOUSE_YPOS   );

      button_     &= btnMask_;
      mouse.button = button_;

      return true;
    }

    if ( ! down && button_ )
    {
      params.get ( bttn, Params::MOUSE_BUTTON );

      if ( bttn & button_ )
      {
        if ( takeSample_( gtx, depth_, sample_, *target_ ) )
        {
          lastTime_ = Time::now ();

          gtx.repostCallback ( sampleCallback_, this, lastTime_ );
        }
        else
        {
          reset_ ();
        }

        return true;
      }
    }

    return false;
  }

  if ( action == GLActions::CLICK2_EVENT && mouse.button )
  {
    gtx.repostCallback ( click2Callback_, this, Time::now() );

    return true;
  }

  if ( action == GLActions::MOTION_EVENT && button_ )
  {
    int  x, y;

    params.get ( x, Params::MOUSE_XPOS );
    params.get ( y, Params::MOUSE_YPOS );

    if ( ::abs(x - mouse.x) > MouseDrag::THRESHOLD ||
         ::abs(y - mouse.y) > MouseDrag::THRESHOLD )
    {
      button_ = 0;
    }

    return true;
  }

  if ( action == GLActions::KEY_EVENT )
  {
    button_ = 0;
    return true;
  }

  return false;
}


//-----------------------------------------------------------------------
//   getVisibility
//-----------------------------------------------------------------------


void PointSampler::getVisibility

  ( bool&             viz,
    Box&              box,
    const Transform&  proj ) const

{
  viz = false;

  box.makeEmpty ();
}


//-----------------------------------------------------------------------
//   takeSample
//-----------------------------------------------------------------------


void PointSampler::takeSample

  ( GfxContext&        gtx,
    const MouseClick&  click )

{
  Sample   sample;
  GLfloat  depth;


  sample.mouse = click;

  if ( takeSample_( gtx, depth, sample, *target_ ) )
  {
    sampleEvent.emit ( gtx, sample );
  }
}


//-----------------------------------------------------------------------
//   setTarget
//-----------------------------------------------------------------------


void PointSampler::setTarget ( const Ref<GfxNode>& obj )
{
  JEM_PRECHECK ( obj != NIL );

  target_ = obj;

  reset_ ();
}


//-----------------------------------------------------------------------
//   setMouseMask
//-----------------------------------------------------------------------


void PointSampler::setMouseMask ( int btnMask )
{
  btnMask_ = btnMask;

  reset_ ();
}


//-----------------------------------------------------------------------
//   find
//-----------------------------------------------------------------------


Ref<PointSampler> PointSampler::find

  ( const Ref<GfxNode>&  root,
    const Ref<Object>&   target )

{
  if ( root == NIL )
  {
    return NIL;
  }

  Properties   params ( "actionParams" );
  Ref<Object>  obj;

  params.set       ( TARGET_, target );
  root->takeAction ( FIND_SAMPLER_, params );
  params.find      ( obj, SAMPLER_ );

  return dynamicCast<Self> ( obj );
}


//-----------------------------------------------------------------------
//   reset_
//-----------------------------------------------------------------------


void PointSampler::reset_ ()
{
  Sample&  s     =  sample_;

  s.mouse.what   =  MouseClick::SINGLE;
  s.mouse.button =  0;
  s.mouse.mods   =  0;
  s.mouse.x      =  0;
  s.mouse.y      =  0;
  s.point        =  0.0;
  s.error        =  1.0;
  button_        =  0;
  depth_         = -1.0e-10F;
}


//-----------------------------------------------------------------------
//   takeSample_
//-----------------------------------------------------------------------


bool PointSampler::takeSample_

  ( GfxContext&  gtx,
    GLfloat&     depth,
    Sample&      sample,
    GfxNode&     target )

{
  static const GLint  X_OFFSET[4] = { 1,  0, -1,  0 };
  static const GLint  Y_OFFSET[4] = { 0,  1,  0, -1 };

  MouseClick&  mouse = sample.mouse;

  Transform    tr, pr;
  Rect         vport;
  Box          box;
  Vertex3d     a, b;
  GLint        x, y;
  int          sampleCount;


  target.getBBox ( box );

  if ( box.isEmpty() )
  {
    return false;
  }

  getViewport ( vport );

  if ( vport.isEmpty() )
  {
    return false;
  }

  getProjection ( pr );

  tr = getTransformOf ( target.getParent() );

  box.transform ( tr );
  box.project   ( pr, vport );

  if ( box.isEmpty() )
  {
    return false;
  }

  box.getCorners ( a, b );

  x = (GLint) mouse.x;
  y = (GLint) mouse.y;

  if ( (GLdouble) x < a[0] && (GLdouble) x > b[0] &&
       (GLdouble) y < a[1] && (GLdouble) y > b[1] )
  {
    return false;
  }

  glClear ( GL_DEPTH_BUFFER_BIT );

  glPushMatrix ();
  setCurrent   ( tr );

  target.takeGLAction ( gtx, GLActions::DRAW_CDATA, Properties() );

  // Make sure that the correct image is shown. This can be removed
  // when the target object is drawn to a separate buffer.

  gtx.postFastRedraw ( &target );

  // Must flush before reading pixel data!

  glFlush      ();
  glPopMatrix  ();

  glReadPixels ( x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth );

  if ( depth >= 0.999F )
  {
    return false;
  }

  b[0] = (GLdouble) x;
  b[1] = (GLdouble) y;
  b[2] = (GLdouble) depth;

  fromWindow ( b, vport );

  if ( ! unproject( a, pr, b ) )
  {
    return false;
  }

  if ( ! invert( tr ) )
  {
    return false;
  }

  glReadPixels ( x, y, 1, 1, GL_RGBA, GL_FLOAT, sample.color.addr() );

  matmul ( sample.point, tr, a );

  // Determine the error by simulating four mouse clicks next to
  // the original mouse click.

  sample.error = 0.0;
  sampleCount  = 0;

  for ( int i = 0; i < 4; i++ )
  {
    x = (GLint) mouse.x + X_OFFSET[i];
    y = (GLint) mouse.y + Y_OFFSET[i];

    glReadPixels ( x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth );

    if ( depth >= 0.999F )
    {
      continue;
    }

    b[0] = (GLdouble) x;
    b[1] = (GLdouble) y;
    b[2] = (GLdouble) depth;

    fromWindow ( b, vport );

    if ( ! unproject( a, pr, b ) )
    {
      continue;
    }

    matmul ( b, tr, a );

    a = abs ( sample.point - b );

    sample.error =

      where ( a > sample.error, a, sample.error );

    sampleCount++;
  }

  if ( sampleCount )
  {
    sample.error *= 0.5;
  }
  else
  {
    sample.error = abs ( sample.point );
  }

  return true;
}


//-----------------------------------------------------------------------
//   getSampler_()
//-----------------------------------------------------------------------


Ref<PointSampler> PointSampler::getSampler_ ( const Ref<Object>& obj )
{
  Ref<Self>  ps = checkedCast<Self> ( obj );

  GfxNode*   node;
  GfxNode*   root;


  if ( ! ps->sample_.mouse.button )
  {
    return NIL;
  }

  node = ps.get ();

  do
  {
    root = node;
    node = node->getParent ();
  }
  while ( node && ! dynamic_cast<RedrawRoot*>( root ) );

  {
    Properties  params;

    params.set       ( SAMPLER_, ps );
    root->takeAction ( GET_SAMPLE_, params );
    params.get       ( ps, SAMPLER_ );
  }

  return ps;
}


//-----------------------------------------------------------------------
//   sampleCallback_
//-----------------------------------------------------------------------


void PointSampler::sampleCallback_

  ( GfxContext&         gtx,
    const Ref<Object>&  obj )

{
  Ref<Self>  ps = getSampler_ ( obj );

  if ( ps != NIL )
  {
    Sample  sample = ps->sample_;

    ps->button_ = 0;

    gtx.cancelCallbacks  ( sampleCallback_ );
    ps->sampleEvent.emit ( gtx, sample );
  }
}


//-----------------------------------------------------------------------
//   click2Callback_
//-----------------------------------------------------------------------


void PointSampler::click2Callback_

  ( GfxContext&         gtx,
    const Ref<Object>&  obj )

{
  Ref<Self>  ps = getSampler_ ( obj );

  if ( ps != NIL )
  {
    Sample  sample = ps->sample_;

    sample.mouse.what = MouseClick::DOUBLE;

    gtx.cancelCallbacks  ( click2Callback_ );
    ps->sampleEvent.emit ( gtx, sample );
  }
}


JEM_END_PACKAGE( gl )
