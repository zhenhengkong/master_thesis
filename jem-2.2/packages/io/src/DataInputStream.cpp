
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


#include <jem/base/ClassTemplate.h>
#include <jem/io/StreamCodec.h>
#include <jem/io/SerializationException.h>
#include <jem/io/DataInputStream.h>


JEM_DEFINE_CLASS( jem::io::DataInputStream );


JEM_BEGIN_PACKAGE( io )


//=======================================================================
//   class DataInputStream
//=======================================================================

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


DataInputStream::DataInputStream ( Ref<InputStream> in ) :

  Super ( in ),
  SEX_  ( xdr::getByteOrder() )
{}


DataInputStream::~DataInputStream ()
{}


//-----------------------------------------------------------------------
//   read
//-----------------------------------------------------------------------


idx_t DataInputStream::read ( void* buf, idx_t n )
{
  return input_->read ( buf, n );
}


//-----------------------------------------------------------------------
//   decodeBool
//-----------------------------------------------------------------------


bool DataInputStream::decodeBool ()
{
  bool  value;

  StreamCodec::decode ( *input_, value, SEX_ );

  return value;
}


//-----------------------------------------------------------------------
//   decodeByte
//-----------------------------------------------------------------------


byte DataInputStream::decodeByte ()
{
  byte  b;

  if ( input_->read( &b, 1 ) != 1 )
  {
    throw SerializationException (
      JEM_FUNC,
      "unexpected end of input"
    );
  }

  return b;
}


//-----------------------------------------------------------------------
//   decodeChar
//-----------------------------------------------------------------------


char DataInputStream::decodeChar ()
{
  char  value;

  StreamCodec::decode ( *input_, value, SEX_ );

  return value;
}


//-----------------------------------------------------------------------
//   decodeShort
//-----------------------------------------------------------------------


short DataInputStream::decodeShort ()
{
  short  value;

  StreamCodec::decode ( *input_, value, SEX_ );

  return value;
}


//-----------------------------------------------------------------------
//   decodeInt
//-----------------------------------------------------------------------


int DataInputStream::decodeInt ()
{
  int  value;

  StreamCodec::decode ( *input_, value, SEX_ );

  return value;
}


//-----------------------------------------------------------------------
//   decodeLong
//-----------------------------------------------------------------------


lint DataInputStream::decodeLong ()
{
  lint  value;

  StreamCodec::decode ( *input_, value, SEX_ );

  return value;
}


//-----------------------------------------------------------------------
//   decodeFloat
//-----------------------------------------------------------------------


float DataInputStream::decodeFloat ()
{
  float  value;

  StreamCodec::decode ( *input_, value, SEX_ );

  return value;
}


//-----------------------------------------------------------------------
//   decodeDouble
//-----------------------------------------------------------------------


double DataInputStream::decodeDouble ()
{
  double  value;

  StreamCodec::decode ( *input_, value, SEX_ );

  return value;
}


//-----------------------------------------------------------------------
//   decode ( bool* )
//-----------------------------------------------------------------------


void DataInputStream::decode ( bool* buf, idx_t n )
{
  StreamCodec::decode ( *input_, buf, n, SEX_ );
}


//-----------------------------------------------------------------------
//   decode ( byte* )
//-----------------------------------------------------------------------


void DataInputStream::decode ( byte* buf, idx_t n )
{
  while ( n > 0 )
  {
    idx_t  k = input_->read( buf, n );

    if ( k == 0 )
    {
      throw SerializationException (
        JEM_FUNC,
        "unexpected end of input"
      );
    }

    buf += k;
    n   -= k;
  }
}


//-----------------------------------------------------------------------
//   decode ( char* )
//-----------------------------------------------------------------------


void DataInputStream::decode ( char* buf, idx_t n )
{
  StreamCodec::decode ( *input_, buf, n, SEX_ );
}


//-----------------------------------------------------------------------
//   decode ( short* )
//-----------------------------------------------------------------------


void DataInputStream::decode ( short* buf, idx_t n )
{
  StreamCodec::decode ( *input_, buf, n, SEX_ );
}


//-----------------------------------------------------------------------
//   decode ( int* )
//-----------------------------------------------------------------------


void DataInputStream::decode ( int* buf, idx_t n )
{
  StreamCodec::decode ( *input_, buf, n, SEX_ );
}


//-----------------------------------------------------------------------
//   decode ( lint* )
//-----------------------------------------------------------------------


void DataInputStream::decode ( lint* buf, idx_t n )
{
  StreamCodec::decode ( *input_, buf, n, SEX_ );
}


//-----------------------------------------------------------------------
//   decode ( float* )
//-----------------------------------------------------------------------


void DataInputStream::decode ( float* buf, idx_t n )
{
  StreamCodec::decode ( *input_, buf, n, SEX_ );
}


//-----------------------------------------------------------------------
//   decode ( double* )
//-----------------------------------------------------------------------


void DataInputStream::decode ( double* buf, idx_t n )
{
  StreamCodec::decode ( *input_, buf, n, SEX_ );
}


JEM_END_PACKAGE( io )
