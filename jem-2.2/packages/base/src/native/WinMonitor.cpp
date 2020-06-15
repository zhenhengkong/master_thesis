
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


#include <jem/base/Time.h>
#include <jem/base/SystemException.h>
#include "native/WinMonitor.h"


JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class WinMonitor
//=======================================================================

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


WinMonitor::WinMonitor ()
{
  waiting_ = false;

  mutex_   = CreateMutex ( NULL, FALSE, NULL );
  event_   = CreateEvent ( NULL, FALSE, FALSE, NULL );

  if ( ! mutex_ )
  {
    throw SystemException ( JEM_FUNC, "CreateMutex() failed" );
  }

  if ( ! event_ )
  {
    throw SystemException ( JEM_FUNC, "CreateEvent() failed" );
  }
}


WinMonitor::~WinMonitor ()
{
  CloseHandle ( mutex_ );
  CloseHandle ( event_ );
}


//-----------------------------------------------------------------------
//   wait
//-----------------------------------------------------------------------


void WinMonitor::wait ( const Time& timeout )
{
  if ( timeout <= Time::zero() )
  {
    return;
  }

  DWORD  millis = (DWORD)

    (timeout.sec() * JEM_LINT_C(1000) + timeout.nsec() /
     JEM_LINT_C(1000000));

  waiting_ = true;

  SignalObjectAndWait ( mutex_, event_, millis, TRUE );

  waiting_ = false;
}


JEM_END_PACKAGE_BASE
