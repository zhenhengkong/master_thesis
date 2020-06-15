
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
#include <jem/base/CString.h>
#include <jem/base/Throwable.h>
#include <jem/base/LogBuffer.h>


JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class Throwable
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


Throwable::Throwable

  ( const String&  ctx,
    const String&  msg ) :

    where_ ( ctx ),
    what_  ( msg )

{
  LogBuffer::pushBack ( ctx, msg );
}


Throwable::Throwable ( const Throwable& rhs ) :

  where_ ( rhs.where_ ),
  what_  ( rhs.what_  )

{}


Throwable::~Throwable()
{}


//-----------------------------------------------------------------------
//   where
//-----------------------------------------------------------------------


String Throwable::where () const
{
  return where_;
}


//-----------------------------------------------------------------------
//   what
//-----------------------------------------------------------------------


String Throwable::what () const
{
  return what_;
}


//-----------------------------------------------------------------------
//   name
//-----------------------------------------------------------------------


String Throwable::name () const
{
  return "jem::Throwable";
}


//-----------------------------------------------------------------------
//   getStackTrace
//-----------------------------------------------------------------------


String Throwable::getStackTrace () const
{
  return String ();
}


//-----------------------------------------------------------------------
//   setContext
//-----------------------------------------------------------------------


void Throwable::setContext ( const String& ctx )
{
  where_ = ctx;
}


//-----------------------------------------------------------------------
//   setMessage
//-----------------------------------------------------------------------


void Throwable::setMessage ( const String& msg )
{
  what_ = msg;
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   abort
//-----------------------------------------------------------------------


void abort ( const Throwable& ex )
{
  String  trace = ex.getStackTrace ();

  if ( trace.size() )
  {
    jem::abort ( (const char*) makeCString ( ex.where() ),
                 (const char*) makeCString ( ex.what()  ),
                 (const char*) makeCString ( trace      ) );
  }
  else
  {
    jem::abort ( (const char*) makeCString ( ex.where() ),
                 (const char*) makeCString ( ex.what()  ) );
  }
}


JEM_END_PACKAGE_BASE