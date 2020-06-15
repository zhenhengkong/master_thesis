
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


#include <jem/base/utilities.h>
#include <jem/base/Once.h>
#include <jem/base/EnvParams.h>
#include <jem/base/LogBuffer.h>
#include <jem/base/StringBuffer.h>
#include <jem/base/RecursiveMutex.h>
#include <jem/io/TextOutput.h>


JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class LogBuffer::Data_
//=======================================================================


class LogBuffer::Data_
{
 public:

                          Data_        ();
                         ~Data_        ();

  static inline Data_&    getInstance  ();


 public:

  idx_t                   last;
  idx_t                   buflen;
  String*                 buffer;
  RecursiveMutex          mutex;
  volatile int            inUse;


 private:

  static void             init_        ();


 private:

  static Data_*           instance_;

};


//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


LogBuffer::Data_*  LogBuffer::Data_::instance_ = NULL;


//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


LogBuffer::Data_::Data_ ()
{
  idx_t  n = 128;
  lint   k;

  if ( igetenv( k, EnvParams::LOG_BUFFER_SIZE ) )
  {
    if      ( k < 0 )
    {
      k = 0;
    }
    else if ( k > MAX_SIZE )
    {
      k = MAX_SIZE;
    }

    n = (idx_t) k;
  }

  last   = 0;
  buflen = 0;
  buffer = NULL;
  inUse  = 0;

  if ( n )
  {
    buffer = new String[2 * n];
    buflen = 2 * n;
  }
}


LogBuffer::Data_::~Data_ ()
{
  if ( buffer )
  {
    delete [] buffer;

    buffer = NULL;
  }
}


//-----------------------------------------------------------------------
//   getInstance
//-----------------------------------------------------------------------


inline LogBuffer::Data_& LogBuffer::Data_::getInstance ()
{
  static Once  once = JEM_ONCE_INITIALIZER;

  runOnce ( once, init_ );

  return *instance_;
}


//-----------------------------------------------------------------------
//   init_
//-----------------------------------------------------------------------


void LogBuffer::Data_::init_ ()
{
  instance_ = new Data_ ();
}


//=======================================================================
//   class LogBuffer::Lock_
//=======================================================================


class LogBuffer::Lock_
{
 public:

  explicit inline         Lock_

    ( Data_&                data );

  inline                 ~Lock_  ();


 private:

  Data_&                  data_;

};


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


inline LogBuffer::Lock_::Lock_ ( Data_& data ) :

  data_ ( data )

{
  data_.mutex.lock ();

  data_.inUse++;
}


inline LogBuffer::Lock_::~Lock_ ()
{
  data_.inUse--;

  data_.mutex.unlock ();
}


//=======================================================================
//   class LogBuffer
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const idx_t  LogBuffer::MAX_SIZE = 128 * 1024;


//-----------------------------------------------------------------------
//   clear
//-----------------------------------------------------------------------


void LogBuffer::clear ()
{
  Data_&  data = Data_::getInstance ();
  Lock_   lock ( data );

  if ( data.inUse <= 1 )
  {
    String  empty;

    data.last = 0;

    for ( idx_t i = 0; i < data.buflen; i++ )
    {
      data.buffer[i] = empty;
    }
  }
}


//-----------------------------------------------------------------------
//   resize
//-----------------------------------------------------------------------


idx_t LogBuffer::resize ( idx_t size )
{
  Data_&  data  = Data_::getInstance ();
  Lock_   lock  ( data );
  idx_t   size0 = data.buflen / 2;

  if ( data.inUse <= 1 )
  {
    data.last = 0;

    if ( data.buffer )
    {
      delete [] data.buffer;

      data.buffer = NULL;
      data.buflen = 0;
    }

    if ( size > 0 )
    {
      if ( size > MAX_SIZE )
      {
        size = MAX_SIZE;
      }

      data.buffer = new String[2 * size];
      data.buflen = 2 * size;
    }
  }

  return size0;
}


//-----------------------------------------------------------------------
//   pushBack
//-----------------------------------------------------------------------


void LogBuffer::pushBack

  ( const String&  where,
    const String&  what )

{
  if ( where.size() + what.size() )
  {
    Data_&  data = Data_::getInstance ();

    if ( data.buflen && ! data.inUse )
    {
      Lock_  lock ( data );
      idx_t  last = data.last;

      if ( last < data.buflen )
      {
        data.buffer[last + 0] = where;
        data.buffer[last + 1] = what;
      }

      last += 2;

      if ( last >= data.buflen )
      {
        last = 0;
      }

      data.last = last;
    }
  }
}


//-----------------------------------------------------------------------
//   getTrace
//-----------------------------------------------------------------------


String LogBuffer::getTrace ( idx_t count )
{
  Data_&  data = Data_::getInstance ();
  Lock_   lock ( data );
  String  str;

  if ( data.inUse <= 1 )
  {
    StringBuffer  buf;
    idx_t         n;

    n = data.buflen / 2;

    if      ( count < 0 )
    {
      count = n;
    }
    else if ( count > n )
    {
      count = n;
    }

    buf.reserve ( count * 16 );

    for ( idx_t i = 0; i < count; i++ )
    {
      String  where;
      String  what;
      idx_t   last;

      last = data.last;

      if ( last <= 0 )
      {
        last = data.buflen;
      }

      what .swap ( data.buffer[last - 1] );
      where.swap ( data.buffer[last - 2] );

      if ( (what.size() + where.size()) == 0 )
      {
        break;
      }

      data.last = last - 2;

      if ( i )
      {
        buf += '\n';
      }

      buf += String ( i );
      buf += ' ';

      if ( where.size() )
      {
        buf += where;
      }

      if ( what.size() )
      {
        if ( where.size() )
        {
          buf += ':';
          buf += ' ';
        }

        buf += what;
      }
    }

    str = buf.toString ();
  }

  return str;
}


//-----------------------------------------------------------------------
//   printTrace
//-----------------------------------------------------------------------


void LogBuffer::printTrace

  ( io::TextOutput&  out,
    idx_t            count )

{
  Data_&  data = Data_::getInstance ();
  Lock_   lock ( data );

  if ( data.inUse <= 1 )
  {
    idx_t  n = data.buflen / 2;

    if      ( count < 0 )
    {
      count = n;
    }
    else if ( count > n )
    {
      count = n;
    }

    for ( idx_t i = 0; i < count; i++ )
    {
      String  where;
      String  what;
      idx_t   last;

      last = data.last;

      if ( last <= 0 )
      {
        last = data.buflen;
      }

      what .swap ( data.buffer[last - 1] );
      where.swap ( data.buffer[last - 2] );

      if ( (what.size() + where.size()) == 0 )
      {
        break;
      }

      data.last = last - 2;

      print ( out, i, ' ' );

      if ( where.size() )
      {
        print ( out, where );
      }

      if ( what.size() )
      {
        if ( where.size() )
        {
          print ( out, ':', ' ' );
        }

        print ( out, what );
      }

      print ( out, '\n' );
    }
  }
}


JEM_END_PACKAGE_BASE
