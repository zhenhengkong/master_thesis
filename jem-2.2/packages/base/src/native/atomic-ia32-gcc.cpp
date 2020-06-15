
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


//=======================================================================
//   class AtomicInteger
//=======================================================================

//-----------------------------------------------------------------------
//   incr
//-----------------------------------------------------------------------


int AtomicInteger::incr ()
{
  int  result;

  asm volatile (
    "lock; xaddl %0,%2"
    : "=r" (result)
    : "0"  (1), "m" (this->value_)
    : "memory"
  );

  return (result + 1);
}


//-----------------------------------------------------------------------
//   decr
//-----------------------------------------------------------------------


int AtomicInteger::decr ()
{
  int  result;

  asm volatile (
    "lock; xaddl %0,%2"
    : "=r" (result)
    : "0"  (-1), "m" (this->value_)
    : "memory"
  );

  return (result - 1);
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
    "xchgl %0,%1"
    : "=q" (tmp), "=m" (this->lock_)
    : "0"  (1)
    : "memory"
  );

  return (tmp == 0);
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
