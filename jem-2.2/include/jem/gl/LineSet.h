
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

#ifndef JEM_GL_LINESET_H
#define JEM_GL_LINESET_H

#include <jem/gl/Array.h>
#include <jem/gl/LeafNode.h>


JEM_BEGIN_PACKAGE( gl )


class ColorMap;
class VertexSet;


//-----------------------------------------------------------------------
//   class LineSet
//-----------------------------------------------------------------------


class LineSet : public LeafNode
{
 public:

  JEM_DECLARE_CLASS       ( LineSet, LeafNode );


  enum                      ColorMode
  {
                              NO_COLORS,
                              LINE_COLORS,
                              VERTEX_COLORS
  };

  static const int          SAMPLE_COUNT;


                            LineSet

    ( const String&           name,
      const Ref<VertexSet>&   vx );

  virtual void              getBBox

    ( Box&                    box )              const;

  virtual void              redraw

    ( GfxContext&             gtx );

  virtual void              redrawFast

    ( GfxContext&             gtx,
      double                  lod );

  virtual bool              exeCommand

    ( GfxContext&             gtx,
      const String&           cmd,
      const Properties&       params );

  virtual void              listOptions

    ( Menu&                   menu )             const;

  inline idx_t              size              () const;

  void                      setTopology

    ( const IdxVector&       offsets,
      const IdxVector&       indices );

  IdxVector                 getLineOffsets    () const;
  IdxVector                 getVertexIndices  () const;
  void                      setNoColors       ();

  void                      setLineColors

    ( const GLubyteMatrix&    colors );

  void                      setVertexColors

    ( const GLfloatVector&    colors,
      const Ref<ColorMap>&    colorMap );

  inline ColorMode          getColorMode      () const;
  inline GLubyteMatrix      getLineColors     () const;
  inline GLfloatVector      getVertexColors   () const;
  inline ColorMap*          getColorMap       () const;

  void                      findLine

    ( idx_t&                  index,
      GLfloat&                dist,
      GLfloat                 x,
      GLfloat                 y,
      GLfloat                 z = 0.0F )         const;


 protected:

  virtual                  ~LineSet           ();


 private:

  void                      drawNoColors2D_   () const;
  void                      drawNoColors3D_   () const;
  void                      drawLnColors2D_   () const;
  void                      drawLnColors3D_   () const;
  void                      drawVxColors2D_   () const;
  void                      drawVxColors3D_   () const;
  void                      blendNoColors3D_  () const;
  void                      blendLnColors3D_  () const;
  void                      blendVxColors3D_  () const;

  void                      plotNoColors2D_

    ( double                  lod )              const;

  void                      plotNoColors3D_

    ( double                  lod )              const;

  void                      plotLnColors2D_

    ( double                  lod )              const;

  void                      plotLnColors3D_

    ( double                  lod )              const;

  void                      plotVxColors2D_

    ( double                  lod )              const;

  void                      plotVxColors3D_

    ( double                  lod )              const;

  inline void               beginBlend_

    ( GLboolean&              zmask )            const;

  void                      endBlend_

    ( GLboolean               zmask )            const;

  void                      checkData_

    ( const String&           context );

  void                      connectHandlers_  ();
  void                      newShapeHandler_  ();


 private:

  class                     Work_;
  friend class              Work_;


  Ref<VertexSet>            vertices_;
  IdxVector                 lnOffsets_;
  IdxVector                 vxIndices_;

  ColorMode                 colorMode_;
  GLubyteMatrix             lnColors_;
  GLfloatVector             vxColors_;
  Ref<ColorMap>             colorMap_;

  bool                      opaque_;
  bool                      noStrips_;
  bool                      checked_;

};




//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   size
//-----------------------------------------------------------------------


inline idx_t LineSet::size () const
{
  return (lnOffsets_.size() - 1);
}


//-----------------------------------------------------------------------
//   getColorMode
//-----------------------------------------------------------------------


inline LineSet::ColorMode LineSet::getColorMode () const
{
  return colorMode_;
}


//-----------------------------------------------------------------------
//   getLineColors
//-----------------------------------------------------------------------


inline GLubyteMatrix LineSet::getLineColors () const
{
  return lnColors_;
}


//-----------------------------------------------------------------------
//   getVertexColors
//-----------------------------------------------------------------------


inline GLfloatVector LineSet::getVertexColors () const
{
  return vxColors_;
}


//-----------------------------------------------------------------------
//   getColorMap
//-----------------------------------------------------------------------


inline ColorMap* LineSet::getColorMap () const
{
  return colorMap_.get ();
}


JEM_END_PACKAGE( gl )

#endif
