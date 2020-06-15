
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

#ifndef JEM_GL_GLSETUP3D_H
#define JEM_GL_GLSETUP3D_H

#include <jem/gl/StdGLSetup.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class GLSetup3D
//-----------------------------------------------------------------------


class GLSetup3D : public StdGLSetup
{
 public:

  typedef GLSetup3D         Self;
  typedef StdGLSetup        Super;


                            GLSetup3D   ();

  virtual void              initGL      ();


 protected:

  virtual                  ~GLSetup3D   ();

};


JEM_END_PACKAGE( gl )

#endif
