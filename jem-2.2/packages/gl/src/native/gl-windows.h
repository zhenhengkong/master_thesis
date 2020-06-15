
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


// To avoid polluting the namespace the <windows.h> header file
// is not included here.

#ifndef APIENTRY
#  if defined(_MSC_VER)
#    if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#      define APIENTRY __stdcall
#    else
#      define APIENTRY
#    endif
#  endif
#endif

#ifndef WINGDIAPI
#  if defined(_MSC_VER)
#    define WINGDIAPI __declspec(dllimport)
#    ifndef _WCHAR_T_DEFINED
       typedef unsigned short wchar_t;
#      define _WCHAR_T_DEFINED
#    endif
#  endif
#endif

#if defined(_MSC_VER)
#  pragma comment (lib,"winmm.lib")
#  pragma comment (lib,"user32.lib")
#  pragma comment (lib,"gdi32.lib")
#  pragma comment (lib,"opengl32.lib")
#  pragma comment (lib,"glu32.lib")
#endif

extern "C"
{
  #include <GL/gl.h>
}
