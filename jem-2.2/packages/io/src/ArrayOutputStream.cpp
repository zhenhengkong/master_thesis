
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


#include <jem/base/assert.h>
#include <jem/base/utilities.h>
#include <jem/io/ArrayOutputStream.h>


JEM_BEGIN_PACKAGE( io )


//=======================================================================
//   class ArrayOutputStream
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


ArrayOutputStream::ArrayOutputStream ()
{}


ArrayOutputStream::ArrayOutputStream

  ( const Array<byte>& buf ) :

    buffer_ ( buf, JEM_IDX_C(0) )

{}


ArrayOutputStream::ArrayOutputStream ( const Self& rhs ) :

  buffer_ ( rhs.buffer_ )

{}


ArrayOutputStream::~ArrayOutputStream ()
{}


//-----------------------------------------------------------------------
//   dup
//-----------------------------------------------------------------------


Ref<OutputStream> ArrayOutputStream::dup ()
{
  return newInstance<Self> ( *this );
}


//-----------------------------------------------------------------------
//   write
//-----------------------------------------------------------------------


void ArrayOutputStream::write ( const void* buf, idx_t n )
{
  JEM_PRECHECK( n >= 0 );

  const byte*  first = (byte*) buf;
  const byte*  last  = first + n;

  buffer_.pushBack ( first, last );
}


//-----------------------------------------------------------------------
//   writeNoThrow
//-----------------------------------------------------------------------


void ArrayOutputStream::writeNoThrow ( const void* buf, idx_t n )
{
  if ( n > JEM_IDX_C(0) )
  {
    const byte*  first = (byte*) buf;
    const byte*  last  = first + n;

    buffer_.pushBack ( first, last );
  }
}


//-----------------------------------------------------------------------
//   reset
//-----------------------------------------------------------------------


void ArrayOutputStream::reset ()
{
  buffer_.clear ();
}


//-----------------------------------------------------------------------
//   toArray
//-----------------------------------------------------------------------


Array<byte> ArrayOutputStream::toArray () const
{
  return buffer_.toArray ();
}


JEM_END_PACKAGE( io )
