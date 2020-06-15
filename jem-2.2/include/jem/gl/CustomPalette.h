
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

#ifndef JEM_GL_CUSTOMPALETTE_H
#define JEM_GL_CUSTOMPALETTE_H

#include <jem/gl/Array.h>
#include <jem/gl/Palette.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class CustomPalette
//-----------------------------------------------------------------------


class CustomPalette : public Palette
{
 public:

  JEM_DECLARE_CLASS       ( CustomPalette, Palette );

  static const int          HIDE_MIN_MAX;


                            CustomPalette ();

                            CustomPalette

    ( const GLfloatMatrix&    colors,
      const Array<double>&    ranges );

  virtual void              getColor

    ( Color&                  col,
      double                  val )          const;

  virtual double            findValue

    ( double&                 val,
      double                  prec,
      const Color&            col )          const;

  virtual double            getMinValue   () const;
  virtual double            getMaxValue   () const;
  virtual GLfloat           getMinAlpha   () const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )        const;

  virtual void              listProps

    ( const Properties&       props )        const;

  void                      setColorRanges

    ( const GLfloatMatrix&    colors,
      const Array<double>&    ranges );

  inline GLfloatMatrix      getColors     () const;
  inline Array<double>      getRanges     () const;

  void                      setOptions

    ( int                     options );

  int                       getOptions    () const;

  virtual void              fillRectangle

    ( GfxContext&             gtx,
      GLint                   width,
      GLint                   height )       const;

  virtual void              drawTickMarks

    ( GfxContext&             gtx,
      TextPainter&            txp,
      GLint                   width,
      GLint                   height,
      GLint                   xtick,
      Alignment               align,
      double                  scale,
      double                  bias )         const;


 protected:

  virtual                  ~CustomPalette ();


 private:

  static bool               checkRanges_

    ( const Array<double>&    ranges );


 private:

  GLfloatMatrix             colors_;
  Array<double>             ranges_;
  int                       options_;

};



//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getColors
//-----------------------------------------------------------------------


inline GLfloatMatrix CustomPalette::getColors () const
{
  return colors_;
}


//-----------------------------------------------------------------------
//   getRanges
//-----------------------------------------------------------------------


inline Array<double> CustomPalette::getRanges () const
{
  return ranges_;
}


JEM_END_PACKAGE( gl )

#endif
