
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


#include <cmath>
#include <cstdio>
#include <jem/base/assert.h>
#include <jem/base/ClassTemplate.h>
#include <jem/base/IllegalInputException.h>
#include <jem/base/IllegalArgumentException.h>
#include <jem/util/Properties.h>
#include <jem/gl/PropertyNames.h>
#include <jem/gl/TextPainter.h>
#include <jem/gl/CustomPalette.h>


JEM_DEFINE_CLASS( jem::gl::CustomPalette );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class CustomPalette
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const int  CustomPalette::HIDE_MIN_MAX = 1 << 0;


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


CustomPalette::CustomPalette () :

  colors_ ( 3, 1 ),
  ranges_ (    2 )

{
  colors_    = 0.5F;
  ranges_[0] = 0.0;
  ranges_[1] = 1.0;
  options_   = 0;
}


CustomPalette::CustomPalette

  ( const GLfloatMatrix&  colors,
    const Array<double>&  ranges )

{
  options_ = 0;

  setColorRanges ( colors, ranges );
}


CustomPalette::~CustomPalette ()
{}


//-----------------------------------------------------------------------
//   getColor
//-----------------------------------------------------------------------


void CustomPalette::getColor

  ( Color&  col,
    double  val ) const

{
  const bool   flat = (ranges_.size() != colors_.size(1));

  const idx_t  m    = colors_.size(0);

  idx_t        i, j, k;


  i = 0;
  j = ranges_.size () - 2;

  while ( i < j )
  {
    k = i + (j - i + 1) / 2;

    if ( val >= ranges_[k] )
    {
      i = k;
    }
    else
    {
      j = k - 1;
    }
  }

  col[3] = 1.0;

  if ( flat )
  {
    col[0] = colors_(0,j);
    col[1] = colors_(1,j);
    col[2] = colors_(2,j);

    if ( m == 4 )
    {
      col[3] = colors_(3,j);
    }
  }
  else
  {
    double  a, b;

    k = j + 1;

    if      ( val <= ranges_[j] )
    {
      a = 1.0;
    }
    else if ( val >= ranges_[k] )
    {
      a = 0.0;
    }
    else
    {
      a = (ranges_[k] - val) / (ranges_[k] - ranges_[j]);
    }

    b = 1.0 - a;

    for ( i = 0; i < m; i++ )
    {
      col[i] = (GLfloat) (a * colors_(i,j) + b * colors_(i,k));
    }
  }
}


//-----------------------------------------------------------------------
//   findValue
//-----------------------------------------------------------------------


double CustomPalette::findValue

  ( double&       val,
    double        prec,
    const Color&  col ) const

{
  const idx_t  m = colors_.size (0);
  const idx_t  n = colors_.size (1);

  double       eps, eps0;
  double       val0;
  idx_t        i, j;


  eps0 = 10.0;
  val0 = ranges_.front ();

  if ( n == ranges_.size() )
  {
    Color  minColor;
    Color  maxColor;

    for ( i = 0; i < m; i++ )
    {
      maxColor[i] = colors_(i,0);
    }

    for ( j = 1; j < n; j++ )
    {
      minColor = maxColor;

      for ( i = 0; i < m; i++ )
      {
        maxColor[i] = colors_(i,j);
      }

      eps = mapColorToValue ( val, prec, col, minColor, maxColor );

      if ( eps < eps0 )
      {
        eps0 = eps;
        val0 = ranges_[j - 1] + val * (ranges_[j] - ranges_[j - 1]);
      }
    }
  }
  else
  {
    double  err0;
    double  delta;

    err0 = 0.0;
    eps0 = eps0 * eps0;

    for ( i = m; i < 4; i++ )
    {
      delta = 1.0 - (double) col[i];
      err0 += delta * delta;
    }

    for ( j = 0; j < n; j++ )
    {
      eps = err0;

      for ( i = 0; i < m; i++ )
      {
        delta = (double) colors_(i,j) - (double) col[i];
        eps  += delta * delta;
      }

      if ( eps < eps0 )
      {
        eps0 = eps;
        val0 = ranges_[j];
      }
    }

    eps0 = std::sqrt ( eps0 );
  }

  val = val0;

  return eps0;
}


//-----------------------------------------------------------------------
//   getMinValue & getMaxValue
//-----------------------------------------------------------------------


double CustomPalette::getMinValue () const
{
  return ranges_.front ();
}


double CustomPalette::getMaxValue () const
{
  return ranges_.back ();
}


//-----------------------------------------------------------------------
//   getMinAlpha
//-----------------------------------------------------------------------


