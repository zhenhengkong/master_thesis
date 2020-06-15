
/*
 *  Copyright (C) 2016 DRG. All rights reserved.
 *
 *  This file is part of Jive, an object oriented toolkit for solving
 *  partial differential equations.
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
 *  This file is part of Jive, an object oriented toolkit for
 *  solving partial differential equations.
 *
 *  Jive version: 2.2
 *  Date:         Thu 28 Jan 10:31:15 CET 2016
 */


#include <jem/base/ClassTemplate.h>
#include <jem/util/Properties.h>
#include <jive/gl/MeshAttribs.h>


JEM_DEFINE_CLASS( jive::gl::MeshAttribs );


JIVE_BEGIN_PACKAGE( gl )


using jem::newInstance;


//=======================================================================
//   class MeshAttribs
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  MeshAttribs::EDGE_COLOR_NAME = "edgeColor";
const char*  MeshAttribs::NODE_COLOR_NAME = "nodeColor";
const char*  MeshAttribs::SHOW_EDGES_NAME = "showEdges";
const char*  MeshAttribs::SHOW_LINES_NAME = "showLines";
const char*  MeshAttribs::SHOW_FACES_NAME = "showFaces";


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


MeshAttribs::MeshAttribs ()
{
  options_   = SHOW_EDGES | SHOW_LINES | SHOW_FACES;
  edgeColor_ = Color::black ();
  nodeColor_ = edgeColor_;
}


MeshAttribs::MeshAttribs ( const Self& rhs ) :

  Super ( rhs )

{
  options_   = rhs.options_;
  edgeColor_ = rhs.edgeColor_;
  nodeColor_ = rhs.nodeColor_;
}


MeshAttribs::~MeshAttribs ()
{}


//-----------------------------------------------------------------------
//   clone
//-----------------------------------------------------------------------


Ref<Object> MeshAttribs::clone () const
{
  return newInstance<Self> ( *this );
}


//-----------------------------------------------------------------------
//   copyFrom
//-----------------------------------------------------------------------


int MeshAttribs::copyFrom ( const Properties& props )
{
  const int    OPT_BITS [3] = { SHOW_EDGES,
                                SHOW_LINES,
                                SHOW_FACES };

  const char*  OPT_NAMES[3] = { SHOW_EDGES_NAME,
                                SHOW_LINES_NAME,
                                SHOW_FACES_NAME };

  int  suFields = Super::copyFrom ( props );
  int  myFields = 0;


  if ( edgeColor_.configure( EDGE_COLOR_NAME, props ) )
  {
    myFields |= EDGE_COLOR;
  }

  if ( nodeColor_.configure( NODE_COLOR_NAME, props ) )
  {
    myFields |= NODE_COLOR;
  }

  for ( int i = 0; i < 3; i++ )
  {
    bool  yesno;

    if ( props.find( yesno, OPT_NAMES[i] ) )
    {
      if ( yesno )
      {
        options_ |=  OPT_BITS[i];
      }
      else
      {
        options_ &= ~OPT_BITS[i];
      }

      myFields |= OPT_BITS[i];
    }
  }

  if ( myFields )
  {
    newValuesEvent.emit ( myFields, *this );
  }

  return (suFields | myFields);
}


//-----------------------------------------------------------------------
//   copyTo
//-----------------------------------------------------------------------


void MeshAttribs::copyTo

  ( const Properties&  props,
    int                fields ) const

{
  const int    OPT_BITS [3] = { SHOW_EDGES,
                                SHOW_LINES,
                                SHOW_FACES };

  const char*  OPT_NAMES[3] = { SHOW_EDGES_NAME,
                                SHOW_LINES_NAME,
                                SHOW_FACES_NAME };


  Super::copyTo ( props, fields );

  if ( fields & EDGE_COLOR )
  {
    edgeColor_.getConfig ( EDGE_COLOR_NAME, props );
  }

  if ( fields & NODE_COLOR )
  {
    nodeColor_.getConfig ( NODE_COLOR_NAME, props );
  }

  for ( int i = 0; i < 3; i++ )
  {
    if ( fields & OPT_BITS[i] )
    {
      bool  yesno = ((options_ & OPT_BITS[i]) != 0);

      props.set ( OPT_NAMES[i], yesno );
    }
  }
}


//-----------------------------------------------------------------------
//   setDefaults
//-----------------------------------------------------------------------


void MeshAttribs::setDefaults ()
{
  int  fields;

  Super::setDefaults ();

  options_   = SHOW_EDGES | SHOW_LINES | SHOW_FACES;
  edgeColor_ = Color::black ();
  nodeColor_ = edgeColor_;

  fields = (EDGE_COLOR | NODE_COLOR | options_);

  newValuesEvent.emit ( fields, *this );
}


//-----------------------------------------------------------------------
//   setOption
//-----------------------------------------------------------------------


void MeshAttribs::setOption

  ( int   what,
    bool  yesno )

{
  int  oldOptions = options_;

  if ( yesno )
  {
    options_ |=  what;
  }
  else
  {
    options_ &= ~what;
  }

  if ( options_ != oldOptions )
  {
    newValuesEvent.emit ( options_ ^ oldOptions, *this );
  }
}


//-----------------------------------------------------------------------
//   setEdgeColor
//-----------------------------------------------------------------------


void MeshAttribs::setEdgeColor ( const Color& color )
{
  edgeColor_ = color;

  newValuesEvent.emit ( EDGE_COLOR, *this );
}


//-----------------------------------------------------------------------
//   setNodeColor
//-----------------------------------------------------------------------


void MeshAttribs::setNodeColor ( const Color& color )
{
  nodeColor_ = color;

  newValuesEvent.emit ( NODE_COLOR, *this );
}


JIVE_END_PACKAGE( gl )
