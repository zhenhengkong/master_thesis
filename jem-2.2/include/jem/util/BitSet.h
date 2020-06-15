
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

#ifndef JEM_UTIL_BITSET_H
#define JEM_UTIL_BITSET_H

#include <jem/base/TypeTraits.h>


namespace jem
{
  namespace io
  {
    class DataInput;
    class DataOutput;
    class TextOutput;
  }
}


JEM_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class BitSet
//-----------------------------------------------------------------------


class BitSet
{
 public:

  typedef BitSet        Self;


                        BitSet        ();

  explicit              BitSet

    ( idx_t               cap );

                        BitSet

    ( const BitSet&       rhs );

                       ~BitSet        ();

  void                  readFrom

    ( io::DataInput&      in );

  void                  writeTo

    ( io::DataOutput&     out )          const;

  BitSet&               operator =

    ( const BitSet&       rhs );

  void                  swap

    ( BitSet&             rhs );

  void                  set

    ( idx_t                i );

  void                  set

    ( idx_t               i,
      bool                value );

  bool                  get

    ( idx_t               i )            const;

  void                  flip

    ( idx_t               i );

  void                  clear         ();

  void                  clear

    ( idx_t               i );

  void                  clear

    ( const BitSet&       mask );

  void                  andWith

    ( const BitSet&       rhs );

  void                  orWith

    ( const BitSet&       rhs );

  void                  xorWith

    ( const BitSet&       rhs );

  void                  reserve

    ( idx_t               n );

  void                  trimToSize    ();
  idx_t                 count         () const;
  inline idx_t          size          () const;


 private:

  void                  expand_

    ( idx_t               cap );

  void                  realloc_

    ( idx_t               cap );

  void                  updateSize_   ();

  void                  updateSize_

    ( idx_t               i,
      idx_t               j );

  static inline idx_t    byteCount_

    ( idx_t               nbits );

  static void           indexError_

    ( const char*         where,
      idx_t               index );


 private:

  static const int      BITS_PER_BYTE_;
  static const idx_t    SHIFT_BITS_;
  static const idx_t    BYTE_MASK_;

  idx_t                 size_;
  idx_t                 capacity_;
  byte*                 data_;

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


inline void             swap

  ( BitSet&               lhs,
    BitSet&               rhs );

template <class Input>

  inline void           decode

  ( Input&                in,
    BitSet&               bs );

template <class Output>

  inline void           encode

  ( Output&               out,
    const BitSet&         bs );

void                    print

  ( io::TextOutput&       out,
    const BitSet&         bs );





//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class BitSet
//=======================================================================

//-----------------------------------------------------------------------
//   size
//-----------------------------------------------------------------------


inline idx_t BitSet::size () const
{
  return size_;
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


inline void             swap

  ( BitSet&               lhs,
    BitSet&               rhs )

{
  lhs.swap ( rhs );
}


//-----------------------------------------------------------------------
//   decode
//-----------------------------------------------------------------------


template <class Input>

  inline void           decode

  ( Input&                in,
    BitSet&               bs )

{
  bs.readFrom ( in );
}


//-----------------------------------------------------------------------
//   encode
//-----------------------------------------------------------------------


template <class Output>

  inline void           encode

  ( Output&               out,
    const BitSet&         bs )

{
  bs.writeTo ( out );
}


JEM_END_PACKAGE( util )

JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class TypeTraits < util::BitSet >
//=======================================================================


template <>

  class TypeTraits< util::BitSet > : public BasicTypeTraits

{
 public:

  static const bool   IS_SERIALIZABLE = true;

  static String       whatis   ();
  static String       typeName ();

};


JEM_END_PACKAGE_BASE

#endif
