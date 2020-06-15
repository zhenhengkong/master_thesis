
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

#ifndef JEM_GL_LIGHTS_H
#define JEM_GL_LIGHTS_H

#include <jem/gl/Color.h>
#include <jem/gl/Vertex.h>
#include <jem/gl/GfxNode.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class Lights
//-----------------------------------------------------------------------


class Lights : public GfxNode
{
 public:

  JEM_DECLARE_CLASS       ( Lights, GfxNode );

  enum                      LightID
  {
                              LIGHT0 = 0,
                              LIGHT1,
                              LIGHT2,
                              LIGHT3
  };

  enum                      CoordSystem
  {
                              EYE_COORDS,
                              OBJECT_COORDS
  };

  static const int          MAX_LIGHTS = 4;


  explicit                  Lights

    ( const String&           name   = "lights",
      CoordSystem             coords = EYE_COORDS );

  virtual bool              takeGLAction

    ( GfxContext&             gtx,
      int                     action,
      const Properties&       params );

  virtual void              getVisibility

    ( bool&                   viz,
      Box&                    vbox,
      const Transform&        proj )         const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )        const;

  virtual void              listProps

    ( const Properties&       props )        const;

  void                      enable

    ( int                     idx,
      bool                    choice = true );

  bool                      isEnabled

    ( int                     idx )          const;

  void                      setColor

    ( int                     idx,
      const Color&            color );

  Color                     getColor

    ( int                     idx )          const;

  void                      setDirection

    ( int                     idx,
      const Vertex3f&         dir );

  Vertex3f                  getDirection

    ( int                     idx )          const;

  void                      setAmbient

    ( const Color&            color );

  Color                     getAmbient    () const;

  void                      setSpecular

    ( bool                    choice = true );

  bool                      getSpecular   () const;


 protected:

  virtual                  ~Lights        ();


 private:

  struct                    Light_
  {
    Vertex4f                  pos;
    Color                     color;
    int                       glidx;
    bool                      enabled;
  };

  void                      initLights_   ();

  void                      initLight_

    ( Light_&                 light );


 private:

  const CoordSystem         coords_;

  Light_                    lights_[MAX_LIGHTS];
  Color                     ambient_;
  bool                      specular_;
  int                       dirtyBits_;

};


JEM_END_PACKAGE( gl )

#endif
