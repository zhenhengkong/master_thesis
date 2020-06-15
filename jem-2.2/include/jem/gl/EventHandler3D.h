
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

#ifndef JEM_GL_EVENTHANDLER3D_H
#define JEM_GL_EVENTHANDLER3D_H

#include <jem/gl/StdEventHandler.h>


JEM_BEGIN_PACKAGE( gl )


class Axes3D;
class Transformer;


//-----------------------------------------------------------------------
//   class EventHandler3D
//-----------------------------------------------------------------------


class EventHandler3D : public StdEventHandler
{
 public:

  JEM_DECLARE_CLASS       ( EventHandler3D, StdEventHandler );


                            EventHandler3D

    ( const Ref<Camera>&      camera,
      const Ref<GfxNode>&     world );

  virtual void              redraw

    ( GfxContext&             gtx,
      int                     action,
      const Properties&       params );

  virtual void              keyPressed

    ( GfxContext&             gtx,
      const KeyPress&         key  );

  virtual void              mouseClicked

    ( GfxContext&             gtx,
      const MouseClick&       mouse );

  virtual void              mouseDragged

    ( GfxContext&             gtx,
      const MouseDrag&        mouse );

  virtual bool              exeCommand

    ( GfxContext&             gtx,
      const String&           cmd );

  virtual void              printHelp

    ( PrintWriter&            pr );

  virtual void              setFocus

    ( const Ref<GfxNode>&     node );

  virtual void              listOptions

    ( Menu&                   menu )           const;


 protected:

  virtual                  ~EventHandler3D  ();


 private:

  void                      rotate_

    ( const KeyPress&         key );

  void                      translate_

    ( const KeyPress&         key );


 private:

  Ref<Axes3D>               axes_;
  Ref<Transformer>          transformer_;

};


JEM_END_PACKAGE( gl )

#endif
