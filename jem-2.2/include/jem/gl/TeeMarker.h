
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

#ifndef JEM_GL_TEEMARKER_H
#define JEM_GL_TEEMARKER_H

#include <jem/gl/MarkerStyle.h>
#include <jem/gl/ColoredMarker.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class TeeMarker
//-----------------------------------------------------------------------


class TeeMarker : public MarkerStyle,
                  public ColoredMarker
{
 public:

  JEM_DECLARE_CLASS       ( TeeMarker, MarkerStyle );


  explicit                  TeeMarker

    ( GLfloat                 size  = -0.02F,
      const Color&            color =  Color::black() );

  virtual String            toString    () const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )      const;

  virtual void              listProps

    ( const Properties&       props )      const;

  virtual void              beginDraw

    ( GfxContext&             gtx,
      const Box&              box,
      idx_t                   count );

  virtual void              endDraw     ();

  virtual void              drawMarker

    ( idx_t                   tag,
      const GLfloat*          pos );

  virtual void              setColor

    ( const Color&            color );

  virtual Color             getColor      () const;

  void                      setSize

    ( GLfloat                 size );

  inline GLfloat            getSize       () const;

  void                      setAngle

    ( GLfloat                 angle );

  inline GLfloat            getAngle      () const;


 protected:

  virtual                  ~TeeMarker     ();


 private:

  static const byte         INACTIVE_;
  static const byte         DRAW_QUADS_;
  static const byte         DRAW_LINES_;


  GLfloat                   size_;
  GLfloat                   angle_;
  Color                     color_;

  GLfloat                   geom_[2];
  Color                     color0_;
  GLfloat                   lwidth_;
  GLboolean                 light_;
  GLboolean                 blend_;

  byte                      state_;

};




//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getSize
//-----------------------------------------------------------------------


inline GLfloat TeeMarker::getSize () const
{
  return size_;
}


//-----------------------------------------------------------------------
//   getAngle
//-----------------------------------------------------------------------


inline GLfloat TeeMarker::getAngle () const
{
  return angle_;
}


JEM_END_PACKAGE( gl )

#endif
