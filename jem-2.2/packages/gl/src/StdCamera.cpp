
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
#include <jem/base/limits.h>
#include <jem/base/ClassTemplate.h>
#include <jem/util/Timer.h>
#include <jem/util/Properties.h>
#include <jem/gl/gl.h>
#include <jem/gl/utilities.h>
#include <jem/gl/Box.h>
#include <jem/gl/Actions.h>
#include <jem/gl/PropertyNames.h>
#include <jem/gl/DummyNode.h>
#include <jem/gl/StdCamera.h>


JEM_DEFINE_CLASS( jem::gl::StdCamera );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class StdCamera
//=======================================================================

/*
  Implementation note: some methods of the StdCamera class hold a
  local reference to the child node. This is necessary because the
  child node may be deleted by itself during the method call.
*/

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const int      StdCamera::MAX_QUALITY  = 8;

const int      StdCamera::AUTO_QUALITY = 1 << 0;

const GLfloat  StdCamera::JITTER_[58]  =

  {
    // 1 sample:

    0.0F, 0.0F,

    // 2 samples:

    0.25F, 0.75F, 0.75F, 0.25F,

    // 3 samples:

    0.5033922635F, 0.8317967229F, 0.7806016275F, 0.2504380877F,
    0.2261828938F, 0.4131553612F,

    // 4 samples:

    0.375F, 0.250F, 0.125F, 0.75F, 0.875F, 0.25F, 0.625F, 0.75F,

    // 5 samples:

    0.5F, 0.5F, 0.3F, 0.1F, 0.7F, 0.9F, 0.9F, 0.3F, 0.1F, 0.7F,

    // 6 samples:

    0.4646464646F, 0.4646464646F, 0.1313131313F, 0.7979797979F,
    0.5353535353F, 0.8686868686F, 0.8686868686F, 0.5353535353F,
    0.7979797979F, 0.1313131313F, 0.2020202020F, 0.2020202020F,

    // 8 samples:

    0.5625F, 0.4375F, 0.0625F, 0.9375F, 0.3125F, 0.6875F,
    0.6875F, 0.8125F, 0.8125F, 0.1875F, 0.9375F, 0.5625F,
    0.4375F, 0.0625F, 0.1875F, 0.3125F
  };

const int      StdCamera::JINDEX_[16] =

  {  0,  2,  // quality 1 (1 sample)
     2,  6,  // quality 2 (2 samples)
     6, 12,  // quality 3 (3 samples)
    12, 20,  // quality 4 (4 samples)
    20, 30,  // quality 5 (5 samples)
    30, 42,  // quality 6 (6 samples)
    42, 58,  // quality 7 (8 samples)
    42, 58   // quality 8 (8 samples)
  };


//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


StdCamera::StdCamera

  ( const String&        name,
    const Ref<GfxNode>&  child ) :

    Super  ( name  ),
    child_ ( child )

{
  if ( child_ == NIL )
  {
    child_ = newInstance<DummyNode> ( myName_ );
  }

  child_ ->setParent   ( this );
  proj_   .setIdentity ();
  transfm_.setIdentity ();

  zoom_        = 0.0;
  options_     = 0;
  newProj_     = true;
  quality_     = 1;
  maxQuality_  = MAX_QUALITY;
  nextQuality_ = (double) quality_;
  lastLowered_ = Time::zero ();
}


StdCamera::~StdCamera ()
{
  child_->clearParent ();
}


//-----------------------------------------------------------------------
//   setChild
//-----------------------------------------------------------------------


void StdCamera::setChild ( const Ref<GfxNode>& child )
{
  child_->clearParent ();

  if ( child == NIL )
  {
    child_ = newInstance<DummyNode> ( myName_ );
  }
  else
  {
    child_ = child;
  }

  child_->setParent ( this );
}


//-----------------------------------------------------------------------
//   getChild
//-----------------------------------------------------------------------


Ref<GfxNode> StdCamera::getChild () const
{
  return child_;
}


//-----------------------------------------------------------------------
//   findNode
//-----------------------------------------------------------------------


