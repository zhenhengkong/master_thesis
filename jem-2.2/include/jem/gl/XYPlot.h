
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

#ifndef JEM_GL_XYPLOT_H
#define JEM_GL_XYPLOT_H

#include <jem/base/array/Array.h>
#include <jem/util/ObjFlex.h>
#include <jem/gl/Size.h>
#include <jem/gl/Color.h>
#include <jem/gl/Vertex.h>
#include <jem/gl/LeafNode.h>


JEM_BEGIN_PACKAGE( gl )


class Font;
class MarkerStyle;
class TextProjector;


//-----------------------------------------------------------------------
//   class XYPlot
//-----------------------------------------------------------------------


class XYPlot : public LeafNode
{
 public:

  JEM_DECLARE_CLASS       ( XYPlot, LeafNode );

  typedef Tuple<double,2>   Range;

  class                     DataSet;

  static const int          SHOW_KEY;
  static const int          SHOW_XTICKS;
  static const int          SHOW_YTICKS;
  static const int          SHOW_TICKS;
  static const int          DEFAULT_OPTIONS;

  static const int          LINES;
  static const int          POINTS;
  static const int          LINES_POINTS;


  explicit                  XYPlot

    ( const String&           name );

                            XYPlot

    ( const String&           name,
      const Size2f&           size );

  virtual void              getBBox

    ( Box&                    box )            const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )          const;

  virtual void              listProps

    ( const Properties&       props )          const;

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

    ( Menu&                   menu )           const;

  virtual bool              setFont         ();

  void                      clear           ();

  DataSet*                  newDataSet

    ( const String&           key   = "",
      int                     style = LINES );

  DataSet*                  newDataSet

    ( const String&           key,
      const Array<double>&    xData,
      const Array<double>&    yData,
      int                     style = LINES );

  bool                      eraseDataSet

    ( DataSet*                dset );

  DataSet*                  findDataSet

    ( const String&           key )            const;

  Array<DataSet*>           getDataSets     () const;

  inline Size2f             size            () const;

  void                      resize

    ( const Size2f&           sz );

  void                      setOption

    ( int                     option,
      bool                    choice = true );

  void                      setOptions

    ( int                     options );

  void                      toggleOption

    ( int                     option );

  inline int                getOptions      () const;

  void                      setKeyPos

    ( GLfloat                 xpos,
      GLfloat                 ypos );

  inline GLfloat            getKeyXPos      () const;
  inline GLfloat            getKeyYPos      () const;

  void                      setXRange

    ( const Range&            range );

  void                      setYRange

    ( const Range&            range );

  Range                     getXRange       () const;
  Range                     getYRange       () const;
  void                      setAutoScale    ();
  void                      setXAutoScale   ();
  void                      setYAutoScale   ();
  inline bool               getXAutoScale   () const;
  inline bool               getYAutoScale   () const;

  void                      setScaleStep

    ( double                  step );

  inline double             getScaleStep    () const;

  Vertex2d                  mapPoint

    ( const Vertex2d&         p )              const;

  inline void               adjustRange

    ( Range&                  r )              const;


 public:

  String                    title;
  String                    xLabel;
  String                    yLabel;


 protected:

  virtual                  ~XYPlot          ();


 private:

  void                      init_

    ( const Size2f&           sz );

  void                      update_         ();

  void                      drawData_

    ( GfxContext&             gtx,
      const Box&              bbox )           const;

  void                      drawFrame_      () const;

  void                      drawXTicks_

    ( GfxContext&             gtx,
      TextProjector&          txp,
      int                     count,
      const GLfloat           psize[2],
      const GLfloat           space[3] );

  void                      drawYTicks_

    ( GfxContext&             gtx,
      TextProjector&          txp,
      int                     count,
      const GLfloat           psize[2],
      const GLfloat           space[3] );

  void                      drawLabels_

    ( GfxContext&             gtx,
      TextProjector&          txp,
      const GLfloat           psize[2],
      const GLfloat           space[3] );

  void                      drawKey_

    ( GfxContext&             gtx,
      TextProjector&          txp,
      const Box&              bbox,
      const GLfloat           psize[2],
      const GLfloat           space[3] );

  void                      getPlotSize_

    ( GLfloat                 psize[2] )       const;

  void                      setFontSize_

    ( Font&                   font,
      const GLfloat           psize[2] )       const;

  void                      setPlotRect_    ();

  void                      setPlotRect_

    ( int                     ticks[2],
      const GLfloat           psize[2],
      const GLfloat           space[3],
      const Font&             font );

  void                      setRanges_

    ( const Properties&       props );

  static void               adjustRange_

    ( Range&                  range,
      double                  step = 0.0 );


 private:

  static const char*        TICK_FORMAT_;

  class                     DataSet_;
  friend class              DataSet_;

  util::ObjFlex             dataSets_;
  int                       options_;
  GLfloat                   width_;
  GLfloat                   height_;
  GLfloat                   keyPos_[2];
  GLfloat                   bbox_ [4];
  GLfloat                   prect_[4];
  GLfloat                   depth_;
  Range                     xRange_;
  Range                     yRange_;
  bool                      xAutoScale_;
  bool                      yAutoScale_;
  double                    scaleStep_;
  bool                      updated_;
  int                       nextStyle_;

};


