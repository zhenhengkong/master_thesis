
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

#ifndef JEM_DEFINES_H
#define JEM_DEFINES_H

#include <jem/config.h>


//-----------------------------------------------------------------------
//   generic macros
//-----------------------------------------------------------------------


#define JEM_CONCAT_(a,b) a ## b
#define JEM_CONCAT(a,b)  JEM_CONCAT_(a,b)


//-----------------------------------------------------------------------
//   file system macros
//-----------------------------------------------------------------------


#if   defined(JEM_OS_POSIX)
#  define JEM_FS_UNIX
#elif defined(JEM_OS_WINDOWS)
#  define JEM_FS_WINDOWS
#else
#  error "Unsupported file system -- sorry"
#endif


//-----------------------------------------------------------------------
//   the macro JEM_FUNC
//-----------------------------------------------------------------------


#if   defined(__GNUC__)
#  define JEM_FUNC __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#  define JEM_FUNC __FUNCSIG__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#  define JEM_FUNC __func__
#else
#  define JEM_FUNC "unknown function"
#endif


//-----------------------------------------------------------------------
//   the macro JEM_RESTRICT
//-----------------------------------------------------------------------


#if   defined(__GNUC__)
#  define JEM_RESTRICT __restrict__
#elif defined(__sgi)
#  define JEM_RESTRICT restrict
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#  define JEM_RESTRICT restrict
#else
#  define JEM_RESTRICT
#endif


//-----------------------------------------------------------------------
//   some other macros
//-----------------------------------------------------------------------


#if defined(JEM_USE_THREADS) && ! defined(_REENTRANT)
#  define _REENTRANT
#endif


//-----------------------------------------------------------------------
//   include standard types and NULL
//-----------------------------------------------------------------------


#include <cstddef>


//-----------------------------------------------------------------------
//   namespace macros
//-----------------------------------------------------------------------


#define JEM_BEGIN_PACKAGE_BASE       namespace jem {
#define JEM_END_PACKAGE_BASE         }

#define JEM_BEGIN_PACKAGE( name )    namespace jem { namespace name {
#define JEM_END_PACKAGE( name )      } }

#define JEM_BEGIN_INTERNAL           namespace internal {
#define JEM_END_INTERNAL             }

#define JEM_BEGIN_NAMESPACE( name )  namespace name {
#define JEM_END_NAMESPACE( name )    }


//-----------------------------------------------------------------------
//   some forward declarations and typedefs
//-----------------------------------------------------------------------


namespace jem
{
  typedef unsigned char     byte;

  template <class T> class  TypeTraits;
  template <class T> class  ObjectTraits;
}


#endif
