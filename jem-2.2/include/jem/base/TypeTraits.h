
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

#ifndef JEM_BASE_TYPETRAITS_H
#define JEM_BASE_TYPETRAITS_H

#include <jem/base/utilities.h>
#include <jem/base/String.h>
#include <jem/util/hash.h>


JEM_BEGIN_PACKAGE_BASE


//-----------------------------------------------------------------------
//   class BasicTypeTraits
//-----------------------------------------------------------------------


class BasicTypeTraits
{
 public:

  static const bool     IS_SERIALIZABLE  = false;
  static const bool     HAS_TRIVIAL_CTOR = false;
  static const bool     HAS_TRIVIAL_COPY = false;
  static const bool     HAS_TRIVIAL_DTOR = false;


  static String         whatis    ()
  {
    return String ( "an unknown object" );
  }


  static String         typeName  ()
  {
    return String ( "unknown" );
  }


  template <class T>

    static lint         hashValue ( const T& )

  {
    return 0L;
  }


  template <class T>

    static const T&     clone     ( const T& rhs )

  {
    return rhs;
  }

};


//-----------------------------------------------------------------------
//   class TypeTraits (primary template)
//-----------------------------------------------------------------------


template <class T>

  class TypeTraits : public BasicTypeTraits {};


//-----------------------------------------------------------------------
//   utility type traits classes
//-----------------------------------------------------------------------


template <class T>

  class IsSerializable :
  public BoolConstant<TypeTraits<T>::IS_SERIALIZABLE>    {};

template <class T>

  class HasTrivialCtor :
  public BoolConstant<TypeTraits<T>::HAS_TRIVIAL_CTOR>   {};

template <class T>

  class HasTrivialCopy :
  public BoolConstant<TypeTraits<T>::HAS_TRIVIAL_COPY>   {};

template <class T>

  class HasTrivialDtor :
  public BoolConstant<TypeTraits<T>::HAS_TRIVIAL_DTOR>   {};


//-----------------------------------------------------------------------
//   class TypeTraits<void>
//-----------------------------------------------------------------------


template <>

  class TypeTraits<void> : public BasicTypeTraits

{
 public:

  static String         whatis    ();
  static String         typeName  ();

};


//-----------------------------------------------------------------------
//   class IntTypeTraits
//-----------------------------------------------------------------------


template <class T>

  class IntTypeTraits : public BasicTypeTraits

{
 public:

  static const bool     IS_SERIALIZABLE  = true;
  static const bool     HAS_TRIVIAL_CTOR = true;
  static const bool     HAS_TRIVIAL_COPY = true;
  static const bool     HAS_TRIVIAL_DTOR = true;


  static String         whatis    ()
  {
    return String ( "an integer" );
  }

  static lint           hashValue ( T value )
  {
    return (lint) value;
  }

};


//-----------------------------------------------------------------------
//   TypeTraits specializations for integral types
//-----------------------------------------------------------------------


template <>

  class TypeTraits<bool> : public IntTypeTraits<bool>

{
 public:

  static String         whatis   ();
  static String         typeName ();

};


template <>

  class TypeTraits<byte> : public IntTypeTraits<byte>

{
 public:

  static String         whatis   ();
  static String         typeName ();

};


template <>

  class TypeTraits<char> : public IntTypeTraits<char>

{
 public:

  static String         whatis   ();
  static String         typeName ();

};



template <>

  class TypeTraits<short> : public IntTypeTraits<short>

{
 public:

  static String         typeName ();

};


template <>

  class TypeTraits<int> : public IntTypeTraits<int>

{
 public:

  static String         typeName ();

};


template <>

  class TypeTraits<lint> : public IntTypeTraits<lint>

{
 public:

  static String         typeName ();

};


//-----------------------------------------------------------------------
//   class FloatTypeTraits
//-----------------------------------------------------------------------


template <class T>

  class FloatTypeTraits : public BasicTypeTraits

{
 public:

  static const bool     IS_SERIALIZABLE  = true;
  static const bool     HAS_TRIVIAL_CTOR = true;
  static const bool     HAS_TRIVIAL_COPY = true;
  static const bool     HAS_TRIVIAL_DTOR = true;


  static String         whatis ()
  {
    return String ( "a floating point number" );
  }

};


//-----------------------------------------------------------------------
//   TypeTraits specializations for floating types
//-----------------------------------------------------------------------


template <>

  class TypeTraits<float> : public FloatTypeTraits<float>

{
 public:

  static String         typeName  ();

  static lint           hashValue ( float f )
  {
    return util::hash ( &f, sizeof(f) );
  }

};


template <>

  class TypeTraits<double> : public FloatTypeTraits<double>

