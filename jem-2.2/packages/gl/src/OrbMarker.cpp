
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
#include <jem/gl/Box.h>
#include <jem/gl/Orb.h>
#include <jem/gl/OrbMarker.h>


JEM_DEFINE_CLASS( jem::gl::OrbMarker );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class OrbMarker
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


OrbMarker::OrbMarker

  ( GLfloat       size,
    const Color&  color )

{
  size_    =  size;
  color_   =  clamped ( color );
  quality_ =  1;
  radius_  = -1.0F;
}


OrbMarker::~OrbMarker ()
{}


//-----------------------------------------------------------------------
//   toString
//-----------------------------------------------------------------------


String OrbMarker::toString () const
{
  return "orb";
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void OrbMarker::configure ( const Properties& props )
{
  props.find ( size_,    PropNames::SIZE );
  props.find ( quality_, PropNames::QUALITY, 0, Orb::MAX_QUALITY );

  color_.configure ( PropNames::COLOR, props );
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void OrbMarker::getConfig ( const Properties& conf ) const
{
  conf.set ( PropNames::SIZE,    size_    );
  conf.set ( PropNames::QUALITY, quality_ );

  color_.getConfig ( PropNames::COLOR, conf );
}


//-----------------------------------------------------------------------
//   listProps
//-----------------------------------------------------------------------


void OrbMarker::listProps ( const Properties& conf ) const
{
  conf.set ( PropNames::SIZE,
             "The size of the orb markers. "
             "A negative size is relative to the size of the object "
             "for which the markers are drawn" );
  conf.set ( PropNames::QUALITY,
             "The quality of the orbs. The higher this number, the "
             "more triangles are used to draw an orb" );
  conf.set ( PropNames::COLOR,
             "The color of the orbs" );
}


//-----------------------------------------------------------------------
//   beginDraw
//-----------------------------------------------------------------------


void OrbMarker::beginDraw

  ( GfxContext&  gtx,
    const Box&   box,
    idx_t        count )

{
  if      ( size_ < 0.0 )
  {
    radius_ = (GLfloat) (-size_ * box.size());
  }
  else if ( size_ > 0.0 )
  {
    radius_ = size_;
  }
  else
  {
    radius_ = -1.0F;
    orb_    =  NIL;

    return;
  }

  blend_ = glIsEnabled ( GL_BLEND );

  if ( blend_ == GL_TRUE && color_.isOpaque() )
  {
    glDisable ( GL_BLEND );
  }

  getCurrent ( color0_ );
  setCurrent ( color_  );

  if ( orb_ == NIL )
  {
    orb_ = newInstance<Orb> ( quality_ );
  }
  else
  {
    orb_->setQuality ( quality_ );
  }
}


//-----------------------------------------------------------------------
//   endDraw
//-----------------------------------------------------------------------


void OrbMarker::endDraw ()
{
  if ( orb_ != NIL )
  {
    setCurrent ( color0_ );

    if ( blend_ == GL_TRUE )
    {
      glEnable ( GL_BLEND );
    }
  }
}


//-----------------------------------------------------------------------
//   drawMarker
//-----------------------------------------------------------------------


void OrbMarker::drawMarker

  ( idx_t           tag,
    const GLfloat*  pos )

{
  if ( orb_ != NIL )
  {
    glPushMatrix  ();
    glTranslatef  ( pos[0], pos[1], pos[2] );
    orb_->drawOrb ( radius_ );
    glPopMatrix   ();
  }
}


//-----------------------------------------------------------------------
//   setColor
//-----------------------------------------------------------------------


void OrbMarker::setColor ( const Color& color )
{
  color_ = clamped ( color );
}


//-----------------------------------------------------------------------
//   getColor
//-----------------------------------------------------------------------


Color OrbMarker::getColor () const
{
  return color_;
}


//-----------------------------------------------------------------------
//   setSize
//-----------------------------------------------------------------------


void OrbMarker::setSize ( GLfloat size )
{
  size_ = size;
}


//-----------------------------------------------------------------------
//   setQuality
//-----------------------------------------------------------------------


void OrbMarker::setQuality ( int level )
{
  quality_ = clamped ( level, 0, Orb::MAX_QUALITY );
}


JEM_END_PACKAGE( gl )
