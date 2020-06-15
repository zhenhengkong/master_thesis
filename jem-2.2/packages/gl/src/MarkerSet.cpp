
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
#include <jem/util/Flex.h>
#include <jem/util/ObjFlex.h>
#include <jem/util/Event.h>
#include <jem/util/Properties.h>
#include <jem/util/FastAllocator.h>
#include <jem/gl/error.h>
#include <jem/gl/utilities.h>
#include <jem/gl/Box.h>
#include <jem/gl/Menu.h>
#include <jem/gl/Color.h>
#include <jem/gl/Commands.h>
#include <jem/gl/VertexSet.h>
#include <jem/gl/MarkerStyle.h>
#include <jem/gl/MarkerSet.h>


JEM_DEFINE_CLASS( jem::gl::MarkerSet );


JEM_BEGIN_PACKAGE( gl )


using jem::util::Flex;
using jem::util::ObjFlex;
using jem::util::FastAllocator;


//=======================================================================
//   class MarkerSet::Marker_
//=======================================================================


class MarkerSet::Marker_
{
 public:

  idx_t                     ivx;
  idx_t                     tag;

  Style_*                   style;

  Marker_*                  next;
  Marker_*                  prev;
  Marker_*                  down;

};


//=======================================================================
//   MarkerSet::Style_
//=======================================================================


class MarkerSet::Style_ : public Object
{
 public:

  explicit inline           Style_

    ( const String&           name,
      const Ref<Style>&       style );


 public:

  String                    name;
  Ref<Style>                style;
  idx_t                     count;
  Marker_                   markerList;

};


//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------


inline MarkerSet::Style_::Style_

  ( const String&      n,
    const Ref<Style>&  s ) :

    name  ( n ),
    style ( s )

{
  count             =   0;

  markerList.ivx    =  -1;
  markerList.tag    =   0;
  markerList.style  =   this;
  markerList.next   = & markerList;
  markerList.prev   = & markerList;
  markerList.down   =   NULL;
}


//=======================================================================
//   class MarkerSet::Data_
//=======================================================================


class MarkerSet::Data_ : public Collectable
{
 public:

  explicit inline           Data_

    ( const Ref<VertexSet>&   vx );

  void                      redraw1D

    ( GfxContext&             gtx )          const;

  void                      redraw2D

    ( GfxContext&             gtx )          const;

  void                      redraw3D

    ( GfxContext&             gtx )          const;

  void                      drawFast1D

    ( double                  lod )          const;

  void                      drawFast2D

    ( double                  lod )          const;

  void                      drawFast3D

    ( double                  lod )          const;

  inline Style_&            getStyle

    ( idx_t                   istyle )       const;

  inline void               checkStyle

    ( idx_t                   istyle )       const;

  inline void               checkVertex

    ( idx_t                   ivx )          const;

  inline void               addMarker

    ( idx_t                   ivx,
      Style_*                 style,
      idx_t                   tag );

  inline void               eraseMarker

    ( idx_t                   ivx,
      Style_*                 style );

  inline void               eraseMarkers

    ( idx_t                   ivx );

  void                      eraseMarkers  ();
  void                      syncMarkers   ();


 public:

  Ref<VertexSet>            vertices;
  ObjFlex                   styles;
  Flex<Marker_*>            markers;


 protected:

  virtual                  ~Data_         ();


 private:

  void                      newMarker_

    ( idx_t                   ivx,
      Style_*                 style,
      idx_t                   tag );

  void                      styleError_

    ( idx_t                   istyle )       const;

  void                      vertexError_

    ( idx_t                    ivx )          const;


 private:

  FastAllocator             allocator_;

};


//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


inline MarkerSet::Data_::Data_ ( const Ref<VertexSet>& vx ) :

  vertices   ( vx ),
  allocator_ ( sizeof(Marker_) )

{
  markers.resize ( vx->size() );

  markers = (Marker_*) NULL;
}


MarkerSet::Data_::~Data_ ()
{
  eraseMarkers ();
}


