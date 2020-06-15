
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

#ifndef JEM_GL_POINTMARKER_H
#define JEM_GL_POINTMARKER_H

#include <jem/gl/MarkerStyle.h>
#include <jem/gl/ColoredMarker.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class PointMarker
//-----------------------------------------------------------------------


class PointMarker : public MarkerStyle,
                    public ColoredMarker
{
 public:

  JEM_DECLARE_CLASS       ( PointMarker, MarkerStyle );

  static const GLfloat      MAX_SIZE;


  explicit                  PointMarker

    ( GLfloat                 size  = 3.0,
      const Color&            color = Color::black() );

  virtual String            toString      () const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )        const;

  virtual void              listProps

    ( const Properties&       props )        const;

  virtual void              beginDraw

    ( GfxContext&             gtx,
      const Box&              box,
      idx_t                   count );

  virtual void              endDraw       ();

  virtual void              drawMarker

    ( idx_t                   tag,
      const GLfloat*          pos );

  virtual void              setColor

    ( const Color&            color );

  virtual Color             getColor      () const;

  void                      setSize

    ( GLfloat                 size );

  inline GLfloat            getSize       () const;


 protected:

  virtual                  ~PointMarker   ();


 private:

  GLfloat                   size_;
  Color                     color_;
  GLboolean                 light_;
  GLboolean                 blend_;
  GLfloat                   size0_;
  Color                     color0_;
  bool                      active_;

};




//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getSize
//-----------------------------------------------------------------------


inline GLfloat PointMarker::getSize () const
{
  return size_;
}


JEM_END_PACKAGE( gl )

#endif
