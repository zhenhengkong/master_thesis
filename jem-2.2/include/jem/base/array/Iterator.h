
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

#ifndef JEM_BASE_ARRAY_ITERATOR_H
#define JEM_BASE_ARRAY_ITERATOR_H

#include <iterator>
#include <jem/base/array/base.h>


JEM_BEGIN_PACKAGE_BASE


//-----------------------------------------------------------------------
//   class ArrayIterator
//-----------------------------------------------------------------------


template <class T, int N>

  class ArrayIterator

{
 public:

  typedef std::bidirectional_iterator_tag
                          iterator_category;

  typedef idx_t           difference_type;
  typedef T               value_type;
  typedef T&              reference;
  typedef T*              pointer;

  typedef ArrayIterator   Self;
  typedef Tuple<idx_t,N>  Index;
  typedef Tuple<idx_t,N>  Shape;

  static const int        LAST_DIM = N - 1;

  enum EndTag           { END_TAG };


  inline                  ArrayIterator ();

                          ArrayIterator

    ( T*                    data,
      const Shape&          shape,
      const Shape&          stride );

                          ArrayIterator

    ( T*                    data,
      const Shape&          shape,
      const Shape&          stride,
      EndTag                tag );

  inline                  ArrayIterator

    ( const Self&           rhs );

  inline Self&            operator ++   ();
  inline Self             operator ++   ( int );
  inline Self&            operator --   ();
  inline Self             operator --   ( int );

  inline reference        operator *    () const;
  inline pointer          operator ->   () const;

  inline bool             operator ==   ( const Self& rhs ) const;
  inline bool             operator !=   ( const Self& rhs ) const;

 private:

  void                    incrIndex_    ();
  void                    decrIndex_    ();


 private:

  T*                      data_;
  idx_t                   offset_;
  Index                   index_;
  Shape                   shape_;
  Shape                   stride_;

};


//-----------------------------------------------------------------------
//   class ArrayIterator<T,1>
//-----------------------------------------------------------------------


template <class T>

  class ArrayIterator<T,1>

{
 public:

  typedef std::random_access_iterator_tag
                          iterator_category;

  typedef idx_t           difference_type;
  typedef T               value_type;
  typedef T&              reference;
  typedef T*              pointer;

  typedef ArrayIterator   Self;
  typedef Tuple<idx_t,1>  Shape;


  inline                  ArrayIterator ();

  inline                  ArrayIterator

    ( T*                    data,
      idx_t                 offset,
      idx_t                 stride );

  inline                  ArrayIterator

    ( const Self&           rhs );

  inline Self&            operator ++   ();
  inline Self             operator ++   ( int );
  inline Self&            operator --   ();
  inline Self             operator --   ( int );

  inline Self&            operator +=   ( idx_t n );
  inline Self&            operator -=   ( idx_t n );
  inline Self             operator +    ( idx_t n )          const;
  inline Self             operator -    ( idx_t n )          const;
  inline idx_t            operator -    ( const Self& rhs ) const;

  inline reference        operator []   ( idx_t i )          const;
  inline reference        operator *    ()                  const;
  inline pointer          operator ->   ()                  const;

  inline bool             operator ==   ( const Self& rhs ) const;
  inline bool             operator !=   ( const Self& rhs ) const;
  inline bool             operator <    ( const Self& rhs ) const;
  inline bool             operator >    ( const Self& rhs ) const;
  inline bool             operator <=   ( const Self& rhs ) const;
  inline bool             operator >=   ( const Self& rhs ) const;


 private:

  T*                      data_;
  idx_t                   offset_;
  idx_t                   stride_;

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


template <class T>

  inline idx_t                distance

  ( const ArrayIterator<T,1>&   first,
    const ArrayIterator<T,1>&   last );





//#######################################################################
//   Implementation
//#######################################################################

JEM_END_PACKAGE_BASE

#include <jem/base/array/Iterator.tcc>

#endif
