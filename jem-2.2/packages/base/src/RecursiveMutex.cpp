
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


#include <jem/base/assert.h>
#include <jem/base/thread/ThreadLib.h>
#include <jem/base/thread/ThreadQueue.h>
#include <jem/base/RecursiveMutex.h>


JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class RecursiveMutex
//=======================================================================

//
// Note: the field `count_' should only be accessed by the owner of the
//       recursive mutex. The owner of an unlocked mutex is NULL.
//

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


RecursiveMutex::RecursiveMutex ()
{
  ThreadLib::init ();

  queue_ = NULL;
  owner_ = NULL;
  count_ = 0;
}


RecursiveMutex::~RecursiveMutex ()
{
  queue_ = NULL;
  owner_ = NULL;
}


//-----------------------------------------------------------------------
//   tryLock
//-----------------------------------------------------------------------


bool RecursiveMutex::tryLock ()
{
  Thread*  self = ThreadLib::getThread ();

  bool     result;


  spinlock_.lock ();

  if ( owner_ && owner_ != self )
  {
    spinlock_.unlock ();
    result = false;
  }
  else
  {
    owner_ = self;
    spinlock_.unlock ();
    count_++;
    result = true;
  }

  return result;
}


//-----------------------------------------------------------------------
//   lock
//-----------------------------------------------------------------------


void RecursiveMutex::lock ()
{
  Thread*  self = ThreadLib::getThread ();


  spinlock_.lock ();

  if ( owner_ && owner_ != self )
  {
    ThreadNode  node ( self );

    ThreadQueue::push ( queue_, &node );
    node.waitNoCancel ( spinlock_ );
  }
  else
  {
    owner_ = self;
  }

  spinlock_.unlock ();

  count_++;
}


//-----------------------------------------------------------------------
//   unlock
//-----------------------------------------------------------------------


void RecursiveMutex::unlock ()
{
  JEM_ASSERT_NOTHROW ( locked() );

  if ( (--count_) > 0 )
  {
    return;
  }

  spinlock_.lock ();

  if ( queue_ )
  {
    ThreadNode*  node = ThreadQueue::pop ( queue_ );

    owner_ = static_cast<Thread*> ( node->getData() );

    node->notify ();
  }
  else
  {
    owner_ = NULL;
  }

  spinlock_.unlock ();
}


JEM_END_PACKAGE_BASE