GLfloat CustomPalette::getMinAlpha () const
{
  GLfloat  minAlpha = 1.0F;

  if ( colors_.size(0) == 4 )
  {
    const idx_t  n  = colors_.size (1);

    for ( idx_t j = 0; j < n; j++ )
    {
      if ( colors_(3,j) < minAlpha )
      {
        minAlpha = colors_(3,j);
      }
    }
  }

  return minAlpha;
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void CustomPalette::configure ( const Properties& props )
{
  using util::findBool;

  Array<String>  colors;
  Array<double>  ranges;

  GLfloatMatrix  cmat;

  bool           newRanges;
  bool           newColors;

  idx_t          m, n;


  newRanges = props.find ( ranges, PropNames::RANGES );
  newColors = props.find ( colors, PropNames::COLORS );

  m = newRanges ? ranges.size () : ranges_.size ();
  n = newColors ? colors.size () : colors_.size (1);

  if ( newRanges )
  {
    if ( m <= 1 )
    {
      props.propertyError (
        PropNames::RANGES,
        "range array must contain at least two elements"
      );
    }

    if ( m < n || m > n + 1 )
    {
      props.propertyError (
        PropNames::RANGES,
        String::format (
          "range array has size %d but should have size %d or %d",
          m, n, (n + 1)
        )
      );
    }

    if ( ! checkRanges_( ranges ) )
    {
      props.propertyError (
        PropNames::RANGES,
        "range array must contain strictly ascending elements"
      );
    }
  }

  if ( newColors )
  {
    Color  c;

    if ( n < m - 1 || n > m )
    {
      props.propertyError (
        PropNames::COLORS,
        String::format (
          "color array has size %d but should have size %d or %d",
          n, (m - 1), m
        )
      );
    }

    cmat.resize ( 4, n );

    try
    {
      for ( idx_t j = 0; j < n; j++ )
      {
        c = Color::parse ( colors[j] );

        for ( idx_t i = 0; i < 4; i++ )
        {
          cmat(i,j) = c[i];
        }
      }
    }
    catch ( const IllegalInputException& ex )
    {
      props.propertyError ( PropNames::COLORS, ex.what() );
    }
  }

  // No errors, so commit the changes

  if ( newRanges )
  {
    ranges_.ref ( ranges );
  }

  if ( newColors )
  {
    colors_.ref ( cmat );
  }

  findBool ( options_,  HIDE_MIN_MAX, props,
             PropNames::HIDE_MIN_MAX );
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void CustomPalette::getConfig ( const Properties& props ) const
{
  using util::setBool;

  const idx_t    m = colors_.size (0);
  const idx_t    n = colors_.size (1);

  Array<String>  colors ( n );

  Color          c;

  idx_t          i, j;


  c[3] = 1.0;

  for ( j = 0; j < n; j++ )
  {
    for ( i = 0; i < m; i++ )
    {
      c[i] = colors_(i,j);
    }

    colors[j] = c.toString ();
  }

  props.set ( PropNames::COLORS, colors  );
  props.set ( PropNames::RANGES, ranges_ );

  setBool   ( props,
              PropNames::HIDE_MIN_MAX,
              options_,  HIDE_MIN_MAX );
}


//-----------------------------------------------------------------------
//   listProps
//-----------------------------------------------------------------------


void CustomPalette::listProps ( const Properties& props ) const
{
  props.set ( PropNames::COLORS,
              "An array with colors" );
  props.set ( PropNames::RANGES,
              "An array specifying the range for each color" );
  props.set ( PropNames::HIDE_MIN_MAX,
              "Whether the minimum and maximum values "
              "should be hidden" );
}


//-----------------------------------------------------------------------
//   setColorRanges
//-----------------------------------------------------------------------


void CustomPalette::setColorRanges

  ( const GLfloatMatrix&  colors,
    const Array<double>&  ranges )

{
  JEM_PRECHECK ( colors.size(0) == 3 || colors.size(0) == 4 );

  const idx_t  m = ranges.size ();
  const idx_t  n = colors.size (1);

  if ( m <= 1 )
  {
    throw IllegalArgumentException (
      JEM_FUNC,
      "range array must contain at least two elements"
    );
  }

  if ( ! checkRanges_( ranges ) )
  {
    throw IllegalInputException (
      JEM_FUNC,
      "range array must contain strictly ascending elements"
    );
  }

  if ( n < m - 1 || n > m )
  {
    throw IllegalArgumentException (
      JEM_FUNC,
      String::format (
        "color array has size %d but should have size %d or %d",
        n, (m - 1), m
      )
    );
  }

  colors_.ref ( colors );
  ranges_.ref ( ranges );
}


//-----------------------------------------------------------------------
//   setOptions
//-----------------------------------------------------------------------


void CustomPalette::setOptions ( int options )
{
  options_ = options;
}


//-----------------------------------------------------------------------
//   getOptions
//-----------------------------------------------------------------------


int CustomPalette::getOptions () const
{
  return options_;
}


//-----------------------------------------------------------------------
//   fillRectangle
//-----------------------------------------------------------------------


void CustomPalette::fillRectangle

  ( GfxContext&  gtx,
    GLint        w,
    GLint        h ) const

{
  const bool    flat   = (ranges_.size() != colors_.size(1));

  const idx_t   m      = colors_.size(0);
  const idx_t   n      = ranges_.size() - 1;

  const double  minVal =      ranges_[0];
  const double  scale  = h / (ranges_[n] - minVal);

  GLfloat       r, g, b;
  GLint         y0, y1;


  y0 = 0;

  glBegin ( GL_QUADS );

  if ( flat )
  {
    for ( idx_t j = 0; j < n; j++ )
    {
      if ( j + 1 == n )
      {
        y1 = h;
      }
      else
      {
        y1 = (GLint) (scale * (ranges_[j + 1] - minVal));
      }

      r = colors_(0,j);
      g = colors_(1,j);
      b = colors_(2,j);

      if ( m == 4 )
      {
        glColor4f ( r, g, b, colors_(3,j) );
      }
      else
      {
        glColor3f ( r, g, b );
      }

      glVertex2i  ( 0, y0 );
      glVertex2i  ( w, y0 );
      glVertex2i  ( w, y1 );
      glVertex2i  ( 0, y1 );

      y0 = y1;
    }
  }
  else
  {
    r = colors_(0,0);
    g = colors_(1,0);
    b = colors_(2,0);

    if ( m == 4 )
    {
      glColor4f ( r, g, b, colors_(3,0) );
    }
    else
    {
      glColor3f ( r, g, b );
    }

    for ( idx_t j = 1; j <= n; j++ )
    {
      if ( j == n )
      {
        y1 = h;
      }
      else
      {
        y1 = (GLint) (scale * (ranges_[j] - minVal));
      }

      glVertex2i ( 0, y0 );
      glVertex2i ( w, y0 );

      r = colors_(0,j);
      g = colors_(1,j);
      b = colors_(2,j);

      if ( m == 4 )
      {
        glColor4f ( r, g, b, colors_(3,j) );
      }
      else
      {
        glColor3f ( r, g, b );
      }

      glVertex2i  ( w, y1 );
      glVertex2i  ( 0, y1 );

      y0 = y1;
    }
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   drawTickMarks
//-----------------------------------------------------------------------


void CustomPalette::drawTickMarks

  ( GfxContext&   gtx,
    TextPainter&  txp,
    GLint         w,
    GLint         h,
    GLint         xtick,
    Alignment     hAlign,
    double        scale,
    double        bias ) const

{
  const idx_t   n      = ranges_.size() - 1;

  const double  minVal =      ranges_[0];
  const double  hScale = h / (ranges_[n] - minVal);

  char          buf[32];
  GLint         minSpace;
  GLint         y, y0;


  minSpace = 2 * txp.font.getHeight ();

  glBegin ( GL_LINES );

  for ( idx_t j = 1; j < n; j++ )
  {
    y = (GLint) (hScale * (ranges_[j] - minVal));

    glVertex2i ( 0, y );
    glVertex2i ( w, y );
  }

  glEnd ();

  {
    TextScope  textScope ( gtx, txp );

    int        k;

    if ( ! (options_ & HIDE_MIN_MAX) )
    {
      k = std::sprintf ( buf, "%+.2e", (bias + scale * minVal) );

      txp.setCursor ( xtick, 0 );
      txp.writeLine ( buf,   k );
    }

    y0 = 0;

    for ( idx_t j = 1; j < n; j++ )
    {
      y = (GLint) (hScale * (ranges_[j] - minVal));

      if ( y - y0 < minSpace || h - y < minSpace )
      {
        continue;
      }

      k = std::sprintf ( buf, "%+.2e", (bias + scale * ranges_[j]) );

      txp.setCursor ( xtick, y );
      txp.writeLine ( buf,   k );

      y0 = y;
    }

    if ( ! (options_ & HIDE_MIN_MAX) )
    {
      k = std::sprintf ( buf, "%+.2e", (bias + scale * ranges_[n]) );

      txp.setCursor ( xtick, h );
      txp.writeLine ( buf,   k );
    }
  }
}


//-----------------------------------------------------------------------
//   checkRanges_
//-----------------------------------------------------------------------


bool CustomPalette::checkRanges_ ( const Array<double>& r )
{
  const idx_t  n = r.size() - 1;

  for ( idx_t j = 0; j < n; j++ )
  {
    if ( r[j] >= r[j + 1] )
    {
      return false;
    }
  }

  return true;
}


JEM_END_PACKAGE( gl )