//-----------------------------------------------------------------------
//   redraw1D
//-----------------------------------------------------------------------


void MarkerSet::Data_::redraw1D ( GfxContext& gtx ) const
{
  const GLfloat*  coords = vertices->getData ();

  Style*          style;
  Marker_*        list;
  Marker_*        marker;
  Box             box;
  GLfloat         pos[3];


  vertices->getBBox ( box );

  pos[1] = pos[2] = 0.0;

  for ( idx_t i = 0; i < styles.size(); i++ )
  {
    Style_&  s = getStyle(i);

    list  = & s.markerList;
    style = s.style.get ();

    style->beginDraw ( gtx, box, s.count );

    for ( marker  = list->next;
          marker != list;
          marker  = marker->next )
    {
      pos[0] = coords[marker->ivx];

      style->drawMarker ( marker->tag, pos );
    }

    style->endDraw ();
  }
}


//-----------------------------------------------------------------------
//   redraw2D
//-----------------------------------------------------------------------


void MarkerSet::Data_::redraw2D ( GfxContext& gtx ) const
{
  const GLfloat*  coords = vertices->getData ();

  Style*          style;
  Marker_*        list;
  Marker_*        marker;
  Box             box;
  GLfloat         pos[3];


  vertices->getBBox ( box );

  pos[2] = 0.0;

  for ( idx_t i = 0; i < styles.size(); i++ )
  {
    Style_&  s = getStyle(i);

    list  = & s.markerList;
    style = s.style.get ();

    style->beginDraw ( gtx, box, s.count );

    for ( marker  = list->next;
          marker != list;
          marker  = marker->next )
    {
      idx_t  ivx = marker->ivx;

      pos[0] = coords[2 * ivx + 0];
      pos[1] = coords[2 * ivx + 1];

      style->drawMarker ( marker->tag, pos );
    }

    style->endDraw ();
  }
}


//-----------------------------------------------------------------------
//   redraw3D
//-----------------------------------------------------------------------


void MarkerSet::Data_::redraw3D ( GfxContext& gtx ) const
{
  const GLfloat*  coords = vertices->getData ();
  const idx_t     rank   = vertices->rank    ();

  Style*          style;
  Marker_*        list;
  Marker_*        marker;
  Box             box;
  const GLfloat*  pos;


  vertices->getBBox ( box );

  for ( idx_t i = 0; i < styles.size(); i++ )
  {
    Style_&  s = getStyle(i);

    list  = & s.markerList;
    style = s.style.get ();

    style->beginDraw ( gtx, box, s.count );

    for ( marker  = list->next;
          marker != list;
          marker  = marker->next )
    {
      pos = coords + (rank * marker->ivx);

      style->drawMarker ( marker->tag, pos );
    }

    style->endDraw ();
  }
}


//-----------------------------------------------------------------------
//   drawFast1D
//-----------------------------------------------------------------------


void MarkerSet::Data_::drawFast1D ( double lod ) const
{
  const GLfloat*  coords = vertices->getData ();

  const double    stride = 1.0 / lod;

  Marker_*        list;
  Marker_*        marker;
  GLfloat         pos[2];
  idx_t           j, k, p;


  pos[1] = 0.0;

  for ( idx_t i = 0; i < styles.size(); i++ )
  {
    Style_&  s = getStyle(i);

    list = & s.markerList;

    for ( marker  = list->next, j = k = p = 0;
          marker != list;
          marker  = marker->next, j++ )
    {
      if ( j >= p )
      {
        p      = (idx_t) (stride * (double) (++k));
        pos[0] = coords[marker->ivx];

        glVertex2fv ( pos );
      }
    }
  }
}


//-----------------------------------------------------------------------
//   drawFast2D
//-----------------------------------------------------------------------


