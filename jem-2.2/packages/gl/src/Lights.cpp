
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
#include <jem/base/System.h>
#include <jem/base/ClassTemplate.h>
#include <jem/util/Properties.h>
#include <jem/gl/utilities.h>
#include <jem/gl/Box.h>
#include <jem/gl/PropertyNames.h>
#include <jem/gl/Actions.h>
#include <jem/gl/Lights.h>


JEM_DEFINE_CLASS( jem::gl::Lights );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class Lights
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


Lights::Lights

  ( const String&  name,
    CoordSystem    coords ) :

    Super   (   name ),
    coords_ ( coords )

{
  specular_  = false;
  dirtyBits_ = 0;

  for ( int i = 0; i < MAX_LIGHTS; i++ )
  {
    lights_[i].pos     =  0.0;
    lights_[i].color   =  Color ( 0.2F );
    lights_[i].glidx   =  GL_LIGHT0 - 1;
    lights_[i].enabled =  false;
  }

  for ( int i = 0; i < 3; i++ )
  {
    lights_[i].pos[i]  = 1.0;
    lights_[i].enabled = true;
    dirtyBits_        |= (1 << i);
  }

  lights_[2].color  = Color ( 1.0 );
  lights_[3].pos[0] = -1.0;
  lights_[3].pos[1] = -1.0;

  ambient_    = Color ( 0.2F );
  dirtyBits_ |= (1 << MAX_LIGHTS);
}


Lights::~Lights ()
{}


//-----------------------------------------------------------------------
//   takeGLAction
//-----------------------------------------------------------------------


bool Lights::takeGLAction

  ( GfxContext&        gtx,
    int                action,
    const Properties&  params )

{
  if ( action == GLActions::REDRAW ||
       action == GLActions::REDRAW_FAST )
  {
    if ( dirtyBits_ )
    {
      initLights_ ();
    }

    if ( coords_ == OBJECT_COORDS )
    {
      for ( int i = 0; i < MAX_LIGHTS; i++ )
      {
        if ( lights_[i].enabled )
        {
          glLightfv ( (GLenum)     lights_[i].glidx,
                      GL_POSITION, lights_[i].pos.addr() );
        }
      }
    }

    return true;
  }

  if ( action == GLActions::INIT_GL )
  {
    // Make sure that the lights are re-enabled when the INIT_GL
    // action is requested a second time (when switching to another
    // OpenGL context, for instance).

    dirtyBits_ = 0;

    for ( int i = 0; i < MAX_LIGHTS; i++ )
    {
      lights_[i].glidx = GL_LIGHT0 - 1;

      if ( lights_[i].enabled )
      {
        dirtyBits_ |= (1 << i);
      }
    }

    dirtyBits_ |= (1 << MAX_LIGHTS);

    glEnable      ( GL_LIGHTING );
    glLightModeli ( GL_LIGHT_MODEL_TWO_SIDE, 1 );
    initLights_   ();

    return true;
  }

  return false;
}


//-----------------------------------------------------------------------
//   getVisibility
//-----------------------------------------------------------------------


void Lights::getVisibility

  ( bool&             viz,
    Box&              vbox,
    const Transform&  proj ) const

