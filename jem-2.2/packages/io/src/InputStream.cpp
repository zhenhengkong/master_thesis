
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
#include <jem/base/ClassTemplate.h>
#include <jem/base/IllegalOperationException.h>
#include <jem/io/InputStream.h>


JEM_DEFINE_CLASS( jem::io::InputStream );


JEM_BEGIN_PACKAGE( io )


//=======================================================================
//   class InputStream
//=======================================================================

//-----------------------------------------------------------------------
//   destructor
//-----------------------------------------------------------------------


InputStream::~InputStream ()
{}


//-----------------------------------------------------------------------
//   dup
//-----------------------------------------------------------------------


Ref<InputStream> InputStream::dup ()
{
  throw IllegalOperationException (
    JEM_FUNC,
    "input stream can not be duplicated"
  );

  return NIL;
}


//-----------------------------------------------------------------------
//   close
//-----------------------------------------------------------------------


void InputStream::close ()
{}


//-----------------------------------------------------------------------
//   poll
//-----------------------------------------------------------------------


idx_t InputStream::poll ( const Time& timeout )
{
  return JEM_IDX_C(0);
}


//-----------------------------------------------------------------------
//   skip
//-----------------------------------------------------------------------


idx_t InputStream::skip ( idx_t n )
{
  byte   buf[256];
  idx_t  i;

  i = 0;

  while ( i < n )
  {
    idx_t  k = read ( buf, min( (idx_t) sizeof(buf), n - i ) );

    if ( k == 0 )
    {
      break;
    }

    i += k;
  }

  return i;
}


JEM_END_PACKAGE( io )
