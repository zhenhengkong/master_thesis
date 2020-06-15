
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

#ifndef JEM_GL_RUBBERBAND_H
#define JEM_GL_RUBBERBAND_H

#include <jem/util/event/Event.h>
#include <jem/gl/input.h>
#include <jem/gl/Rect.h>
#include <jem/gl/Vertex.h>
#include <jem/gl/GfxNode.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class RubberBand
//-----------------------------------------------------------------------


class RubberBand : public GfxNode
{
 public:

  JEM_DECLARE_CLASS       ( RubberBand, GfxNode );

  util::Event
    < GfxContext&,
      const Rect&,
      const MouseDrag& >    selectEvent;

  util::Event
    < GfxContext& >         cancelEvent;


  explicit                  RubberBand

    ( const String&           name = "" );

  virtual bool              takeAction

    ( const String&           action,
      const Properties&       params );

  virtual bool              takeGLAction

    ( GfxContext&             gtx,
      int                     action,
      const Properties&       params );

  inline bool               isActive          () const;
  inline bool               isTransient       () const;

  void                      start

    ( GfxContext&             gtx,
      const MouseDrag&        mouse );

  void                      start

    ( GfxContext&             gtx,
      const Ref<GfxNode>&     scene,
      const MouseDrag&        mouse );

  void                      cancel            ();

  void                      cancel

    ( GfxContext&             gtx );

  inline void               setAspectRatio

    ( double                  aspect );

  inline void               unsetAspectRatio  ();


 protected:

  virtual                  ~RubberBand        ();


 private:

  void                      finish_

    ( GfxContext&             gtx );

  void                      redraw_

    ( GfxContext&             gtx );


 private:

  static const char*        CANCEL_;

  static const int          ACTIVE_;
  static const int          TRANSIENT_;

  int                       state_;
  MouseDrag                 mouse_;
  Vertex2i                  origin_;
  Vertex2i                  current_;
  double                    aspect_;

};


//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   isActive
//-----------------------------------------------------------------------


inline bool RubberBand::isActive () const
{
  return ((state_ & ACTIVE_) != 0);
}


//-----------------------------------------------------------------------
//   isTransient
//-----------------------------------------------------------------------


inline bool RubberBand::isTransient () const
{
  return ((state_ & TRANSIENT_) != 0);
}


//-----------------------------------------------------------------------
//   setAspectRatio
//-----------------------------------------------------------------------


inline void RubberBand::setAspectRatio ( double aspect )
{
  aspect_ = aspect;
}


//-----------------------------------------------------------------------
//   unsetAspectRatio
//-----------------------------------------------------------------------


inline void RubberBand::unsetAspectRatio ()
{
  aspect_ = -1.0;
}


JEM_END_PACKAGE( gl )

#endif