{
  viz = true;
  vbox.makeEmpty ();
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void Lights::configure ( const Properties& props )
{
  if ( props.contains( myName_ ) )
  {
    Properties  myProps = props.findProps ( myName_ );

    String      lightName;
    Vertex3f    dir;
    bool        spec;

    for ( int i = 0; i < MAX_LIGHTS; i++ )
    {
      Light_&  light = lights_[i];

      lightName = PropNames::LIGHT + String ( i );

      dir[0] = -light.pos[0];
      dir[1] = -light.pos[1];
      dir[2] = -light.pos[2];

      if ( myProps.contains( lightName ) )
      {
        Properties  lightProps = myProps.getProps ( lightName );

        light.color.configure ( PropNames::COLOR,
                               lightProps );
        Vertex    ::configure ( dir, PropNames::DIRECTION,
                                lightProps );

        lightProps .find ( light.enabled, PropNames::ENABLE );

        light.pos[0] = -dir[0];
        light.pos[1] = -dir[1];
        light.pos[2] = -dir[2];

        dirtyBits_  |= (1 << i);
      }
    }

    if ( myProps.find( spec, PropNames::SPECULAR ) )
    {
      setSpecular ( spec );
    }

    if ( ambient_.configure( PropNames::AMBIENT, myProps ) )
    {
      dirtyBits_ |= (1 << MAX_LIGHTS);
    }
  }
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void Lights::getConfig ( const Properties& conf ) const
{
  Properties  myConf = conf.makeProps ( myName_ );

  Vertex3f    dir;


  for ( int i = 0; i < MAX_LIGHTS; i++ )
  {
    Properties     lightConf =

      myConf.makeProps ( PropNames::LIGHT + String(i) );

    const Light_&  light = lights_[i];

    dir[0] = -light.pos[0];
    dir[1] = -light.pos[1];
    dir[2] = -light.pos[2];

    light.color.getConfig ( PropNames::COLOR,     lightConf );
    Vertex    ::getConfig ( PropNames::DIRECTION, lightConf, dir );

    lightConf.set ( PropNames::ENABLE, light.enabled );
  }

  myConf  .set       ( PropNames::SPECULAR, specular_ );
  ambient_.getConfig ( PropNames::AMBIENT, myConf );
}


//-----------------------------------------------------------------------
//   listProps
//-----------------------------------------------------------------------


void Lights::listProps ( const Properties& conf ) const
{
  Properties  myConf = conf.makeProps ( myName_ );

  for ( int i = 0; i < MAX_LIGHTS; i++ )
  {
    Properties  lightConf =

      myConf.makeProps ( PropNames::LIGHT + String(i) );

    lightConf.set ( PropNames::COLOR,
                    "The color of this light" );
    lightConf.set ( PropNames::DIRECTION,
                    "The direction of this light" );
    lightConf.set ( PropNames::ENABLE,
                    "Whether this light is enabled" );
  }

  myConf.set ( PropNames::SPECULAR,
               "Whether specular lighting should be enabled" );

  myConf.set ( PropNames::AMBIENT,
               "The color of the ambient light" );
}


//-----------------------------------------------------------------------
//   enable
//-----------------------------------------------------------------------


void Lights::enable ( int idx, bool choice )
{
  JEM_PRECHECK ( idx >= 0 && idx < MAX_LIGHTS );

  if ( lights_[idx].enabled != choice )
  {
    lights_[idx].enabled = choice;
    dirtyBits_          |= (1 << idx);
  }
}


//-----------------------------------------------------------------------
//   isEnabled
//-----------------------------------------------------------------------


bool Lights::isEnabled ( int idx ) const
{
  JEM_PRECHECK ( idx >= 0 && idx < MAX_LIGHTS );

  return lights_[idx].enabled;
}


//-----------------------------------------------------------------------
//   setColor
//-----------------------------------------------------------------------


void Lights::setColor

  ( int           idx,
    const Color&  color )

{
  JEM_PRECHECK ( idx >= 0 && idx < MAX_LIGHTS );

  lights_[idx].color = clamped ( color );
  dirtyBits_        |= (1 << idx);
}


//-----------------------------------------------------------------------
//   getColor
//-----------------------------------------------------------------------


Color Lights::getColor ( int idx ) const
{
  JEM_PRECHECK ( idx >= 0 && idx < MAX_LIGHTS );

  return lights_[idx].color;
}


//-----------------------------------------------------------------------
//   setDirection
//-----------------------------------------------------------------------


void Lights::setDirection

  ( int              idx,
    const Vertex3f&  dir )

{
  JEM_PRECHECK ( idx >= 0 && idx < MAX_LIGHTS );

  Light_&  light = lights_[idx];

  light.pos[0] = -dir[0];
  light.pos[1] = -dir[1];
  light.pos[2] = -dir[2];

  dirtyBits_  |= (1 << idx);
}


//-----------------------------------------------------------------------
//   getDirection
//-----------------------------------------------------------------------


Vertex3f Lights::getDirection ( int idx ) const
{
  JEM_PRECHECK ( idx >= 0 && idx < MAX_LIGHTS );

  const Light_&  light = lights_[idx];

  Vertex3f       dir;

  dir[0] = -light.pos[0];
  dir[1] = -light.pos[1];
  dir[2] = -light.pos[2];

  return dir;
}


//-----------------------------------------------------------------------
//   setAmbient
//-----------------------------------------------------------------------


void Lights::setAmbient ( const Color& color )
{
  ambient_    = clamped ( color );
  dirtyBits_ |= (1 << MAX_LIGHTS);
}


//-----------------------------------------------------------------------
//   getAmbient
//-----------------------------------------------------------------------


Color Lights::getAmbient () const
{
  return ambient_;
}


//-----------------------------------------------------------------------
//   setSpecular
//-----------------------------------------------------------------------


void Lights::setSpecular ( bool choice )
{
  if ( choice != specular_ )
  {
    for ( int i = 0; i < MAX_LIGHTS; i++ )
    {
      if ( lights_[i].enabled )
      {
        dirtyBits_ |= (1 << i);
      }
    }

    specular_ = choice;
  }
}


//-----------------------------------------------------------------------
//   getSpecular
//-----------------------------------------------------------------------


bool Lights::getSpecular () const
{
  return specular_;
}


//-----------------------------------------------------------------------
//   initLights_
//-----------------------------------------------------------------------


void Lights::initLights_ ()
{
  for ( int i = 0; i < MAX_LIGHTS; i++ )
  {
    if ( dirtyBits_ & (1 << i) )
    {
      initLight_ ( lights_[i] );
    }
  }

  if ( dirtyBits_ & (1 << MAX_LIGHTS) )
  {
    glLightModelfv ( GL_LIGHT_MODEL_AMBIENT, ambient_.addr() );
  }

  dirtyBits_ = 0;

  checkGLError ( JEM_FUNC );
}


//-----------------------------------------------------------------------
//   initLight_
//-----------------------------------------------------------------------


void Lights::initLight_ ( Light_& light )
{
  int  i;

  if ( ! light.enabled )
  {
    if ( light.glidx >= GL_LIGHT0 )
    {
      glDisable ( (GLenum) light.glidx );

      light.glidx = (int) GL_LIGHT0 - 1;
    }

    return;
  }

  if ( light.enabled && light.glidx < GL_LIGHT0 )
  {
    for ( i = 0; i < GL_MAX_LIGHTS; i++ )
    {
      if ( glIsEnabled( (GLenum) (GL_LIGHT0 + i) ) == GL_FALSE )
      {
        break;
      }
    }

    if ( i >= GL_MAX_LIGHTS )
    {
      print ( System::warn(), myName_,
              " : failed to enable another light\n" );

      light.enabled = false;

      return;
    }

    light.glidx = (int) GL_LIGHT0 + i;

    glEnable ( (GLenum) light.glidx );
  }

  i = light.glidx;

  glLightfv ( (GLenum) i, GL_DIFFUSE,  light.color.addr() );

  if ( specular_ )
  {
    glLightfv ( (GLenum) i, GL_SPECULAR, light.color.addr() );
  }
  else
  {
    GLfloat  color[4] = { 0.0F, 0.0F, 0.0F, 1.0F };

    glLightfv ( (GLenum) i, GL_SPECULAR, color );
  }

  if ( coords_ == EYE_COORDS )
  {
    glPushMatrix   ();
    glLoadIdentity ();
    glLightfv      ( (GLenum) i, GL_POSITION, light.pos.addr() );
    glPopMatrix    ();
  }
}


JEM_END_PACKAGE( gl )
