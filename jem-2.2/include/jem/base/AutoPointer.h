
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

#ifndef JEM_BASE_AUTOPOINTER_H
#define JEM_BASE_AUTOPOINTER_H

#include <jem/base/assert.h>
#include <jem/base/utilities.h>
#include <jem/base/TypeTraits.h>


JEM_BEGIN_PACKAGE_BASE


//-----------------------------------------------------------------------
//   class AutoPointer
//-----------------------------------------------------------------------


template <class T>

  class AutoPointer

{
 public:

  typedef AutoPointer       Self;


  inline                    AutoPointer ();

  inline                    AutoPointer

    ( T*                      rhs );

  inline                    AutoPointer

    ( const AutoPointer&      rhs );

  template <class U>
    inline                  AutoPointer

    ( const AutoPointer<U>&   rhs );

  inline                    AutoPointer

    ( const Nil&              rhs );

  inline                   ~AutoPointer ();

  AutoPointer&              operator  =

    ( const Self&             rhs );

  inline bool               operator ==

    ( const Self&             rhs )        const;

  inline bool               operator ==

    ( const Nil&              rhs )        const;

  inline bool               operator !=

    ( const Self&             rhs )        const;

  inline bool               operator !=

    ( const Nil&              rhs )        const;

  inline T*                 operator -> () const;
  inline T&                 operator  * () const;

  inline void               swap

    ( Self&                   rhs );

  inline bool               isOwner     () const;
  inline T*                 release     () const;
  inline T*                 get         () const;


 private:

  bool                      owner_;
  T*                        object_;

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


template <class T>

  inline void               swap

  ( AutoPointer<T>&           lhs,
    AutoPointer<T>&           rhs );







//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class AutoPointer
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


template <class T>

  inline AutoPointer<T>::AutoPointer ()

{
  owner_  = false;
  object_ = NULL;
}


template <class T>

  inline AutoPointer<T>::AutoPointer ( T* rhs )

{
  owner_  = true;
  object_ = rhs;
}


template <class T>

  inline AutoPointer<T>::AutoPointer ( const Self& rhs )

{
  owner_  = rhs.owner_;
  object_ = rhs.release ();
}


template <class T>
  template <class U>

  inline AutoPointer<T>::AutoPointer ( const AutoPointer<U>& rhs )

{
  owner_  = rhs.isOwner ();
  object_ = rhs.release ();
}


template <class T>

  inline AutoPointer<T>::AutoPointer ( const Nil& )

{
  owner_  = false;
  object_ = NULL;
}


template <class T>

  inline AutoPointer<T>::~AutoPointer ()

{
  if ( owner_ )
  {
    delete object_;
  }
}


//-----------------------------------------------------------------------
//   assignment operator
//-----------------------------------------------------------------------


template <class T>

  AutoPointer<T>& AutoPointer<T>::operator = ( const Self& rhs )

{
  if ( object_ != rhs.object_ )
  {
    if ( owner_ )
    {
      delete object_;
    }

    owner_  = rhs.owner_;
    object_ = rhs.release ();
  }

  return *this;
}


//-----------------------------------------------------------------------
//   equality operators
//-----------------------------------------------------------------------


template <class T>

  inline bool AutoPointer<T>::operator == ( const Self& rhs ) const

{
  return (object_ == rhs.object_);
}


template <class T>

  inline bool AutoPointer<T>::operator == ( const Nil& ) const

{
  return (object_ == NULL);
}


//-----------------------------------------------------------------------
//   inequality operators
//-----------------------------------------------------------------------


template <class T>

  inline bool AutoPointer<T>::operator != ( const Self& rhs ) const

{
  return (object_ != rhs.object_);
}


template <class T>

  inline bool AutoPointer<T>::operator != ( const Nil& ) const

{
  return (object_ != NULL);
}


//-----------------------------------------------------------------------
//   access operators
//-----------------------------------------------------------------------


template <class T>

  inline T* AutoPointer<T>::operator -> () const

{
  JEM_ASSERT2 ( get(), "NULL pointer" );

  return object_;
}


template <class T>

  inline T& AutoPointer<T>::operator * () const

{
  JEM_ASSERT2 ( get(), "NULL pointer" );

  return *object_;
}


//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


template <class T>

  inline void AutoPointer<T>::swap ( AutoPointer& rhs )

{
  jem::swap ( owner_,  rhs.owner_ );
  jem::swap ( object_, rhs.object_ );
}


//-----------------------------------------------------------------------
//   isOwner
//-----------------------------------------------------------------------


template <class T>

  inline bool AutoPointer<T>::isOwner () const

{
  return owner_;
}


//-----------------------------------------------------------------------
//   release
//-----------------------------------------------------------------------


template <class T>

  inline T* AutoPointer<T>::release () const

{
  const_cast<AutoPointer<T>*>( this )->owner_ = false;
  return object_;
}


//-----------------------------------------------------------------------
//   get
//-----------------------------------------------------------------------


template <class T>

  inline T* AutoPointer<T>::get () const

{
  return object_;
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


template <class T>

  inline void swap ( AutoPointer<T>& lhs, AutoPointer<T>& rhs )

{
  lhs.swap ( rhs );
}


//=======================================================================
//   class TypeTraits< AutoPointer<T> >
//=======================================================================


template <class T>

  class TypeTraits< AutoPointer<T> > : public BasicTypeTraits

{
 public:

  static String             whatis   ()
  {
    return TypeTraits<T>::whatis() + String ( " pointer" );
  }

  static String             typeName ()
  {
    return ( String ( "jem::AutoPointer<" ) +
             TypeTraits<T>::typeName ()     +
             String ( '>' ) );
  }

  static lint               hashValue

    ( const AutoPointer<T>&   p )

  {
    return TypeTraits<T*>::hashValue ( p.get() );
  }

};


JEM_END_PACKAGE_BASE

#endif
