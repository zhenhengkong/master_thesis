
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

#ifndef JEM_BASE_REF_H
#define JEM_BASE_REF_H

#include <jem/base/Collectable.h>


JEM_BEGIN_PACKAGE_BASE


class Object;

template <class T> class  Ref;
template <>        class  Ref<Object>;


//-----------------------------------------------------------------------
//   class RawRef
//-----------------------------------------------------------------------


class                     RawRef {};

extern const RawRef       RAW_REF;


//-----------------------------------------------------------------------
//   class Ref
//-----------------------------------------------------------------------


template <class T>

  class Ref

{
 public:

  inline                  Ref         ();

  inline                  Ref

    ( const Nil&            rhs );

  inline                  Ref

    ( T*                    rhs );

  inline                  Ref

    ( const Ref&            rhs );

  template <class U>
    inline                Ref

    ( const Ref<U>&         rhs );

  inline                  Ref

    ( T&                    rhs,
      Collectable&          base );

  inline                  Ref

    ( T&                    rhs,
      Collectable&          base,
      const RawRef&         raw );

  inline                 ~Ref         ();

  Ref&                    operator  =

    ( const Ref&            rhs );

  template <class U>
    Ref&                  operator  =

    ( const Ref<U>&         rhs );

  Ref&                    operator  =

    ( T*                    rhs );

  inline Ref&             operator  =

    ( const Nil&            rhs );

  template <class U>
    inline bool           operator ==

    ( const Ref<U>&         rhs )        const;

  template <class U>
    inline bool           operator ==

    ( const U*              rhs )        const;

  inline bool             operator ==

    ( const Nil&            rhs )        const;

  inline T*               operator -> () const;
  inline T&               operator  * () const;

  inline void             swap

    ( Ref&                  rhs );

  inline T*               get         () const;
  inline Collectable*     getBase     () const;


 private:

  T*                      obj_;
  Collectable*            base_;

};


//-----------------------------------------------------------------------
//   class RefCaster
//-----------------------------------------------------------------------


template <class T, class U>

  class RefCaster

{
 public:

  typedef Ref<T>          ReturnType;


  static inline Ref<T>    staticCast

    ( const Ref<U>&         rhs );

  static inline Ref<T>    dynamicCast

    ( const Ref<U>&         rhs );

  static Ref<T>           checkedCast

    ( const Ref<U>&         rhs );

};


//-----------------------------------------------------------------------
//   class RefCaster<T*,U>
//-----------------------------------------------------------------------


template <class T, class U>

  class RefCaster<T*,U>

{
 public:

  typedef T*              ReturnType;


  static inline T*        staticCast

    ( const Ref<U>&         rhs );

  static inline T*        dynamicCast

    ( const Ref<U>&         rhs );

  static T*               checkedCast

    ( const Ref<U>&         rhs );

};


//-----------------------------------------------------------------------
//   class RefCaster<T&,U>
//-----------------------------------------------------------------------


template <class T, class U>

  class RefCaster<T&,U>

{
 public:

  typedef T&              ReturnType;


  static inline T&        staticCast

    ( const Ref<U>&         rhs );

  static T&               dynamicCast

    ( const Ref<U>&         rhs );

  static T&               checkedCast

    ( const Ref<U>&         rhs );

};


//-----------------------------------------------------------------------
//   class DynRefCaster<T,U>
//-----------------------------------------------------------------------


template <class T, class U>

  class DynRefCaster

{
 public:

  static inline T*        dynamicCast

    ( const Ref<U>&         rhs );

};

//-----------------------------------------------------------------------
//   Related functions
//-----------------------------------------------------------------------


template <class T>

  inline void             swap

  ( Ref<T>&                 lhs,
    Ref<T>&                 rhs );

template <class T>

  Ref<T>                  newInstance ();

template <class T, class A>

  Ref<T>                  newInstance

  ( const A&                a );

template <class T, class A, class B>

  Ref<T>                  newInstance

  ( const A&                a,
    const B&                b );

template <class T, class A, class B,
          class C>

  Ref<T>                  newInstance

  ( const A&                a,
    const B&                b,
    const C&                c );

