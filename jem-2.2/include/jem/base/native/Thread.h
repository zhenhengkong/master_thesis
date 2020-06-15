
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

#ifndef JEM_BASE_NATIVE_POSIXTHREAD_H
#define JEM_BASE_NATIVE_POSIXTHREAD_H

#include <jem/tls.h>
#include <jem/base/Object.h>

extern "C"
{
  #include <pthread.h>
}


JEM_BEGIN_PACKAGE_BASE


class Thread;
class ThreadCache;
class ThreadStart;


//-----------------------------------------------------------------------
//   class PosixThread
//-----------------------------------------------------------------------


class PosixThread : public Object
{
 public:

  typedef PosixThread     Self;
  typedef Object          Super;
  typedef ThreadCache     Cache;


                          PosixThread     ();
                         ~PosixThread     ();

  void                    interrupt       ();
  void                    clearInterrupt  ();

  static void             init            ();

  static void             start

    ( ThreadStart*          start );

  static void             yield

    ( lint                  nsecs );

  static inline Thread*   getThread       ();

  static inline void      setThread

    ( Thread*               thread );

  static inline Cache*    getCache        ();

  static inline void      setCache

    ( Cache*                cache );


 private:

  pthread_t               tid_;

  static int              sigint_;

#ifndef JEM_TLS

  static pthread_key_t    threadKey_;
  static pthread_key_t    cacheKey_;

#else

  JEM_TLS( static, void*, thread_ );
  JEM_TLS( static, void*, cache_  );

#endif

};


typedef PosixThread       NativeThread;





//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getThread
//-----------------------------------------------------------------------


inline Thread* PosixThread::getThread ()
{
#ifdef JEM_TLS

  return (Thread*) thread_;

#else

  return (Thread*) pthread_getspecific ( threadKey_ );

#endif
}


//-----------------------------------------------------------------------
//   setThread
//-----------------------------------------------------------------------


inline void PosixThread::setThread ( Thread* t )
{
#ifdef JEM_TLS

  thread_ = t;

#else

  pthread_setspecific ( threadKey_, t );

#endif
}


//-----------------------------------------------------------------------
//   getCache
//-----------------------------------------------------------------------


inline ThreadCache* PosixThread::getCache ()
{
#ifdef JEM_TLS

  return (Cache*) cache_;

#else

  return (Cache*) pthread_getspecific ( cacheKey_ );

#endif
}


//-----------------------------------------------------------------------
//   setCache
//-----------------------------------------------------------------------


inline void PosixThread::setCache ( Cache* c )
{
#ifdef JEM_TLS

  cache_ = c;

#else

  pthread_setspecific ( cacheKey_, c );

#endif
}


JEM_END_PACKAGE_BASE

#endif
