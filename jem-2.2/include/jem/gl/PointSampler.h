
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

#ifndef JEM_GL_POINTSAMPLER_H
#define JEM_GL_POINTSAMPLER_H

#include <jem/base/Time.h>
#include <jem/util/event/Event.h>
#include <jem/gl/input.h>
#include <jem/gl/Color.h>
#include <jem/gl/Vertex.h>
#include <jem/gl/GfxNode.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class PointSampler
//-----------------------------------------------------------------------


class PointSampler : public GfxNode
{
 public:

  JEM_DECLARE_CLASS       ( PointSampler, GfxNode );

  static const double       MIN_INTERVAL;

  struct                    Sample
  {
    MouseClick                mouse;
    Color                     color;
    Vertex3d                  point;
    Vertex3d                  error;
  };

  util::Event
    < GfxContext&,
      const Sample& >       sampleEvent;


                            PointSampler

    ( const String&           name,
      const Ref<GfxNode>&     obj );

  virtual bool              takeAction

    ( const String&           action,
      const Properties&       params );

  virtual bool              takeGLAction

    ( GfxContext&             gtx,
      int                     action,
      const Properties&       params );

  virtual void              getVisibility

    ( bool&                   viz,
      Box&                    vbox,
      const Transform&        proj )         const;

  void                      takeSample

    ( GfxContext&             gtx,
      const MouseClick&       click );

  void                      setTarget

    ( const Ref<GfxNode>&     obj );

  inline GfxNode*           getTarget     () const;

  void                      setMouseMask

    ( int                     btnMask );

  static Ref<Self>          find

    ( const Ref<GfxNode>&     root,
      const Ref<Object>&      target );


 protected:

  virtual                  ~PointSampler  ();


 private:

  void                      reset_        ();

  static bool               takeSample_

    ( GfxContext&             gtx,
      GLfloat&                depth,
      Sample&                 sample,
      GfxNode&                target );

  static Ref<Self>          getSampler_

    ( const Ref<Object>&      obj );

  static void               sampleCallback_

    ( GfxContext&             gtx,
      const Ref<Object>&      obj );

  static void               click2Callback_

    ( GfxContext&             gtx,
      const Ref<Object>&      obj );


 private:

  static const char*        TARGET_;
  static const char*        SAMPLER_;
  static const char*        GET_SAMPLE_;
  static const char*        FIND_SAMPLER_;


  Ref<GfxNode>              target_;
  int                       btnMask_;
  Sample                    sample_;
  int                       button_;
  GLfloat                   depth_;
  bool                      focus_;
  Time                      lastTime_;

};




//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getTarget
//-----------------------------------------------------------------------


inline GfxNode* PointSampler::getTarget () const
{
  return target_.get ();
}


JEM_END_PACKAGE( gl )

#endif