template <class T, class A, class B,
          class C, class D>

  Ref<T>                  newInstance

  ( const A&                a,
    const B&                b,
    const C&                c,
    const D&                d );

template <class T, class A, class B,
          class C, class D, class E>

  Ref<T>                  newInstance

  ( const A&                a,
    const B&                b,
    const C&                c,
    const D&                d,
    const E&                e );

template <class T, class A, class B,
          class C, class D, class E, class F>

  Ref<T>                  newInstance

  ( const A&                a,
    const B&                b,
    const C&                c,
    const D&                d,
    const E&                e,
    const F&                f );

template <class T, class U>

  inline bool             operator ==

  ( const T*                lhs,
    const Ref<U>&           rhs );

template <class T>

  inline bool             operator ==

  ( const Nil&              lhs,
    const Ref<T>&           rhs );

template <class T, class U>

  inline bool             operator !=

  ( const Ref<T>&           lhs,
    const Ref<U>&           rhs );

template <class T, class U>

  inline bool             operator !=

  ( const Ref<T>&           lhs,
    const U*                rhs );

template <class T, class U>

  inline bool             operator !=

  ( const T*                lhs,
    const Ref<U>&           rhs );

template <class T>

  inline bool             operator !=

  ( const Ref<T>&           lhs,
    const Nil&              rhs );

template <class T>

  inline bool             operator !=

  ( const Nil&              lhs,
    const Ref<T>&           rhs );

template <class T, class U>

  inline
  typename RefCaster<T,U>::ReturnType

                          staticCast

  ( const Ref<U>&           rhs );

template <class T, class U>

  inline
  typename RefCaster<T,U>::ReturnType

                          dynamicCast

  ( const Ref<U>&           rhs );

template <class T, class U>

  inline
  typename RefCaster<T,U>::ReturnType

                          checkedCast

  ( const Ref<U>&           rhs );

template <class T>

  inline void             castError

  ( const char*             where,
    const Ref<T>&           rhs );




//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//  class Ref
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


template <class T>

  inline Ref<T>::Ref ()

{
  obj_  = NULL;
  base_ = NULL;
}


template <class T>

  inline Ref<T>::Ref ( const Nil& )

{
  obj_  = NULL;
  base_ = NULL;
}


template <class T>

  inline Ref<T>::Ref ( T* rhs )

{
  obj_  = rhs;
  base_ = rhs;

  Collectable::addRef ( base_ );
}


template <class T>

  inline Ref<T>::Ref ( const Ref& rhs )

{
  obj_  = rhs.obj_;
  base_ = rhs.base_;

  Collectable::addRef ( base_ );
}


template <class T>
  template <class U>

  inline Ref<T>::Ref ( const Ref<U>& rhs )

{
  obj_  = rhs.get     ();
  base_ = rhs.getBase ();

  Collectable::addRef ( base_ );
}


template <class T>

  inline Ref<T>::Ref

  ( T&            rhs,
    Collectable&  base )

{
  obj_  = & rhs;
  base_ = & base;

  Collectable::addRef ( base );
}


template <class T>

  inline Ref<T>::Ref

  ( T&             rhs,
    Collectable&   base,
    const RawRef&  raw )

{
  obj_  = & rhs;
  base_ = & base;
}


template <class T>

  inline Ref<T>::~Ref ()

{
  Collectable::delRef ( base_ );
}


//-----------------------------------------------------------------------
//   assignment operators
//-----------------------------------------------------------------------


template <class T>

  Ref<T>& Ref<T>::operator = ( const Ref& rhs )

{
  Collectable::addRef ( rhs.base_ );
  Collectable::delRef (     base_ );

  obj_  = rhs.obj_;
  base_ = rhs.base_;

  return *this;
}


template <class T>
  template <class U>

  Ref<T>& Ref<T>::operator = ( const Ref<U>& rhs )

{
  Collectable::addRef ( rhs.getBase() );
  Collectable::delRef ( base_ );

  obj_  = rhs.get     ();
  base_ = rhs.getBase ();

  return *this;
}


template <class T>

  Ref<T>& Ref<T>::operator = ( T* rhs )

{
  Collectable::addRef ( rhs );
  Collectable::delRef ( base_ );

  obj_  = rhs;
  base_ = rhs;

  return *this;
}