void MarkerSet::Data_::drawFast2D ( double lod ) const
{
  const GLfloat*  coords = vertices->getData ();

  const double    stride = 1.0 / lod;

  Marker_*        list;
  Marker_*        marker;
  GLfloat         pos[2];
  idx_t           j, k, p;


  for ( idx_t i = 0; i < styles.size(); i++ )
  {
    Style_&  s = getStyle(i);

    list = & s.markerList;

    for ( marker  = list->next, j = k = p = 0;
          marker != list;
          marker  = marker->next, j++ )
    {
      if ( j >= p )
      {
        idx_t  ivx = marker->ivx;

        p      = (idx_t) (stride * (double) (++k));
        pos[0] = coords[2 * ivx + 0];
        pos[1] = coords[2 * ivx + 1];

        glVertex2fv ( pos );
      }
    }
  }
}


//-----------------------------------------------------------------------
//   drawFast3D
//-----------------------------------------------------------------------


void MarkerSet::Data_::drawFast3D ( double lod ) const
{
  const GLfloat*  coords = vertices->getData ();

  const double    stride = 1.0 / lod;
  const idx_t     rank   = vertices->rank    ();

  Marker_*        list;
  Marker_*        marker;
  const GLfloat*  pos;
  idx_t           j, k, p;


  for ( idx_t i = 0; i < styles.size(); i++ )
  {
    Style_&  s = getStyle(i);

    list = & s.markerList;

    for ( marker  = list->next, j = k = p = 0;
          marker != list;
          marker  = marker->next, j++ )
    {
      if ( j >= p )
      {
        p   = (idx_t) (stride * (double) (++k));
        pos = coords + (rank * marker->ivx);

        glVertex3fv ( pos );
      }
    }
  }
}


//-----------------------------------------------------------------------
//   getStyle
//-----------------------------------------------------------------------


inline MarkerSet::Style_&

  MarkerSet::Data_::getStyle ( idx_t istyle ) const

{
  return static_cast<Style_&> ( * styles[istyle] );
}


//-----------------------------------------------------------------------
//   checkStyle
//-----------------------------------------------------------------------


inline void MarkerSet::Data_::checkStyle ( idx_t istyle ) const
{
  if ( istyle < 0 || istyle >= styles.size() )
  {
    styleError_ ( istyle );
  }
}


//-----------------------------------------------------------------------
//   checkVertex
//-----------------------------------------------------------------------


inline void MarkerSet::Data_::checkVertex ( idx_t ivx ) const
{
  if ( ivx < 0 || ivx >= markers.size() )
  {
    vertexError_ ( ivx );
  }
}


//-----------------------------------------------------------------------
//   addMarker
//-----------------------------------------------------------------------


inline void MarkerSet::Data_::addMarker

  ( idx_t    ivx,
    Style_*  style,
    idx_t    tag )

{
  Marker_*  marker = markers[ivx];

  while ( marker )
  {
    if ( marker->style == style )
    {
      marker->tag = tag;
      return;
    }

    marker = marker->down;
  }

  newMarker_ ( ivx, style, tag );
}


//-----------------------------------------------------------------------
//   eraseMarker
//-----------------------------------------------------------------------


inline void MarkerSet::Data_::eraseMarker

  ( idx_t    ivx,
    Style_*  style )

{
  Marker_*  marker = markers[ivx];
  Marker_*  prev   = NULL;


  while ( marker )
  {
    if ( marker->style == style )
    {
      if ( prev )
      {
        prev->down = marker->down;
      }
      else
      {
        markers[ivx] = marker->down;
      }

      marker->next->prev = marker->prev;
      marker->prev->next = marker->next;

      allocator_.dealloc ( marker );

      style->count--;

      break;
    }

    prev   = marker;
    marker = marker->down;
  }
}


//-----------------------------------------------------------------------
//   eraseMarkers
//-----------------------------------------------------------------------


inline void MarkerSet::Data_::eraseMarkers ( idx_t ivx )
{
  Marker_*  marker = markers[ivx];

  while ( marker )
  {
    marker->next->prev = marker->prev;
    marker->prev->next = marker->next;

    markers[ivx]       = marker->next;

    marker->style->count--;

    allocator_.dealloc ( marker );

    marker = markers[ivx];
  }
}


