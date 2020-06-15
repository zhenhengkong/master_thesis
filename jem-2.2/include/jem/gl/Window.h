
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

#ifndef JEM_GL_WINDOW_H
#define JEM_GL_WINDOW_H

#include <jem/gl/Size.h>
#include <jem/gl/Color.h>
#include <jem/gl/Pivotable.h>
#include <jem/gl/RedrawRoot.h>
#include <jem/gl/WrapperNode.h>


JEM_BEGIN_PACKAGE( gl )


class GLSetup;


//-----------------------------------------------------------------------
//   class Window
//-----------------------------------------------------------------------


class Window : public WrapperNode,
               public Pivotable,
               public RedrawRoot
{
 public:

  JEM_DECLARE_CLASS       ( Window, WrapperNode );


                            Window

    ( const String&           name,
      const Ref<GfxNode>&     child,
      const Ref<GLSetup>&     setup );

  virtual bool              takeGLAction

    ( GfxContext&             gtx,
      int                     action,
      const Properties&       params );

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )        const;

  virtual void              listProps

    ( const Properties&       props )        const;

  virtual void              setChild

    ( const Ref<GfxNode>&     child );

  virtual Ref<GfxNode>      getChild      () const;
  inline Size2i             size          () const;

  void                      setBgColor

    ( const Color&            bg );

  inline Color              getBgColor    () const;


 protected:

  virtual                  ~Window        ();


 private:

  void                      initGL_       ();
  void                      setupChanged_ ();


 private:

  Ref<GLSetup>              setup_;
  Color                     bgColor_;
  bool                      newSetup_;
  Size2i                    size_;
  GLbitfield                clearMask_;

};




//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   size
//-----------------------------------------------------------------------


inline Size2i Window::size () const
{
  return size_;
}


//-----------------------------------------------------------------------
//   getBgColor
//-----------------------------------------------------------------------


inline Color Window::getBgColor () const
{
  return bgColor_;
}


JEM_END_PACKAGE( gl )

#endif
