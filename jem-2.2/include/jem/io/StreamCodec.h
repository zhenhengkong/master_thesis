
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

#ifndef JEM_IO_STREAMCODEC_H
#define JEM_IO_STREAMCODEC_H

#include <jem/base/utilities.h>
#include <jem/base/PrecheckException.h>
#include <jem/io/SerializationException.h>
#include <jem/io/InputStream.h>
#include <jem/io/OutputStream.h>
#include <jem/io/xdr.h>


JEM_BEGIN_PACKAGE( io )


//-----------------------------------------------------------------------
//   class StreamCodec
//-----------------------------------------------------------------------


class StreamCodec
{
 public:

  template <class T>
    static inline void    decode

    ( InputStream&          in,
      T&                    value,
      int                   sex );

  template <class T>
    static void           decode

    ( InputStream&          in,
      T*                    values,
      idx_t                 n,
      int                   sex );

  template <class T>
    static inline void    encode

    ( OutputStream&         out,
      const T&              value,
      int                   sex );

  template <class T>
    static void           encode

    ( OutputStream&         in,
      const T*              values,
      idx_t                 n,
      int                   sex );

  template <class T>
    static inline void    encodeNoThrow

    ( OutputStream&         out,
      const T&              value,
      int                   sex );


 private:

  static inline void      read_

    ( InputStream&          in,
      void*                 buf,
      idx_t                 n );


 private:

  static const int        BUFSIZE_ = 256;

};





//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   decode
//-----------------------------------------------------------------------


template <class T>

  inline void StreamCodec::decode

  ( InputStream&   in,
    T&             value,
    int            sex )

{
  byte  buf[ xdr::TypeTraits<T>::XDR_SIZE ];

  read_       ( in, buf, xdr::TypeTraits<T>::XDR_SIZE );
  xdr::decode ( value, buf, sex );
}


template <class T>

  void StreamCodec::decode

  ( InputStream&   in,
    T*             values,
    idx_t          n,
    int            sex )

{
  JEM_PRECHECK ( n >= 0 );

  const int  XDR_SIZE = xdr::TypeTraits<T>::XDR_SIZE;
  const int  N        = BUFSIZE_ / XDR_SIZE;

  byte       buf[BUFSIZE_];
  int        k;

  while ( n > 0 )
  {
    k = (int) min ( n, (idx_t) N );

    read_       ( in, buf, k * XDR_SIZE );
    xdr::decode ( values, k, buf, sex );

    values += k;
    n      -= k;
  }
}


//-----------------------------------------------------------------------
//   encode
//-----------------------------------------------------------------------


template <class T>

  inline void StreamCodec::encode

  ( OutputStream&  out,
    const T&       value,
    int            sex )

{
  byte  buf[ xdr::TypeTraits<T>::XDR_SIZE ];

  xdr::encode ( buf, value, sex );
  out.write   ( buf, xdr::TypeTraits<T>::XDR_SIZE );
}


template <class T>

  void StreamCodec::encode

  ( OutputStream&  out,
    const T*       values,
    idx_t          n,
    int            sex )

{
  const int  XDR_SIZE = xdr::TypeTraits<T>::XDR_SIZE;
  const int  N        = BUFSIZE_ / XDR_SIZE;

  byte       buf[BUFSIZE_];
  int        k;

  while ( n > 0 )
  {
    k = (int) min ( n, (idx_t) N );

    xdr::encode ( buf, values, k, sex );
    out.write   ( buf, k * XDR_SIZE );

    values += k;
    n      -= k;
  }
}


//-----------------------------------------------------------------------
//   encodeNoThrow
//-----------------------------------------------------------------------


template <class T>

  inline void StreamCodec::encodeNoThrow

  ( OutputStream&  out,
    const T&       value,
    int            sex )

{
  byte  buf[ xdr::TypeTraits<T>::XDR_SIZE ];

  xdr::encode      ( buf, value, sex );
  out.writeNoThrow ( buf, xdr::TypeTraits<T>::XDR_SIZE );
}


//-----------------------------------------------------------------------
//   read_
//-----------------------------------------------------------------------


inline void StreamCodec::read_

  ( InputStream&  in,
    void*         buf,
    idx_t         n )

{
  byte*  dest = (byte*) buf;

  while ( n > 0 )
  {
    idx_t  k = in.read ( dest, n );

    if ( k == 0 )
    {
      throw SerializationException (
        JEM_FUNC,
        "unexpected end of stream"
      );
    }

    dest += k;
    n    -= k;
  }
}


JEM_END_PACKAGE( io )

#endif
