
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
#include <jem/base/rtti.h>
#include <jem/base/assert.h>
#include <jem/base/limits.h>
#include <jem/base/Float.h>
#include <jem/base/ClassTemplate.h>
#include <jem/base/IllegalInputException.h>
#include <jem/base/array/utilities.h>
#include <jem/util/None.h>
#include <jem/util/Flex.h>
#include <jem/util/Properties.h>
#include <jem/gl/utilities.h>
#include <jem/gl/Box.h>
#include <jem/gl/Menu.h>
#include <jem/gl/Rect.h>
#include <jem/gl/Symbols.h>
#include <jem/gl/Commands.h>
#include <jem/gl/PropertyNames.h>
#include <jem/gl/Transform.h>
#include <jem/gl/TextProjector.h>
#include <jem/gl/NumberMarker.h>
#include <jem/gl/SymbolMarker.h>
#include <jem/gl/XYPlot.h>


JEM_DEFINE_CLASS( jem::gl::XYPlot );


JEM_BEGIN_PACKAGE( gl )


using util::Flex;


//=======================================================================
//   class XYPlot::DataSet
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


XYPlot::DataSet::DataSet ()
{
  lnWidth_    = 0.0;
  maxPoints_  = maxOf ( maxPoints_ );
  baseNumber_ = JEM_IDX_C(0);
}


XYPlot::DataSet::~DataSet ()
{}


//-----------------------------------------------------------------------
//   clear
//-----------------------------------------------------------------------


void XYPlot::DataSet::clear ()
{
  Array<double>  empty;

  setPoints ( empty, empty );
}


//-----------------------------------------------------------------------
//   setStyle
//-----------------------------------------------------------------------


void XYPlot::DataSet::setStyle

  ( GLfloat                lwidth,
    const Color&           lcolor,
    const Ref<PntStyle>&   pstyle )

{
  lnWidth_ = clamped ( lwidth, (GLfloat) 64.0 );
  lnColor_ = clamped ( lcolor );
  ptStyle_ = pstyle;
}


//-----------------------------------------------------------------------
//   setLineWidth
//-----------------------------------------------------------------------


void XYPlot::DataSet::setLineWidth ( GLfloat width )
{
  lnWidth_ = clamped ( width, (GLfloat) 64.0 );
}


//-----------------------------------------------------------------------
//   setLineColor
//-----------------------------------------------------------------------


void XYPlot::DataSet::setLineColor ( const Color& col )
{
  lnColor_ = clamped ( col );

  ColoredMarker*  cm = dynamicCast<ColoredMarker*> ( ptStyle_ );

  if ( cm )
  {
    cm->setColor ( lnColor_ );
  }
}


//-----------------------------------------------------------------------
//   setPointStyle
//-----------------------------------------------------------------------


void XYPlot::DataSet::setPointStyle ( const Ref<PntStyle>& style )
{
  ptStyle_ = style;
}


//-----------------------------------------------------------------------
//   setBaseNumber
//-----------------------------------------------------------------------