void MarkerSet::Data_::eraseMarkers ()
{
  Marker_*   list;
  Marker_*   marker;


  markers = (Marker_*) NULL;

  for ( idx_t i = 0; i < styles.size(); i++ )
  {
    Style_&  s = getStyle(i);

    list = & s.markerList;

    while ( list->next != list )
    {
      marker     = list->next;
      list->next = marker->next;

      allocator_.dealloc ( marker );
    }

    s.count    = 0;
    list->next = list;
    list->prev = list;
  }
}


//-----------------------------------------------------------------------
//   syncMarkers
//-----------------------------------------------------------------------


inline void MarkerSet::Data_::syncMarkers ()
{
  idx_t  n = vertices->size() - markers.size();

  if      ( n > 0 )
  {
    markers.pushBack ( (Marker_*) NULL, n );
  }
  else if ( n < 0 )
  {
    if ( vertices->size() == 0 )
    {
      eraseMarkers ();
    }
    else
    {
      n = -n;

      for ( idx_t i = n; i < markers.size(); i++ )
      {
        eraseMarkers ( i );
      }

      markers.popBack ( n );
    }

    markers.trimToSize ();
  }
}


//-----------------------------------------------------------------------
//   newMarker_
//-----------------------------------------------------------------------


void MarkerSet::Data_::newMarker_

  ( idx_t    ivx,
    Style_*  style,
    idx_t    tag )

{
  Marker_*  marker = (Marker_*) allocator_ .alloc ();
  Marker_*  list   = & style->markerList;


  marker->ivx      = ivx;
  marker->style    = style;
  marker->tag      = tag;

  marker->next     = list->next;
  marker->prev     = list;
  list->next       = marker;
  list->next->prev = marker;

  marker->down     = markers[ivx];
  markers[ivx]     = marker;

  style->count++;
}


//-----------------------------------------------------------------------
//   styleError_
//-----------------------------------------------------------------------


void MarkerSet::Data_::styleError_ ( idx_t istyle ) const
{
  String  where = String::format ( "class `%s\'", CLASS_NAME );

  indexError ( where, "style", istyle, styles.size() );
}


//-----------------------------------------------------------------------
//   vertexError_
//-----------------------------------------------------------------------


void MarkerSet::Data_::vertexError_ ( idx_t ivx ) const
{
  String  where = String::format ( "class `%s\'", CLASS_NAME );

  indexError ( where, "vertex", ivx, vertices->size() );
}



//=======================================================================
//   class MarkerSet
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


MarkerSet::MarkerSet

  ( const String&          name,
    const Ref<VertexSet>&  vx ) :

    Super ( name )

{
  using jem::util::connect;

  JEM_PRECHECK ( vx != NIL );

  hidden = false;
  data_  = newInstance<Data_> ( vx );

  connect ( vx->newShapeEvent, this, & Self::shapeChanged_ );
}


MarkerSet::~MarkerSet ()
{}


//-----------------------------------------------------------------------
//   getBBox
//-----------------------------------------------------------------------


void MarkerSet::getBBox ( Box& box ) const
{
  data_->vertices->getBBox ( box );
}


//-----------------------------------------------------------------------
//   redraw
//-----------------------------------------------------------------------


void MarkerSet::redraw ( GfxContext& gtx )
{
  Data_&  d = * data_;

  switch ( d.vertices->rank() )
  {
  case 0:

    break;

  case 1:

    d.redraw1D ( gtx );

    break;

  case 2:

    d.redraw2D ( gtx );

    break;

  default:

    d.redraw3D ( gtx );
  }
}


//-----------------------------------------------------------------------
//   redrawFast
//-----------------------------------------------------------------------


void MarkerSet::redrawFast

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

  Data_&        d = * data_;

  DisableScope  blendScope ( GL_BLEND    );
  DisableScope  lightScope ( GL_LIGHTING );

  GLfloat       ptSize;
  Color         col;


  getCurrent  ( col );
  setCurrent  ( Color::black() );
  glGetFloatv ( GL_POINT_SIZE, & ptSize );
  glPointSize ( 3.0 );
  glBegin     ( GL_POINTS );

  switch ( d.vertices->rank() )
  {
  case 0:

    break;

  case 1:

    d.drawFast1D ( lod );
    break;

  case 2:

    d.drawFast2D ( lod );
    break;

  default:

    d.drawFast3D ( lod );
  }

  glEnd       ();
  setCurrent  ( col );
  glPointSize ( ptSize );
}


