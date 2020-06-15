
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
#include <jem/base/limits.h>
#include <jem/base/ClassTemplate.h>
#include <jem/util/Properties.h>
#include <jem/gl/utilities.h>
#include <jem/gl/StdAttribs.h>


JEM_DEFINE_CLASS( jem::gl::StdAttribs );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class StdAttribs
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  StdAttribs::POINT_SIZE_NAME  = "pointSize";
const char*  StdAttribs::LINE_WIDTH_NAME  = "lineWidth";
const char*  StdAttribs::SHADE_MODEL_NAME = "shadeModel";
const char*  StdAttribs::COLOR_NAME       = "color";
const char*  StdAttribs::MATERIAL_NAME    = "material";
const char*  StdAttribs::SPECULAR_NAME    = "specular";
const char*  StdAttribs::SHININESS_NAME   = "shininess";


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


StdAttribs::StdAttribs ()
{
  Self::setDefaults ();
}


StdAttribs::StdAttribs ( const Self& rhs )
{
  pointSize_  = rhs.pointSize_;
  lineWidth_  = rhs.lineWidth_;
  shadeModel_ = rhs.shadeModel_;
  color_      = rhs.color_;
  specular_   = rhs.specular_;
  shininess_  = rhs.shininess_;
}


StdAttribs::~StdAttribs ()
{}


//-----------------------------------------------------------------------
//   clone
//-----------------------------------------------------------------------


Ref<Object> StdAttribs::clone () const
{
  return newInstance<Self> ( *this );
}


//-----------------------------------------------------------------------
//   copyFrom
//-----------------------------------------------------------------------


int StdAttribs::copyFrom ( const Properties& props )
{
  Properties  matProps = props.findProps ( MATERIAL_NAME );

  String      s;
  double      d;
  int         fields = 0;


  if ( props.find( d, POINT_SIZE_NAME, 0.0, 100.0 ) )
  {
    fields    |= POINT_SIZE;
    pointSize_ = (GLfloat) d;
  }

  if ( props.find( d, LINE_WIDTH_NAME, 0.0, 100.0 ) )
  {
    fields    |= LINE_WIDTH;
    lineWidth_ = (GLfloat) d;
  }

  if ( props.find( s, SHADE_MODEL_NAME ) )
  {
    if      ( s.equalsIgnoreCase( "FLAT" ) )
    {
      shadeModel_ = GL_FLAT;
    }
    else if ( s.equalsIgnoreCase( "SMOOTH" ) )
    {
      shadeModel_ = GL_SMOOTH;
    }
    else
    {
      props.propertyError (
        SHADE_MODEL_NAME,
        String::format (
          "invalid shade model: `%s\'; should be either "
          "`Flat\' or `Smooth\'",
          s
        )
      );
    }

    fields |= SHADE_MODEL;
  }

  if ( color_.configure( COLOR_NAME, props ) )
  {
    fields |= COLOR;
  }

  if ( specular_.configure( SPECULAR_NAME, matProps ) )
  {
    fields |= SPECULAR;
  }

  if ( matProps.find( d, SHININESS_NAME, 0.0, 128.0 ) )
  {
    fields    |= SHININESS;
    shininess_ = (GLfloat) d;
  }

  if ( fields )
  {
    newValuesEvent.emit ( fields, *this );
  }

  return fields;
}


//-----------------------------------------------------------------------
//   copyTo
//-----------------------------------------------------------------------


void StdAttribs::copyTo

  ( const Properties&  props,
    int                fields ) const

{
  if ( fields & POINT_SIZE )
  {
    props.set ( POINT_SIZE_NAME, (double) pointSize_ );
  }

  if ( fields & LINE_WIDTH )
  {
    props.set ( LINE_WIDTH_NAME, (double) lineWidth_ );
  }

  if ( fields & SHADE_MODEL )
  {
    String  s;

    if ( shadeModel_ == GL_FLAT )
    {
      s = "Flat";
    }
    else
    {
      s = "Smooth";
    }

    props.set ( SHADE_MODEL_NAME, s );
  }

  if ( fields & COLOR )
  {
    color_.getConfig ( COLOR_NAME, props );
  }

  if ( fields & (SPECULAR | SHININESS) )
  {
    Properties  matProps = props.makeProps ( MATERIAL_NAME );

    if ( fields & SPECULAR )
    {
      specular_.getConfig ( SPECULAR_NAME, matProps );
    }

    if ( fields & SHININESS )
    {
      matProps.set ( SHININESS_NAME, (double) shininess_ );
    }
  }
}


//-----------------------------------------------------------------------
//   listProps
//-----------------------------------------------------------------------


