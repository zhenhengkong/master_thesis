
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
#include <jem/io/DataOutputStream.h>


JEM_DEFINE_CLASS( jem::io::DataOutputStream );


JEM_BEGIN_PACKAGE( io )


//=======================================================================
//   class DataOutputStream
//=======================================================================

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


DataOutputStream::DataOutputStream

  ( Ref<OutputStream>  out ) :

    Super ( out ),
    SEX_  ( xdr::getByteOrder() )

{}


DataOutputStream::~DataOutputStream ()
{}


//-----------------------------------------------------------------------
//   encode ( bool )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( bool b )
{
  StreamCodec::encode ( *output_, b, SEX_ );
}


//-----------------------------------------------------------------------
//   encode ( byte )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( byte b )
{
  output_->write ( &b, 1 );
}


//-----------------------------------------------------------------------
//   encode ( char )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( char c )
{
  StreamCodec::encode ( *output_, c, SEX_ );
}


//-----------------------------------------------------------------------
//   encode ( short s )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( short s )
{
  StreamCodec::encode ( *output_, s, SEX_ );
}


//-----------------------------------------------------------------------
//   encode ( int )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( int i )
{
  StreamCodec::encode ( *output_, i, SEX_ );
}


//-----------------------------------------------------------------------
//   encode ( lint )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( lint l )
{
  StreamCodec::encode ( *output_, l, SEX_ );
}


//-----------------------------------------------------------------------
//   encode ( float )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( float f )
{
  StreamCodec::encode ( *output_, f, SEX_ );
}


//-----------------------------------------------------------------------
//   encode ( double )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( double d )
{
  StreamCodec::encode ( *output_, d, SEX_ );
}


//-----------------------------------------------------------------------
//   encode ( const bool* )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( const bool* buf, idx_t n )
{
  StreamCodec::encode ( *output_, buf, n, SEX_ );
}


//-----------------------------------------------------------------------
//   encode ( const byte* )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( const byte* buf, idx_t n )
{
  output_->write( buf, n );
}


//-----------------------------------------------------------------------
//   encode ( const char* )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( const char* buf, idx_t n )
{
  StreamCodec::encode ( *output_, buf, n, SEX_ );
}


//-----------------------------------------------------------------------
//   encode ( const short* )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( const short* buf, idx_t n )
{
  StreamCodec::encode ( *output_, buf, n, SEX_ );
}


//-----------------------------------------------------------------------
//   encode ( const int* )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( const int* buf, idx_t n )
{
  StreamCodec::encode ( *output_, buf, n, SEX_ );
}


//-----------------------------------------------------------------------
//   encode ( const lint* )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( const lint* buf, idx_t n )
{
  StreamCodec::encode ( *output_, buf, n, SEX_ );
}


//-----------------------------------------------------------------------
//   encode ( const float* )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( const float* buf, idx_t n )
{
  StreamCodec::encode ( *output_, buf, n, SEX_ );
}


//-----------------------------------------------------------------------
//   encode ( const double* )
//-----------------------------------------------------------------------


void DataOutputStream::encode ( const double* buf, idx_t n )
{
  StreamCodec::encode ( *output_, buf, n, SEX_ );
}


JEM_END_PACKAGE( io )
