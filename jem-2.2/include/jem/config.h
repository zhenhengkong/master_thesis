
// Generated on zo 19 mei 2019 19:19:38 CEST.
// CHANGES TO THIS FILE WILL BE LOST.

#ifndef JEM_CONFIG_H
#define JEM_CONFIG_H

#define JEM_OS_LINUX
#define JEM_ARCH_X86_64

#define JEM_USE_THREADS
#define JEM_OS_POSIX
#define JEM_OS_UNIX98

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


#ifndef _XOPEN_SOURCE
#  define _XOPEN_SOURCE 600
#endif

#define JEM_LINT_IS_LONG
#define JEM_INDEX_IS_INT

namespace jem
{
  typedef long lint;
  typedef unsigned long ulint;
  typedef int idx_t;
  typedef unsigned int uidx_t;
}

#define JEM_LINT_MAX   LONG_MAX
#define JEM_LINT_MIN   LONG_MIN
#define JEM_ULINT_MAX  ULONG_MAX
#define JEM_IDX_MAX  INT_MAX
#define JEM_IDX_MIN  INT_MIN
#define JEM_UIDX_MAX UINT_MAX

#define JEM_LINT_C(i)   JEM_CONCAT(i,L)
#define JEM_ULINT_C(i)  JEM_CONCAT(i,UL)
#define JEM_IDX_C(i)  i
#define JEM_UIDX_C(i) JEM_CONCAT(i,U)

#endif
