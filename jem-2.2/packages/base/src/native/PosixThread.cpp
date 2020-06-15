
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


#include <cstdio>
#include <ctime>
#include <jem/base/EnvParams.h>
#include <jem/base/SystemException.h>
#include <jem/base/thread/ThreadStart.h>
#include "native/PosixThread.h"

extern "C"
{
  #include <signal.h>
  #include <sched.h>
}


JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   non-member functions
//=======================================================================


JEM_BEGIN_NAMESPACE( posix )


//-----------------------------------------------------------------------
//   interrupHandler
//-----------------------------------------------------------------------


extern "C"
{
  static void interruptHandler ( int )
  {
    // Does nothing
  }
}


//-----------------------------------------------------------------------
//   threadFunc
//-----------------------------------------------------------------------


extern "C"
{
  static void* threadFunc ( void* arg )
  {
    pthread_detach ( pthread_self() );

    // Make sure that no exceptions propagate beyond this
    // function, since this is the upper most stack frame
    // of the thread.

    try
    {
      ((ThreadStart*) arg)->start ( (ThreadStart*) arg );
    }
    catch ( ... )
    {
      abort ( JEM_FUNC,
              "uncaught exception in POSIX thread entry point" );
    }

    return NULL;
  }
}


JEM_END_NAMESPACE( posix )


//=======================================================================
//   class PosixThread
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


int            PosixThread::sigint_ = -1;

#ifndef JEM_TLS

pthread_key_t  PosixThread::threadKey_;
pthread_key_t  PosixThread::cacheKey_;

#else

JEM_TLS( JEM_NO_STORAGE, void*, PosixThread::thread_ = 0 );
JEM_TLS( JEM_NO_STORAGE, void*, PosixThread::cache_  = 0 );

#endif


//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


PosixThread::PosixThread ()
{
  tid_ = pthread_self ();
}


PosixThread::~PosixThread ()
{}


//-----------------------------------------------------------------------
//   interrupt
//-----------------------------------------------------------------------


void PosixThread::interrupt ()
{
  if ( sigint_ > 0 )
  {
    pthread_kill ( tid_, sigint_ );
  }
}


//-----------------------------------------------------------------------
//   clearInterrupt
//-----------------------------------------------------------------------


void PosixThread::clearInterrupt ()
{
  // Does nothing.
}


//-----------------------------------------------------------------------
//   init
//-----------------------------------------------------------------------


void PosixThread::init ()
{
  struct sigaction  action;

#ifndef JEM_TLS

  if ( pthread_key_create( &threadKey_, NULL ) != 0 )
  {
    abort ( JEM_FUNC, "pthread_key_create() failed" );
  }
  if ( pthread_key_create( &cacheKey_, NULL ) != 0 )
  {
    abort ( JEM_FUNC, "pthread_key_create() failed" );
  }

#endif

  sigint_ = SIGUSR2;

  action.sa_handler = posix::interruptHandler;
  action.sa_flags   = 0;

  sigemptyset ( &action.sa_mask );

  if ( sigaction( sigint_, &action, NULL ) != 0 )
  {
    sigint_ = -1;
  }

#ifdef JEM_OS_UNIX98

  lint  conLevel = 0;

  if ( igetenv( conLevel, EnvParams::CONCURRENCY ) )
  {
    if ( conLevel >= 0 )
    {
      ::pthread_setconcurrency ( (int) conLevel );
    }
  }

#endif
}


//-----------------------------------------------------------------------
//   start
//-----------------------------------------------------------------------


void PosixThread::start ( ThreadStart* start )
{
  pthread_t  thread;
  int        result;


  result = pthread_create ( & thread,
                              NULL,
                            & posix::threadFunc,
                              start );

  if ( result != 0 )
  {
    throw SystemException (
      JEM_FUNC,
      "pthread_create() failed (too many threads?)"
    );
  }
}


//-----------------------------------------------------------------------
//   yield
//-----------------------------------------------------------------------


void PosixThread::yield ( lint nsecs )
{
  if ( nsecs <= 0 )
  {
    sched_yield ();
  }
  else
  {
    struct timespec  ts;

    ts.tv_sec  = (time_t) 0;
    ts.tv_nsec = nsecs;

    ::nanosleep ( & ts, NULL );
  }
}


JEM_END_PACKAGE_BASE
