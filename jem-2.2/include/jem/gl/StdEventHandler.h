
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

#ifndef JEM_GL_STDEVENTHANDLER_H
#define JEM_GL_STDEVENTHANDLER_H

#include <jem/gl/Color.h>
#include <jem/gl/EventHandler.h>


JEM_BEGIN_PACKAGE( gl )


class Camera;


//-----------------------------------------------------------------------
//   class StdEventHandler
//-----------------------------------------------------------------------


class StdEventHandler : public EventHandler
{
 public:

  JEM_DECLARE_CLASS       ( StdEventHandler, EventHandler );

  static const char         NULL_MODE;
  static const char         ZOOM_MODE;
  static const char         ROTATION_MODE;
  static const char         TRANSLATION_MODE;


                            StdEventHandler

    ( const Ref<Camera>&      camera,
      const Ref<GfxNode>&     world );

  virtual void              reset             ();

  virtual void              redraw

    ( GfxContext&             gtx,
      int                     action,
      const Properties&       params );

  virtual void              newView

    ( GfxContext&             gtx,
      const Rect&             vport );

  virtual void              keyPressed

    ( GfxContext&             gtx,
      const KeyPress&         key );

  virtual void              mouseDragged

    ( GfxContext&             gtx,
      const MouseDrag&        mouse );

  virtual bool              exeCommand

    ( GfxContext&             gtx,
      const String&           cmd );

  virtual void              printHelp

    ( PrintWriter&            pr );

  virtual void              initMenu

    ( Menu&                   menu )             const;

  virtual void              listOptions

    ( Menu&                   menu )             const;

  virtual void              setFocus

    ( const Ref<GfxNode>&     node );

  inline char               getMode           () const;

  void                      setMode

    ( char                    mode );


 public:

  Color                     focusColor;


 protected:

  virtual                  ~StdEventHandler   ();

  void                      zoomView_

    ( GfxContext&             gtx,
      int                     step );

  void                      scaleFocus_

    ( GfxContext&             gtx,
      int                     step );


 protected:

  Ref<Camera>               camera_;
  Ref<GfxNode>              world_;
  Ref<GfxNode>              focus_;
  Ref<GfxNode>              noFocus_;
  char                      mode_;


 private:

  void                      centerObj_        ();

};



//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getMode
//-----------------------------------------------------------------------


inline char StdEventHandler::getMode () const
{
  return mode_;
}


JEM_END_PACKAGE( gl )

#endif