template <class T>

  inline Ref<T>& Ref<T>::operator = ( const Nil& rhs )

{
  Collectable::delRef ( base_ );

  obj_  = NULL;
  base_ = NULL;

  return *this;
}


//-----------------------------------------------------------------------
//   equality operators
//-----------------------------------------------------------------------


template <class T>
  template <class U>

  inline bool Ref<T>::operator == ( const Ref<U>& rhs ) const

{
  return (base_ == rhs.getBase());
}


template <class T>
  template <class U>

  inline bool Ref<T>::operator == ( const U* rhs ) const

{
  return (obj_ == rhs);
}


template <class T>

  inline bool Ref<T>::operator == ( const Nil& rhs ) const

{
  return (obj_ == 0);
}


//-----------------------------------------------------------------------
//   dereferencing operators
//-----------------------------------------------------------------------


template <class T>

  inline T* Ref<T>::operator -> () const

{
  JEM_ASSERT2 ( get(), "NULL pointer" );

  return obj_;
}


template <class T>

  inline T& Ref<T>::operator * () const

{
  JEM_ASSERT2 ( get(), "NULL pointer" );

  return *obj_;
}


//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


template <class T>

  inline void Ref<T>::swap ( Ref& rhs )

{
  jem::swap ( obj_,  rhs.obj_  );
  jem::swap ( base_, rhs.base_ );
}


//-----------------------------------------------------------------------
//   get & getBase
//-----------------------------------------------------------------------


template <class T>

  inline T* Ref<T>::get () const

{
  return obj_;
}


template <class T>

  inline Collectable* Ref<T>::getBase () const

{
  return base_;
}


//=======================================================================
//   class RefCaster
//=======================================================================

//-----------------------------------------------------------------------
//   staticCast
//-----------------------------------------------------------------------


template <class T, class U>

  inline Ref<T> RefCaster<T,U>::staticCast ( const Ref<U>& rhs )

{
  if ( rhs == NIL )
  {
    return NIL;
  }
  else
  {
    return Ref<T> ( static_cast<T&>( *rhs ), *(rhs.getBase()) );
  }
}


//-----------------------------------------------------------------------
//   dynamicCast
//-----------------------------------------------------------------------


template <class T, class U>

  inline Ref<T> RefCaster<T,U>::dynamicCast ( const Ref<U>& rhs )

{
  T*  tmp = DynRefCaster<T,U>::dynamicCast ( rhs );

  if ( tmp )
  {
    return Ref<T> ( *tmp, *(rhs.getBase()) );
  }
  else
  {
    return NIL;
  }
}


//-----------------------------------------------------------------------
//   checkedCast
//-----------------------------------------------------------------------


template <class T, class U>

  Ref<T> RefCaster<T,U>::checkedCast ( const Ref<U>& rhs )

{
  if ( rhs == NIL )
  {
    return NIL;
  }
  else
  {
    T*  tmp = DynRefCaster<T,U>::dynamicCast ( rhs );

    if ( ! tmp )
    {
      castError ( JEM_FUNC, rhs );
    }

    return Ref<T> ( *tmp, *(rhs.getBase()) );
  }
}


//=======================================================================
//   class RefCaster<T*,U>
//=======================================================================

//-----------------------------------------------------------------------
//   staticCast
//-----------------------------------------------------------------------


template <class T, class U>

  inline T* RefCaster<T*,U>::staticCast ( const Ref<U>& rhs )

{
  return static_cast<T*> ( rhs.get() );
}


//-----------------------------------------------------------------------
//   dynamicCast
//-----------------------------------------------------------------------


template <class T, class U>

  inline T* RefCaster<T*,U>::dynamicCast ( const Ref<U>& rhs )

{
  return DynRefCaster<T,U>::dynamicCast ( rhs );
}


//-----------------------------------------------------------------------
//   checkedCast
//-----------------------------------------------------------------------


template <class T, class U>

  T* RefCaster<T*,U>::checkedCast ( const Ref<U>& rhs )

{
  T*  tmp = DynRefCaster<T,U>::dynamicCast ( rhs );

  if ( ! tmp && rhs != NIL )
  {
    castError ( JEM_FUNC, rhs );
  }

  return tmp;
}