GfxNode* StdCamera::findNode ( const String& name ) const
{
  if ( name == myName_ )
  {
    return const_cast<Self*> ( this );
  }
  else
  {
    return child_->findNode ( name );
  }
}


//-----------------------------------------------------------------------
//   takeAction
//-----------------------------------------------------------------------


bool StdCamera::takeAction

  ( const String&      action,
    const Properties&  params )

{
  return passAction ( child_, action, params );
}


//-----------------------------------------------------------------------
//   takeGLAction
//-----------------------------------------------------------------------


bool StdCamera::takeGLAction

  ( GfxContext&        gtx,
    int                action,
    const Properties&  params )

{
  using util::Timer;

  bool  myResult    = false;
  bool  childResult = false;


  if ( action == GLActions::INIT_VIEW )
  {
    int  xpos,  ypos;
    int  width, height;

    params.get ( xpos,   GLActionParams::VIEW_XPOS   );
    params.get ( ypos,   GLActionParams::VIEW_YPOS   );
    params.get ( width,  GLActionParams::VIEW_WIDTH  );
    params.get ( height, GLActionParams::VIEW_HEIGHT );

    vport_.x      = (GLint)   xpos;
    vport_.y      = (GLint)   ypos;
    vport_.width  = (GLsizei) width;
    vport_.height = (GLsizei) height;

    newProj_ = true;
    myResult = true;
  }

  if ( newProj_ )
  {
    proj_    = newProjection_ ();
    newProj_ = false;
  }

  if ( action == GLActions::REDRAW && quality_ > 1 )
  {
    if ( options_ & AUTO_QUALITY )
    {
      Timer  tm;

      tm.start    ();
      redrawFSAA_ ( gtx, params );
      tm.stop     ();

      adjustQuality_ ( tm.toDouble() );
    }
    else
    {
      redrawFSAA_ ( gtx, params );
    }

    if ( glGetError() )
    {
      // The error may very well be caused by the absence of the
      // accumulation buffer. Disable anti-aliasing.

      quality_  = 1;
      options_ &= ~AUTO_QUALITY;
    }

    return true;
  }

  glMatrixMode ( GL_PROJECTION );
  setCurrent   ( proj_         );
  glMatrixMode ( GL_MODELVIEW  );
  setCurrent   ( transfm_      );

  if ( action == GLActions::REDRAW ||
       action == GLActions::REDRAW_FAST )
  {
    double  lod = 1.0;

    if ( action == GLActions::REDRAW_FAST )
    {
      params.find ( lod, GLActionParams::DETAIL_LEVEL );
    }

    if ( lod > 0.95 )
    {
      if ( (options_ & AUTO_QUALITY) && quality_ == 1 )
      {
        Timer  tm;

        tm.start       ();
        passGLAction   ( child_, gtx, GLActions::REDRAW, params );
        tm.stop        ();

        adjustQuality_ ( tm.toDouble() );
      }
      else
      {
        passGLAction ( child_, gtx, GLActions::REDRAW, params );
      }
    }
    else
    {
      passGLAction ( child_, gtx, action, params );
    }

    return true;
  }

  childResult = passGLAction ( child_, gtx, action, params );

  return (myResult || childResult);
}


//-----------------------------------------------------------------------
//   getBBox
//-----------------------------------------------------------------------


void StdCamera::getBBox ( Box& box ) const
{
  child_->getBBox ( box );
  box.transform   ( transfm_ );
}


//-----------------------------------------------------------------------
//   getBareBBox
//-----------------------------------------------------------------------


void StdCamera::getBareBBox ( Box& box ) const
{
  child_->getBBox ( box );
}


//-----------------------------------------------------------------------
//   getVisibility
//-----------------------------------------------------------------------


void StdCamera::getVisibility

  ( bool&             viz,
    Box&              vbox,
    const Transform&  proj ) const