//-----------------------------------------------------------------------
//   class XYPlot::DataSet
//-----------------------------------------------------------------------


class XYPlot::DataSet
{
 public:

  typedef MarkerStyle       PntStyle;


  void                      clear           ();

  virtual void              setPoints

    ( const Array<double>&    x,
      const Array<double>&    y )                    = 0;

  virtual void              addPoint

    ( double                  x,
      double                  y )                    = 0;

  virtual void              addPoints

    ( const Array<double>&    x,
      const Array<double>&    y )                    = 0;

  virtual idx_t             pointCount      () const = 0;

  virtual void              setMaxPoints

    ( idx_t                   count )                = 0;

  inline idx_t              getMaxPoints    () const;

  void                      setStyle

    ( GLfloat                 lwidth,
      const Color&            lcolor,
      const Ref<PntStyle>&    pstyle );

  void                      setLineWidth

    ( GLfloat                 width );

  inline GLfloat            getLineWidth    () const;

  void                      setLineColor

    ( const Color&            color );

  inline Color              getLineColor    () const;

  void                      setPointStyle

    ( const Ref<PntStyle>&    style );

  inline PntStyle*          getPointStyle   () const;

  void                      setBaseNumber

    ( idx_t                   nr0 );

  inline idx_t              getBaseNumber   () const;

  void                      configure

    ( const Properties&       props );

  void                      getConfig

    ( const Properties&       conf )           const;

  void                      listProps

    ( const Properties&       conf )           const;


 public:

  String                    key;


 protected:

                            DataSet         ();
  virtual                  ~DataSet         ();


 protected:

  GLfloat                   lnWidth_;
  Color                     lnColor_;
  Ref<PntStyle>             ptStyle_;
  idx_t                     maxPoints_;
  idx_t                     baseNumber_;

};





//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class XYPlot
//=======================================================================

//-----------------------------------------------------------------------
//   size
//-----------------------------------------------------------------------


inline Size2f XYPlot::size () const
{
  return Size2f( width_, height_ );
}


//-----------------------------------------------------------------------
//   getOptions
//-----------------------------------------------------------------------


inline int XYPlot::getOptions () const
{
  return options_;
}


//-----------------------------------------------------------------------
//   getKey[X|Y]Pos
//-----------------------------------------------------------------------


inline GLfloat XYPlot::getKeyXPos () const
{
  return keyPos_[0];
}


inline GLfloat XYPlot::getKeyYPos () const
{
  return keyPos_[1];
}


//-----------------------------------------------------------------------
//   get[X|Y]AutoScale
//-----------------------------------------------------------------------


inline bool XYPlot::getXAutoScale () const
{
  return xAutoScale_;
}


inline bool XYPlot::getYAutoScale () const
{
  return yAutoScale_;
}


//-----------------------------------------------------------------------
//   getScaleStep
//-----------------------------------------------------------------------


inline double XYPlot::getScaleStep () const
{
  return scaleStep_;
}


//-----------------------------------------------------------------------
//   adjustRange
//-----------------------------------------------------------------------


inline void XYPlot::adjustRange ( Range& r ) const
{
  adjustRange_ ( r, scaleStep_ );
}


//=======================================================================
//   class XYPlot::DataSet
//=======================================================================

//-----------------------------------------------------------------------
//   getMaxPoints
//-----------------------------------------------------------------------


inline idx_t XYPlot::DataSet::getMaxPoints () const
{
  return maxPoints_;
}


//-----------------------------------------------------------------------
//   getLineWidth
//-----------------------------------------------------------------------


inline GLfloat XYPlot::DataSet::getLineWidth () const
{
  return lnWidth_;
}


//-----------------------------------------------------------------------
//   getLineColor
//-----------------------------------------------------------------------


inline Color XYPlot::DataSet::getLineColor () const
{
  return lnColor_;
}


//-----------------------------------------------------------------------
//   getPointStyle
//-----------------------------------------------------------------------


inline MarkerStyle* XYPlot::DataSet::getPointStyle () const
{
  return ptStyle_.get ();
}


//-----------------------------------------------------------------------
//   getBaseNumber
//-----------------------------------------------------------------------


inline idx_t XYPlot::DataSet::getBaseNumber () const
{
  return baseNumber_;
}


JEM_END_PACKAGE( gl )

#endif
