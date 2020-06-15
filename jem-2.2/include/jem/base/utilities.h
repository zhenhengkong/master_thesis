
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

#ifndef JEM_BASE_UTILITIES_H
#define JEM_BASE_UTILITIES_H

#include <jem/defines.h>


JEM_BEGIN_PACKAGE_BASE


//-----------------------------------------------------------------------
//   class Nil
//-----------------------------------------------------------------------


class                 Nil {};
extern const Nil      NIL;


//-----------------------------------------------------------------------
//   struct POD
//-----------------------------------------------------------------------


struct POD {};


//-----------------------------------------------------------------------
//   class IntConstant
//-----------------------------------------------------------------------


template <int N>

  class IntConstant

{
 public:

  typedef int         Type;

  static const int    VALUE = N;

};


//-----------------------------------------------------------------------
//   class BoolConstant
//-----------------------------------------------------------------------


template <bool B>

  class BoolConstant

{
 public:

  typedef bool         Type;

  static const bool    VALUE = B;

};


typedef BoolConstant<true>   True;
typedef BoolConstant<false>  False;


//-----------------------------------------------------------------------
//   expression classes
//-----------------------------------------------------------------------


template <class Op, class A>            class UnaryExpr    {};
template <class Op, class A, class B>   class BinaryExpr   {};
template <class A,  class B, class C>   class WhereExpr    {};


//-----------------------------------------------------------------------
//   class CompileTimeCheck
//-----------------------------------------------------------------------


template <bool Value>

  class CompileTimeCheck

{};


template <>

  class CompileTimeCheck<true>

{
 public:

  static inline void check ();

};


//-----------------------------------------------------------------------
//   class Lock
//-----------------------------------------------------------------------


template <class M>

  class Lock

{
 public:

  explicit inline     Lock ( const M& mutex );
  inline             ~Lock ();


 private:

  M&                  mutex_;

};


//-----------------------------------------------------------------------
//   enumerations/constants
//-----------------------------------------------------------------------


enum                  ThrowMode
{
                        NO_THROW,
                        CAN_THROW
};


//-----------------------------------------------------------------------
//   utility functions
//-----------------------------------------------------------------------


void                  abort

  ( const char*         where,
    const char*         what,
    const char*         trace = 0 );

template <class T>

  inline const T&     min

  ( const T&            a,
    const T&            b );

template <class T>

  inline const T&     min

  ( const T&            a,
    const T&            b,
    const T&            c );

template <class T>

  inline const T&     min

  ( const T&            a,
    const T&            b,
    const T&            c,
    const T&            d );

template <class T>

  inline const T&     max

  ( const T&            a,
    const T&            b );

template <class T>

  inline const T&     max

  ( const T&            a,
    const T&            b,
    const T&            c );

template <class T>

  inline const T&     max

  ( const T&            a,
    const T&            b,
    const T&            c,
    const T&            d );

template <class T>

  inline void         swap

  ( T&                  lhs,
    T&                  rhs );

template <class Iterator>

  inline idx_t        distance

  ( Iterator            first,
    Iterator            last );

template <class T>

  inline idx_t        distance

  ( T*                  first,
    T*                  last );

template <class T>

  inline idx_t        distance

  ( const T*            first,
    const T*            last );




//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   class BoolConstant
//-----------------------------------------------------------------------


template <bool B1, bool B2>

  inline BoolConstant<(B1 || B2)>  operator ||

  ( const BoolConstant<B1>&  lhs,
    const BoolConstant<B2>&  rhs )

{
  return BoolConstant<(B1 || B2)> ();
}


template <bool B1, bool B2>

  inline BoolConstant<(B1 && B2)>  operator &&

  ( const BoolConstant<B1>&  lhs,
    const BoolConstant<B2>&  rhs )

{
  return BoolConstant<(B1 && B2)> ();
}


//-----------------------------------------------------------------------
//   class CompileTimeCheck<true>
//-----------------------------------------------------------------------


inline void CompileTimeCheck<true>::check ()
{}


//-----------------------------------------------------------------------
//   class Lock
//-----------------------------------------------------------------------


template <class M>

  inline Lock<M>::Lock ( const M& mutex ) :

    mutex_ ( const_cast<M&>( mutex ) )

{
  mutex_.lock();
}


template <class M>

  inline Lock<M>::~Lock ()

{
  mutex_.unlock();
}


//-----------------------------------------------------------------------
//   min
//-----------------------------------------------------------------------


template <class T>

  inline const T& min ( const T& a, const T& b )

{
  if ( a < b )
  {
    return a;
  }
  else
  {
    return b;
  }
}


template <class T>

  inline const T& min ( const T& a, const T& b,
                        const T& c )

{
  return jem::min ( a, jem::min( b, c ) );
}


template <class T>

  inline const T& min ( const T& a, const T& b,
                        const T& c, const T& d )

{
  return jem::min ( jem::min( a, b ), jem::min( c, d ) );
}


//-----------------------------------------------------------------------
//   max
//-----------------------------------------------------------------------


template <class T>

  inline const T& max ( const T& a, const T& b )

{
  if ( a < b )
  {
    return b;
  }
  else
  {
    return a;
  }
}


template <class T>

  inline const T& max ( const T& a, const T& b,
                        const T& c )

{
  return jem::max ( a, jem::max( b, c ) );
}


template <class T>

  inline const T& max ( const T& a, const T& b,
                        const T& c, const T& d )

{
  return jem::max ( jem::max( a, b ), jem::max( c, d ) );
}


//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


template <class T>

  inline void swap ( T& lhs, T& rhs )

{
  T tmp ( lhs );

  lhs = rhs;
  rhs = tmp;
}


//-----------------------------------------------------------------------
//   distance
//-----------------------------------------------------------------------


template <class Iterator>

  inline idx_t  distance

  ( Iterator      first,
    Iterator      last )

{
  idx_t  n = 0;

  for ( ; first != last; ++first )
  {
    n++;
  }

  return n;
}


template <class T>

  inline idx_t  distance

  ( T*            first,
    T*            last )

{
  return (idx_t) (last - first);
}


template <class T>

  inline idx_t  distance

  ( const T*      first,
    const T*      last )

{
  return (idx_t) (last - first);
}


JEM_END_PACKAGE_BASE

#endif
