
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


#include <jem/base/CString.h>
#include <jem/base/native/System.h>
#include <jem/io/IOException.h>
#include "native/PosixDirListing.h"

extern "C"
{
  #include <dirent.h>
}


JEM_BEGIN_PACKAGE( io )


//=======================================================================
//   class PosixDirListing
//=======================================================================

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


PosixDirListing::PosixDirListing ( const String& name ) :

  dir_  ( NULL ),
  name_ ( name )

{
  dir_ = ::opendir ( makeCString( name ) );

  if ( ! dir_ )
  {
    String  details = NativeSystem::strerror ();

    throw IOException (
      JEM_FUNC,
      String::format (
        "error opening directory `%s\' (%s)",
        name,
        details
      )
    );
  }
}


PosixDirListing::~PosixDirListing ()
{
  if ( dir_ )
  {
    ::closedir ( (DIR*) dir_ );

    dir_ = NULL;
  }
}


//-----------------------------------------------------------------------
//   rewind
//-----------------------------------------------------------------------


void PosixDirListing::rewind ()
{
  ::rewinddir ( (DIR*) dir_ );
}


//-----------------------------------------------------------------------
//   nextEntry
//-----------------------------------------------------------------------


String PosixDirListing::nextEntry ()
{
  struct dirent*  entry;

#if defined(JEM_OS_UNIX98)

  struct dirent   tmp;

  if ( ::readdir_r( (DIR*) dir_, &tmp, &entry ) )
  {
    String  details = NativeSystem::strerror ();

    throw IOException (
      JEM_FUNC,
      String::format (
        "error reading directory `%s\' (%s)",
        name_,
        details
      )
    );
  }

#else

  entry = ::readdir ( (DIR*) dir_ );

#endif

  if ( entry )
  {
    return jem::clone ( entry->d_name );
  }
  else
  {
    return String ();
  }
}


JEM_END_PACKAGE( io )