//=======================================================================
//   class RefCaster<T&,U>
//=======================================================================

//-----------------------------------------------------------------------
//   staticCast
//-----------------------------------------------------------------------


template <class T, class U>

  inline T& RefCaster<T&,U>::staticCast ( const Ref<U>& rhs )

{
  return static_cast<T&> ( *rhs );
}


//-----------------------------------------------------------------------
//   dynamicCast
//-----------------------------------------------------------------------


template <class T, class U>

  T& RefCaster<T&,U>::dynamicCast ( const Ref<U>& rhs )

{
  T*  tmp = DynRefCaster<T,U>::dynamicCast ( rhs );

  if ( ! tmp )
  {
    castError ( JEM_FUNC, rhs );
  }

  return *tmp;
}


//-----------------------------------------------------------------------
//   checkedCast
//-----------------------------------------------------------------------


template <class T, class U>

  T& RefCaster<T&,U>::checkedCast ( const Ref<U>& rhs )

{
  T*  tmp = DynRefCaster<T,U>::dynamicCast ( rhs );

  if ( ! tmp )
  {
    castError ( JEM_FUNC, rhs );
  }

  return *tmp;
}


//=======================================================================
//   class DynRefCaster
//=======================================================================


template <class T, class U>

  inline T* DynRefCaster<T,U>::dynamicCast ( const Ref<U>& rhs )

