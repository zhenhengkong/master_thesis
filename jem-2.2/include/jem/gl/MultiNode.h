
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

#ifndef JEM_GL_MULTINODE_H
#define JEM_GL_MULTINODE_H

#include <jem/util/ObjFlex.h>
#include <jem/util/event/Event.h>
#include <jem/gl/Compound.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class MultiNode
//-----------------------------------------------------------------------


class MultiNode : public GfxNode, public Compound
{
 public:

  JEM_DECLARE_CLASS       ( MultiNode, GfxNode );

  util::Event
    < Ref<GfxNode>,
      Self& >               newChildEvent;
  util::Event
    < Ref<GfxNode>,
      Self& >               childErasedEvent;


  explicit                  MultiNode

    ( const String&           name = "" );

  virtual GfxNode*          findNode

    ( const String&           name )         const;

  virtual bool              takeAction

    ( const String&           action,
      const Properties&       params );

  virtual bool              takeGLAction

    ( GfxContext&             gtx,
      int                     action,
      const Properties&       params );

  virtual void              getBBox

    ( Box&                    box )          const;

  virtual void              getVisibility

    ( bool&                   viz,
      Box&                    vbox,
      const Transform&        proj )         const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )        const;

  virtual void              listProps

    ( const Properties&       props )        const;

  virtual void              clear         ();
  inline  idx_t             size          () const;
  virtual idx_t             childCount    () const;

  virtual void              addChild

    ( const Ref<GfxNode>&     child );

  virtual bool              eraseChild

    ( const Ref<GfxNode>&     child );

  virtual void              eraseChild

    ( idx_t                   index );

  GfxNode*                  getChild

    ( idx_t                   index )        const;

  idx_t                      findChild

    ( const Ref<GfxNode>&     child )        const;


 protected:

  virtual                  ~MultiNode     ();


 private:

  class                     PosSaver_;
  friend class              PosSaver_;

  util::ObjFlex             children_;
  idx_t                     pos_;

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


void                        explode

  ( MultiNode&                multi,
    double                    scale = 0.1 );




//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   size
//-----------------------------------------------------------------------


inline idx_t MultiNode::size () const
{
  return children_.size ();
}


JEM_END_PACKAGE( gl )

#endif
