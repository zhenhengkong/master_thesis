
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

#ifndef JEM_BASE_WEAKREF_H
#define JEM_BASE_WEAKREF_H

#include <jem/base/Ref.h>


JEM_BEGIN_PACKAGE_BASE


//-----------------------------------------------------------------------
//   class WeakRefImp
//-----------------------------------------------------------------------


class WeakRefImp
{
 public:

  typedef WeakRefImp      Self;


                          WeakRefImp  ();

                          WeakRefImp

    ( Collectable*          obj );

                          WeakRefImp

    ( const Self&           rhs );

                         ~WeakRefImp  ();


  Self&                   operator =

    ( const Self&           rhs );

  Collectable*            get         () const;

  inline void             swap

    ( Self&                 rhs );


 private:

  WeakProxy*              proxy_;

};


//-----------------------------------------------------------------------
//   class WeakRef
//-----------------------------------------------------------------------


template <class T>

  class WeakRef

{
 public:

  typedef WeakRef         Self;


  inline                  WeakRef     ();

  inline                  WeakRef

    ( const Nil&            rhs );

  inline                  WeakRef

    ( const Ref<T>&         rhs );

  inline                  WeakRef

    ( const WeakRef&        rhs );


  inline WeakRef&         operator =

    ( const WeakRef&        rhs );

  inline WeakRef&         operator =

    ( const Nil&            rhs );


  inline Ref<T>           get         () const;

  inline void             swap

    ( WeakRef&              rhs );


 private:

  T*                      object_;
  WeakRefImp              weakref_;

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


template <class T>

  inline void             swap

  ( WeakRef<T>&             lhs,
    WeakRef<T>&             rhs );







//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class WeakRefImp
//=======================================================================


inline void WeakRefImp::swap ( Self& rhs )
{
  jem::swap ( proxy_, rhs.proxy_ );
}


//=======================================================================
//   class WeakRef
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


template <class T>

  inline WeakRef<T>::WeakRef () : object_ ( 0 )

{}


template <class T>

  inline WeakRef<T>::WeakRef ( const Nil& ) : object_ ( 0 )

{}


template <class T>

  inline WeakRef<T>::WeakRef ( const Ref<T>& rhs ) :

    object_  ( rhs.get()     ),
    weakref_ ( rhs.getBase() )

{}


template <class T>

  inline WeakRef<T>::WeakRef ( const Self& rhs ) :

    object_  ( rhs.object_  ),
    weakref_ ( rhs.weakref_ )

{}


//-----------------------------------------------------------------------
//   assignment operators
//-----------------------------------------------------------------------


template <class T>

  inline WeakRef<T>& WeakRef<T>::operator = ( const Self& rhs )

{
  object_  = rhs.object_;
  weakref_ = rhs.weakref_;

  return *this;
}


template <class T>

  inline WeakRef<T>& WeakRef<T>::operator = ( const Nil& )

{
  object_  = 0;
  weakref_ = WeakRefImp ();

  return *this;
}


//-----------------------------------------------------------------------
//   get
//-----------------------------------------------------------------------


template <class T>

  inline Ref<T> WeakRef<T>::get () const

{
  Collectable*  base = weakref_.get ();

  if ( base )
  {
    return Ref<T> ( *object_, *base, RAW_REF );
  }
  else
  {
    const_cast<Self*> ( this ) -> object_ = 0;

    return Ref<T> ();
  }
}


//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


template <class T>

  inline void WeakRef<T>::swap ( WeakRef& rhs )

{
  jem    ::swap ( object_, rhs.object_  );
  weakref_.swap (          rhs.weakref_ );
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


template <class T>

  inline void    swap

  ( WeakRef<T>&    lhs,
    WeakRef<T>&    rhs )

{
  lhs.swap ( rhs );
}


JEM_END_PACKAGE_BASE

#endif
