
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

#ifndef JEM_UTIL_FLEX_H
#define JEM_UTIL_FLEX_H

#include <jem/base/utilities.h>
#include <jem/base/TypeTraits.h>


JEM_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class Flex
//-----------------------------------------------------------------------


template <class T>

  class Flex

{
 public:

  typedef T               ValueType;
  typedef T*              Iterator;
  typedef const T*        ConstIterator;

  static const float      EXPANSION_FACTOR;


                          Flex                ();

  explicit                Flex

    ( idx_t                 n );

  inline                  Flex

    ( const Flex&           rhs );

  template <class InputIterator>

    inline                Flex

    ( InputIterator         first,
      InputIterator         last );

                         ~Flex                ();

  inline Iterator         begin               ();
  inline ConstIterator    begin               () const;
  inline Iterator         end                 ();
  inline ConstIterator    end                 () const;

  inline T&               front               ();
  inline const T&         front               () const;
  inline T&               back                ();
  inline const T&         back                () const;

  Flex&                   operator =

    ( const Flex&           rhs );

  Flex&                   operator =

    ( T                     rhs );

  void                    swap

    ( Flex&                 rhs );

  inline T&               operator            []

    ( idx_t                 i );

  inline const T&         operator            []

    ( idx_t                 i )                  const;

  T*                      extend

    ( idx_t                 n );

  inline void             pushBack

    ( T                     item );

  void                    pushBack

    ( T                     item,
      idx_t                 n );

  template <class InputIterator>

    void                  pushBack

    ( InputIterator         first,
      InputIterator         last );

  inline void             popBack             ();

  inline void             popBack

    ( idx_t                 n );

  void                    insert

    ( Iterator              pos,
      T                     item );

  void                    insert

    ( Iterator              pos,
      T                     item,
      idx_t                 n );

  template <class InputIterator>

    void                  insert

    ( Iterator              pos,
      InputIterator         first,
      InputIterator         last );

  inline void             erase

    ( Iterator              pos );

  inline void             erase

    ( Iterator              first,
      Iterator              last );

  inline void             clear               ();

  void                    resize

    ( idx_t                 n );

  inline void             reserve

    ( idx_t                 cap );

  inline void             setExpansionFactor

    ( float                 x );

  inline float            getExpansionFactor  () const;
  inline void             trimToSize          ();
  inline idx_t            capacity            () const;
  inline idx_t            size                () const;

  inline T*               addr

    ( idx_t                 i = 0 );

  inline const T*         addr

    ( idx_t                 i = 0 )              const;


 private:

  template <class InputIterator>

    void                  init_

    ( InputIterator         first,
      InputIterator         last );

  void                    shiftRight_

    ( idx_t                 pos,
      idx_t                 n );

  void                    shiftLeft_

    ( idx_t                 pos,
      idx_t                 n );

  void                    grow_

    ( idx_t                 cap );

  inline void             setCapacity_

    ( idx_t                 cap );

  inline void             setCapacity_

    ( idx_t                 cap,
      const True&           realloc );

  void                    setCapacity_

    ( idx_t                 cap,
      const False&          realloc );


 private:

  T*                      data_;
  idx_t                   size_;
  idx_t                   capacity_;
  float                   expand_;

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


template <class Input, class T>

  void                    decode

  ( Input&                  in,
    Flex<T>&                flx );

template <class Output, class T>

  void                    encode

  ( Output&                 out,
    const Flex<T>&          flx );

template <class Output, class T>

  void                    print

  ( Output&                 out,
    const Flex<T>&          flx );

template <class T>

  inline void             swap

  ( Flex<T>&                lhs,
    Flex<T>&                rhs );




//#######################################################################
//   Implementation
//#######################################################################

JEM_END_PACKAGE( util )

#include <jem/util/Flex.tcc>

#endif
