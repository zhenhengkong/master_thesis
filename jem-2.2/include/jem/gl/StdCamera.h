
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

#ifndef JEM_GL_STDCAMERA_H
#define JEM_GL_STDCAMERA_H

#include <jem/base/Time.h>
#include <jem/gl/Rect.h>
#include <jem/gl/Camera.h>
#include <jem/gl/Pivotable.h>
#include <jem/gl/Transformable.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class StdCamera
//-----------------------------------------------------------------------


class StdCamera : public Camera,
                  public Pivotable,
                  public Transformable
{
 public:

  JEM_DECLARE_CLASS       ( StdCamera, Camera );

  static const int          MAX_QUALITY;

  static const int          AUTO_QUALITY;


  explicit                  StdCamera

    ( const String&           name  = "camera",
      const Ref<GfxNode>&     child = NIL );

  virtual void              setChild

    ( const Ref<GfxNode>&     child );

  virtual Ref<GfxNode>      getChild        () const;

  virtual GfxNode*          findNode

    ( const String&           name )           const;

  virtual bool              takeAction

    ( const String&           action,
      const Properties&       params );

  virtual bool              takeGLAction

    ( GfxContext&             gtx,
      int                     action,
      const Properties&       params );

  virtual void              getBBox

    ( Box&                    box )            const;

  virtual void              getBareBBox

    ( Box&                    box )            const;

  virtual void              getVisibility

    ( bool&                   viz,
      Box&                    vbox,
      const Transform&        proj )           const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )          const;

  virtual void              listProps

    ( const Properties&       props )          const;

  virtual Transform         getTransform    () const;

  virtual void              setTransform

    ( const Transform&        tr );

  virtual Rect              getViewport     () const;

  virtual Transform         getProjection   () const;

  virtual void              reset           ();

  virtual void              translateObj

    ( GfxNode&                node,
      GLdouble                dx,
      GLdouble                dy,
      GLdouble                dz )             const;

  virtual GLdouble          setZoom

    ( GLdouble                factor );

  virtual GLdouble          getZoom         () const;

  int                       setQuality

    ( int                     level );

  int                       getQuality      () const;

  void                      setOptions

    ( int                     options );

  int                       getOptions      () const;


 protected:

  virtual                  ~StdCamera       ();

  virtual Transform         newProjection_  () const = 0;


 protected:

  Ref<GfxNode>              child_;

  Transform                 proj_;
  Transform                 transfm_;

  GLdouble                  zoom_;
  Rect                      vport_;
  int                       options_;
  bool                      newProj_;


 private:

  void                      redrawFSAA_

    ( GfxContext&             gtx,
      const Properties&       params );

  void                      adjustQuality_

    ( double                  dt );


 private:

  static const GLfloat      JITTER_[];
  static const int          JINDEX_[];

  int                       quality_;
  int                       maxQuality_;
  double                    nextQuality_;
  Time                      lastLowered_;

};


JEM_END_PACKAGE( gl )

#endif
