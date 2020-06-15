
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


#undef JEM_PPC_SYNC

#if defined(JEM_USE_PPC_SYNC)
#  define JEM_PPC_SYNC "sync\n\t"
#else
#  define JEM_PPC_SYNC
#endif


JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class AtomicInteger
//=======================================================================

//-----------------------------------------------------------------------
//   incr
//-----------------------------------------------------------------------


int AtomicInteger::incr ()
{
  int  tmp;

  asm volatile (
    "1:\t"
    "lwarx   %0,0,%1\n\t"
    "addic   %0,%0,1\n\t"
    JEM_PPC_SYNC
    "stwcx.  %0,0,%1\n\t"
    "bne-    1b"
    : "=&r"  (tmp)
    : "r"    (&this->value_)
    : "cc",  "memory"
  );

  return tmp;
}


//-----------------------------------------------------------------------
//   decr
//-----------------------------------------------------------------------


int AtomicInteger::decr ()
{
  int  tmp;

  asm volatile (
    "1:\t"
    "lwarx   %0,0,%1\n\t"
    "addic   %0,%0,-1\n\t"
    JEM_PPC_SYNC
    "stwcx.  %0,0,%1\n\t"
    "bne-    1b"
    : "=&r"  (tmp)
    : "r"    (&this->value_)
    : "cc",  "memory"
  );

  return tmp;
}


//=======================================================================
//   class AtomicLock
//=======================================================================

//-----------------------------------------------------------------------
//   tryLock
//-----------------------------------------------------------------------


int AtomicLock::tryLock ()
{
  int  tmp;

  asm volatile (
    "1:\t"
    "lwarx   %0,0,%1\n\t"
    JEM_PPC_SYNC
    "stwcx.  %2,0,%1\n\t"
    "bne-    1b\n\t"
    "isync"
    : "=&r"  (tmp)
    : "r"    (&this->lock_), "r" (1)
    : "cr0", "memory"
  );

  return ! tmp;
}


//-----------------------------------------------------------------------
//   unlock
//-----------------------------------------------------------------------


void AtomicLock::unlock ()
{
  asm volatile ("\tsync": : :"memory");

  lock_ = 0;
}


JEM_END_PACKAGE_BASE
