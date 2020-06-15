
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
#include <jem/io/params.h>
#include <jem/io/ZipException.h>
#include <jem/io/ZipInputStream.h>
#include <jem/io/Inflator.h>


JEM_DEFINE_CLASS( jem::io::ZipInputStream );


JEM_BEGIN_PACKAGE( io )


//=======================================================================
//   class ZipInputStream
//=======================================================================

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


ZipInputStream::ZipInputStream

  ( Ref<InputStream>  in,
    idx_t             bufsize ) :

    input_ ( in )

{
  JEM_PRECHECK ( in != NIL );

  if ( bufsize < 0 )
  {
    bufsize = DEFAULT_BUFSIZE;
  }

  inflator_ = newInstance<Inflator> ( in, ZLIB_MODE, bufsize );
}


ZipInputStream::~ZipInputStream ()
{
  if ( inflator_ != NIL )
  {
    inflator_->finish ( NO_THROW );
  }
}


//-----------------------------------------------------------------------
//   poll
//-----------------------------------------------------------------------


idx_t ZipInputStream::poll ( const Time& timeout )
{
  if ( inflator_ != NIL )
  {
    return inflator_->poll ( timeout );
  }
  else
  {
    return 0;
  }
}


//-----------------------------------------------------------------------
//   close
//-----------------------------------------------------------------------


void ZipInputStream::close ()
{
  if ( inflator_ != NIL )
  {
    inflator_->finish ();
    input_   ->close  ();

    inflator_ = NIL;
  }
}


//-----------------------------------------------------------------------
//   read
//-----------------------------------------------------------------------


idx_t ZipInputStream::read ( void* buf, idx_t n )
{
  if ( inflator_ == NIL )
  {
    return 0;
  }
  else
  {
    return inflator_->inflate ( buf, n );
  }
}


JEM_END_PACKAGE( io )
