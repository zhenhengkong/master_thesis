
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

#ifndef JEM_BASE_THREAD_THREADQUEUE_H
#define JEM_BASE_THREAD_THREADQUEUE_H

#include <jem/base/assert.h>
#include <jem/base/SpinLock.h>
#include <jem/base/thread/ThreadMonitor.h>


JEM_BEGIN_PACKAGE_BASE


class ThreadQueue;


//-----------------------------------------------------------------------
//   class ThreadNode
//-----------------------------------------------------------------------


class ThreadNode
{
 public:

  typedef ThreadMonitor   Monitor;

  friend class            ThreadQueue;


  explicit inline         ThreadNode

    ( void*                 data    = 0 );

  inline void*            getData      () const;

  byte                    wait

    ( SpinLock&             mutex,
      const Time*           timeout = 0 );

  void                    waitNoCancel

    ( SpinLock&             mutex );

  inline void             notify       ();


 private:

                          ThreadNode   ( const ThreadNode& );
  ThreadNode&             operator =   ( const ThreadNode& );


 private:

  void* const             data_;

  ThreadNode*             next_;
  Monitor*                monitor_;

  volatile bool           notified_;

};


//-----------------------------------------------------------------------
//   class ThreadQueue
//-----------------------------------------------------------------------


class ThreadQueue
{
 public:

  typedef ThreadNode      Node;


  static inline void      push

    ( Node*&                queue,
      Node*                 node  );

  static inline Node*     pop

    ( Node*&                queue );

  static bool             remove

    ( Node*&                queue,
      Node*                 node  );

};





//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class ThreadNode
//=======================================================================

//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------


inline ThreadNode::ThreadNode ( void* data ) :

  data_ ( data )

{
  next_     = this;
  monitor_  = 0;
  notified_ = false;
}


//-----------------------------------------------------------------------
//   getData
//-----------------------------------------------------------------------


inline void* ThreadNode::getData () const
{
  return data_;
}


//-----------------------------------------------------------------------
//   notify
//-----------------------------------------------------------------------


inline void ThreadNode::notify ()
{
  Monitor*  mon = monitor_;

  monitor_ = 0;

  if ( mon )
  {
    mon->lock   ();
    mon->notify ( SigThread::NOTIFY );
    mon->unlock ();
  }

  notified_ = true;
}


//=======================================================================
//   class ThreadQueue
//=======================================================================

//-----------------------------------------------------------------------
//   push
//-----------------------------------------------------------------------


inline void ThreadQueue::push

  ( Node*&  queue,
    Node*   node )

{
  if ( queue )
  {
    node ->next_ = queue->next_;
    queue->next_ = node;
  }

  queue = node;
}


//-----------------------------------------------------------------------
//   pop
//-----------------------------------------------------------------------


inline ThreadNode* ThreadQueue::pop ( Node*&  queue )
{
  JEM_ASSERT_NOTHROW ( queue );

  Node*  first;


  if ( queue == queue->next_ )
  {
    first        = queue;
    queue        = 0;
  }
  else
  {
    first        = queue->next_;
    queue->next_ = first->next_;
    first->next_ = first;
  }

  return first;
}


JEM_END_PACKAGE_BASE

#endif
