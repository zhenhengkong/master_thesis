
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

#ifndef JEM_GL_NATIVEWINDOW_H
#define JEM_GL_NATIVEWINDOW_H

#include <jem/base/Object.h>
#include <jem/base/Monitor.h>
#include <jem/util/Timer.h>
#include <jem/gl/import.h>
#include <jem/gl/GfxContext.h>
#include <jem/gl/EventQueue.h>
#include <jem/gl/CallbackQueue.h>
#include <jem/gl/TextureManager.h>


JEM_BEGIN_PACKAGE( gl )


class GfxError;
class ErrorHandler;
class NativeDisplay;

using jem::util::Timer;


//-----------------------------------------------------------------------
//   class NativeWindow
//-----------------------------------------------------------------------


class NativeWindow : public Object,
                     public GfxContext
{
 public:

  typedef Object            Super;
  typedef NativeWindow      Self;
  typedef NativeDisplay     Display;


  virtual void              close         ();
  virtual void              setCurrent    () = 0;

  virtual void              postCallback

    ( Callback                cb,
      const Ref<Object>&      obj,
      const Time&             tm );

  virtual void              repostCallback

    ( Callback                cb,
      const Ref<Object>&      obj,
      const Time&             tm );

  virtual void              cancelCallback

    ( Callback                cb,
      const Ref<Object>&      obj );

  virtual void              cancelCallbacks

    ( Callback                cb );

  virtual GLuint            getTexture

    ( const void*             addr )         const;

  virtual GLuint            newTexture

    ( const void*             addr,
      const Ref<Object>&      owner );

  virtual void              delTexture

    ( const void*             addr );

  virtual Vertex2i          getMousePos   () const;
  virtual int               getActionID   () const;
  int                       incrActionID  ();
  void                      startWork     ();
  void                      stopWork      ();
  double                    getWorkLoad   () const;

  void                      contextError

    ( const String&           where )        const;


 public:

  Monitor                   mon;
  EventQueue                events;
  CallbackQueue             callbacks;
  TextureManager            textures;
  Ref<ErrorHandler>         errHandler;

  Ref<GfxError>             err;
  Ref<GfxNode>              scene;
  Vertex2i                  mousePos;


 protected:

                            NativeWindow  ();
  virtual                  ~NativeWindow  ();

  virtual void              postRedraw_

    ( const Ref<GfxNode>&     root );

  virtual void              postFastRedraw_

    ( const Ref<GfxNode>&     root );


 private:

  int                       actionID_;

  Timer                     runTimer_;
  Timer                     workTimer_;
  double                    workLoad_;
  double                    workLoad0_;

};


JEM_END_PACKAGE( gl )

#endif
