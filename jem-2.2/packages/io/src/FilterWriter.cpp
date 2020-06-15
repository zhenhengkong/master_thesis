
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
#include <jem/io/FilterWriter.h>


JEM_DEFINE_CLASS( jem::io::FilterWriter );


JEM_BEGIN_PACKAGE( io )


//=======================================================================
//   class FilterWriter
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


FilterWriter::FilterWriter ()
{}


FilterWriter::FilterWriter ( const Ref<Writer>& out ) :

  output_ ( out )

{
  JEM_PRECHECK ( out != NIL );
}


FilterWriter::FilterWriter ( const Self& rhs ) :

  output_ ( rhs.output_->dup() )

{}


FilterWriter::~FilterWriter ()
{}


//-----------------------------------------------------------------------
//   close
//-----------------------------------------------------------------------


void FilterWriter::close ()
{
  output_->close ();
}


//-----------------------------------------------------------------------
//   flush
//-----------------------------------------------------------------------


void FilterWriter::flush ()
{
  output_->flush ();
}


//-----------------------------------------------------------------------
//   write
//-----------------------------------------------------------------------


void FilterWriter::write ( int c )
{
  output_->write ( c );
}


void FilterWriter::write ( int c, idx_t n )
{
  output_->write ( c, n );
}


void FilterWriter::write ( const char* buf, idx_t n )
{
  output_->write ( buf, n );
}


//-----------------------------------------------------------------------
//   writeNoThrow
//-----------------------------------------------------------------------


void FilterWriter::writeNoThrow ( const char* buf, idx_t n )
{
  output_->writeNoThrow ( buf, n );
}


JEM_END_PACKAGE( io )
