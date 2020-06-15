
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


#include <jem/base/WeakProxy.h>
#include <jem/base/SerialSection.h>
#include <jem/base/WeakRef.h>


JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class WeakRefImp
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


WeakRefImp::WeakRefImp ()
{
  proxy_ = 0;
}


WeakRefImp::WeakRefImp ( Collectable* c )
{
  if ( c )
  {
    if ( ! c->proxy_ )
    {
      SerialSection  serialSection;

      if ( ! c->proxy_ )
      {
        proxy_           = new WeakProxy ( c );
        proxy_->refcount = 2;
        c->proxy_        = proxy_;
      }
    }
    else
    {
      proxy_ = c->proxy_;
      WeakProxy::addRef ( proxy_ );
    }
  }
  else
  {
    proxy_ = 0;
  }
}


WeakRefImp::WeakRefImp ( const WeakRefImp& rhs )
{
  proxy_ = rhs.proxy_;

  WeakProxy::addRef ( proxy_ );
}


WeakRefImp::~WeakRefImp ()
{
  WeakProxy::delRef ( proxy_ );

  proxy_ = 0;
}


//-----------------------------------------------------------------------
//   assignment operator
//-----------------------------------------------------------------------


WeakRefImp& WeakRefImp::operator = ( const WeakRefImp& rhs )
{
  if ( proxy_ != rhs.proxy_  )
  {
    WeakProxy*  tmp = proxy_;

    proxy_ = rhs.proxy_;

    WeakProxy::addRef ( proxy_ );
    WeakProxy::delRef ( tmp    );
  }

  return *this;
}


//-----------------------------------------------------------------------
//   get
//-----------------------------------------------------------------------


Collectable* WeakRefImp::get () const
{
  Collectable*  c = 0;

  if ( proxy_ )
  {
    proxy_->spinlock.lock();

    if ( proxy_->collectable )
    {
      // This part is tricky since it is possible that the collectable
      // object is executing the Collectable::kill_ method but has not
      // yet locked the proxy_->spinlock. If this is the case, its
      // reference count will be zero.

      c = proxy_->collectable;

      if ( c->refcount_.incr() == 1 )
      {
        c->refcount_.decr();
        c = 0;
      }
    }

    proxy_->spinlock.unlock ();

    if ( ! c )
    {
      WeakProxy::delRef ( proxy_ );

      const_cast<WeakRefImp*>( this )->proxy_ = 0;
    }
  }

  return c;
}


JEM_END_PACKAGE_BASE