//-----------------------------------------------------------------------
//   exeCommand
//-----------------------------------------------------------------------


bool MarkerSet::exeCommand

  ( GfxContext&        gtx,
    const String&      cmd,
    const Properties&  params )

{
  if ( cmd == Commands::HIDE_MARKERS )
  {
    hidden = true;
    return   true;
  }

  if ( cmd == Commands::SHOW_MARKERS )
  {
    hidden = false;
    return   true;
  }

  return false;
}


//-----------------------------------------------------------------------
//   listOptions
//-----------------------------------------------------------------------


void MarkerSet::listOptions ( Menu& menu ) const
{
  menu.addOption ( Commands::HIDE_MARKERS,
                   Commands::SHOW_MARKERS, hidden );
}


//-----------------------------------------------------------------------
//   setFont
//-----------------------------------------------------------------------


bool MarkerSet::setFont ()
{
  Data_&  d = * data_;

  for ( idx_t i = 0; i < d.styles.size(); i++ )
  {
    d.getStyle(i).style->setFont ();
  }

  return true;
}


//-----------------------------------------------------------------------
//   styleCount
//-----------------------------------------------------------------------


idx_t MarkerSet::styleCount () const
{
  return (idx_t) data_->styles.size ();
}


//-----------------------------------------------------------------------
//   addStyle
//-----------------------------------------------------------------------


idx_t MarkerSet::addStyle

  ( const String&      name,
    const Ref<Style>&  style )

{
  JEM_PRECHECK ( style != NIL );

  Data_&  d = * data_;
  idx_t   i;

  for ( i = 0; i < d.styles.size(); i++ )
  {
    Style_&  s = d.getStyle(i);

    if ( s.name == name )
    {
      s.style = style;

      return i;
    }
  }

  d.styles.pushBack ( newInstance<Style_>( name, style ) );

  return i;
}


//-----------------------------------------------------------------------
//   findStyle
//-----------------------------------------------------------------------


idx_t MarkerSet::findStyle ( const String& name ) const
{
  Data_&  d = * data_;

  for ( idx_t i = 0; i < d.styles.size(); i++ )
  {
    Style_&  s = d.getStyle(i);

    if ( s.name == name )
    {
      return i;
    }
  }

  return -1;
}


//-----------------------------------------------------------------------
//   getStyle
//-----------------------------------------------------------------------


MarkerStyle* MarkerSet::getStyle ( idx_t istyle ) const
{
  Data_&   d = * data_;

  d.checkStyle ( istyle );

  return d.getStyle(istyle).style.get ();
}


//-----------------------------------------------------------------------
//   setStyle
//-----------------------------------------------------------------------


void MarkerSet::setStyle

  ( idx_t              istyle,
    const Ref<Style>&  style )

{
  Data_&   d = * data_;

  d.checkStyle ( istyle );

  d.getStyle(istyle).style = style;
}


//-----------------------------------------------------------------------
//   getStyleName
//-----------------------------------------------------------------------


String MarkerSet::getStyleName ( idx_t istyle ) const
{
  data_->checkStyle ( istyle );

  return data_->getStyle(istyle).name;
}


//-----------------------------------------------------------------------
//   eraseStyle
//-----------------------------------------------------------------------


void MarkerSet::eraseStyle ( const String& name )
{
  Data_&    d = * data_;
  idx_t     n = d.styles.size ();

  Style_*   style;
  Marker_*  list;


  for ( idx_t i = 0; i < n; i++ )
  {
    style = & d.getStyle(i);

    if ( style->name == name )
    {
      list = & style->markerList;

      while ( list->next != list )
      {
        d.eraseMarker ( list->next->ivx, style );
      }

      style = NULL;

      if ( i < n - 1 )
      {
        d.styles.swap ( i, n - 1 );
      }

      d.styles.popBack ();

      return;
    }
  }
}


