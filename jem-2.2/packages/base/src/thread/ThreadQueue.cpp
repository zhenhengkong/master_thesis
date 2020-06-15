
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
#include <jem/base/thread/ThreadQueue.h>


JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class ThreadNode
//=======================================================================

//-----------------------------------------------------------------------
//   wait
//-----------------------------------------------------------------------


byte ThreadNode::wait

  ( SpinLock&    mutex,
    const Time*  timeout )

{
  const lint  n = ThreadLib::SPIN_COUNT;

  Monitor*    mon;
  byte        sig;
  lint        i;


  if ( n > 0 && ! timeout )
  {
    mutex.unlock ();

    JEM_SPIN_WHILE ( ! notified_, i, n );

    mutex.lock   ();
  }

  if ( notified_ )
  {
    notified_ = false;
    return SigThread::NOTIFY;
  }

  mon = monitor_ = ThreadLib::getMonitor ();

  mutex.unlock ();
  mon ->lock   ();

  if ( timeout )
  {
    sig = mon->wait ( SigThread::ALL, *timeout );
  }
  else
  {
    sig = mon->wait ( SigThread::ALL );
  }

  mon->unlock ();
  mutex.lock  ();

  if ( ! (sig & SigThread::NOTIFY) )
  {
    Monitor*  tmp;

    tmp      = monitor_;
    monitor_ = 0;

    if ( ! tmp )
    {
      mutex.unlock ();
      mon ->lock   ();

      sig = (byte) (sig | mon->wait( SigThread::NOTIFY ));

      mon->unlock  ();
      mutex.lock   ();
    }
  }

  notified_ = false;

  return sig;
}


//-----------------------------------------------------------------------
//   waitNoCancel
//-----------------------------------------------------------------------


void ThreadNode::waitNoCancel ( SpinLock& mutex )
{
  const lint  n = ThreadLib::SPIN_COUNT;

  Monitor*    mon;
  lint        i;


  if ( n > 0 )
  {
    mutex.unlock ();

    JEM_SPIN_WHILE ( ! notified_, i, n );

    mutex.lock   ();
  }

  if ( notified_ )
  {
    notified_ = false;
    return;
  }

  mon = monitor_ = ThreadLib::getMonitor ();

  mutex.unlock ();
  mon ->lock   ();
  mon ->wait   ( SigThread::NOTIFY );
  mon ->unlock ();
  mutex.lock   ();

  notified_ = false;
}


//=======================================================================
//   class ThreadQueue
//=======================================================================

//-----------------------------------------------------------------------
//   remove
//-----------------------------------------------------------------------


bool ThreadQueue::remove ( Node*& queue, Node* node )
{
  if ( ! queue )
  {
    return false;
  }
  else if ( queue == node && node->next_ == node )
  {
    queue = 0;
    return true;
  }
  else
  {
    Node*  helper = queue;

    while ( helper->next_ != queue )
    {
      if ( helper->next_ == node )
      {
        helper->next_ = node->next_;
        node  ->next_ = node;

        if ( queue == node )
        {
          queue = helper;
        }

        return true;
      }

      helper = helper->next_;
    }

    return false;
  }
}


JEM_END_PACKAGE_BASE
