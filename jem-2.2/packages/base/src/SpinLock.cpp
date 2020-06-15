
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


#include <jem/base/thread/spin.h>
#include <jem/base/thread/ThreadLib.h>
#include <jem/base/SpinLock.h>


JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class SpinLock
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const lint  SpinLock::MAX_SPIN_CYCLES_ = JEM_LINT_C(50);

// Max sleep time in nano seconds:

const lint  SpinLock::MAX_SLEEP_TIME_  = JEM_LINT_C(128)  *
                                         JEM_LINT_C(1024) *
                                         JEM_LINT_C(1024);


//-----------------------------------------------------------------------
//   spin_
//-----------------------------------------------------------------------


void SpinLock::spin_ ()
{
  lint   timeout;
  ulint  j, k, n;
  lint   i;


  ThreadLib::init ();

  n = (ulint) ThreadLib::SPIN_COUNT;

  if ( n == JEM_ULINT_C(0) )
  {
    k       = (ulint) this % JEM_ULINT_C(128);
    timeout = JEM_LINT_C(128) + (lint) k;

    while ( true )
    {
      for ( i = 0; i < MAX_SPIN_CYCLES_; i++ )
      {
        if ( spinlock_.tryLock() )
        {
          return;
        }

        ThreadLib::yield ();
      }

      ThreadLib::yield ( timeout );

      timeout *= JEM_LINT_C(2);

      if ( timeout > MAX_SLEEP_TIME_ )
      {
        timeout = MAX_SLEEP_TIME_;
      }
    }
  }
  else
  {
    k = n + ((ulint) this % n);

    JEM_SPIN_WHILE ( spinlock_.locked(), j, k );

    if ( spinlock_.tryLock() )
    {
      return;
    }

    timeout = JEM_LINT_C(128);

    while ( true )
    {
      for ( i = 0; i < MAX_SPIN_CYCLES_; i++ )
      {
        JEM_SPIN_WHILE ( spinlock_.locked(), j, k );

        if ( spinlock_.tryLock() )
        {
          return;
        }

        k = n + (k * JEM_ULINT_C(389)) % n;

        ThreadLib::yield ();
      }

      ThreadLib::yield ( timeout );

      timeout *= JEM_LINT_C(2);

      if ( timeout > MAX_SLEEP_TIME_ )
      {
        timeout = MAX_SLEEP_TIME_;
      }
    }
  }
}


JEM_END_PACKAGE_BASE
