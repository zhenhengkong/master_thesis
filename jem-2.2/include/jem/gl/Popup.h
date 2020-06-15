
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

#ifndef JEM_GL_POPUP_H
#define JEM_GL_POPUP_H

#include <jem/gl/Font.h>
#include <jem/gl/GfxNode.h>


namespace jem
{
  class Time;
}


JEM_BEGIN_PACKAGE( gl )


class Viewer;


//-----------------------------------------------------------------------
//   class Popup
//-----------------------------------------------------------------------


class Popup : public GfxNode
{
 public:

  JEM_DECLARE_CLASS       ( Popup, GfxNode );


  explicit                  Popup

    ( const String&           name = "popup" );

  virtual bool              takeAction

    ( const String&           action,
      const Properties&       params );

  virtual bool              takeGLAction

    ( GfxContext&             gtx,
      int                     action,
      const Properties&       params );

  virtual void              hide      ();

  virtual void              hideAt

    ( GfxContext&             gtx,
      const Time&             tm );

  virtual void              show

    ( Viewer&                 viewer,
      const Ref<GfxNode>&     node );

  virtual void              show

    ( GfxContext&             gtx,
      const Ref<GfxNode>&     node );

  virtual void              redraw

    ( GfxContext&             gtx )      = 0;


 public:

  Font                      font;


 protected:

  virtual                  ~Popup     ();


 private:

  void                      show_

    ( GfxNode*                node );

  static void               hideCallback_

    ( GfxContext&             gtx,
      const Ref<Object>&      obj );


 private:

  static const char*        HIDE_;

  int                       actionID_;

};


JEM_END_PACKAGE( gl )

#endif
