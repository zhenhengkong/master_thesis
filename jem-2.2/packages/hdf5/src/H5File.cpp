
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
#include <jem/base/LogBuffer.h>
#include <jem/base/ClassTemplate.h>
#include <jem/hdf5/Library.h>
#include <jem/hdf5/H5Exception.h>
#include <jem/hdf5/Group.h>
#include <jem/hdf5/H5File.h>

extern "C"
{
  #include <hdf5.h>
}


JEM_DEFINE_CLASS( jem::hdf5::H5File );


JEM_BEGIN_PACKAGE( hdf5 )


//=======================================================================
//   class H5File
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const int  H5File::TRUNCATE   = 1 << 0;
const int  H5File::EXCLUSIVE  = 1 << 1;
const int  H5File::READ_WRITE = 1 << 2;
const int  H5File::READ_ONLY  = 1 << 3;


//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


H5File::H5File

  ( const Handle&  fid,
    const String&  name ) :

    fid_  ( fid ),
    name_ ( name )

{
  JEM_PRECHECK ( fid.isValid() );
}


H5File::~H5File ()
{
  if ( fid_.isValid() )
  {
    H5Fflush ( (hid_t) fid_.get(), H5F_SCOPE_LOCAL );

    LogBuffer::pushBack (
      JEM_FUNC,
      String::format (
        "flushed the HDF5 file `%s\'",
        name_
      )
    );

  }
}


//-----------------------------------------------------------------------
//   open
//-----------------------------------------------------------------------


Ref<H5File> H5File::open

  ( const String&  name,
    int            flags )

{
  int    oflags = 0;
  hid_t  fid;


  Library::init ();

  if ( flags & READ_WRITE )
  {
    oflags |= H5F_ACC_RDWR;
  }

  if ( flags & READ_ONLY )
  {
    oflags |= H5F_ACC_RDONLY;
  }

  fid = H5Fopen ( makeCString( name ), (unsigned int) oflags,
                  H5P_DEFAULT );

  if ( fid < 0 )
  {
    throw H5Exception (
      JEM_FUNC,
      String::format (
        "failed to open the HDF5 file `%s\'",
        name
      ),
      Library::strerror ()
    );
  }

  LogBuffer::pushBack (
    JEM_FUNC,
    String::format (
      "opened the HDF5 file `%s\'",
      name
    )
  );

  return newInstance<H5File> ( Handle( fid, Handle::ALLOCATED ),
                               name );
}


//-----------------------------------------------------------------------
//   create
//-----------------------------------------------------------------------


Ref<H5File> H5File::create

  ( const String&  name,
    int            flags )

{
  int    cflags = 0;
  hid_t  fid;


  Library::init ();

  if ( flags & EXCLUSIVE )
  {
    cflags |= H5F_ACC_EXCL;
  }

  if ( flags & TRUNCATE )
  {
    cflags |= H5F_ACC_TRUNC;
  }

  fid = H5Fcreate ( makeCString( name ), (unsigned int) cflags,
                    H5P_DEFAULT, H5P_DEFAULT );

  if ( fid < 0 )
  {
    throw H5Exception (
      JEM_FUNC,
      String::format (
        "failed to create the HDF5 file `%s\'",
        name
      ),
      Library::strerror ()
    );
  }

  LogBuffer::pushBack (
    JEM_FUNC,
    String::format (
      "created the HDF5 file `%s\'",
      name
    )
  );

  return newInstance<H5File> ( Handle( fid, Handle::ALLOCATED ),
                               name );
}


//-----------------------------------------------------------------------
//   flush
//-----------------------------------------------------------------------


void H5File::flush () const
{
  if ( ! fid_.isValid() )
  {
    throw H5Exception (
      JEM_FUNC,
      "invalid operation on a closed HDF5 file (flush)"
    );
  }

  herr_t  err = H5Fflush ( (hid_t) fid_.get(), H5F_SCOPE_LOCAL );

  if ( err )
  {
    throw H5Exception (
      JEM_FUNC,
      String::format (
        "failed to flush the HDF5 file `%s\'",
        name_
      ),
      Library::strerror ()
    );
  }
}


//-----------------------------------------------------------------------
//   close
//-----------------------------------------------------------------------


void H5File::close ()
{
  if ( fid_.isValid() )
  {
    herr_t  err = H5Fclose ( (hid_t) fid_.get() );

    if ( err )
    {
      throw H5Exception (
        JEM_FUNC,
        String::format (
          "failed to close the HDF5 file `%s\'",
          name_
        ),
        Library::strerror ()
      );
    }

    LogBuffer::pushBack (
      JEM_FUNC,
      String::format (
        "closed the HDF5 file `%s\'",
        name_
      )
    );

    fid_.release ();
    name_ = "";
  }
}


//-----------------------------------------------------------------------
//   getName
//-----------------------------------------------------------------------


String H5File::getName () const
{
  return name_;
}


//-----------------------------------------------------------------------
//   getRoot
//-----------------------------------------------------------------------


Ref<Group> H5File::getRoot () const
{
  if ( ! fid_.isValid() )
  {
    throw H5Exception (
      JEM_FUNC,
      "invalid operation on a closed HDF5 file (getRoot)"
    );
  }

  hid_t  gid = H5Gopen ( (hid_t) fid_.get(), "/", H5P_DEFAULT );

  if ( gid < 0 )
  {
    throw H5Exception (
      JEM_FUNC,
      String::format (
        "failed to open the root group in the HDF5 file `%s\'",
        name_
      ),
      Library::strerror ()
    );
  }

  return newInstance<Group> ( Handle( gid, Handle::ALLOCATED ) );
}


JEM_END_PACKAGE( hdf5 )

