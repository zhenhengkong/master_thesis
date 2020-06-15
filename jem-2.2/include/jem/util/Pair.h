
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

#ifndef JEM_UTIL_PAIR_H
#define JEM_UTIL_PAIR_H

#include <jem/base/TypeTraits.h>


JEM_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   struct Pair
//-----------------------------------------------------------------------


template <class T, class U = T>

  class Pair

{
 public:

  typedef T               FirstValueType;
  typedef U               SecondValueType;


  inline                  Pair  ();

  inline                  Pair

    ( const T&              t,
      const U&              u );

  inline                  Pair

    ( const Pair&           rhs );

  template <class V, class W>

    inline                Pair

    ( const Pair<V,W>&      rhs );


 public:

  T                       first;
  U                       second;

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


template <class T, class U>

  inline Pair<T,U>        makePair

  ( const T&                first,
    const U&                second );

template <class Input,
          class T, class U>

  inline void             decode

  ( Input&                  in,
    Pair<T,U>&              p );

template <class Output,
          class T, class U>

  inline void             encode

  ( Output&                 out,
    const Pair<T,U>&        p );

template <class Output,
          class T, class U>

  inline void             print

  ( Output&                 out,
    const Pair<T,U>&        p );

template <class T, class U>

  inline bool             operator ==

  ( const Pair<T,U>&        lhs,
    const Pair<T,U>&        rhs );

template <class T, class U>

  inline bool             operator !=

  ( const Pair<T,U>&        lhs,
    const Pair<T,U>&        rhs );

template <class T, class U>

  inline bool             operator <

  ( const Pair<T,U>&        lhs,
    const Pair<T,U>&        rhs );

template <class T, class U>

  inline bool             operator <=

  ( const Pair<T,U>&        lhs,
    const Pair<T,U>&        rhs );

template <class T, class U>

  inline bool             operator >

  ( const Pair<T,U>&        lhs,
    const Pair<T,U>&        rhs );

template <class T, class U>

  inline bool             operator >=

  ( const Pair<T,U>&        lhs,
    const Pair<T,U>&        rhs );





//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class Pair
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


template <class T, class U>

  inline Pair<T,U>::Pair ()

{}


template <class T, class U>

  inline Pair<T,U>::Pair ( const T& t, const U& u ) :

    first  ( t ),
    second ( u )

{}


template <class T, class U>

  inline Pair<T,U>::Pair ( const Pair& rhs ) :

    first  (  rhs.first ),
    second ( rhs.second )

{}


template <class T, class U>
  template <class V, class W>

  inline Pair<T,U>::Pair ( const Pair<V,W>& rhs ) :

    first  (  rhs.first ),
    second ( rhs.second )

{}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   makePair
//-----------------------------------------------------------------------


template <class T, class U>

  inline Pair<T,U>        makePair

  ( const T&                first,
    const U&                second )

{
  return Pair<T,U> ( first, second );
}


//-----------------------------------------------------------------------
//   decode
//-----------------------------------------------------------------------


template <class Input,
          class T, class U>

  inline void             decode

  ( Input&                  in,
    Pair<T,U>&              p )

{
  decode ( in, p.first, p.second );
}


template <class Output,
          class T, class U>

  inline void             encode

  ( Output&                 out,
    const Pair<T,U>&        p )

{
  encode ( out, p.first, p.second );

  return out;
}


//-----------------------------------------------------------------------
//   print
//-----------------------------------------------------------------------


template <class Output,
          class T, class U>

  inline void             print

  ( Output&                 out,
    const Pair<T,U>&        p )

{
  print ( out, '(', p.first, ',', p.second, ')' );
}


//-----------------------------------------------------------------------
//   comparison operators
//-----------------------------------------------------------------------


template <class T, class U>

  inline bool             operator ==

  ( const Pair<T,U>&        lhs,
    const Pair<T,U>&        rhs )

{
  return (lhs.first == rhs.first && lhs.second == rhs.second);
}


template <class T, class U>

  inline bool             operator !=

  ( const Pair<T,U>&        lhs,
    const Pair<T,U>&        rhs )

{
  return ! (lhs == rhs);
}


template <class T, class U>

  inline bool             operator <

  ( const Pair<T,U>&        lhs,
    const Pair<T,U>&        rhs )

{
  return (lhs.first < rhs.first ||
          (! (rhs.first < lhs.first) && lhs.second < rhs.second));
}

template <class T, class U>

  inline bool             operator <=

  ( const Pair<T,U>&        lhs,
    const Pair<T,U>&        rhs )

{
  return !(rhs < lhs);
}


template <class T, class U>

  inline bool             operator >

  ( const Pair<T,U>&        lhs,
    const Pair<T,U>&        rhs )

{
  return (rhs < lhs);
}


template <class T, class U>

  inline bool             operator >=

  ( const Pair<T,U>&        lhs,
    const Pair<T,U>&        rhs )

{
  return !(lhs < rhs);
}


JEM_END_PACKAGE( util )

JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class TypeTraits< util::Pair<T,U> >
//=======================================================================


template <class T, class U>

  class TypeTraits< util::Pair<T,U> > : public BasicTypeTraits

{
 public:

  static const bool     IS_SERIALIZABLE    =

    IsSerializable<T>::VALUE && IsSerializable<U>::VALUE;

  static const bool     HAS_TRIVIAL_CTOR   =

    HasTrivialCtor<T>::VALUE && HasTrivialCtor<U>::VALUE;

  static const bool     HAS_TRIVIAL_COPY =

    HasTrivialCopy<T>::VALUE && HasTrivialCopy<U>::VALUE;

  static const bool     HAS_TRIVIAL_DTOR   =

    HasTrivialDtor<T>::VALUE && HasTrivialDtor<U>::VALUE;


  static String  whatis   ()
  {
    return ( TypeTraits<T>::typeName () +
             String ( " and " )         +
             TypeTraits<U>::typeName () +
             String ( " pair" ) );
  }

  static String  typeName ()
  {
    return ( String ( "jem::util::Pair<" ) +
             TypeTraits<T>::typeName ()    +
             String ( ',' )                +
             TypeTraits<U>::typeName ()    +
             String ( '>' ) );
  }

  static lint    hashValue

    ( const util::Pair<T,U>& p )

  {
    return ( TypeTraits<T>::hashValue( p.first ) ^
             TypeTraits<U>::hashValue( p.second ) );
  }

};


JEM_END_PACKAGE_BASE

#endif
