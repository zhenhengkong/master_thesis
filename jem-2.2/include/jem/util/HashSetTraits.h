
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

#ifndef JEM_UTIL_HASHSETTRAITS_H
#define JEM_UTIL_HASHSETTRAITS_H

#include <jem/base/TypeTraits.h>
#include <jem/util/StdAllocator.h>


JEM_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class HashSetTraits
//-----------------------------------------------------------------------


template <class T, class A = StdAllocator>

  class HashSetTraits

{
 public:

  typedef T                 KeyType;
  typedef T                 ValueType;
  typedef const T&          Reference;
  typedef const T&          ConstReference;
  typedef const T*          Pointer;
  typedef const T*          ConstPointer;
  typedef A                 Allocator;


  static const KeyType&     getKey

    ( const ValueType&        item )

  {
    return item;
  }

  static bool               equalKeys

    ( const KeyType&          lhs,
      const KeyType&          rhs )

  {
    return (lhs == rhs);
  }

  static lint               hashValue

    ( const KeyType&          key )

  {
    return TypeTraits<KeyType>::hashValue ( key );
  }

};


JEM_END_PACKAGE( util )

#endif
