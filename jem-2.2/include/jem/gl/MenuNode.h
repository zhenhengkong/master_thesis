
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

#ifndef JEM_GL_MENUNODE_H
#define JEM_GL_MENUNODE_H

#include <jem/base/array/Array.h>
#include <jem/util/event/Event.h>
#include <jem/gl/Font.h>
#include <jem/gl/Rect.h>
#include <jem/gl/Pivotable.h>
#include <jem/gl/WrapperNode.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class MenuNode
//-----------------------------------------------------------------------


class MenuNode : public WrapperNode,
                 public Pivotable
{
 public:

  JEM_DECLARE_CLASS       ( MenuNode, WrapperNode );

  util::Event
    < GfxContext&,
      const String& >       selectEvent;


                            MenuNode

    ( const String&           name,
      const Menu&             menu );

                            MenuNode

    ( const String&           name,
      const Array<String>&    list );

  virtual bool              takeAction

    ( const String&           action,
      const Properties&       params );

  virtual bool              takeGLAction

    ( GfxContext&             gtx,
      int                     action,
      const Properties&       params );

  virtual void              setChild

    ( const Ref<GfxNode>&     child );

  virtual Ref<GfxNode>      getChild      () const;

  void                      show

    ( GfxContext&             gtx,
      const Ref<GfxNode>&     node );

  void                      show

    ( GfxContext&             gtx,
      const Ref<GfxNode>&     node,
      int                     xPos,
      int                     yPos );

  void                      hide          ();

  void                      hide

    ( GfxContext&             gtx );

  inline bool               isActive      () const;


 protected:

  virtual                  ~MenuNode      ();


 private:

  void                      show_

    ( GfxContext&             gtx,
      Pivotable&              pivot,
      GLint                   xPos,
      GLint                   yPos );

  void                      init_

    ( GLint                   xPos,
      GLint                   yPos );

  void                      redraw_

    ( GfxContext&             gtx );

  void                      setFocus_

    ( GfxContext&             gtx,
      GLint                   xPos,
      GLint                   yPos );

  void                      moveFocus_

    ( GfxContext&             gtx,
      int                     dir );

  void                      selectFocus_

    ( GfxContext&             gtx );


 private:

  struct                    Item_
  {
    Rect                      rect;
    String                    label;
  };

  static const char*        HIDE_;
  static const idx_t        FOCUS_NONE_;

  Array<Item_>              items_;
  Font                      font_;
  Rect                      rect_;
  GLsizei                   space_;
  idx_t                     focus_;

};



//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   isActive
//-----------------------------------------------------------------------


inline bool MenuNode::isActive () const
{
  return (getParent() != 0);
}


JEM_END_PACKAGE( gl )

#endif
