
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

#include <intrin.h>


JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class AtomicInteger
//=======================================================================

//-----------------------------------------------------------------------
//   incr
//-----------------------------------------------------------------------


long AtomicInteger::incr ()
{
  return _InterlockedIncrement ( &value_ );
}


//-----------------------------------------------------------------------
//   decr
//-----------------------------------------------------------------------


long AtomicInteger::decr ()
{
  return _InterlockedDecrement ( &value_ );
}


//=======================================================================
//   class AtomicLock
//=======================================================================

//-----------------------------------------------------------------------
//   tryLock
//-----------------------------------------------------------------------


long AtomicLock::tryLock ()
{
  long  result = _InterlockedExchange ( &lock_, 1L );

  return ! result;
}


//-----------------------------------------------------------------------
//   unlock
//-----------------------------------------------------------------------


void AtomicLock::unlock ()
{
  _InterlockedExchange ( &lock_, 0L );
}


JEM_END_PACKAGE_BASE
