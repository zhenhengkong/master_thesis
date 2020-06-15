
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

#ifndef JEM_GL_OSCONTEXT_H
#define JEM_GL_OSCONTEXT_H

#include <jem/base/Object.h>
#include <jem/gl/GfxContext.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class OSContext
//-----------------------------------------------------------------------


class OSContext : public Object, public GfxContext
{
 public:

  virtual void              postCallback

    ( Callback                cb,
      const Ref<Object>&      obj,
      const Time&             tm );

  virtual void              repostCallback

    ( Callback                cb,
      const Ref<Object>&      obj,
      const Time&             tm );

  virtual void              cancelCallback

    ( Callback                cb,
      const Ref<Object>&      obj );

  virtual void              cancelCallbacks

    ( Callback                cb );

  virtual void              makeCurrent     ()       = 0;
  virtual void              swapBuffers     ();
  virtual int               getActionID     () const;
  virtual Size2i            getWindowSize   () const = 0;


 protected:

  virtual                  ~OSContext       ();

  virtual void              postRedraw_

    ( const Ref<GfxNode>&     root );

  virtual void              postFastRedraw_

    ( const Ref<GfxNode>&     root );

};


JEM_END_PACKAGE( gl )

#endif
