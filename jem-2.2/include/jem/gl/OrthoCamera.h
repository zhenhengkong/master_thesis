
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

#ifndef JEM_GL_ORTHOCAMERA_H
#define JEM_GL_ORTHOCAMERA_H

#include <jem/gl/StdCamera.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class OrthoCamera
//-----------------------------------------------------------------------


class OrthoCamera : public StdCamera
{
 public:

  JEM_DECLARE_CLASS       ( OrthoCamera, StdCamera );

  static const GLdouble     MIN_ZOOM;
  static const GLdouble     MAX_ZOOM;

  static const GLdouble     RANGE;


  explicit                  OrthoCamera

    ( const String&           name  = "camera",
      const Ref<GfxNode>&     child = NIL );

  virtual bool              isOrtho         () const;
  virtual void              reset           ();

  virtual void              lookAtBox

    ( const Box&              box );

  virtual void              getVisibleBox

    ( Box&                    box )            const;

  virtual void              translateObj

    ( GfxNode&                node,
      GLdouble                dx,
      GLdouble                dy,
      GLdouble                dz )             const;

  virtual GLdouble          setZoom

    ( GLdouble                zoom );

  virtual void              jitter

    ( Transform&              proj,
      GLdouble                dx,
      GLdouble                dy )             const;

  void                      setRange

    ( GLdouble                range );

  inline GLdouble           getRange        () const;
  virtual void              updateZRange    ();

  void                      setZRange

    ( GLdouble                clip0,
      GLdouble                clip1 );

  inline GLdouble           getNearClip     () const;
  inline GLdouble           getFarClip      () const;


 protected:

  virtual                  ~OrthoCamera     ();

  virtual Transform         newProjection_  () const;


 protected:

  GLdouble                  range_;
  GLdouble                  nearClip_;
  GLdouble                  farClip_;

};



//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getRange
//-----------------------------------------------------------------------


inline GLdouble OrthoCamera::getRange () const
{
  return range_;
}


//-----------------------------------------------------------------------
//   getNearClip
//-----------------------------------------------------------------------


GLdouble OrthoCamera::getNearClip () const
{
  return nearClip_;
}


//-----------------------------------------------------------------------
//   getFarClip
//-----------------------------------------------------------------------


GLdouble OrthoCamera::getFarClip () const
{
  return farClip_;
}


JEM_END_PACKAGE( gl )

#endif
