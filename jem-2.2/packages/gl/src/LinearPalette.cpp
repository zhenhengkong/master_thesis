
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
#include <jem/gl/LinearPalette.h>


JEM_DEFINE_CLASS( jem::gl::LinearPalette );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class LinearPalette
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


LinearPalette::LinearPalette ()
{
  minColor_  = Color::blue ();
  maxColor_  = Color::red  ();
  minValue_  = 0.0;
  maxValue_  = 1.0;
}


LinearPalette::~LinearPalette ()
{}


//-----------------------------------------------------------------------
//   getColor
//-----------------------------------------------------------------------


void LinearPalette::getColor

  ( Color&  col,
    double  val ) const

{
  if      ( val <= minValue_ )
  {
    col = minColor_;
  }
  else if ( val >= maxValue_ )
  {
    col = maxColor_;
  }
  else
  {
    const double  a = (maxValue_ - val) / (maxValue_ - minValue_);
    const double  b = 1.0 - a;

    for ( int i = 0; i < 4; i++ )
    {
      col[i] = (GLfloat) (a * minColor_[i] + b * maxColor_[i]);
    }
  }
}


//-----------------------------------------------------------------------
//   findValue
//-----------------------------------------------------------------------


double LinearPalette::findValue

  ( double&       val,
    double        prec,
    const Color&  col ) const

{
  double  eps =

    mapColorToValue ( val, prec, col, minColor_, maxColor_ );

  val = minValue_ + val * (maxValue_ - minValue_);

  return eps;
}


//-----------------------------------------------------------------------
//   setRange
//-----------------------------------------------------------------------


void LinearPalette::setRange

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


double LinearPalette::getMinValue () const
{
  return minValue_;
}


double LinearPalette::getMaxValue () const
{
  return maxValue_;
}


//-----------------------------------------------------------------------
//   getMinAlpha
//-----------------------------------------------------------------------


GLfloat LinearPalette::getMinAlpha () const
{
  if ( minColor_[3] < maxColor_[3] )
  {
    return minColor_[3];
  }
  else
  {
    return maxColor_[3];
  }
}


//-----------------------------------------------------------------------
//   setColors
//-----------------------------------------------------------------------


void LinearPalette::setColors

  ( const Color&  lo,
    const Color&  hi )

{
  minColor_ = clamped ( lo );
  maxColor_ = clamped ( hi );

  changeEvent.emit ( *this );
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void LinearPalette::configure ( const Properties& props )
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

  minColor_.configure ( PropNames::MIN_COLOR, props );
  maxColor_.configure ( PropNames::MAX_COLOR, props );
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void LinearPalette::getConfig ( const Properties& conf ) const
{
  conf.set ( PropNames::MIN_VALUE, minValue_ );
  conf.set ( PropNames::MAX_VALUE, maxValue_ );

  minColor_.getConfig ( PropNames::MIN_COLOR, conf );
  maxColor_.getConfig ( PropNames::MAX_COLOR, conf );
}


//-----------------------------------------------------------------------
//   listProps
//-----------------------------------------------------------------------


void LinearPalette::listProps ( const Properties& conf ) const
{
  conf.set ( PropNames::MIN_VALUE,
             "The lower bound of the range associated with this "
             "palette. Values smaller than or equal to the lower "
             "bound are mapped to the minimum color" );
  conf.set ( PropNames::MAX_VALUE,
             "The upper bound of the range associated with this "
             "palette. Values larger than or equal to the upper "
             "bound are mapped to the maximum color" );
  conf.set ( PropNames::MIN_COLOR,
             "The color associated with the lower bound of "
             "the range" );
  conf.set ( PropNames::MAX_COLOR,
             "The color associated with the upper bound of "
             "the range" );
}


//-----------------------------------------------------------------------
//   fillRectangle
//-----------------------------------------------------------------------


void LinearPalette::fillRectangle

  ( GfxContext&  gtx,
    GLint        w,
    GLint        h ) const

{
  glBegin ( GL_QUADS );
  {
    setCurrent ( minColor_ );

    glVertex2i ( 0, 0 );
    glVertex2i ( w, 0 );

    setCurrent ( maxColor_ );

    glVertex2i ( w, h );
    glVertex2i ( 0, h );
  }
  glEnd ();
}


JEM_END_PACKAGE( gl )
