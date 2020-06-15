
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


#include <jem/hdf5/Handle.h>
#include <jem/hdf5/Library.h>
#include <jem/hdf5/H5Exception.h>

extern "C"
{
  #include <hdf5.h>
}


JEM_BEGIN_PACKAGE( hdf5 )


//=======================================================================
//   class Handle
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const int  Handle::VALID     = 1 << 0;
const int  Handle::ALLOCATED = 1 << 1;


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


Handle::Handle ( const Handle& rhs )
{
  Library::init ();

  id_    = rhs.id_;
  flags_ = rhs.flags_;

  if ( flags_ & ALLOCATED )
  {
    if ( H5Iinc_ref( (hid_t) id_ ) < 0 )
    {
      throw H5Exception (
        JEM_FUNC,
        "failed to increment a handle reference count",
        Library::strerror ()
      );
    }
  }
}


Handle::~Handle ()
{
  if ( flags_ & ALLOCATED )
  {
    H5Idec_ref ( (hid_t) id_ );
  }
}


//-----------------------------------------------------------------------
//   operator =
//-----------------------------------------------------------------------


Handle& Handle::operator = ( const Handle& rhs )
{
  if ( id_ == rhs.id_ )
  {
    return *this;
  }

  if ( flags_ & ALLOCATED )
  {
    if ( H5Idec_ref( (hid_t) id_ ) < 0 )
    {
      throw H5Exception (
        JEM_FUNC,
        "failed to decrement a handle reference count",
        Library::strerror ()
      );
    }
  }

  id_    = rhs.id_;
  flags_ = rhs.flags_;

  if ( flags_ & ALLOCATED )
  {
    if ( H5Iinc_ref( (hid_t) id_ ) < 0 )
    {
      id_    = 0;
      flags_ = 0;

      throw H5Exception (
        JEM_FUNC,
        "failed to increment a handle reference count",
        Library::strerror ()
      );
    }
  }

  return *this;
}


JEM_END_PACKAGE( hdf5 )

