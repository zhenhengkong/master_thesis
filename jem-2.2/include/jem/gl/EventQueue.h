
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

#ifndef JEM_GL_EVENTQUEUE_H
#define JEM_GL_EVENTQUEUE_H

#include <jem/base/assert.h>
#include <jem/base/utilities.h>
#include <jem/gl/Event.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class EventQueue
//-----------------------------------------------------------------------


class EventQueue
{
 public:

  static const int          MAX_EVENTS  = 256;


                            EventQueue  ();
                           ~EventQueue  ();


  inline idx_t              size        () const;
  inline GfxEvent*          peek        () const;
  inline EventPtr           pop         ();

  idx_t                     push

    ( GfxEvent*               ev );

  void                      clear       ();

  inline void               swap

    ( EventQueue&             rhs );


 private:

                            EventQueue

    ( const EventQueue&       rhs );

  EventQueue&               operator =

    ( const EventQueue&       rhs );

  void                      prune_      ();


 private:

  GfxEvent*                 tail_;
  idx_t                     size_;

};




//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   size
//-----------------------------------------------------------------------


inline idx_t EventQueue::size () const
{
  return size_;
}


//-----------------------------------------------------------------------
//   peek
//-----------------------------------------------------------------------


inline GfxEvent* EventQueue::peek () const
{
  if ( tail_ )
  {
    return tail_->next ();
  }
  else
  {
    return NULL;
  }
}


//-----------------------------------------------------------------------
//   pop
//-----------------------------------------------------------------------


inline EventPtr EventQueue::pop ()
{
  JEM_ASSERT ( size() );

  GfxEvent*  ev = tail_->next ();

  ev->unlink ();

  size_--;

  return ev;
}


//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


inline void EventQueue::swap ( EventQueue& rhs )
{
  jem::swap ( tail_, rhs.tail_ );
  jem::swap ( size_, rhs.size_ );
}


JEM_END_PACKAGE( gl )

#endif
