
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


#include <jem/base/atomic.h>


JEM_BEGIN_PACKAGE_BASE


typedef unsigned long u64;


//=======================================================================
//   class AtomicInteger
//=======================================================================

//-----------------------------------------------------------------------
//   incr
//-----------------------------------------------------------------------


int AtomicInteger::incr ()
{
  volatile int*  p = &value_;
  volatile u64   r;

  asm volatile (
    "fetchadd4.rel %0=[%1],%2"
    : "=r" (r)
    : "r" (p), "i" (1)
    : "memory"
  );

  return (int) (r + 1);
}


//-----------------------------------------------------------------------
//   decr
//-----------------------------------------------------------------------


int AtomicInteger::decr ()
{
  volatile int*  p = *value_;
  volatile u64   r;

  asm volatile (
    "fetchadd4.rel %0=[%1],%2"
    : "=r" (r)
    : "r"  (p), "i" (-1)
    : "memory"
  );

  return (int) (r - 1);
}


//=======================================================================
//   class AtomicLock
//=======================================================================

//-----------------------------------------------------------------------
//   tryLock
//-----------------------------------------------------------------------


int AtomicLock::tryLock ( volatile int* addr, int value )
{
  volatile int*  p = &lock_;
  volatile u64   i = 1;
  volatile u64   r;

  asm volatile (
    "xchg4 %0=[%1],%2"
    : "=r" (r)
    : "r"  (p), "r" (i)
    : "memory"
  );

  return (r == 0);
}


//-----------------------------------------------------------------------
//   unlock
//-----------------------------------------------------------------------


void AtomicLock::unlock ()
{
  asm volatile ( "" : : : "memory" );

  lock_ = 0;
}


JEM_END_PACKAGE_BASE