{
 public:

  static String         typeName  ();

  static lint           hashValue ( double d )
  {
    return util::hash ( &d, sizeof(d) );
  }

};


//-----------------------------------------------------------------------
//   class PtrTypeTraits
//-----------------------------------------------------------------------


template <class T>

  class PtrTypeTraits : public BasicTypeTraits

{
 public:

  static const bool     IS_SERIALIZABLE  = false;
  static const bool     HAS_TRIVIAL_CTOR = true;
  static const bool     HAS_TRIVIAL_COPY = true;
  static const bool     HAS_TRIVIAL_DTOR = true;


  static String         whatis ()
  {
    return TypeTraits<T>::whatis() + String ( " pointer" );
  }

};


//-----------------------------------------------------------------------
//   TypeTraits specializations for pointers
//-----------------------------------------------------------------------


template <>

  class TypeTraits<void*> : public PtrTypeTraits<void>

{
 public:

  static String         whatis    ();
  static String         typeName  ();

  static lint           hashValue ( void* p )
  {
    if ( sizeof(void*) <= sizeof(lint) )
    {
      return (lint) p;
    }
    else
    {
      return util::hash ( &p, sizeof(p) );
    }
  }

};


template <class T>

  class TypeTraits<T*> : public PtrTypeTraits<T>

{
 public:

  static String         typeName  ()
  {
    return ( TypeTraits<T>::typeName() + String( '*' ) );
  }

  static lint           hashValue ( T* p )
  {
    return TypeTraits<void*>::hashValue ( (void*) p );
  }

};


template <class T>

  class TypeTraits<const T*> : public PtrTypeTraits<T>

{
 public:

  static String         typeName  ()
  {
    return ( String ( "const " )        +
             TypeTraits<T>::typeName () +
             String ( '*' ) );
  }

  static lint           hashValue ( const T* p )
  {
    return TypeTraits<void*>::hashValue ( (void*) p );
  }

};


//-----------------------------------------------------------------------
//   TypeTraits specializations for reference types
//-----------------------------------------------------------------------


template <class T>

  class TypeTraits<T&> : public BasicTypeTraits

{
 public:

  static const bool     HAS_TRIVIAL_DTOR = true;


  static String         whatis    ()
  {
    return TypeTraits<T>::whatis()   + String ( " reference" );
  }

  static String         typeName  ()
  {
    return TypeTraits<T>::typeName() + String ( '&' );
  }

  static lint           hashValue ( T& value )
  {
    return TypeTraits<void*>::hashValue ( (void*) &value );
  }

};


template <class T>

  class TypeTraits<const T&> : public BasicTypeTraits

{
 public:

  static const bool     HAS_TRIVIAL_DTOR = true;


  static String         whatis    ()
  {
    return TypeTraits<T>::whatis() + String ( " reference" );
  }

  static String         typeName  ()
  {
    return ( String ( "const " )        +
             TypeTraits<T>::typeName () +
             String ( '&' ) );
  }

  static lint           hashValue ( const T& value )
  {
    return TypeTraits<void*>::hashValue( (void*) &value );
  }

};


//-----------------------------------------------------------------------
//   TypeTraits specialization for String
//-----------------------------------------------------------------------


template <>

  class TypeTraits<String> : public BasicTypeTraits

{
 public:

  static const bool     IS_SERIALIZABLE = true;


  static String         whatis    ();
  static String         typeName  ();

  static lint           hashValue ( const String& str )
  {
    return str.hashValue ();
  }

};


//-----------------------------------------------------------------------
//   class IsBaseDerived
//-----------------------------------------------------------------------


template <class B>

  class IsBaseOf_

{
 public:

  static lint           test ( B*    );
  static byte           test ( void* );

};


template <class B, class D>

  class IsBaseDerived_

{
 public:

  static const bool     VALUE =

    (sizeof(IsBaseOf_<B>::test((D*) 0)) == sizeof(lint));

};


template <class B, class D>

  class IsBaseDerived : public BoolConstant
    <IsBaseDerived_<B,D>::VALUE>

{};


//-----------------------------------------------------------------------
//   class IsPOD
//-----------------------------------------------------------------------


template <class T>

  class IsPOD_

{
 public:

  static const bool     VALUE =

    ( TypeTraits<T>::HAS_TRIVIAL_CTOR   &&
      TypeTraits<T>::HAS_TRIVIAL_COPY   &&
      TypeTraits<T>::HAS_TRIVIAL_DTOR ) ||

    IsBaseDerived<POD,T>::VALUE;

};


template <class T>

  class IsPOD : public BoolConstant< IsPOD_<T>::VALUE > {};


JEM_END_PACKAGE_BASE

#endif