//-----------------------------------------------------------------------
//   markVertex
//-----------------------------------------------------------------------


void MarkerSet::markVertex

  ( idx_t  ivx,
    idx_t  istyle,
    idx_t  tag )

{
  Data_&  d = * data_;

  d.checkStyle  ( istyle );
  d.checkVertex ( ivx    );
  d.addMarker   ( ivx, & d.getStyle(istyle), tag );
}


//-----------------------------------------------------------------------
//   markVertices
//-----------------------------------------------------------------------


void MarkerSet::markVertices

  ( const IdxVector&  ivx,
    idx_t             istyle,
    idx_t             tag )

{
  const idx_t  n = ivx.size ();
  Data_&       d = * data_;

  Style_*      style;


  d.checkStyle ( istyle );

  style = & d.getStyle(istyle);

  for ( idx_t i = 0; i < n; i++ )
  {
    d.checkVertex ( ivx[i] );
    d.addMarker   ( ivx[i], style, tag );
  }
}


void MarkerSet::markVertices

  ( const IdxVector&  ivx,
    idx_t             istyle,
    const IdxVector&  tags )

{
  JEM_PRECHECK ( ivx.size() == tags.size() );

  const idx_t  n = ivx.size ();
  Data_&       d = * data_;

  Style_*      style;


  d.checkStyle ( istyle );

  style = & d.getStyle(istyle);

  for ( idx_t i = 0; i < n; i++ )
  {
    d.checkVertex ( ivx[i] );
    d.addMarker   ( ivx[i], style, tags[i] );
  }
}


void MarkerSet::markVertices

  ( const IdxVector&  ivx,
    const IdxVector&  istyles,
    const IdxVector&  tags )

{
  JEM_PRECHECK ( ivx.size() == istyles.size() &&
                 ivx.size() == tags   .size() );

  const idx_t  n = ivx.size ();
  Data_&       d = * data_;


  for ( idx_t i = 0; i < n; i++ )
  {
    d.checkStyle  ( istyles[i] );
    d.checkVertex ( ivx[i]     );
    d.addMarker   ( ivx[i], & d.getStyle(istyles[i]), tags[i] );
  }
}


//-----------------------------------------------------------------------
//   unmarkVertex
//-----------------------------------------------------------------------


void MarkerSet::unmarkVertex

  ( idx_t  ivx,
    idx_t  istyle )

{
  Data_&  d = * data_;

  d.checkVertex ( ivx );

  if ( istyle < 0 )
  {
    d.eraseMarkers ( ivx );
  }
  else
  {
    d.checkStyle   ( istyle );
    d.eraseMarker  ( ivx, & d.getStyle(istyle) );
  }
}


//-----------------------------------------------------------------------
//   unmarkVertices
//-----------------------------------------------------------------------


void MarkerSet::unmarkVertices

  ( const IdxVector&  ivx,
    idx_t             istyle )

{
  const idx_t  n = ivx.size ();
  Data_&       d = * data_;

  Style_*      style;


  if ( istyle < 0 )
  {
    for ( idx_t i = 0; i < n; i++ )
    {
      d.checkVertex  ( ivx[i] );
      d.eraseMarkers ( ivx[i] );
    }
  }
  else
  {
    d.checkStyle ( istyle );

    style = & d.getStyle(istyle);

    for ( idx_t i = 0; i < n; i++ )
    {
      d.checkVertex ( ivx[i] );
      d.eraseMarker ( ivx[i], style );
    }
  }
}


//-----------------------------------------------------------------------
//   unmarkAll
//-----------------------------------------------------------------------


void MarkerSet::unmarkAll ()
{
  data_->eraseMarkers ();
}


//-----------------------------------------------------------------------
//   shapeChanged_
//-----------------------------------------------------------------------


void MarkerSet::shapeChanged_ ()
{
  data_->syncMarkers ();
}


JEM_END_PACKAGE( gl )