void XYPlot::DataSet::setBaseNumber ( idx_t nr0 )
{
  baseNumber_ = nr0;
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void XYPlot::DataSet::configure ( const Properties& props )
{
  String   ps;
  Color    lc;
  double   lw;
  idx_t    mp;
  idx_t    nr0;


  props.find ( key, PropNames::KEY );

  if ( props.find( mp, PropNames::MAX_POINTS ) )
  {
    setMaxPoints ( mp );
  }

  if ( props.find( ps, PropNames::POINT_STYLE ) )
  {
    Ref<PntStyle>  style;

    if ( ps.size() )
    {
      if ( ps.equalsIgnoreCase( "number" ) )
      {
        Ref<NumberMarker>  nm =

          newInstance<NumberMarker> ( lnColor_ );

        nm->writer.hAlignment = CENTERED;
        nm->writer.vAlignment = CENTERED;

        style = nm;
      }
      else
      {
        char  symbol = Symbols::byName ( ps.toLower() );

        if ( ! symbol )
        {
          String  choice = Symbols::getChoice ();

          props.propertyError (
            PropNames::POINT_STYLE,
            String::format (
              "invalid point style: %s; should be %s",
              ps,
              choice
            )
          );
        }

        style = newInstance<SymbolMarker> ( symbol, lnColor_ );
      }
    }

    setPointStyle ( style );
  }

  if ( props.find( lw, PropNames::LINE_WIDTH, 0.0, 64.0 ) )
  {
    setLineWidth ( (GLfloat) lw );
  }

  if ( lc.configure( PropNames::LINE_COLOR, props ) )
  {
    setLineColor ( lc );
  }

  if ( dynamicCast<NumberMarker*>( ptStyle_ ) )
  {
    if ( props.find( nr0, PropNames::BASE_NUMBER ) )
    {
      setBaseNumber ( nr0 );
    }
  }
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void XYPlot::DataSet::getConfig ( const Properties& conf )  const
{
  using jem::util::None;

  String  ps;
  idx_t   mp;


  mp = maxPoints_;

  if ( mp == maxOf( mp ) )
  {
    mp = -1;
  }

  conf.set ( PropNames::KEY,        key );
  conf.set ( PropNames::MAX_POINTS, mp  );

  if ( ptStyle_ == NIL )
  {
    conf.set ( PropNames::POINT_STYLE, None::getInstance() );
  }
  else
  {
    conf.set ( PropNames::POINT_STYLE, ptStyle_->toString() );
  }

  conf.set ( PropNames::LINE_WIDTH, (double) lnWidth_ );

  lnColor_.getConfig ( PropNames::LINE_COLOR, conf );

  if ( dynamicCast<NumberMarker*>( ptStyle_ ) )
  {
    conf.set ( PropNames::BASE_NUMBER, baseNumber_ );
  }
}


//-----------------------------------------------------------------------
//   listProps
//-----------------------------------------------------------------------


void XYPlot::DataSet::listProps ( const Properties& conf )  const
{
  conf.set ( PropNames::KEY,
             "The key (title)" );
  conf.set ( PropNames::MAX_POINTS,
             "The maximum number of points" );
  conf.set ( PropNames::POINT_STYLE,
             "The point style" );
  conf.set ( PropNames::LINE_WIDTH,
             "The line width (in pixels)" );
  conf.set ( PropNames::LINE_COLOR,
             "The line color" );

  if ( dynamicCast<NumberMarker*>( ptStyle_ ) )
  {
    conf.set ( PropNames::BASE_NUMBER,
               "The starting number" );
  }
}


//=======================================================================
//   class XYPlot::DataSet_
//=======================================================================


class XYPlot::DataSet_ : public Object, public DataSet
{
 public:

  explicit inline           DataSet_

    ( XYPlot*                 plot );

  virtual void              setPoints

    ( const Array<double>&    x,
      const Array<double>&    y );

  virtual void              addPoint

    ( double                  x,
      double                  y );

  virtual void              addPoints

    ( const Array<double>&    x,
      const Array<double>&    y );

  virtual idx_t              pointCount      () const;

  virtual void              setMaxPoints

    ( idx_t                    count );

  inline Range              getXRange       () const;

  Range                     getXRange

    ( const Range&            yRange )         const;

  inline Range              getYRange       () const;

  Range                     getYRange

    ( const Range&            xRange )         const;

  void                      sieve

    ( const Range&            xRange,
      const Range&            yRange );

  void                      drawFast

    ( const double            scale[2],
      const double            bias [2],
      double                  lod )            const;

  void                      drawLines

    ( const double            scale[2],
      const double            bias [2] )       const;

  void                      drawMarkers

    ( GfxContext&             gtx,
      const double            scale[2],
      const double            bias [2],
      const Box&              bbox )           const;


 private:

  void                      update_         ();

  Range                     getRange_

    ( const Range&            vRange,
      const Array<double>&    uData,
      const Array<double>&    vData )          const;


 private:

  XYPlot*                   plot_;
  idx_t                     size_;
  idx_t                     first_;
  idx_t                     last_;
  Array<double>             xData_;
  Array<double>             yData_;
  Range                     xRange_;
  Range                     yRange_;
  Flex<idx_t>               pointList_;

};


//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


inline XYPlot::DataSet_::DataSet_ ( XYPlot* p ) :

  plot_ ( p )

{
  size_   = JEM_IDX_C(0);
  first_  = JEM_IDX_C(0);
  last_   = JEM_IDX_C(0);
  xRange_ = 0.0;
  yRange_ = 0.0;
}


//-----------------------------------------------------------------------
//   setPoints
//-----------------------------------------------------------------------


void XYPlot::DataSet_::setPoints

  ( const Array<double>&  x,
    const Array<double>&  y )

{
  JEM_PRECHECK ( x.size() == y.size() );

  if ( Float::isNaN( sum( x ) + sum( y ) ) )
  {
    throw IllegalInputException (
      plot_->getContext (),
      "invalid input data: NaN"
    );
  }

  xData_.ref ( makeContiguous( x ) );
  yData_.ref ( makeContiguous( y ) );

  size_ = x.size ();

  if ( size_ > maxPoints_ )
  {
    maxPoints_ = size_;
  }

  update_ ();
}


//-----------------------------------------------------------------------
//   addPoint
//-----------------------------------------------------------------------


void XYPlot::DataSet_::addPoint

  ( double  x,
    double  y )

{
  if ( Float::isNaN( x + y ) )
  {
    throw IllegalInputException (
      plot_->getContext (),
      "invalid input data: NaN"
    );
  }

  if ( size_ == xData_.size() )
  {
    idx_t  n = size_ + size_ / 2 + 8;

    if ( n > maxPoints_ )
    {
      n = maxPoints_;
    }

    xData_.reshape ( n );
    yData_.reshape ( n );
  }

  if ( size_ == maxPoints_ )
  {
    shift ( xData_, -1 );
    shift ( yData_, -1 );

    size_--;
    baseNumber_++;

    update_ ();
  }

  if ( size_ == 0 )
  {
    xRange_ = x;
    yRange_ = y;

    plot_->updated_ = false;
  }
  else
  {
    if      ( x < xRange_[0] )
    {
      xRange_[0] = x;
    }
    else if ( x > xRange_[1] )
    {
      xRange_[1] = x;
    }

    if      ( y < yRange_[0] )
    {
      yRange_[0] = y;
    }
    else if ( y > yRange_[1] )
    {
      yRange_[1] = y;
    }

    if ( plot_->updated_        &&
         x >= plot_->xRange_[0] &&
         x <= plot_->xRange_[1] &&
         y >= plot_->yRange_[0] &&
         y <= plot_->yRange_[1] )
    {
      pointList_.pushBack ( size_ );
    }
    else
    {
      plot_->updated_ = false;
    }
  }

  xData_[size_] = x;
  yData_[size_] = y;

  size_++;

  last_ = size_;
}


//-----------------------------------------------------------------------
//   addPoints
//-----------------------------------------------------------------------


void XYPlot::DataSet_::addPoints

  ( const Array<double>&  x,
    const Array<double>&  y )

{
  JEM_PRECHECK ( x.size() == y.size() );

  if ( Float::isNaN( sum( x ) + sum( y ) ) )
  {
    throw IllegalInputException (
      plot_->getContext (),
      "invalid input data: NaN"
    );
  }

  const idx_t  n = x.size ();

  double       u, v;
  idx_t        k;


  if ( n >= maxPoints_ )
  {
    k = n - maxPoints_;

    setPoints ( x[slice(k,END)], y[slice(k,END)] );

    return;
  }

  if ( size_ + n > xData_.size() )
  {
    k = size_ + n + xData_.size() / 2 + 8;

    if ( k > maxPoints_ )
    {
      k = maxPoints_;
    }

    xData_.reshape ( k );
    yData_.reshape ( k );
  }

  if ( size_ + n > maxPoints_ )
  {
    k = size_ + n - maxPoints_;

    shift ( xData_, -k );
    shift ( yData_, -k );

    size_       -= k;
    baseNumber_ += k;

    update_ ();
  }

  if ( size_ == 0 && n > 0 )
  {
    xRange_ = x[0];
    yRange_ = y[0];

    plot_->updated_ = false;
  }

  for ( idx_t i = 0; i < n; i++ )
  {
    u = x[i];
    v = y[i];

    if      ( u < xRange_[0] )
    {
      xRange_[0] = u;
    }
    else if ( u > xRange_[1] )
    {
      xRange_[1] = u;
    }

    if      ( v < yRange_[0] )
    {
      yRange_[0] = v;
    }
    else if ( v > yRange_[1] )
    {
      yRange_[1] = v;
    }

    if ( plot_->updated_        &&
         u >= plot_->xRange_[0] &&
         u <= plot_->xRange_[1] &&
         v >= plot_->yRange_[0] &&
         v <= plot_->yRange_[1] )
    {
      pointList_.pushBack ( size_ );
    }
    else
    {
      plot_->updated_ = false;
    }

    xData_[size_] = u;
    yData_[size_] = v;

    size_++;
  }

  last_ = size_;
}


//-----------------------------------------------------------------------
//   pointCount
//-----------------------------------------------------------------------


idx_t XYPlot::DataSet_::pointCount () const
{
  return size_;
}


//-----------------------------------------------------------------------
//   setMaxPoints
//-----------------------------------------------------------------------


void XYPlot::DataSet_::setMaxPoints ( idx_t count )
{
  if      ( count <= 0 )
  {
    count = maxOf ( count );
  }
  else if ( count < size_ )
  {
    size_ = count;

    update_ ();
  }

  maxPoints_ = count;
}


//-----------------------------------------------------------------------
//   getXRange
//-----------------------------------------------------------------------


inline XYPlot::Range XYPlot::DataSet_::getXRange () const
{
  return xRange_;
}


XYPlot::Range XYPlot::DataSet_::getXRange

  ( const Range&  yRange ) const

{
  if ( this->yRange_[0] >= yRange[0] &&
       this->yRange_[1] <= yRange[1] )
  {
    return this->xRange_;
  }
  else
  {
    return getRange_ ( yRange, xData_, yData_ );
  }
}


//-----------------------------------------------------------------------
//   getYRange
//-----------------------------------------------------------------------


inline XYPlot::Range XYPlot::DataSet_::getYRange () const
{
  return yRange_;
}


XYPlot::Range XYPlot::DataSet_::getYRange

  ( const Range&  xRange ) const

{
  if ( this->xRange_[0] >= xRange[0] &&
       this->xRange_[1] <= xRange[1] )
  {
    return this->yRange_;
  }
  else
  {
    return getRange_ ( xRange, yData_, xData_ );
  }
}


//-----------------------------------------------------------------------
//   sieve
//-----------------------------------------------------------------------


void XYPlot::DataSet_::sieve

  ( const Range&  xr,
    const Range&  yr )

{
  using jem::min;
  using jem::max;

  const double*  xp = xData_.addr ();
  const double*  yp = yData_.addr ();

  const idx_t    np = size_;

  double         t;
  idx_t          i, j;


  pointList_.resize ( np );

  // Check for special case: all points are within the specified
  // ranges.

  if ( xr[0] <= xRange_[0] &&
       xr[1] >= xRange_[1] &&
       yr[0] <= yRange_[0] &&
       yr[1] >= yRange_[1] )
  {
    for ( i = 0; i < np; i++ )
    {
      pointList_[i] = i;
    }

    first_ = 0;
    last_  = size_;

    return;
  }

  // Find all points within the specified ranges.

  for ( i = j = 0; i < np; i++ )
  {
    t = xp[i];

    if ( t < xr[0] || t > xr[1] )
    {
      continue;
    }

    t = yp[i];

    if ( t < yr[0] || t > yr[1] )
    {
      continue;
    }

    pointList_[j++] = i;
  }

  pointList_.resize ( j );

  // Find the first line segment within the x-range.

  for ( i = 0; i < np; i++ )
  {
    if ( xp[i] >= xr[0] )
    {
      break;
    }
  }

  first_ = max ( JEM_IDX_C(0), i - 1 );

  // Find the last line segment within the x-range.

  for ( i = np - 1; i >= 0; i-- )
  {
    if ( xp[i] <= xr[1] )
    {
      break;
    }
  }

  last_ = min ( np, i + 2 );
}


//-----------------------------------------------------------------------
//   drawFast
//-----------------------------------------------------------------------


void XYPlot::DataSet_::drawFast

  ( const double  scale[2],
    const double  bias [2],
    double        lod ) const

{
  const idx_t*  plist = pointList_.addr ();
  const idx_t   np    = pointList_.size ();

  const double  st    = 1.0 / lod;

  GLfloat       x, y;
  idx_t         i, j, k;


  setCurrent ( lnColor_  );
  glBegin    ( GL_POINTS );

  for ( i = j = 0; j < np; i++, j = (idx_t) ((double) i * st) )
  {
    k = plist[j];
    x = (GLfloat) (bias[0] + scale[0] * xData_[k]);
    y = (GLfloat) (bias[1] + scale[1] * yData_[k]);

    glVertex2f ( x, y );
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   drawLines
//-----------------------------------------------------------------------


void XYPlot::DataSet_::drawLines

  ( const double  scale[2],
    const double  bias [2] ) const

{
  if ( (last_ - first_) > 1 && lnWidth_ > 0.1 )
  {
    DisableScope   blendScope ( GL_BLEND, lnColor_.isOpaque() );

    const double*  xp = xData_.addr ();
    const double*  yp = yData_.addr ();

    GLfloat        x, y;


    // Draw all selected line segments; the clipping planes will take
    // care of points outside the plot frame.

    setCurrent  ( lnColor_ );
    glLineWidth ( lnWidth_ );
    glBegin     ( GL_LINE_STRIP );

    for ( idx_t i = first_; i < last_; i++ )
    {
      x = (GLfloat) (bias[0] + scale[0] * xp[i]);
      y = (GLfloat) (bias[1] + scale[1] * yp[i]);

      glVertex2f ( x, y );
    }

    glEnd ();
  }
}


//-----------------------------------------------------------------------
//   drawMarkers
//-----------------------------------------------------------------------


void XYPlot::DataSet_::drawMarkers

  ( GfxContext&   gtx,
    const double  scale[2],
    const double  bias [2],
    const Box&    bbox )     const

{
  if ( ptStyle_ != NIL )
  {
    const double*  xp    = xData_    .addr ();
    const double*  yp    = yData_    .addr ();
    const idx_t*   plist = pointList_.addr ();
    const idx_t    np    = pointList_.size ();
    const idx_t    nr0   = baseNumber_;

    GLfloat        pos[3];


    pos[2] = 0.0;

    ptStyle_->beginDraw ( gtx, bbox, np );

    for ( idx_t i = 0; i < np; i++ )
    {
      idx_t j =  plist[i];
      pos[0] = (GLfloat) (bias[0] + scale[0] * xp[j]);
      pos[1] = (GLfloat) (bias[1] + scale[1] * yp[j]);

      ptStyle_->drawMarker ( nr0 + j, pos );
    }

    ptStyle_->endDraw ();
  }
}


//-----------------------------------------------------------------------
//   update_
//-----------------------------------------------------------------------


void XYPlot::DataSet_::update_ ()
{
  const double*  xp = xData_.addr ();
  const double*  yp = yData_.addr ();

  const idx_t    np = size_;


  if ( np == 0 )
  {
    xRange_ = yRange_ = Range ( 0.0, 1.0 );
  }
  else
  {
    double  xmin = xp[0];
    double  xmax = xp[0];
    double  ymin = yp[0];
    double  ymax = yp[0];

    double  t;


    for ( idx_t i = 1; i < np; i++ )
    {
      t = xp[i];

      if      ( t < xmin )
      {
        xmin = t;
      }
      else if ( t > xmax )
      {
        xmax = t;
      }

      t = yp[i];

      if      ( t < ymin )
      {
        ymin = t;
      }
      else if ( t > ymax )
      {
        ymax = t;
      }
    }

    xRange_[0] = xmin;
    xRange_[1] = xmax;
    yRange_[0] = ymin;
    yRange_[1] = ymax;
  }

  pointList_.resize ( 0 );

  first_ = 0;
  last_  = np;

  plot_->updated_ = false;
}


//-----------------------------------------------------------------------
//   getRange_
//-----------------------------------------------------------------------


XYPlot::Range XYPlot::DataSet_::getRange_

  ( const Range&          vRange,
    const Array<double>&  uData,
    const Array<double>&  vData ) const

{
  const double*  up    = uData.addr ();
  const double*  vp    = vData.addr ();

  const idx_t    np    = size_;

  const double   vmin  = vRange[0];
  const double   vmax  = vRange[1];

  double         umin  = 0.0;
  double         umax  = 1.0;
  bool           empty = true;
  double         u, v;


  for ( idx_t i = 0; i < np; i++ )
  {
    v = vp[i];

    if ( v < vmin || v > vmax )
    {
      continue;
    }

    u = up[i];

    if      ( empty )
    {
      umin  = umax = u;
      empty = false;
    }
    else if ( u < umin )
    {
      umin  = u;
    }
    else if ( u > umax )
    {
      umax  = u;
    }
  }

  return Range ( umin, umax );
}


//=======================================================================
//   class XYPlot
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const int     XYPlot::SHOW_KEY        =  1 << 0;
const int     XYPlot::SHOW_XTICKS     =  1 << 1;
const int     XYPlot::SHOW_YTICKS     =  1 << 2;

const int     XYPlot::SHOW_TICKS      = (SHOW_XTICKS |
                                         SHOW_YTICKS);

const int     XYPlot::DEFAULT_OPTIONS = (SHOW_KEY    |
                                         SHOW_TICKS);

const int     XYPlot::LINES           =  1 << 0;
const int     XYPlot::POINTS          =  1 << 1;

const int     XYPlot::LINES_POINTS    = (LINES | POINTS);

const char*   XYPlot::TICK_FORMAT_    = "%.3g";


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


XYPlot::XYPlot ( const String& name ) :

  Super ( name )

{
  init_ ( Size2f( 1.0F ) );
}


XYPlot::XYPlot

  ( const String&  name,
    const Size2f&  sz ) :

    Super ( name )

{
  init_ ( sz );
}


XYPlot::~XYPlot ()
{}


//-----------------------------------------------------------------------
//   getBBox
//-----------------------------------------------------------------------


void XYPlot::getBBox ( Box& box ) const
{
  GLfloat  a[3];
  GLfloat  b[3];

  a[0] =  bbox_[0];
  a[1] =  bbox_[1];
  a[2] = -depth_;

  b[0] =  bbox_[2];
  b[1] =  bbox_[3];
  b[2] =  depth_;

  box.setCorners3D ( a, b );
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void XYPlot::configure ( const Properties& props )
{
  if ( ! props.contains( myName_ ) )
  {
    return;
  }

  Properties  myProps = props.findProps ( myName_ );

  bool        newWidth;
  bool        newHeight;
  Vertex2f    keyPos;
  double      w, h;
  bool        choice;


  Super::configure ( props );

  w = (double) width_;
  h = (double) height_;

  newWidth  = myProps.find ( w, PropNames::WIDTH,  1.0e-10, 1.0e10 );
  newHeight = myProps.find ( h, PropNames::HEIGHT, 1.0e-10, 1.0e10 );

  if ( newWidth || newHeight )
  {
    resize ( Size2f( (GLfloat) w, (GLfloat) h ) );
  }

  keyPos = 0.0F;

  if ( Vertex::configure( keyPos, PropNames::KEY_POS, myProps ) )
  {
    setKeyPos ( keyPos[0], keyPos[1] );
  }

  if ( myProps.find( choice, PropNames::SHOW_KEY ) )
  {
    setOption ( SHOW_KEY, choice );
  }

  myProps.find ( title,      PropNames::TITLE  );
  myProps.find ( xLabel,     PropNames::XLABEL );
  myProps.find ( yLabel,     PropNames::YLABEL );
  myProps.find ( scaleStep_, PropNames::SCALE_STEP, 0.0, 100.0 );

  setRanges_   ( myProps );
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void XYPlot::getConfig ( const Properties& props ) const
{
  Properties  myProps = props.makeProps ( myName_ );

  Vertex2f    keyPos  ( keyPos_[0], keyPos_[1] );

  bool        showKey = (bool) (options_ & SHOW_KEY);

  double      width   = width_;
  double      height  = height_;


  Super::getConfig  ( props );

  myProps.set ( PropNames::WIDTH,      width   );
  myProps.set ( PropNames::HEIGHT,     height  );
  myProps.set ( PropNames::SHOW_KEY,   showKey );

  Vertex::getConfig ( PropNames::KEY_POS, myProps, keyPos );

  myProps.set ( PropNames::TITLE,      title      );
  myProps.set ( PropNames::XLABEL,     xLabel     );
  myProps.set ( PropNames::YLABEL,     yLabel     );
  myProps.set ( PropNames::SCALE_STEP, scaleStep_ );

  if ( xAutoScale_ )
  {
    myProps.set ( PropNames::XRANGE, "auto" );
  }
  else
  {
    Array<double>  r ( 2 );

    r[0] = xRange_[0];
    r[1] = xRange_[1];

    myProps.set ( PropNames::XRANGE, r );
  }

  if ( yAutoScale_ )
  {
    myProps.set ( PropNames::YRANGE, "auto" );
  }
  else
  {
    Array<double>  r ( 2 );

    r[0] = yRange_[0];
    r[1] = yRange_[1];

    myProps.set ( PropNames::YRANGE, r );
  }
}


//-----------------------------------------------------------------------
//   listProps
//-----------------------------------------------------------------------


void XYPlot::listProps ( const Properties& props ) const
{
  Properties  myProps = props.makeProps ( myName_ );

  Super::listProps ( props );

  myProps.set ( PropNames::WIDTH,
                "The width of the plot" );
  myProps.set ( PropNames::HEIGHT,
                "The height of the plot" );
  myProps.set ( PropNames::SHOW_KEY,
                "Whether to show the plot key(s)" );
  myProps.set ( PropNames::KEY_POS,
                "The relative position of the plot key(s)" );
  myProps.set ( PropNames::TITLE,
                "The title of the plot" );
  myProps.set ( PropNames::XLABEL,
                "The label below the x-axis" );
  myProps.set ( PropNames::YLABEL,
                "The label left of the y-axis" );
  myProps.set ( PropNames::SCALE_STEP,
                "The relative rounding (on a log scale) of the "
                "range of the axes" );
  myProps.set ( PropNames::XRANGE,
                "The range of the x-axis" );
  myProps.set ( PropNames::YRANGE,
                "The range of the y-axis" );
}


//-----------------------------------------------------------------------
//   redraw
//-----------------------------------------------------------------------


void XYPlot::redraw ( GfxContext& gtx )
{
  DisableScope  lightScope ( GL_LIGHTING );

  Box           bbox;
  Color         col0;
  GLfloat       psize[2];
  GLfloat       lw0;


  if ( ! updated_ )
  {
    update_ ();
  }

  glGetFloatv  ( GL_LINE_WIDTH, & lw0 );
  getCurrent   ( col0  );
  getBBox      ( bbox  );
  getPlotSize_ ( psize );

  if ( psize[0] < 100.0 || psize[1] < 100.0 )
  {
    setPlotRect_ ();
    drawFrame_   ();
    drawData_    ( gtx, bbox );
  }
  else
  {
    TextProjector  txp;

    GLfloat  space[3];
    int      ticks[2];

    GLint    stpRepeat;
    GLint    stpPattern;


    glGetIntegerv ( GL_LINE_STIPPLE_REPEAT,  &stpRepeat  );
    glGetIntegerv ( GL_LINE_STIPPLE_PATTERN, &stpPattern );

    psize[0] = width_  / psize[0];
    psize[1] = height_ / psize[1];

    glLineStipple ( 1, 255 );
    setFontSize_  ( txp.font, psize );

    space[0] = 0.5F * psize[1] * (GLfloat) txp.font.getHeight ();
    space[1] =        space[0];
    space[2] = depth_ / 3.0F;

    setPlotRect_  ( ticks, psize, space, txp.font );
    drawFrame_    ();
    drawData_     ( gtx, bbox );

    if ( ticks[0] > 0 )
    {
      drawXTicks_ ( gtx, txp, ticks[0], psize, space );
    }

    if ( ticks[1] > 0 )
    {
      drawYTicks_ ( gtx, txp, ticks[1], psize, space );
    }

    if ( title.size() + xLabel.size() + yLabel.size() > 0 )
    {
      drawLabels_ ( gtx, txp, psize, space );
    }

    if ( options_ & SHOW_KEY )
    {
      drawKey_    ( gtx, txp, bbox, psize, space );
    }

    bbox_[0] = 0.0F;
    bbox_[1] = 0.0F;
    bbox_[2] = width_;
    bbox_[3] = height_;

    glLineStipple ( stpRepeat, (GLushort) stpPattern );
  }

  setCurrent  ( col0 );
  glLineWidth ( lw0  );
}


//-----------------------------------------------------------------------
//   redrawFast
//-----------------------------------------------------------------------


void XYPlot::redrawFast

  ( GfxContext&  gtx,
    double       lod )

{
  if ( lod <= 0.0 )
  {
    return;
  }

  if ( lod >= 1.0 )
  {
    Self::redraw ( gtx ); return;
  }

  DisableScope  lightScope ( GL_LIGHTING );
  DisableScope  blendScope ( GL_BLEND    );

  const idx_t   n = dataSets_.size ();

  Color         col0;
  GLfloat       ps0;
  GLfloat       lw0;

  double        scale[2];
  double        bias [2];


  if ( ! updated_ )
  {
    update_ ();
  }

  glGetFloatv ( GL_POINT_SIZE, & ps0 );
  glGetFloatv ( GL_LINE_WIDTH, & lw0 );
  getCurrent  ( col0 );
  glPointSize ( 3.0  );

  scale[0] = (prect_[2] - prect_[0]) / (xRange_[1] - xRange_[0]);
  scale[1] = (prect_[3] - prect_[1]) / (yRange_[1] - yRange_[0]);

  bias [0] = prect_[0] - scale[0] * xRange_[0];
  bias [1] = prect_[1] - scale[1] * yRange_[0];

  for ( idx_t i = 0; i < n; i++ )
  {
    dataSets_.getAs<DataSet_>(i)->drawFast ( scale, bias, lod );
  }

  drawFrame_  ();
  setCurrent  ( col0 );
  glLineWidth ( lw0  );
  glPointSize ( ps0  );
}


//-----------------------------------------------------------------------
//   exeCommand
//-----------------------------------------------------------------------


bool XYPlot::exeCommand

  ( GfxContext&        gtx,
    const String&      cmd,
    const Properties&  params )

{
  if ( cmd == Commands::AUTO_SCALE )
  {
    setAutoScale ();
    return true;
  }

  if ( cmd == Commands::HIDE_KEY )
  {
    options_ &= ~SHOW_KEY;
    return true;
  }

  if ( cmd == Commands::SHOW_KEY )
  {
    options_ |= SHOW_KEY;
    return true;
  }

  if ( cmd == Commands::HIDE_XTICKS )
  {
    options_ &= ~SHOW_XTICKS;
    return true;
  }

  if ( cmd == Commands::SHOW_XTICKS )
  {
    options_ |= SHOW_XTICKS;
    return true;
  }

  if ( cmd == Commands::HIDE_YTICKS )
  {
    options_ &= ~SHOW_YTICKS;
    return true;
  }

  if ( cmd == Commands::SHOW_YTICKS )
  {
    options_ |= SHOW_YTICKS;
    return true;
  }

  return false;
}


//-----------------------------------------------------------------------
//   listOptions
//-----------------------------------------------------------------------


void XYPlot::listOptions ( Menu& menu ) const
{
  bool  showXTicks = ((options_ & SHOW_XTICKS) != 0);
  bool  showYTicks = ((options_ & SHOW_YTICKS) != 0);

  if ( ! (xAutoScale_ && yAutoScale_) )
  {
    menu.addEntry ( Commands::AUTO_SCALE );
  }

  if ( dataSets_.size() > 0 )
  {
    bool  showKey = (bool) (options_ & SHOW_KEY);

    menu.addOption ( Commands::SHOW_KEY,
                     Commands::HIDE_KEY, showKey );
  }

  menu.addOption ( Commands::SHOW_XTICKS,
                   Commands::HIDE_XTICKS, showXTicks );
  menu.addOption ( Commands::SHOW_YTICKS,
                   Commands::HIDE_YTICKS, showYTicks );
}


//-----------------------------------------------------------------------
//   setFont
//-----------------------------------------------------------------------


bool XYPlot::setFont ()
{
  const idx_t  n = dataSets_.size ();

  for ( idx_t i = 0; i < n; i++ )
  {
    DataSet_*     dset  = dataSets_.getAs<DataSet_> ( i );
    MarkerStyle*  style = dset    ->getPointStyle   ();

    if ( style )
    {
      style->setFont ();
    }
  }

  return true;
}


//-----------------------------------------------------------------------
//   clear
//-----------------------------------------------------------------------


void XYPlot::clear ()
{
  dataSets_.clear ();

  if ( xAutoScale_ )
  {
    xRange_ = Range ( 0.0, 1.0 );
  }

  if ( yAutoScale_ )
  {
    yRange_ = Range ( 0.0, 1.0 );
  }

  updated_   = true;
  nextStyle_ = 0;
}


//-----------------------------------------------------------------------
//   newDataSet
//-----------------------------------------------------------------------


XYPlot::DataSet* XYPlot::newDataSet

  ( const String&  key,
    int            style )

{
  Ref<DataSet_>  dset  = newInstance<DataSet_> ( this );
  Color          color = Color::black          ();


  switch ( nextStyle_ % 7 )
  {
  case 1: color = Color::red    (); break;
  case 2: color = Color::green  (); break;
  case 3: color = Color::blue   (); break;
  case 4: color = Color::yellow (); break;
  case 5: color = Color::orange (); break;
  case 6: color = Color::purple (); break;
  }

  dset->setLineColor ( color );

  if ( style & LINES )
  {
    dset->setLineWidth ( 1.2F );
  }

  if ( style & POINTS )
  {
    char  symbol = Symbols::PLUS;

    switch ( nextStyle_ % 7 )
    {
    case 2: symbol = Symbols::DIAMOND;  break;
    case 3: symbol = Symbols::TRIANGLE; break;
    case 4: symbol = Symbols::CIRCLE;   break;
    case 1: symbol = Symbols::SQUARE;   break;
    case 5: symbol = Symbols::STAR;     break;
    case 6: symbol = Symbols::DISC;     break;
    }

    dset->setPointStyle (
      newInstance<SymbolMarker> ( symbol, color )
    );
  }

  nextStyle_++;

  if ( nextStyle_ > 100 )
  {
    nextStyle_ = 0;
  }

  dset->key = key;
  updated_  = false;

  dataSets_.pushBack ( dset.get() );

  return dset.get ();
}


XYPlot::DataSet* XYPlot::newDataSet

  ( const String&         key,
    const Array<double>&  xData,
    const Array<double>&  yData,
    int                   style )

{
  JEM_PRECHECK ( xData.size() == yData.size() );

  DataSet*  dset = newDataSet ( key, style );

  dset->setPoints ( xData, yData );

  return dset;
}


//-----------------------------------------------------------------------
//   findDataSet
//-----------------------------------------------------------------------


XYPlot::DataSet* XYPlot::findDataSet ( const String& key ) const
{
  const idx_t  n = dataSets_.size ();

  DataSet_*    dset;


  for ( idx_t i = 0; i < n; i++ )
  {
    dset = dataSets_.getAs<DataSet_> ( i );

    if ( dset->key == key )
    {
      return dset;
    }
  }

  return NULL;
}


//-----------------------------------------------------------------------
//   eraseDataSet
//-----------------------------------------------------------------------


bool XYPlot::eraseDataSet ( DataSet* dset )
{
  const idx_t  n = dataSets_.size ();

  for ( idx_t i = 0; i < n; i++ )
  {
    if ( dataSets_.getAs<DataSet_>(i) == dset )
    {
      dataSets_.erase ( i );

      updated_ = false;

      if ( dataSets_.size() == 0 )
      {
        nextStyle_ = 0;
      }

      return true;
    }
  }

  return false;
}


//-----------------------------------------------------------------------
//   getDataSets
//-----------------------------------------------------------------------


Array<XYPlot::DataSet*> XYPlot::getDataSets () const
{
  const idx_t      n = dataSets_.size ();

  Array<DataSet*>  dsets ( n );


  for ( idx_t i = 0; i < n; i++ )
  {
    dsets[i] = dataSets_.getAs<DataSet_> ( i );
  }

  return dsets;
}


//-----------------------------------------------------------------------
//   resize
//-----------------------------------------------------------------------


void XYPlot::resize ( const Size2f& sz )
{
  JEM_PRECHECK ( sz.width > 0.0 && sz.height > 0.0 );

  width_  = sz.width;
  height_ = sz.height;

  setPlotRect_ ();
}


//-----------------------------------------------------------------------
//   setOption
//-----------------------------------------------------------------------


void XYPlot::setOption

  ( int   option,
    bool  choice )

{
  if ( choice )
  {
    options_ |= option;
  }
  else
  {
    options_ &= ~option;
  }
}


//-----------------------------------------------------------------------
//   setOptions
//-----------------------------------------------------------------------


void XYPlot::setOptions ( int options )
{
  options_ = options;
}


//-----------------------------------------------------------------------
//   toggleOption
//-----------------------------------------------------------------------


void XYPlot::toggleOption ( int option )
{
  options_ ^= option;
}


//-----------------------------------------------------------------------
//   setKeyPos
//-----------------------------------------------------------------------


void XYPlot::setKeyPos ( GLfloat x, GLfloat y )
{
  keyPos_[0] = clamped ( x, 1.0F );
  keyPos_[1] = clamped ( y, 1.0F );
}


//-----------------------------------------------------------------------
//   set[X|Y]Range
//-----------------------------------------------------------------------


void XYPlot::setXRange ( const Range& range )
{
  JEM_PRECHECK ( (range[1] - range[0]) >= 0.0 );

  xRange_     = range;
  xAutoScale_ = false;
  updated_    = false;

  adjustRange_ ( xRange_ );
}


void XYPlot::setYRange ( const Range& range )
{
  JEM_PRECHECK ( (range[1] - range[0]) >= 0.0 );

  yRange_     = range;
  yAutoScale_ = false;
  updated_    = false;

  adjustRange_ ( yRange_ );
}


//-----------------------------------------------------------------------
//   get[X|Y]Range
//-----------------------------------------------------------------------


XYPlot::Range XYPlot::getXRange () const
{
  if ( xAutoScale_ && ! updated_ )
  {
    const_cast<Self*>(this)->update_ ();
  }

  return xRange_;
}


XYPlot::Range XYPlot::getYRange () const
{
  if ( yAutoScale_ && ! updated_ )
  {
    const_cast<Self*>(this)->update_ ();
  }

  return yRange_;
}


//-----------------------------------------------------------------------
//   set[X|Y]AutoScale
//-----------------------------------------------------------------------


void XYPlot::setAutoScale ()
{
  setXAutoScale ();
  setYAutoScale ();
}


void XYPlot::setXAutoScale ()
{
  if ( ! xAutoScale_ )
  {
    xAutoScale_ = true;
    updated_    = false;
  }
}


void XYPlot::setYAutoScale ()
{
  if ( ! yAutoScale_ )
  {
    yAutoScale_ = true;
    updated_    = false;
  }
}


//-----------------------------------------------------------------------
//   setScaleStep
//-----------------------------------------------------------------------


void XYPlot::setScaleStep ( double step )
{
  JEM_PRECHECK ( step >= 0.0 );

  scaleStep_ = step;
}


//-----------------------------------------------------------------------
//   mapPoint
//-----------------------------------------------------------------------


Vertex2d XYPlot::mapPoint ( const Vertex2d& p ) const
{
  if ( ! updated_ )
  {
    const_cast<Self*>(this)->update_ ();
  }

  Vertex2d  v;
  double    scale[2];
  double    bias [2];

  scale[0] = (prect_[2] - prect_[0]) / (xRange_[1] - xRange_[0]);
  scale[1] = (prect_[3] - prect_[1]) / (yRange_[1] - yRange_[0]);

  bias [0] = prect_[0] - scale[0] * xRange_[0];
  bias [1] = prect_[1] - scale[1] * yRange_[0];

  v[0] = (p[0] - bias[0]) / scale[0];
  v[1] = (p[1] - bias[1]) / scale[1];

  return v;
}


//-----------------------------------------------------------------------
//   init_
//-----------------------------------------------------------------------


void XYPlot::init_ ( const Size2f& sz )
{
  hidden      = false;
  options_    = DEFAULT_OPTIONS;
  xRange_     = Range ( 0.0, 1.0 );
  yRange_     = xRange_;
  xAutoScale_ = true;
  yAutoScale_ = true;
  scaleStep_  = 0.1;
  updated_    = true;
  nextStyle_  = 0;
  keyPos_[0]  = 0.90F;
  keyPos_[1]  = 0.90F;

  resize ( sz );
}


//-----------------------------------------------------------------------
//   update_
//-----------------------------------------------------------------------


void XYPlot::update_ ()
{
  const idx_t  n = dataSets_.size ();


  if ( xAutoScale_ )
  {
    Range  r ( 0.0, 1.0 );

    xRange_ = r;

    for ( idx_t i = 0; i < n; i++ )
    {
      DataSet_*  dset = dataSets_.getAs<DataSet_> ( i );

      if ( yAutoScale_ )
      {
        r = dset->getXRange ();
      }
      else
      {
        r = dset->getXRange ( yRange_ );
      }

      if ( i == 0 )
      {
        xRange_ = r;
      }
      else
      {
        if ( r[0] < xRange_[0] )
        {
          xRange_[0] = r[0];
        }

        if ( r[1] > xRange_[1] )
        {
          xRange_[1] = r[1];
        }
      }
    }

    adjustRange_ ( xRange_, scaleStep_ );
  }

  if ( yAutoScale_ )
  {
    Range  r ( 0.0, 1.0 );

    yRange_ = r;

    for ( idx_t i = 0; i < n; i++ )
    {
      DataSet_*  dset = dataSets_.getAs<DataSet_> ( i );

      if ( xAutoScale_ )
      {
        r = dset->getYRange ();
      }
      else
      {
        r = dset->getYRange ( xRange_ );
      }

      if ( i == 0 )
      {
        yRange_ = r;
      }
      else
      {
        if ( r[0] < yRange_[0] )
        {
          yRange_[0] = r[0];
        }

        if ( r[1] > yRange_[1] )
        {
          yRange_[1] = r[1];
        }
      }
    }

    adjustRange_ ( yRange_, scaleStep_ );
  }

  for ( idx_t i = 0; i < n; i++ )
  {
    dataSets_.getAs<DataSet_>(i)->sieve ( xRange_, yRange_ );
  }

  updated_ = true;
}


//-----------------------------------------------------------------------
//   drawData_
//-----------------------------------------------------------------------


void XYPlot::drawData_

  ( GfxContext&  gtx,
    const Box&   bbox ) const

{
  const idx_t  n = dataSets_.size ();

  GLdouble     clipEqn[4];
  double       scale[2];
  double       bias [2];


  // Set up the clipping planes.

  for ( idx_t i = 0; i < 4; i++ )
  {
    clipEqn[i] = 0.0;
  }

  // The left border.

  clipEqn[0] =  1.0;
  clipEqn[3] = -prect_[0];

  glClipPlane ( GL_CLIP_PLANE0, clipEqn );
  glEnable    ( GL_CLIP_PLANE0 );

  // The right border.

  clipEqn[0] = -1.0;
  clipEqn[3] =  prect_[2];

  glClipPlane ( GL_CLIP_PLANE1, clipEqn );
  glEnable    ( GL_CLIP_PLANE1 );

  // The lower border.

  clipEqn[0] =  0.0;
  clipEqn[1] =  1.0;
  clipEqn[3] = -prect_[1];

  glClipPlane ( GL_CLIP_PLANE2, clipEqn );
  glEnable    ( GL_CLIP_PLANE2 );

  // The upper border.

  clipEqn[1] = -1.0;
  clipEqn[3] =  prect_[3];

  glClipPlane ( GL_CLIP_PLANE3, clipEqn );
  glEnable    ( GL_CLIP_PLANE3 );

  // Draw the data sets.

  scale[0] = (prect_[2] - prect_[0]) / (xRange_[1] - xRange_[0]);
  scale[1] = (prect_[3] - prect_[1]) / (yRange_[1] - yRange_[0]);

  bias [0] = prect_[0] - scale[0] * xRange_[0];
  bias [1] = prect_[1] - scale[1] * yRange_[0];

  // First draw the lines.

  for ( idx_t i = 0; i < n; i++ )
  {
    dataSets_.getAs<DataSet_>(i)->drawLines ( scale, bias );
  }

  // Disable the clipping planes.

  for ( idx_t i = 0; i < 4; i++ )
  {
    glDisable ( (GLenum) (GL_CLIP_PLANE0 + i) );
  }

  // Then draw the markers. This must be done with the clipping planes
  // disabled because a marker may set up its own projection or
  // model-view matrix.

  for ( idx_t i = 0; i < n; i++ )
  {
    dataSets_.getAs<DataSet_>(i) ->

      drawMarkers ( gtx, scale, bias, bbox );
  }
}


//-----------------------------------------------------------------------
//   drawFrame_
//-----------------------------------------------------------------------


void XYPlot::drawFrame_ () const
{
  glColor3f   ( 0.0, 0.0, 0.0 );
  glLineWidth ( 2.0 );

  glBegin ( GL_LINE_STRIP );
  {
    glVertex2f ( prect_[0], prect_[1] );
    glVertex2f ( prect_[2], prect_[1] );
    glVertex2f ( prect_[2], prect_[3] );
    glVertex2f ( prect_[0], prect_[3] );
    glVertex2f ( prect_[0], prect_[1] );
  }
  glEnd ();
}


//-----------------------------------------------------------------------
//   drawXTicks_
//-----------------------------------------------------------------------


void XYPlot::drawXTicks_

  ( GfxContext&     gtx,
    TextProjector&  txp,
    int             count,
    const GLfloat   psize[2],
    const GLfloat   space[3] )


{
  const GLfloat  fnHeight = psize[1] * (GLfloat) txp.font.getHeight ();

  char           buf[64];
  double         v, dv;
  GLfloat        x, dx, y, dy;
  GLfloat        w;
  int            i, k;


  txp.hAlignment = CENTERED;
  txp.vAlignment = TOP_ALIGNED;

  // Draw the values along the x-axis.

  {
    OrthoTextScope  textScope ( gtx, txp );

    glColor3f ( 0.0F, 0.0F, 0.0F );

    w  =  prect_[2] - prect_[0];
    y  =  bbox_ [1];
    dx =  w                        / ((GLfloat) count - 1.0F);
    dv = (xRange_[1] - xRange_[0]) / ((GLfloat) count - 1.0F);
    k  =  std::sprintf ( buf, TICK_FORMAT_, xRange_[0] );

    txp.setCursor ( prect_[0], y );
    txp.writeLine ( buf,       k );

    for ( i = 1; i < count - 1; i++ )
    {
      x = prect_ [0] + (GLfloat) i * dx;
      v = xRange_[0] + (GLfloat) i * dv;
      k = std::sprintf ( buf, TICK_FORMAT_, v );

      txp.setCursor ( x,   y );
      txp.writeLine ( buf, k );
    }

    k = std::sprintf ( buf, TICK_FORMAT_, xRange_[1] );

    txp.setCursor ( prect_[2], y );
    txp.writeLine ( buf,       k );
  }

  bbox_[1] -= space[1] + fnHeight;

  // Draw the tick marks.

  glLineWidth ( 1.2F );
  glColor4f   ( 0.25F, 0.25F, 0.25F, 0.5F );
  glEnable    ( GL_LINE_STIPPLE );
  glBegin     ( GL_LINES );

  for ( i = 1; i < count - 1; i++ )
  {
    x = prect_[0] + (GLfloat) i * dx;

    glVertex3f ( x, prect_[1], -space[2] );
    glVertex3f ( x, prect_[3], -space[2] );
  }

  glEnd     ();
  glDisable ( GL_LINE_STIPPLE );

  // Draw the y-axis if visible.

  if ( xRange_[0] < 0.0 && xRange_[1] > 0.0 )
  {
    x = prect_[0] - (GLfloat)

      ( (xRange_[0] * w) / (xRange_[1] - xRange_[0]) );

    glBegin ( GL_LINES );
    {
      glVertex3f ( x, prect_[1], -space[2] );
      glVertex3f ( x, prect_[3], -space[2] );
    }
    glEnd ();
  }

  glColor3f   ( 0.0F, 0.0F, 0.0F );
  glLineWidth ( 2.2F );
  glBegin     ( GL_LINES );

  count = 2 * (count - 1);
  dx    = w / (GLfloat) count;
  dy    = 0.5F * space[1];

  for ( i = 1; i < count; i++ )
  {
    x = prect_[0] + (GLfloat) i * dx;

    glVertex2f ( x, prect_[1] );
    glVertex2f ( x, prect_[1] + dy );
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   drawYTicks_
//-----------------------------------------------------------------------


void XYPlot::drawYTicks_

  ( GfxContext&      gtx,
    TextProjector&   txp,
    int              count,
    const GLfloat    psize[2],
    const GLfloat    space[3] )

{
  char     buf[64];
  double   v,  dv;
  GLfloat  x,  dx, y, dy, h;
  GLsizei  w,  wmax;
  int      i,  k;


  txp.hAlignment = RIGHT_ALIGNED;
  txp.vAlignment = CENTERED;

  // Draw the y-values.

  {
    OrthoTextScope  textScope ( gtx, txp );

    glColor3f ( 0.0F, 0.0F, 0.0F );

    h  =  prect_[3]  - prect_[1];
    x  =  bbox_ [0];
    dy =  h                        / ((GLfloat) count - 1.0F);
    dv = (yRange_[1] - yRange_[0]) / ((GLfloat) count - 1.0F);
    k  =  std::sprintf ( buf, TICK_FORMAT_, yRange_[0] );

    txp.setCursor ( x, prect_[1] );

    wmax = txp.writeLine ( buf, k );

    for ( i = 1; i < count - 1; i++ )
    {
      y = prect_ [1] + (GLfloat) i * dy;
      v = yRange_[0] + (GLfloat) i * dv;
      k = std::sprintf ( buf, TICK_FORMAT_, v );

      txp.setCursor ( x, y );

      w = txp.writeLine ( buf, k );

      if ( w > wmax )
      {
        wmax = w;
      }
    }

    k = std::sprintf ( buf, TICK_FORMAT_, yRange_[1] );

    txp.setCursor ( x,  prect_[3] );

    w = txp.writeLine ( buf, k );

    if ( w > wmax )
    {
      wmax = w;
    }
  }

  bbox_[0] -= space[0] + (GLfloat) wmax * psize[0];

  // Draw the tick marks.

  glLineWidth ( 1.2F );
  glEnable    ( GL_LINE_STIPPLE );
  glColor4f   ( 0.25F, 0.25F, 0.25F, 0.5F );
  glBegin     ( GL_LINES );

  for ( i = 1; i < count - 1; i++ )
  {
    y = prect_[1] + (GLfloat) i * dy;

    glVertex3f ( prect_[0], y, -space[2] );
    glVertex3f ( prect_[2], y, -space[2] );
  }

  glEnd     ();
  glDisable ( GL_LINE_STIPPLE );

  // Draw the x-axis if visible.

  if ( yRange_[0] < 0.0 && yRange_[1] > 0.0 )
  {
    y = prect_[1] - (GLfloat)

      ( (yRange_[0] * h) / (yRange_[1] - yRange_[0]) );

    glBegin ( GL_LINES );
    {
      glVertex3f ( prect_[0], y, -space[2] );
      glVertex3f ( prect_[2], y, -space[2] );
    }
    glEnd ();
  }

  glColor3f   ( 0.0F, 0.0F, 0.0F );
  glLineWidth ( 2.2F );
  glBegin     ( GL_LINES );

  count = 2    * (count - 1);
  dx    = 0.5F * space[0];
  dy    = h    / (GLfloat) count;

  for ( i = 1; i < count; i++ )
  {
    y = prect_[1] + (GLfloat) i * dy;

    glVertex2f ( prect_[0],      y );
    glVertex2f ( prect_[0] + dx, y );
  }

  glEnd ();
}


//-----------------------------------------------------------------------
//   drawLabels_
//-----------------------------------------------------------------------


void XYPlot::drawLabels_

  ( GfxContext&     gtx,
    TextProjector&  txp,
    const GLfloat   psize[2],
    const GLfloat   space[3] )

{
  OrthoTextScope  textScope ( gtx, txp );

  const GLfloat   fnHeight = psize[1] * (GLfloat) txp.font.getHeight ();

  GLfloat         x, y, dy;
  idx_t           i, n;


  glColor3f ( 0.0F, 0.0F, 0.0F );

  txp.hAlignment = CENTERED;

  if ( xLabel.size() )
  {
    x = 0.5F * (prect_[0] + prect_[2]);
    y = bbox_[1];

    txp.vAlignment = TOP_ALIGNED;

    txp.setCursor ( x, y );
    txp.writeLine ( xLabel );

    bbox_[1] -= space[1] + fnHeight;
  }

  n = yLabel.size ();

  if ( n )
  {
    x  = bbox_[0] - 0.5F * fnHeight;
    dy = 0.95F * fnHeight;
    y  = 0.50F * (prect_[1] + prect_[3] + (GLfloat) n * dy);

    txp.vAlignment = CENTERED;

    for ( i = 0; i < n; i++ )
    {
      txp.setCursor ( x, y );
      txp.write     ( yLabel[i] );

      y -= dy;
    }

    bbox_[0] -= space[0] + fnHeight;
  }

  if ( title.size() )
  {
    x = 0.5F * width_;
    y = bbox_[3];

    txp.vAlignment = BOT_ALIGNED;

    txp.setCursor ( x, y );
    txp.writeLine ( title );

    bbox_[3] += space[1] + fnHeight;
  }
}


//-----------------------------------------------------------------------
//   drawKey_
//-----------------------------------------------------------------------


void XYPlot::drawKey_

  ( GfxContext&     gtx,
    TextProjector&  txp,
    const Box&      bbox,
    const GLfloat   psize[2],
    const GLfloat   space[3] )

{
  const idx_t    dsetCount = dataSets_.size ();
  const GLfloat  fnHeight  = psize[1] * (GLfloat) txp.font.getHeight ();

  MarkerStyle*   ps;
  DataSet_*      dset;
  GLfloat        x1, x2, x3, x4;
  GLfloat        y,  dy;
  GLfloat        z;
  GLfloat        w,  wmax;
  GLfloat        lw;
  GLfloat        xKey, yKey;
  GLfloat        pos[3];
  idx_t          keyCount;


  txp.hAlignment = RIGHT_ALIGNED;
  txp.vAlignment = CENTERED;

  xKey     = prect_[0] + keyPos_[0] * (prect_[2] - prect_[0]);
  yKey     = prect_[1] + keyPos_[1] * (prect_[3] - prect_[1]);

  x1       = xKey   -        space[0];
  x2       = x1     - 2.0F * space[0];
  x3       = x2     - 2.0F * space[0];
  x4       = x3     -        space[0];

  dy       = fnHeight + 0.5F * space[1];
  y        = yKey - space[1] - 0.5F * dy;
  z        = 2.0F * space[2];

  pos[0]   = x2;
  pos[2]   = z;

  keyCount = 0;
  wmax     = 0.0F;

  for ( idx_t i = 0; i < dsetCount; i++ )
  {
    if ( y < space[1] )
    {
      break;
    }

    dset = dataSets_.getAs<DataSet_> ( i );

    if ( dset->key.size() == 0 )
    {
      continue;
    }

    lw = dset->getLineWidth ();

    if ( lw > 0.1F )
    {
      glLineWidth ( lw );
      setCurrent  ( dset->getLineColor() );

      glBegin ( GL_LINES );
      {
        glVertex3f ( x1, y, z );
        glVertex3f ( x3, y, z );
      }
      glEnd ();
    }

    ps = dset->getPointStyle ();

    if ( ps )
    {
      pos[1] = y;

      ps->beginDraw  ( gtx, bbox, 1 );
      ps->drawMarker ( i, pos );
      ps->endDraw    ();
    }

    glColor3f     ( 0.0F, 0.0F, 0.0F );
    txp.begin     ( gtx );
    txp.setCursor ( x4, y, z );

    w = (GLfloat) txp.writeLine ( dset->key );

    txp.end ();

    if ( w > wmax )
    {
      wmax = w;
    }

    keyCount++;

    y = y - dy;
  }

  if ( keyCount == 0 )
  {
    return;
  }

  x1 = x4 - wmax * psize[0] - space[0];
  y  = y  + 0.5F *       dy - space[1];
  z  = space[2];

  glColor4f ( 1.0F, 1.0F, 1.0F, 0.85F );

  glBegin ( GL_QUADS );
  {
    glVertex3f (    x1,   y, z );
    glVertex3f ( xKey,    y, z );
    glVertex3f ( xKey, yKey, z );
    glVertex3f (   x1, yKey, z );
  }
  glEnd ();

  glTranslatef ( 0.0F, 0.0F,  z );
  renderShadow ( x1, y, xKey, yKey, space[0] );
  glTranslatef ( 0.0F, 0.0F, -z );

  // A rendering problem has been experienced with some video drivers
  // (Intel) on Windows. A work-around is to enable/disable lighting.

#ifdef JEM_OS_WINDOWS

  glEnable     ( GL_LIGHTING );
  glDisable    ( GL_LIGHTING );

#endif

  glColor3f    ( 0.0F, 0.0F, 0.0F );
  glLineWidth  ( 2.0F );

  z += space[2];

  glBegin ( GL_LINE_STRIP );
  {
    glVertex3f (   x1,    y, z );
    glVertex3f ( xKey,    y, z );
    glVertex3f ( xKey, yKey, z );
    glVertex3f (   x1, yKey, z );
    glVertex3f (   x1,    y, z );
  }
  glEnd ();
}


//-----------------------------------------------------------------------
//   getPlotSize_
//-----------------------------------------------------------------------


void XYPlot::getPlotSize_ ( GLfloat psize[2] ) const
{
  Transform  t, p;
  Rect       vport;
  Vertex3d   a,  b, c;
  double     dx, dy;


  glGetDoublev ( GL_MODELVIEW_MATRIX,  t.addr() );
  glGetDoublev ( GL_PROJECTION_MATRIX, p.addr() );

  psize[0] = 100.0;
  psize[1] = 100.0;

  a        = 0.0;

  matmul ( b, t, a );

  if ( ! project( a, p, b ) )
  {
    return;
  }

  getViewport ( vport );

  toWindow ( a, vport );

  b[0] = width_;
  b[1] = 0.0;
  b[2] = 0.0;

  matmul ( c, t, b );

  if ( ! project( b, p, c ) )
  {
    return;
  }

  toWindow ( b, vport );

  dx = b[0] - a[0];
  dy = b[1] - a[1];

  psize[0] = (GLfloat) std::sqrt ( dx * dx + dy * dy );

  b[0] = 0.0;
  b[1] = height_;
  b[2] = 0.0;

  matmul ( c, t, b );

  if ( ! project( b, p, c ) )
  {
    return;
  }

  toWindow ( b, vport );

  dx = b[0] - a[0];
  dy = b[1] - a[1];

  psize[1] = (GLfloat) std::sqrt ( dx * dx + dy * dy );
}


//-----------------------------------------------------------------------
//   setFontSize_
//-----------------------------------------------------------------------


void XYPlot::setFontSize_

  ( Font&          font,
    const GLfloat  psize[2] ) const

{
  GLfloat  fnWidth  = psize[0] * (GLfloat) font.getWidthOf ( '0' );
  GLfloat  fnHeight = psize[1] * (GLfloat) font.getHeight  ();

  if ( 30.0F * fnHeight > height_ ||
       50.0F * fnWidth  > width_ )
  {
    font--;
  }
}


//-----------------------------------------------------------------------
//   setPlotRect_
//-----------------------------------------------------------------------


void XYPlot::setPlotRect_ ()
{
  bbox_ [0] = 0.0F;
  bbox_ [1] = 0.0F;
  bbox_ [2] = width_;
  bbox_ [3] = height_;
  depth_    = 0.05F * jem::max ( width_, height_ );

  prect_[0] = 0.05F * width_;
  prect_[1] = 0.05F * height_;
  prect_[2] = 0.95F * width_;
  prect_[3] = 0.95F * height_;
}


void XYPlot::setPlotRect_

  ( int            ticks[2],
    const GLfloat  psize[2],
    const GLfloat  space[3],
    const Font&    font )

{
  const GLfloat  fnWidth  = psize[0] * (GLfloat) font.getWidthOf ( '0' );
  const GLfloat  fnHeight = psize[1] * (GLfloat) font.getHeight  ();

  char           buf[64];
  GLfloat        h,  w;
  GLfloat        dx, dy;
  double         v,  dv;
  int            i,  k, n;


  // Initial plot rectangle.

  prect_[0] = 0.0F;
  prect_[1] = 0.0F;
  prect_[2] = width_;
  prect_[3] = height_;

  // Space for the labels and title.

  if ( xLabel.size() )
  {
    prect_[1] += space[1] + fnHeight;
  }

  if ( yLabel.size() )
  {
    prect_[0] += space[0] + fnHeight;
  }

  if ( title.size() )
  {
    prect_[3] -= space[1] + fnHeight;
  }

  // Determine the number of tick marks.

  if ( options_ & SHOW_XTICKS )
  {
    dx = fnWidth;
    w  = prect_[2] - prect_[0] - 2.0F * dx;

    if ( w <= 0.0 )
    {
      ticks[0] = 0;
    }
    else
    {
      v = 0.5 * (xRange_[0] + xRange_[1]);
      n = 3;
      k = std::sprintf ( buf, TICK_FORMAT_, xRange_[0] );
      n = max          ( n, k );
      k = std::sprintf ( buf, TICK_FORMAT_, v );
      n = max          ( n, k );
      k = std::sprintf ( buf, TICK_FORMAT_, xRange_[1] );
      n = max          ( n, k );
      n = n + 2;

      ticks[0] = (int) (0.5F + w / ((GLfloat) n * dx));

      adjustTickCount ( ticks[0] );
    }
  }
  else
  {
    ticks[0] = 0;
  }

  if ( options_ & SHOW_YTICKS )
  {
    dy = fnHeight;
    h  = prect_[3] - prect_[1] - 2.0F * dy;

    if ( h <= 0.0 )
    {
      ticks[1] = 0;
    }
    else
    {
      ticks[1] = (int) (0.5 + h / (4.0 * dy));

      adjustTickCount ( ticks[1] );
    }
  }
  else
  {
    ticks[1] = 0;
  }

  // Space for the tick marks (phase 1).

  if ( ticks[0] > 0 )
  {
    prect_[1] += space[1] + fnHeight;
  }

  if ( ticks[1] > 0 )
  {
    n  = ticks[1];
    dv = (yRange_[1] - yRange_[0]) / (n - 1.0);

    k  = std::sprintf ( buf, TICK_FORMAT_, yRange_[0] );
    w  = (GLfloat) k * fnWidth;

    for ( i = 1; i < n - 1; i++ )
    {
      v = yRange_[0] + i * dv;
      k = std::sprintf ( buf, TICK_FORMAT_, v );
      w = jem::max     ( w, (GLfloat) k * fnWidth );
    }

    k = std::sprintf ( buf, TICK_FORMAT_, yRange_[1] );
    w = jem::max     ( w, (GLfloat) k * fnWidth );

    // Note that the average character is less wide than a '-'; hence
    // the multiplication factor.

    prect_[0] += space[0] + w * 0.9F;
  }

  prect_[0] += space[0];
  prect_[1] += space[1];
  prect_[2] -= space[0];
  prect_[3] -= space[1];

  // Space for the tick marks (phase 2).

  if ( ticks[0] > 0 )
  {
    k = std::sprintf ( buf, TICK_FORMAT_, xRange_[0] );
    w = 0.5F * (GLfloat) k * fnWidth + space[0];

    if ( w > prect_[0] )
    {
      prect_[0] = w;
    }

    k = std::sprintf ( buf, TICK_FORMAT_, xRange_[1] );
    w = 0.5F * (GLfloat) k * fnWidth + space[0];

    if ( w > width_ - prect_[2] )
    {
      prect_[2] = width_ - w;
    }
  }

  if ( ticks[1] > 0 )
  {
    h = space[1] + 0.5F * fnHeight;

    if ( h > prect_[1] )
    {
      prect_[1] = h;
    }

    if ( h > height_ - prect_[3] )
    {
      prect_[3] = height_ - h;
    }
  }

  // Make sure the plot rectange is valid.

  if ( prect_[0] >= prect_[2] )
  {
    prect_[0] = prect_[2] - 0.01F * width_;
  }

  if ( prect_[1] >= prect_[3] )
  {
    prect_[1] = prect_[3] - 0.01F * height_;
  }

  // Initialize the bounding box.

  bbox_[0] = prect_[0] - space[0];
  bbox_[1] = prect_[1] - space[1];
  bbox_[2] = prect_[2] + space[0];
  bbox_[3] = prect_[3] + space[1];
}


//-----------------------------------------------------------------------
//   setRanges_
//-----------------------------------------------------------------------


void XYPlot::setRanges_ ( const Properties& props )
{
  const char*  RANGE_PROPS[2] = { PropNames::XRANGE,
                                  PropNames::YRANGE };

  for ( int i = 0; i < 2; i++ )
  {
    Ref<Object>  obj;

    if ( ! props.find( obj, RANGE_PROPS[i] ) )
    {
      continue;
    }

    Range  newRange;
    bool   autoScale;

    autoScale = false;
    newRange  = 0.0;

    if ( isInstance<String>( obj ) )
    {
      String  str = toValue<String> ( obj );

      if ( str.equalsIgnoreCase( "auto" ) )
      {
        autoScale = true;
      }
      else
      {
        props.propertyError (
          RANGE_PROPS[i],
          String::format (
            "invalid range: %s; should be `auto\' or "
            "an array of length 2",
            str
          )
        );
      }
    }
    else
    {
      Array<double>  r;

      props.get ( r, RANGE_PROPS[i] );

      if ( r.size() == 0 )
      {
        autoScale = true;
      }
      else
      {
        if ( r.size() != 2 )
        {
          props.propertyError (
            RANGE_PROPS[i],
            String::format (
              "invalid range: array has length %d but "
              "should have length 2 or zero (for auto scale)",
              r.size ()
            )
          );
        }

        if ( (r[1] - r[0]) < 0.0 )
        {
          props.propertyError (
            RANGE_PROPS[i],
            String::format ( "invalid range: [%g,%g]", r[0], r[1] )
          );
        }

        newRange[0] = r[0];
        newRange[1] = r[1];
      }
    }

    if ( autoScale )
    {
      if ( i == 0 )
      {
        setXAutoScale ();
      }
      else
      {
        setYAutoScale ();
      }
    }
    else
    {
      if ( i == 0 )
      {
        setXRange ( newRange );
      }
      else
      {
        setYRange ( newRange );
      }
    }
  }
}


//-----------------------------------------------------------------------
//   adjustRange_
//-----------------------------------------------------------------------


void XYPlot::adjustRange_

  ( Range&  r,
    double  step )

{
  const double  SMALL_VAL = 1.0e-30;
  const double  LARGE_VAL = 1.0e+30;

  double        x;


  if ( (r[1] - r[0]) < SMALL_VAL )
  {
    if ( std::fabs( r[0] ) < SMALL_VAL )
    {
      r[0] = -SMALL_VAL;
      r[1] =  SMALL_VAL;
    }
    else
    {
      r[1] = r[0] + 0.1 * std::fabs( r[0] );
    }
  }

  if ( step < 1.0e-5 )
  {
    return;
  }

  // Adjust the range so that the lower and upper points are visible.

  x     = 0.25 * step * (r[1] - r[0]);
  r[0] -= x;
  r[1] += x;
  x     = r[1] - r[0];

  if ( x >= SMALL_VAL && x <= LARGE_VAL )
  {
    x = ::log10( x );
    x = ::pow( 10.0, ::floor( x + 0.5 ) );
    x = x * step;

    if ( r[0] >= 0.0 )
    {
      r[0] = r[0] / x;
      r[0] = x * ::floor( r[0] );
    }
    else
    {
      r[0] = -r[0] / x;
      r[0] = -x * ::ceil( r[0] );
    }

    if ( r[1] >= 0.0 )
    {
      r[1] = r[1] / x;
      r[1] = x * ::ceil( r[1] );
    }
    else
    {
      r[1] = -r[1] / x;
      r[1] = -x * ::floor( r[1] );
    }
  }
}


JEM_END_PACKAGE( gl )
