
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


#include <jem/base/assert.h>
#include <jem/base/ClassTemplate.h>
#include <jem/util/Properties.h>
#include <jem/gl/PropertyNames.h>
#include <jem/gl/RainbowPalette.h>


JEM_DEFINE_CLASS( jem::gl::RainbowPalette );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class RainbowPalette
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


RainbowPalette::RainbowPalette ()
{
  minValue_ = 0.0;
  maxValue_ = 1.0;
}


RainbowPalette::~RainbowPalette ()
{}


//-----------------------------------------------------------------------
//   getColor
//-----------------------------------------------------------------------


void RainbowPalette::getColor

  ( Color&  col,
    double  val ) const

{
  if      ( val <= minValue_ )
  {
    // Blue

    col[0] = 0.0;
    col[1] = 0.0;
    col[2] = 1.0;
    col[3] = 1.0;
  }
  else if ( val >= maxValue_ )
  {
    // Red

    col[0] = 1.0;
    col[1] = 0.0;
    col[2] = 0.0;
    col[3] = 1.0;
  }
  else
  {
    Color   minColor;
    Color   maxColor;

    double  a, b;


    a = 4.0 * (val - minValue_) / (maxValue_ - minValue_);

    if      ( a < 1.0 )
    {
      minColor[2] = 1.0;
      maxColor[1] = 1.0;
      maxColor[2] = 1.0;
    }
    else if ( a < 2.0 )
    {
      minColor[1] = 1.0;
      minColor[2] = 1.0;
      maxColor[1] = 1.0;

      a -= 1.0;
    }
    else if ( a < 3.0 )
    {
      minColor[1] = 1.0;
      maxColor[0] = 1.0;
      maxColor[1] = 1.0;

      a -= 2.0;
    }
    else
    {
      minColor[0] = 1.0;
      minColor[1] = 1.0;
      maxColor[0] = 1.0;

      a -= 3.0;
    }

    b = 1.0 - a;

    for ( int i = 0; i < 3; i++ )
    {
      col[i] = (GLfloat) (b * minColor[i] + a * maxColor[i]);
    }

    col[3] = 1.0;
  }
}


//-----------------------------------------------------------------------
//   findValue
//-----------------------------------------------------------------------


double RainbowPalette::findValue

  ( double&       val,
    double        prec,
    const Color&  col ) const

{
  Color   minColor;
  Color   maxColor;

  double  eps, eps0;
  double  val0;


  // Check minColor = blue, maxColor = cyan

  minColor[2] = 1.0;
  maxColor[1] = 1.0;
  maxColor[2] = 1.0;

  eps0 = mapColorToValue ( val0, prec, col, minColor, maxColor );

  // Check minColor = cyan, maxColor = green

  minColor[1] = 1.0;
  maxColor[2] = 0.0;

  eps = mapColorToValue ( val, prec, col, minColor, maxColor );

  if ( eps < eps0 )
  {
    eps0 = eps;
    val0 = val + 1.0;
  }

  // Check minColor = green, maxColor = yellow

  minColor[2] = 0.0;
  maxColor[0] = 1.0;

  eps = mapColorToValue ( val, prec, col, minColor, maxColor );

  if ( eps < eps0 )
  {
    eps0 = eps;
    val0 = val + 2.0;
  }

  // Check minColor = yellow, maxColor = red

  minColor[0] = 1.0;
  maxColor[1] = 0.0;

  eps = mapColorToValue ( val, prec, col, minColor, maxColor );

  if ( eps < eps0 )
  {
    eps0 = eps;
    val0 = val + 3.0;
  }

  val = minValue_ + val0 * (maxValue_ - minValue_) / 4.0;

  return eps0;
}


//-----------------------------------------------------------------------
//   setRange
//-----------------------------------------------------------------------


void RainbowPalette::setRange

  ( double  lo,
    double  hi )

{
  JEM_PRECHECK ( (hi - lo) > 0.0 );

  minValue_ = lo;
  maxValue_ = hi;

  changeEvent.emit ( *this );
}


//-----------------------------------------------------------------------
//   getMinValue & getMaxValue
//-----------------------------------------------------------------------


double RainbowPalette::getMinValue () const
{
  return minValue_;
}


double RainbowPalette::getMaxValue () const
{
  return maxValue_;
}


//-----------------------------------------------------------------------
//   getMinAlpha
//-----------------------------------------------------------------------


GLfloat RainbowPalette::getMinAlpha () const
{
  return 1.0F;
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void RainbowPalette::configure ( const Properties& props )
{
  double  minVal = minValue_;
  double  maxVal = maxValue_;

  props.find ( minVal, PropNames::MIN_VALUE );
  props.find ( maxVal, PropNames::MAX_VALUE );

  if ( (maxVal - minVal) <= 0.0 )
  {
    props.propertyError ( PropNames::MAX_VALUE, "invalid range" );
  }

  minValue_ = minVal;
  maxValue_ = maxVal;
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void RainbowPalette::getConfig ( const Properties& props ) const
{
  props.set ( PropNames::MIN_VALUE, minValue_ );
  props.set ( PropNames::MAX_VALUE, maxValue_ );
}


//-----------------------------------------------------------------------
//   listProps
//-----------------------------------------------------------------------


void RainbowPalette::listProps ( const Properties& props ) const
{
  props.set ( PropNames::MIN_VALUE,
              "The lower bound of the range associated with this "
              "palette. Values smaller than or equal to the lower "
              "bound are mapped to the color blue" );
  props.set ( PropNames::MAX_VALUE,
              "The upper bound of the range associated with this "
              "palette. Values larger than or equal to the upper "
              "bound are mapped to the color red" );
}


//-----------------------------------------------------------------------
//   fillRectangle
//-----------------------------------------------------------------------


void RainbowPalette::fillRectangle

  ( GfxContext&  gtx,
    GLint        w,
    GLint        h ) const

{
  GLint  y0, y1, dy;

  dy = h / 4;

  glBegin ( GL_QUADS );
  {
    y0 = 0;
    y1 = dy;

    glColor3f  ( 0.0, 0.0, 1.0 );
    glVertex2i (   0,  y0 );
    glVertex2i (   w,  y0 );
    glColor3f  ( 0.0, 1.0, 1.0 );
    glVertex2i (   w,  y1 );
    glVertex2i (   0,  y1 );

    y0  = y1;
    y1 += dy;

    glColor3f  ( 0.0, 1.0, 1.0 );
    glVertex2i (   0,  y0 );
    glVertex2i (   w,  y0 );
    glColor3f  ( 0.0, 1.0, 0.0 );
    glVertex2i (   w,  y1 );
    glVertex2i (   0,  y1 );

    y0  = y1;
    y1 += dy;

    glColor3f  ( 0.0, 1.0, 0.0 );
    glVertex2i (   0,  y0 );
    glVertex2i (   w,  y0 );
    glColor3f  ( 1.0, 1.0, 0.0 );
    glVertex2i (   w,  y1 );
    glVertex2i (   0,  y1 );

    y0 = y1;
    y1 = h;

    glColor3f  ( 1.0, 1.0, 0.0 );
    glVertex2i (   0,  y0 );
    glVertex2i (   w,  y0 );
    glColor3f  ( 1.0, 0.0, 0.0 );
    glVertex2i (   w,  y1 );
    glVertex2i (   0,  y1 );

  }
  glEnd ();
}


JEM_END_PACKAGE( gl )
