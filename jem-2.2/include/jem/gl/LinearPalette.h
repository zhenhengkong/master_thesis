
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

#ifndef JEM_GL_LINEARPALETTE_H
#define JEM_GL_LINEARPALETTE_H

#include <jem/gl/Palette.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class LinearPalette
//-----------------------------------------------------------------------


class LinearPalette : public Palette
{
 public:

  JEM_DECLARE_CLASS       ( LinearPalette, Palette );


                            LinearPalette ();

  virtual void              getColor

    ( Color&                  col,
      double                  val )          const;

  virtual double            findValue

    ( double&                 val,
      double                  prec,
      const Color&            col )          const;

  void                      setRange

    ( double                  lo,
      double                  hi );

  virtual double            getMinValue   () const;
  virtual double            getMaxValue   () const;
  virtual GLfloat           getMinAlpha   () const;

  void                      setColors

    ( const Color&            lo,
      const Color&            hi );

  inline Color              getMinColor   () const;
  inline Color              getMaxColor   () const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )        const;

  virtual void              listProps

    ( const Properties&       props )        const;

  virtual void              fillRectangle

    ( GfxContext&             gtx,
      GLint                   width,
      GLint                   height )       const;


 protected:

  virtual                  ~LinearPalette ();


 private:

  Color                     minColor_;
  Color                     maxColor_;

  double                    minValue_;
  double                    maxValue_;

};



//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getMinColor & getMaxColor
//-----------------------------------------------------------------------


inline Color LinearPalette::getMinColor () const
{
  return minColor_;
}


inline Color LinearPalette::getMaxColor () const
{
  return maxColor_;
}


JEM_END_PACKAGE( gl )

#endif
