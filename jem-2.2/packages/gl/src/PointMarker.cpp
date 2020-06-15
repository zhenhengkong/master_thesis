
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


#include <jem/base/ClassTemplate.h>
#include <jem/util/Properties.h>
#include <jem/gl/utilities.h>
#include <jem/gl/PropertyNames.h>
#include <jem/gl/PointMarker.h>


JEM_DEFINE_CLASS( jem::gl::PointMarker );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class PointMarker
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const GLfloat  PointMarker::MAX_SIZE = 10.0;


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


PointMarker::PointMarker

  ( GLfloat       size,
    const Color&  color )

{
  size_   = clamped ( size, MAX_SIZE );
  color_  = clamped ( color );
  light_  = GL_FALSE;
  size0_  = 0.0;
  active_ = false;
}


PointMarker::~PointMarker ()
{}


//-----------------------------------------------------------------------
//   toString
//-----------------------------------------------------------------------


String PointMarker::toString () const
{
  return "point";
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void PointMarker::configure ( const Properties& props )
{
  double  sz;

  if ( props.find( sz, PropNames::SIZE, 0.0, (double) MAX_SIZE ) )
  {
    size_ = (GLfloat) sz;
  }

  color_.configure ( PropNames::COLOR, props );
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void PointMarker::getConfig ( const Properties& conf ) const
{
  conf.set ( PropNames::SIZE, (double) size_ );

  color_.getConfig ( PropNames::COLOR, conf );
}


//-----------------------------------------------------------------------
//   listProps
//-----------------------------------------------------------------------


void PointMarker::listProps ( const Properties& conf ) const
{
  conf.set ( PropNames::SIZE,
             "The size (int pixels) of the point markers" );
  conf.set ( PropNames::COLOR,
             "The color of the point markers" );
}


//-----------------------------------------------------------------------
//   beginDraw
//-----------------------------------------------------------------------


void PointMarker::beginDraw

  ( GfxContext&  gtx,
    const Box&   box,
    idx_t        count )

{
  if ( size_ > 0.1 )
  {
    light_ = glIsEnabled ( GL_LIGHTING );

    if ( light_ == GL_TRUE )
    {
      glDisable ( GL_LIGHTING );
    }

    blend_ = glIsEnabled ( GL_BLEND );

    if ( blend_ == GL_TRUE && color_.isOpaque() )
    {
      glDisable ( GL_BLEND );
    }

    glGetFloatv ( GL_POINT_SIZE, & size0_ );
    glPointSize ( size_     );
    getCurrent  ( color0_   );
    setCurrent  ( color_    );
    glBegin     ( GL_POINTS );

    active_ = true;
  }
  else
  {
    active_ = false;
  }
}


//-----------------------------------------------------------------------
//   endDraw
//-----------------------------------------------------------------------


void PointMarker::endDraw ()
{
  if ( active_ )
  {
    glEnd       ();
    setCurrent  ( color0_ );
    glPointSize ( size0_  );

    if ( light_ == GL_TRUE )
    {
      glEnable ( GL_LIGHTING );
    }

    if ( blend_ == GL_TRUE )
    {
      glEnable ( GL_BLEND );
    }
  }
}


//-----------------------------------------------------------------------
//   drawMarker
//-----------------------------------------------------------------------


void PointMarker::drawMarker

  ( idx_t           tag,
    const GLfloat*  pos )

{
  if ( active_ )
  {
    glVertex3fv ( pos );
  }
}


//-----------------------------------------------------------------------
//   setColor
//-----------------------------------------------------------------------


void PointMarker::setColor ( const Color& color )
{
  color_ = clamped ( color );
}


//-----------------------------------------------------------------------
//   getColor
//-----------------------------------------------------------------------


Color PointMarker::getColor () const
{
  return color_;
}


//-----------------------------------------------------------------------
//   setSize
//-----------------------------------------------------------------------


void PointMarker::setSize ( GLfloat size )
{
  size_ = clamped ( size, MAX_SIZE );
}


JEM_END_PACKAGE( gl )
