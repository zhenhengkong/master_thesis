
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

#ifndef JEM_HDF5_HANDLE_H
#define JEM_HDF5_HANDLE_H

#include <jem/defines.h>


JEM_BEGIN_PACKAGE( hdf5 )


//-----------------------------------------------------------------------
//   class Handle
//-----------------------------------------------------------------------


class Handle
{
 public:

  static const int          VALID;
  static const int          ALLOCATED;


  inline                    Handle     ();

  explicit inline           Handle

    ( lint                    id,
      int                     flags = 0 );

                            Handle

    ( const Handle&           rhs );

                           ~Handle     ();

  Handle&                   operator =

    ( const Handle&           rhs );

  inline bool               operator ==

    ( const Handle&           rhs )       const;

  inline bool               operator !=

    ( const Handle&           rhs )       const;

  inline lint               get        () const;
  inline bool               isValid    () const;
  inline lint               release    ();


 private:

  lint                      id_;
  int                       flags_;

};



//#######################################################################
//  Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


inline Handle::Handle ()
{
  id_    = 0;
  flags_ = false;
}


inline Handle::Handle

  ( lint  id,
    int   flags )

{
  id_    = id;
  flags_ = flags | VALID;
}


//-----------------------------------------------------------------------
//   operator ==
//-----------------------------------------------------------------------


inline bool Handle::operator == ( const Handle& rhs ) const
{
  return (id_ == rhs.id_ &&
          (flags_ & VALID) == (rhs.flags_ & VALID));
}


//-----------------------------------------------------------------------
//   operator !=
//-----------------------------------------------------------------------



inline bool Handle::operator != ( const Handle& rhs ) const
{
  return ! operator == ( rhs );
}


//-----------------------------------------------------------------------
//   get
//-----------------------------------------------------------------------


inline lint Handle::get () const
{
  return id_;
}


//-----------------------------------------------------------------------
//   isValid
//-----------------------------------------------------------------------


inline bool Handle::isValid () const
{
  return ((flags_ & VALID) ? true : false);
}


//-----------------------------------------------------------------------
//   release()
//-----------------------------------------------------------------------


inline lint Handle::release ()
{
  lint  id = id_;

  id_    = 0;
  flags_ = 0;

  return id;
}


JEM_END_PACKAGE( hdf5 )

#endif