{
  Transform  t;

  matmul ( t, proj, transfm_ );

  child_->getVisibility ( viz, vbox, t );

  vbox.transform ( transfm_ );
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void StdCamera::configure ( const Properties& props )
{
  using util::findBool;

  if ( props.contains( myName_ ) )
  {
    Properties  myProps = props.findProps ( myName_ );

    double      z;

    if ( myProps.find( z, PropNames::ZOOM ) )
    {
      setZoom ( z );
    }

    if ( myProps.find( quality_, PropNames::QUALITY, 1, MAX_QUALITY ) )
    {
      maxQuality_  = MAX_QUALITY;
      nextQuality_ = (double) quality_;
      lastLowered_ = Time::zero ();
    }

    findBool ( options_,  AUTO_QUALITY, myProps,
               PropNames::AUTO_QUALITY );
  }

  configChild ( child_, props );
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void StdCamera::getConfig ( const Properties& props ) const
{
  Properties  myProps = props.makeProps ( myName_ );

  myProps.set ( PropNames::ZOOM,    (double) zoom_ );
  myProps.set ( PropNames::QUALITY, quality_ );

  setBool     ( myProps,
                PropNames::AUTO_QUALITY,
                options_,  AUTO_QUALITY );

  child_->getConfig ( props );
}


//-----------------------------------------------------------------------
//   listProps
//-----------------------------------------------------------------------


void StdCamera::listProps ( const Properties& props ) const
{
  Properties  myProps = props.makeProps ( myName_ );

  myProps.set ( PropNames::ZOOM,
                "The (exponential) zoom factor" );
  myProps.set ( PropNames::QUALITY,
                "The quality of the rendered image. A quality "
                "larger than one will activate a full-screen "
                "anti-aliasing (oversampling) algorithm)" );
  myProps.set ( PropNames::AUTO_QUALITY,
                "Whether the quality should be adjusted "
                "dynamically depending on the available "
                "CPU/GPU resources" );

  child_->listProps ( props );
}


//-----------------------------------------------------------------------
//   getTransform
//-----------------------------------------------------------------------


Transform StdCamera::getTransform () const
{
  return transfm_;
}


//-----------------------------------------------------------------------
//   setTransform
//-----------------------------------------------------------------------


void StdCamera::setTransform ( const Transform& tr )
{
  transfm_ = tr;
}


//-----------------------------------------------------------------------
//   getViewport
//-----------------------------------------------------------------------


Rect StdCamera::getViewport () const
{
  return vport_;
}


//-----------------------------------------------------------------------
//   getProjection
//-----------------------------------------------------------------------


Transform StdCamera::getProjection () const
{
  // Don't call OpenGL funcs to avoid multi-threading
  // problems in the case that this function is called by
  // another thread than the viewer thread.

  if ( newProj_ )
  {
    return newProjection_ ();
  }
  else
  {
    return proj_;
  }
}


//-----------------------------------------------------------------------
//   reset
//-----------------------------------------------------------------------


void StdCamera::reset ()
{
  transfm_.setIdentity ();

  zoom_    = 0.0;
  newProj_ = true;
}


//-----------------------------------------------------------------------
//   translateObj
//-----------------------------------------------------------------------


void StdCamera::translateObj

  ( GfxNode&  node,
    GLdouble  dx,
    GLdouble  dy,
    GLdouble  dz ) const

{}


//-----------------------------------------------------------------------
//   setZoom
//-----------------------------------------------------------------------


GLdouble StdCamera::setZoom ( GLdouble z )
{
  if ( ! isTiny( z - zoom_ ) )
  {
    zoom_    = z;
    newProj_ = true;
  }

  return zoom_;
}


//-----------------------------------------------------------------------
//   getZoom
//-----------------------------------------------------------------------


GLdouble StdCamera::getZoom () const
{
  return zoom_;
}


//-----------------------------------------------------------------------
//   setQuality
//-----------------------------------------------------------------------


int StdCamera::setQuality ( int level )
{
  quality_     = clamped ( level, 1, MAX_QUALITY );
  maxQuality_  = MAX_QUALITY;
  nextQuality_ = (double) quality_;
  lastLowered_ = Time::zero ();

  return quality_;
}


//-----------------------------------------------------------------------
//   getQuality
//-----------------------------------------------------------------------


int StdCamera::getQuality () const
{
  return quality_;
}


//-----------------------------------------------------------------------
//   setOptions
//-----------------------------------------------------------------------


void StdCamera::setOptions ( int options )
{
  options_ = options;
}


//-----------------------------------------------------------------------
//   getOptions
//-----------------------------------------------------------------------


int StdCamera::getOptions () const
{
  return options_;
}


//-----------------------------------------------------------------------
//   redrawFSAA_
//-----------------------------------------------------------------------


void StdCamera::redrawFSAA_

  ( GfxContext&        gtx,
    const Properties&  params )

{
  // Make sure that the child node does not delete itself during this
  // operation.

  Ref<GfxNode>  child = child_;
  Transform     p     = proj_;

  GLdouble  dx, dx0;
  GLdouble  dy, dy0;

  int       i, j, k;


  k  = 2 * (quality_ - 1);

  i  = JINDEX_[k + 0];
  j  = JINDEX_[k + 1];

  dx = 0.0;
  dy = 0.0;

  for ( k = i; k < j; k += 2 )
  {
    dx0 = dx;
    dy0 = dy;
    dx  = JITTER_[k + 0];
    dy  = JITTER_[k + 1];

    if ( k > i )
    {
      glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }

    jitter       ( p, (dx - dx0), (dy - dy0) );

    glMatrixMode ( GL_PROJECTION );
    setCurrent   ( p             );
    glMatrixMode ( GL_MODELVIEW  );
    setCurrent   ( transfm_      );

    params.set ( GLActionParams::XJITTER, dx );
    params.set ( GLActionParams::YJITTER, dy );

    child->takeGLAction ( gtx, GLActions::REDRAW, params );

    if ( k == i )
    {
      glAccum ( GL_LOAD, 2.0F / (GLfloat) (j - i) );
    }
    else
    {
      glAccum ( GL_ACCUM, 2.0F / (GLfloat) (j - i) );
    }
  }

  params.erase ( GLActionParams::XJITTER );
  params.erase ( GLActionParams::YJITTER );

  glAccum ( GL_RETURN, 1.0 );
  glFlush ();
}


//-----------------------------------------------------------------------
//   adjustQuality_()
//-----------------------------------------------------------------------


void StdCamera::adjustQuality_ ( double dt )
{
  double  target = 0.2;


  // The target draw time is set lower if the current quality
  // level is higher.

  target *= (1.0 - 0.05 * (double) (quality_ - 1));

  if      ( dt < target )
  {
    // Ramp up the quality level.

    nextQuality_ += (target - dt) * nextQuality_;

    if ( dt          < (0.5 * target) &&
         maxQuality_ <  MAX_QUALITY   &&
         maxQuality_ <= quality_ )
    {
      Time  dt = Time::now() - lastLowered_;

      if ( dt.toDouble() > 20.0 )
      {
        maxQuality_++;
      }
    }

    if ( nextQuality_ >= (double) (quality_ + 1) )
    {
      quality_     = jem::min ( quality_ + 1, maxQuality_ );
      nextQuality_ = (double)   quality_;
    }
  }
  else if ( dt > 4.0 )
  {
    // Drop directly to the lowest quality level.

    quality_     = 1;
    maxQuality_  = jem::max ( 1, maxQuality_ - 4 );
    nextQuality_ = (double) quality_;
    lastLowered_ = Time::now ();
  }
  else if ( dt > target )
  {
    // Ramp down the quality level.

    nextQuality_ -= (dt - target);
    nextQuality_ -= 0.2 * nextQuality_;

    if ( dt > (2.0 * target) && maxQuality_ > 1 )
    {
      maxQuality_  = maxQuality_ - 1;
      lastLowered_ = Time::now ();

      if ( quality_ > maxQuality_ )
      {
        quality_     = maxQuality_;
        nextQuality_ = (double) quality_;
      }
    }

    if ( nextQuality_ <= (double) (quality_ - 1) )
    {
      quality_     = jem::max ( quality_ - 1, 1 );
      nextQuality_ = (double)   quality_;
    }
  }
}


JEM_END_PACKAGE( gl )
