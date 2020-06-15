
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

#ifndef JEM_BASE_WEAKPROXY_H
#define JEM_BASE_WEAKPROXY_H

#include <jem/base/SpinLock.h>


JEM_BEGIN_PACKAGE_BASE


class Collectable;


//-----------------------------------------------------------------------
//   class WeakProxy
//-----------------------------------------------------------------------


class WeakProxy
{
 public:

  explicit inline      WeakProxy ( Collectable*  obj   );

  static inline void   addRef    ( WeakProxy*    proxy );
  static inline void   delRef    ( WeakProxy*    proxy );


 public:

  SpinLock             spinlock;
  int                  refcount;
  Collectable*         collectable;

};




//#######################################################################
//   Implementation
//#######################################################################


WeakProxy::WeakProxy ( Collectable* c )
{
  refcount    = 0;
  collectable = c;
}


inline void WeakProxy::addRef ( WeakProxy* proxy )
{
  if ( proxy )
  {
    proxy->spinlock.lock();
    proxy->refcount++;
    proxy->spinlock.unlock();
  }
}


inline void WeakProxy::delRef ( WeakProxy* proxy )
{
  if ( proxy )
  {
    int  refcount;

    proxy->spinlock.lock();
    proxy->refcount--;
    refcount = proxy->refcount;
    proxy->spinlock.unlock();

    if ( refcount == 0 )
    {
      delete proxy;
    }
  }
}


JEM_END_PACKAGE_BASE

#endif
