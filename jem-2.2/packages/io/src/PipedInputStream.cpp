
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
#include <jem/io/Pipe.h>
#include <jem/io/IOException.h>
#include <jem/io/PipedInputStream.h>


JEM_DEFINE_CLASS( jem::io::PipedInputStream );


JEM_BEGIN_PACKAGE( io )


//=======================================================================
//   class PipedInputStream
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


PipedInputStream::PipedInputStream ( const Ref<Pipe>& pipe ) :

  pipe_ ( pipe )

{
  if ( pipe_ != NIL )
  {
    pipe_->addReader ();
  }
}


PipedInputStream::PipedInputStream ( const Self& rhs ) :

  pipe_ ( rhs.pipe_ )

{
  if ( pipe_ != NIL )
  {
    pipe_->addReader ();
  }
}


PipedInputStream::~PipedInputStream ()
{
  if ( pipe_ != NIL )
  {
    pipe_->deleteReader ();
  }
}


//-----------------------------------------------------------------------
//   dup
//-----------------------------------------------------------------------


Ref<InputStream> PipedInputStream::dup ()
{
  return newInstance<Self> ( *this );
}


//-----------------------------------------------------------------------
//   close
//-----------------------------------------------------------------------


void PipedInputStream::close ()
{
  if ( pipe_ != NIL )
  {
    pipe_->deleteReader ();
    pipe_ = NIL;
  }
}


//-----------------------------------------------------------------------
//   poll
//-----------------------------------------------------------------------


idx_t PipedInputStream::poll ( const Time& timeout )
{
  if ( pipe_ != NIL )
  {
    return pipe_->poll ( timeout );
  }
  else
  {
    return 0;
  }
}


//-----------------------------------------------------------------------
//   read
//-----------------------------------------------------------------------


idx_t PipedInputStream::read ( void* buf, idx_t n )
{
  if ( pipe_ != NIL )
  {
    return pipe_->read ( buf, n );
  }
  else
  {
    throw IOException ( JEM_FUNC, "closed pipe" );

    return 0;
  }
}


JEM_END_PACKAGE( io )