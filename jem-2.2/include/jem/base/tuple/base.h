
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

#ifndef JEM_BASE_TUPLE_BASE_H
#define JEM_BASE_TUPLE_BASE_H

#include <jem/base/utilities.h>


JEM_BEGIN_PACKAGE_BASE


//-----------------------------------------------------------------------
//   class Tuple & forward declarations
//-----------------------------------------------------------------------


template <class T, int M, int N = 1, class E = Nil>

  class Tuple {};

template <class T, int M, int N>

  class Tuple<T,M,N,Nil>;

template <class T>

  class Tuple<T,0,0,Nil>;


//-----------------------------------------------------------------------
//   function sizeOf
//-----------------------------------------------------------------------


template <class T, int M, int N, class E>

  inline IntConstant<(M * N)>   sizeOf

  ( const Tuple<T,M,N,E>&         t )

{
  return IntConstant<(M * N)> ();
}


//-----------------------------------------------------------------------
//   class TupleRank
//-----------------------------------------------------------------------


template <int M, int N>

  class TupleRank

{
 public:

  static const int              RANK = 2;

};


template <int M>

  class TupleRank<M,1>

{
 public:

  static const int              RANK = 1;

};


//-----------------------------------------------------------------------
//   class TupleTraits & specializations
//-----------------------------------------------------------------------


template <class T>

  class TupleTraits

{
 public:

  typedef T                     Reference;

};


template <class T, int M, int N>

  class TupleTraits< Tuple<T,M,N,Nil> >

{
 public:

  typedef const Tuple<T,M,N>&   Reference;

};


template <class T>

  class TupleTraits< Tuple<T,0,0,Nil> >

{
 public:

  typedef Tuple<T,0,0,Nil>      Reference;

};


JEM_END_PACKAGE_BASE

#endif