void StdAttribs::listProps ( const Properties& props ) const
{
  props.set ( POINT_SIZE_NAME,
              "The size of a point in pixels" );
  props.set ( LINE_WIDTH_NAME,
              "The width of a line in pixels" );
  props.set ( SHADE_MODEL_NAME,
              "The shading model (Flat or Smooth)" );
  props.set ( COLOR_NAME,
              "The color of points, lines and polygons" );

  Properties  matProps = props.makeProps ( MATERIAL_NAME );

  matProps.set ( SPECULAR_NAME,
                 "The specular color" );
  matProps.set ( SHININESS_NAME,
                 "The shininess coefficient" );
}


//-----------------------------------------------------------------------
//   setDefaults
//-----------------------------------------------------------------------


void StdAttribs::setDefaults ()
{
  pointSize_  = 1.0;
  lineWidth_  = 1.0;
  shadeModel_ = GL_SMOOTH;
  color_      = Color::gray  ();
  specular_   = Color::black ();
  shininess_  = 0.0;

  newValuesEvent.emit ( ALL_FIELDS, *this );
}


//-----------------------------------------------------------------------
//   setCurrent
//-----------------------------------------------------------------------


void StdAttribs::setCurrent ( int fields ) const
{
  if ( fields & POINT_SIZE )
  {
    glPointSize ( pointSize_ );
  }

  if ( fields & LINE_WIDTH )
  {
    glLineWidth ( lineWidth_ );
  }

  if ( fields & SHADE_MODEL )
  {
    glShadeModel ( shadeModel_ );
  }

  if ( fields & COLOR )
  {
    jem::gl::setCurrent ( color_ );
  }

  if ( fields & SPECULAR )
  {
    glMaterialfv ( GL_FRONT_AND_BACK,
                   GL_SPECULAR, specular_.addr() );
  }

  if ( fields & SHININESS )
  {
    glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, shininess_ );
  }
}


//-----------------------------------------------------------------------
//   getCurrent
//-----------------------------------------------------------------------


void StdAttribs::getCurrent ( int fields )
{
  if ( fields & POINT_SIZE )
  {
    glGetFloatv ( GL_POINT_SIZE, & pointSize_ );
  }

  if ( fields & LINE_WIDTH )
  {
    glGetFloatv ( GL_LINE_WIDTH, & lineWidth_ );
  }

  if ( fields & SHADE_MODEL )
  {
    GLint i;

    glGetIntegerv ( GL_SHADE_MODEL, & i );
    shadeModel_   = (GLenum) i;
  }

  if ( fields & COLOR )
  {
    jem::gl::getCurrent ( color_ );
  }

  if ( fields & SPECULAR )
  {
    glGetMaterialfv ( GL_FRONT, GL_SPECULAR, specular_.addr() );
  }

  if ( fields & SHININESS )
  {
    glGetMaterialfv ( GL_FRONT, GL_SHININESS, & shininess_ );
  }

  newValuesEvent.emit ( fields, *this );
}


//-----------------------------------------------------------------------
//   setPointSize
//-----------------------------------------------------------------------


void StdAttribs::setPointSize ( GLfloat size )
{
  if ( ! isTiny( pointSize_ - size ) )
  {
    pointSize_ = clamped ( size, (GLfloat) 100.0 );

    newValuesEvent.emit ( POINT_SIZE, *this );
  }
}


//-----------------------------------------------------------------------
//   setLineWidth
//-----------------------------------------------------------------------


void StdAttribs::setLineWidth ( GLfloat width )
{
  if ( ! isTiny( lineWidth_ - width ) )
  {
    lineWidth_ = clamped ( width, (GLfloat) 100.0 );

    newValuesEvent.emit ( LINE_WIDTH, *this );
  }
}


//-----------------------------------------------------------------------
//   setShadeModel
//-----------------------------------------------------------------------


void StdAttribs::setShadeModel ( GLenum mod )
{
  JEM_PRECHECK ( mod == GL_FLAT || mod == GL_SMOOTH );

  if ( shadeModel_ != mod )
  {
    shadeModel_ = mod;

    newValuesEvent.emit ( SHADE_MODEL, *this );
  }
}


//-----------------------------------------------------------------------
//   setColor
//-----------------------------------------------------------------------


void StdAttribs::setColor ( const Color& col )
{
  color_ = col;

  newValuesEvent.emit ( COLOR, *this );
}


//-----------------------------------------------------------------------
//   setSpecular
//-----------------------------------------------------------------------


void StdAttribs::setSpecular ( const Color& col )
{
  specular_ = col;

  newValuesEvent.emit ( SPECULAR, *this );
}


//-----------------------------------------------------------------------
//   setShininess
//-----------------------------------------------------------------------


void StdAttribs::setShininess ( GLfloat shine )
{
  JEM_PRECHECK ( shine >= 0.0 && shine <= 128.0 );

  if ( ! isTiny( shininess_ - shine ) )
  {
    shininess_ = shine;

    newValuesEvent.emit ( SHININESS, *this );
  }
}


JEM_END_PACKAGE( gl )
