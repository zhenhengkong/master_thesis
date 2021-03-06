
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

#ifndef JEM_BASE_COLLECTABLE_H
#define JEM_BASE_COLLECTABLE_H

#include <new>
#include <jem/base/assert.h>
#include <jem/base/atomic.h>
#include <jem/base/utilities.h>
#include <jem/base/MemCache.h>


JEM_BEGIN_PACKAGE_BASE


class WeakRefImp;
class WeakProxy;


//-----------------------------------------------------------------------
//   class Collectable
//-----------------------------------------------------------------------


class Collectable
{
 public:

  friend class            WeakRefImp;


  inline bool             isShared      () const;

  static inline void      addRef

    ( Collectable&          obj );

  static inline void      addRef

    ( Collectable*          obj );

  static inline void      delRef

    ( Collectable&          obj );

  static inline void      delRef

    ( Collectable*          obj );


 protected:

                          Collectable   ();
                          Collectable   ( const Collectable& );
  virtual                ~Collectable   ();

  void                    operator delete

    ( void*                 ptr,
      size_t                size );


 private:

  Collectable&            operator =    ( const Collectable& );

  static void             kill_

    ( Collectable*          obj );

  void*                   operator new

    ( size_t                size );


 private:

  AtomicInteger           refcount_;
  WeakProxy* volatile     proxy_;

};






//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   isShared
//-----------------------------------------------------------------------


inline bool Collectable::isShared () const
{
  return (refcount_.get() > 1);
}


//-----------------------------------------------------------------------
//   addRef
//-----------------------------------------------------------------------


inline void Collectable::addRef ( Collectable& obj )
{
  obj.refcount_.incr ();
}


inline void Collectable::addRef ( Collectable* obj )
{
  if ( obj )
  {
    obj->refcount_.incr ();
  }
}


//-----------------------------------------------------------------------
//   delRef
//-----------------------------------------------------------------------


inline void Collectable::delRef ( Collectable& obj )
{
  if ( obj.refcount_.decr() == 0 )
  {
    kill_ ( &obj );
  }
}


inline void Collectable::delRef ( Collectable* obj )
{
  if ( obj && obj->refcount_.decr() == 0 )
  {
    kill_ ( obj );
  }
}


JEM_END_PACKAGE_BASE

#endif
