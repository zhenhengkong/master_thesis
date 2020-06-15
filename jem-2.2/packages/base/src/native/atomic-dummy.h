
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

#ifndef JEM_BASE_ATOMIC_H
#define JEM_BASE_ATOMIC_H

#include <jem/defines.h>


JEM_BEGIN_PACKAGE_BASE


//-----------------------------------------------------------------------
//   class AtomicInteger
//-----------------------------------------------------------------------


class AtomicInteger
{
 public:

  explicit inline       AtomicInteger

    ( int                 value = 0 );

  inline void           set

    ( int                 value );

  inline int            get         () const;
  inline int            incr        ();
  inline int            decr        ();


 private:

  int                   value_;

};


//-----------------------------------------------------------------------
//   class AtomicLock
//-----------------------------------------------------------------------


class AtomicLock
{
 public:

  inline                AtomicLock  ();

  inline int            tryLock     ();
  inline int            locked      () const;
  inline int            unlock      ();


 private:

  int                   lock_;

};






//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class AtomicInteger
//=======================================================================


inline AtomicInteger::AtomicInteger ( int value ) :

  value_ ( value )

{}


inline void AtomicInteger::set ( int value )
{
  value_ = value;
}


inline int AtomicInteger::get () const
{
  return value_;
}


inline int AtomicInteger::incr ()
{
  return ++value_;
}


inline int AtomicInteger::decr ()
{
  return --value_;
}


//=======================================================================
//   class AtomicLock
//=======================================================================


inline AtomicLock::AtomicLock () : lock_ ( 0 )
{}


inline int AtomicLock::tryLock ()
{
  if ( lock_ )
  {
    return 0;
  }
  else
  {
    lock_ = 1;

    return 1;
  }
}


inline int AtomicLock::locked () const
{
  return lock_;
}


inline void AtomicLock::unlock ()
{
  lock_ = 0;
}


JEM_END_PACKAGE_BASE

#endif