{
  return dynamic_cast<T*> ( rhs.get() );
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


template <class T>

  inline void   swap

  ( Ref<T>&       lhs,
    Ref<T>&       rhs )

{
  lhs.swap ( rhs );
}


//-----------------------------------------------------------------------
//   newInstance (no args)
//-----------------------------------------------------------------------


template <class T>

  Ref<T> newInstance ()

{
  void*  mem = MemCache::alloc ( sizeof(T) );
  T*     obj;

  try
  {
    obj = ::new (mem) T();
  }
  catch ( ... )
  {
    MemCache::dealloc ( mem, sizeof(T) );
    throw;
  }

  return Ref<T> ( *obj, *obj, RAW_REF );
}


//-----------------------------------------------------------------------
//   newInstance (1 arg)
//-----------------------------------------------------------------------


template <class T, class A>

  Ref<T>    newInstance

  ( const A&  a )

{
  void*  mem = MemCache::alloc ( sizeof(T) );
  T*     obj;

  try
  {
    obj = ::new (mem) T( a );
  }
  catch ( ... )
  {
    MemCache::dealloc ( mem, sizeof(T) );
    throw;
  }

  return Ref<T> ( *obj, *obj, RAW_REF );
}


//-----------------------------------------------------------------------
//   newInstance (2 args)
//-----------------------------------------------------------------------


template <class T, class A, class B>

  Ref<T>    newInstance

  ( const A&  a,
    const B&  b )

{
  void*  mem = MemCache::alloc ( sizeof(T) );
  T*     obj;

  try
  {
    obj = ::new (mem) T( a, b );
  }
  catch ( ... )
  {
    MemCache::dealloc ( mem, sizeof(T) );
    throw;
  }

  return Ref<T> ( *obj, *obj, RAW_REF );
}


//-----------------------------------------------------------------------
//   newInstance (3 args)
//-----------------------------------------------------------------------


template <class T, class A, class B,
          class C>

  Ref<T>    newInstance

  ( const A&  a,
    const B&  b,
    const C&  c )

{
  void*  mem = MemCache::alloc ( sizeof(T) );
  T*     obj;

  try
  {
    obj = ::new (mem) T ( a, b, c );
  }
  catch ( ... )
  {
    MemCache::dealloc ( mem, sizeof(T) );
    throw;
  }

  return Ref<T> ( *obj, *obj, RAW_REF );
}


//-----------------------------------------------------------------------
//   newInstance (4 args)
//-----------------------------------------------------------------------


template <class T, class A, class B,
          class C, class D>

  Ref<T>    newInstance

  ( const A&  a,
    const B&  b,
    const C&  c,
    const D&  d )

{
  void*  mem = MemCache::alloc ( sizeof(T) );
  T*     obj;

  try
  {
    obj = ::new (mem) T ( a, b, c, d );
  }
  catch ( ... )
  {
    MemCache::dealloc ( mem, sizeof(T) );
    throw;
  }

  return Ref<T> ( *obj, *obj, RAW_REF );
}


//-----------------------------------------------------------------------
//   newInstance (5 args)
//-----------------------------------------------------------------------


template <class T, class A, class B,
          class C, class D, class E>

  Ref<T>    newInstance

  ( const A&  a,
    const B&  b,
    const C&  c,
    const D&  d,
    const E&  e )

{
  void*  mem = MemCache::alloc ( sizeof(T) );
  T*     obj;

  try
  {
    obj = ::new (mem) T ( a, b, c, d, e );
  }
  catch ( ... )
  {
    MemCache::dealloc ( mem, sizeof(T) );
    throw;
  }

  return Ref<T> ( *obj, *obj, RAW_REF );
}


//-----------------------------------------------------------------------
//   newInstance (6 args)
//-----------------------------------------------------------------------


template <class T, class A, class B,
          class C, class D, class E, class F>

  Ref<T>    newInstance

  ( const A&  a,
    const B&  b,
    const C&  c,
    const D&  d,
    const E&  e,
    const F&  f )

{
  void*  mem = MemCache::alloc ( sizeof(T) );
  T*     obj;

  try
  {
    obj = ::new (mem) T ( a, b, c, d, e, f );
  }
  catch ( ... )
  {
    MemCache::dealloc ( mem, sizeof(T) );
    throw;
  }

  return Ref<T> ( *obj, *obj, RAW_REF );
}


//-----------------------------------------------------------------------
//   equality operators
//-----------------------------------------------------------------------


template <class T, class U>

  inline bool    operator ==

  ( const T*       lhs,
    const Ref<U>&  rhs )

{
  return rhs.operator == ( lhs );
}


template <class T>

  inline bool    operator ==

  ( const Nil&     lhs,
    const Ref<T>&  rhs )

{
  return rhs.operator == ( lhs );
}


template <class T, class U>

  inline bool    operator !=

  ( const Ref<T>&  lhs,
    const Ref<U>&  rhs )

{
  return ! lhs.operator == ( rhs );
}


template <class T, class U>

  inline bool    operator !=

  ( const Ref<T>&  lhs,
    const U*       rhs )

{
  return ! lhs.operator == ( rhs );
}


template <class T, class U>

  inline bool    operator !=

  ( const T*       lhs,
    const Ref<U>&  rhs )

{
  return ! rhs.operator == ( lhs );
}


template <class T>

  inline bool    operator !=

  ( const Ref<T>&  lhs,
    const Nil&     rhs )

{
  return ! lhs.operator == ( rhs );
}


template <class T>

  inline bool    operator !=

  ( const Nil&     lhs,
    const Ref<T>&  rhs )

{
  return ! rhs.operator == ( lhs );
}


//-----------------------------------------------------------------------
//   staticCast
//-----------------------------------------------------------------------


template <class T, class U>

  inline
  typename RefCaster<T,U>::ReturnType

                  staticCast

  ( const Ref<U>&   rhs )

{
  return RefCaster<T,U>::staticCast ( rhs );
}


//-----------------------------------------------------------------------
//   dynamicCast
//-----------------------------------------------------------------------


template <class T, class U>

  inline
  typename RefCaster<T,U>::ReturnType

                  dynamicCast

  ( const Ref<U>&   rhs )

{
  return RefCaster<T,U>::dynamicCast ( rhs );
}


template <class T, class U>

  inline
  typename RefCaster<T,U>::ReturnType

                  checkedCast

  ( const Ref<U>&   rhs )

{
  return RefCaster<T,U>::checkedCast ( rhs );
}


//-----------------------------------------------------------------------
//   castError
//-----------------------------------------------------------------------


void              castError

  ( const char*     where,
    Collectable*    rhs );


template <class T>

  inline void     castError

  ( const char*     where,
    const Ref<T>&   rhs )

{
  castError ( where, rhs.getBase() );
}


JEM_END_PACKAGE_BASE

#endif
