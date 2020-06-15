
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
#include <jem/base/ClassTemplate.h>
#include <jem/util/Properties.h>
#include <jem/gl/utilities.h>
#include <jem/gl/Box.h>
#include <jem/gl/PropertyNames.h>
#include <jem/gl/TeeMarker.h>


JEM_DEFINE_CLASS( jem::gl::TeeMarker );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class TeeMarker
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const byte  TeeMarker::INACTIVE_   =    0x0;
const byte  TeeMarker::DRAW_QUADS_ = 1 << 0;
const byte  TeeMarker::DRAW_LINES_ = 1 << 1;


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


TeeMarker::TeeMarker

  ( GLfloat       size,
    const Color&  color )

{
  size_    = size;
  angle_   = 0.0F;
  color_   = clamped ( color );

  geom_[0] = 0.0F;
  geom_[1] = 0.0F;
  lwidth_  = 0.0F;
  light_   = GL_FALSE;
  state_   = INACTIVE_;
}


TeeMarker::~TeeMarker ()
{}


//-----------------------------------------------------------------------
//   toString
//-----------------------------------------------------------------------


String TeeMarker::toString () const
{
  return "tee";
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void TeeMarker::configure ( const Properties& props )
{
  props.find ( size_,  PropNames::SIZE  );
  props.find ( angle_, PropNames::ANGLE );

  color_.configure ( PropNames::COLOR, props );
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void TeeMarker::getConfig ( const Properties& conf ) const
{
  conf.set ( PropNames::SIZE,  size_  );
  conf.set ( PropNames::ANGLE, angle_ );

  color_.getConfig ( PropNames::COLOR, conf  );
}


//-----------------------------------------------------------------------
//   listProps
//-----------------------------------------------------------------------


void TeeMarker::listProps ( const Properties& conf ) const
{
  conf.set ( PropNames::SIZE,
             "The size of the tee markers. A negative size is "
             "relative to the object for which the markers are "
             "drawn" );
  conf.set ( PropNames::ANGLE,
             "The rotation angle (in degrees) around the z-axis "
             "(in body coordinates)" );
  conf.set ( PropNames::COLOR,
             "The color of the markers" );
}


//-----------------------------------------------------------------------
//   beginDraw
//-----------------------------------------------------------------------


void TeeMarker::beginDraw

  ( GfxContext&  gtx,
    const Box&   box,
    idx_t        count )

{
  double  alpha;
  double  len;


  state_ = INACTIVE_;

  if      ( size_ < 0.0 )
  {
    len = -size_ * box.size ();
  }
  else if ( size_ > 0.0 )
  {
    len =  size_;
  }
  else
  {
    return;
  }

  alpha    = angle_ * 3.141592653589793 / 180.0;
  geom_[0] = (GLfloat) (0.5 * len * std::cos( alpha ) );
  geom_[1] = (GLfloat) (0.5 * len * std::sin( alpha ) );
  state_  |= DRAW_QUADS_;

  getCurrent ( color0_ );
  setCurrent ( color_  );

  if ( count > 1000 )
  {
    lwidth_ = 0.0F;
  }
  else
  {
    glGetFloatv ( GL_LINE_WIDTH, &lwidth_ );
    glLineWidth ( 1.0F );

    state_ |= DRAW_LINES_;
  }

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
}


//-----------------------------------------------------------------------
//   endDraw
//-----------------------------------------------------------------------


void TeeMarker::endDraw ()
{
  if ( state_ )
  {
    setCurrent ( color0_ );

    if ( state_ & DRAW_LINES_ )
    {
      glLineWidth ( lwidth_ );
    }

    if ( light_ == GL_TRUE )
    {
      glEnable ( GL_LIGHTING );
    }

    if ( blend_ == GL_TRUE )
    {
      glEnable ( GL_BLEND );
    }

    state_ = INACTIVE_;
  }
}


//-----------------------------------------------------------------------
//   drawMarker
//-----------------------------------------------------------------------


void TeeMarker::drawMarker

  ( idx_t           tag,
    const GLfloat*  pos )

{
  if ( state_ )
  {
    const GLfloat  a = 0.15F;
    const GLfloat  b = 1.15F;
    const GLfloat  z = pos[2];

    GLfloat  x1, y1;
    GLfloat  x2, y2;
    GLfloat  dx, dy;


    if ( state_ & DRAW_QUADS_ )
    {
      glBegin ( GL_QUADS );
      {
        x1 = pos[0] - geom_[0];
        y1 = pos[1] - geom_[1];
        x2 = pos[0] + geom_[0];
        y2 = pos[1] + geom_[1];
        dx =      a * geom_[1];
        dy =     -a * geom_[0];

        glVertex3f ( x1 + dx, y1 + dy, z );
        glVertex3f ( x2 + dx, y2 + dy, z );
        glVertex3f ( x2 - dx, y2 - dy, z );
        glVertex3f ( x1 - dx, y1 - dy, z );

        x1 = pos[0];
        y1 = pos[1];
        x2 = x1 - b * geom_[1];
        y2 = y1 + b * geom_[0];
        dx =      a * geom_[0];
        dy =      a * geom_[1];

        glVertex3f ( x1 + dx, y1 + dy, z );
        glVertex3f ( x2 + dx, y2 + dy, z );
        glVertex3f ( x2 - dx, y2 - dy, z );
        glVertex3f ( x1 - dx, y1 - dy, z );
      }
      glEnd ();
    }

    if ( state_ & DRAW_LINES_ )
    {
      glBegin ( GL_LINES );
      {
        x1 = pos[0] - geom_[0];
        y1 = pos[1] - geom_[1];
        x2 = pos[0] + geom_[0];
        y2 = pos[1] + geom_[1];

        glVertex3f ( x1, y1, z );
        glVertex3f ( x2, y2, z );

        x1 = pos[0];
        y1 = pos[1];
        x2 = x1 - b * geom_[1];
        y2 = y1 + b * geom_[0];

        glVertex3f ( x1, y1, z );
        glVertex3f ( x2, y2, z );
      }
      glEnd ();
    }
  }
}


//-----------------------------------------------------------------------
//   setColor
//-----------------------------------------------------------------------


void TeeMarker::setColor ( const Color& color )
{
  color_ = clamped ( color );
}


//-----------------------------------------------------------------------
//   getColor
//-----------------------------------------------------------------------


Color TeeMarker::getColor () const
{
  return color_;
}


//-----------------------------------------------------------------------
//   setSize
//-----------------------------------------------------------------------


void TeeMarker::setSize ( GLfloat size )
{
  size_ = size;
}


//-----------------------------------------------------------------------
//   setAngle
//-----------------------------------------------------------------------


void TeeMarker::setAngle ( GLfloat angle )
{
  angle_ = angle;
}


JEM_END_PACKAGE( gl )
