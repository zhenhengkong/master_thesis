
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
#include <jem/base/Thread.h>
#include <jem/base/RuntimeException.h>
#include "NativeDisplay.h"


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class NativeWindow
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


NativeWindow::NativeWindow ()
{
  mousePos   = 0;
  actionID_  = 0;
  workLoad_  = 0.0;
  workLoad0_ = 0.0;

  runTimer_.start ();
}


NativeWindow::~NativeWindow ()
{}


//-----------------------------------------------------------------------
//   close
//-----------------------------------------------------------------------


void NativeWindow::close ()
{
  if ( textures.hasTextures() )
  {
    setCurrent           ();
    textures.delTextures ();
  }
}


//-----------------------------------------------------------------------
//   postCallback
//-----------------------------------------------------------------------


void NativeWindow::postCallback

  ( Callback            cb,
    const Ref<Object>&  obj,
    const Time&         tm )

{
  JEM_PRECHECK ( cb );

  if ( Thread::getCurrent() != Display::getThread() )
  {
    contextError ( JEM_FUNC );
  }

  callbacks.insert ( cb, obj, tm );
}


//-----------------------------------------------------------------------
//   repostCallback
//-----------------------------------------------------------------------


void NativeWindow::repostCallback

  ( Callback            cb,
    const Ref<Object>&  obj,
    const Time&         tm )

{
  JEM_PRECHECK ( cb );

  if ( Thread::getCurrent() != Display::getThread() )
  {
    contextError ( JEM_FUNC );
  }

  callbacks.update ( cb, obj, tm );
}


//-----------------------------------------------------------------------
//   cancelCallback
//-----------------------------------------------------------------------


void NativeWindow::cancelCallback

  ( Callback            cb,
    const Ref<Object>&  obj )

{
  if ( Thread::getCurrent() != Display::getThread() )
  {
    contextError ( JEM_FUNC );
  }

  callbacks.erase ( cb, obj );
}


//-----------------------------------------------------------------------
//   cancelCallbacks
//-----------------------------------------------------------------------


void NativeWindow::cancelCallbacks ( Callback cb )
{
  if ( Thread::getCurrent() != Display::getThread() )
  {
    contextError ( JEM_FUNC );
  }

  callbacks.erase ( cb );
}


//-----------------------------------------------------------------------
//   getTexture
//-----------------------------------------------------------------------


GLuint NativeWindow::getTexture ( const void* addr ) const
{
  return textures.getTexture ( addr );
}


//-----------------------------------------------------------------------
//   newTexture
//-----------------------------------------------------------------------


GLuint NativeWindow::newTexture

  ( const void*         addr,
    const Ref<Object>&  owner )

{
  return textures.newTexture ( addr, owner );
}


//-----------------------------------------------------------------------
//   delTexture
//-----------------------------------------------------------------------


void NativeWindow::delTexture ( const void* addr )
{
  textures.delTexture ( addr );
}


//-----------------------------------------------------------------------
//   getMousePos
//-----------------------------------------------------------------------


Vertex2i NativeWindow::getMousePos () const
{
  return mousePos;
}


//-----------------------------------------------------------------------
//   getActionID
//-----------------------------------------------------------------------


int NativeWindow::getActionID () const
{
  return actionID_;
}


//-----------------------------------------------------------------------
//   incrActionID
//-----------------------------------------------------------------------


int NativeWindow::incrActionID ()
{
  if ( actionID_ == maxOf( actionID_ ) )
  {
    actionID_ = 1;
  }
  else
  {
    actionID_++;
  }

  return actionID_;
}


//-----------------------------------------------------------------------
//   startWork
//-----------------------------------------------------------------------


void NativeWindow::startWork ()
{
  JEM_ASSERT ( Thread::getCurrent() == Display::getThread() );

  workTimer_.start ();
}


//-----------------------------------------------------------------------
//   stopWork
//-----------------------------------------------------------------------


void NativeWindow::stopWork ()
{
  JEM_ASSERT ( Thread::getCurrent() == Display::getThread() );

  using jem::max;

  workTimer_.stop ();

  double  rt = runTimer_ .toDouble ();
  double  wt = workTimer_.toDouble ();
  double  wl = wt / max ( wt, rt, 0.01 );

  if ( rt > 0.1 )
  {
    Lock<Monitor>  lock ( mon );

    workLoad_ = wl = 0.5 * (workLoad0_ + wl);
  }

  if ( rt > 1.0 )
  {
    workLoad0_ = wl;

    runTimer_ .reset ();
    workTimer_.reset ();
  }
}


//-----------------------------------------------------------------------
//   getWorkLoad
//-----------------------------------------------------------------------


double NativeWindow::getWorkLoad () const
{
  double  load;

  {
    Lock<Monitor>  lock ( mon );

    load = workLoad_;
  }

  return load;
}


//-----------------------------------------------------------------------
//   contextError
//-----------------------------------------------------------------------


void NativeWindow::contextError ( const String& where ) const
{
  throw RuntimeException (
    where,
    "graphics context method invoked outside the display thread"
  );
}


//-----------------------------------------------------------------------
//   postRedraw_
//-----------------------------------------------------------------------


void NativeWindow::postRedraw_ ( const Ref<GfxNode>& root )
{
  if ( Thread::getCurrent() != Display::getThread() )
  {
    contextError ( JEM_FUNC );
  }

  RedrawEventPtr  ev = new RedrawEvent ( root );

  {
    Lock<Monitor>  lock ( mon );

    events.push ( ev.release() );
    mon.notify  ();
  }
}


//-----------------------------------------------------------------------
//   postFastRedraw_
//-----------------------------------------------------------------------


void NativeWindow::postFastRedraw_ ( const Ref<GfxNode>& root )
{
  if ( Thread::getCurrent() != Display::getThread() )
  {
    contextError ( JEM_FUNC );
  }

  RedrawEventPtr  ev = new RedrawEvent ( root );

  ev->flags |= RedrawEvent::FAST;

  {
    Lock<Monitor>  lock ( mon );

    events.push ( ev.release() );
    mon.notify  ();
  }
}


JEM_END_PACKAGE( gl )
