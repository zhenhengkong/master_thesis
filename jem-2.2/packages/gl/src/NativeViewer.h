
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

#ifndef JEM_GL_NATIVEVIEWER_H
#define JEM_GL_NATIVEVIEWER_H

#include <jem/gl/Viewer.h>


JEM_BEGIN_PACKAGE( gl )


class NativeWindow;
class NativeDisplay;


//-----------------------------------------------------------------------
//   class NativeViewer
//-----------------------------------------------------------------------


class NativeViewer : public Viewer
{
 public:

  typedef NativeViewer      Self;
  typedef Viewer            Super;
  typedef NativeWindow      Window;


  explicit                  NativeViewer

    ( const Ref<Window>&      win );

  virtual void              freeze          ();
  virtual void              resume          ();

  virtual void              setScene

    ( const Ref<GfxNode>&     scene );

  virtual Ref<GfxNode>      getScene        () const;
  virtual double            getWorkLoad     () const;
  virtual void              flushEvents     ();


 protected:

  virtual                  ~NativeViewer    ();

  virtual void              postRedraw_

    ( const Ref<GfxNode>&     node );

  virtual void              postFastRedraw_

    ( const Ref<GfxNode>&     node );

  virtual void              grabFrame_

    ( const Ref<GfxNode>&     node,
      OutputStream&           output,
      FrameGrabber&           grabber );


 private:

  Ref<Window>               window_;
  Ref<GfxNode>              scene_;
  NativeDisplay*            display_;

};


JEM_END_PACKAGE( gl )

#endif
