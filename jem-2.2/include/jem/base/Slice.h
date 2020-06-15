
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

#ifndef JEM_BASE_SLICE_H
#define JEM_BASE_SLICE_H

#include <jem/base/assert.h>


JEM_BEGIN_PACKAGE_BASE


//-----------------------------------------------------------------------
//   class Slice
//-----------------------------------------------------------------------


class Slice
{
 public:

  static const idx_t     MAX_INDEX;


  inline                Slice     ();

  inline                Slice

    ( idx_t               first,
      idx_t               last,
      idx_t               stride = 1 );

  inline                Slice

    ( const Slice&        rhs );

  inline idx_t          first     () const;

  inline idx_t          last      () const;
  inline idx_t          last

    ( idx_t               ubound )   const;

  inline idx_t          stride    () const;


 protected:

  idx_t                 first_;
  idx_t                 last_;
  idx_t                 stride_;

};


//-----------------------------------------------------------------------
//   class SliceFromTo
//-----------------------------------------------------------------------


class SliceFromTo : public Slice
{
 public:

  inline                SliceFromTo

    ( idx_t               first,
      idx_t               last );

};


//-----------------------------------------------------------------------
//   class SliceTo
//-----------------------------------------------------------------------


class SliceTo : public SliceFromTo
{
 public:

  inline                SliceTo

    ( idx_t               last );

};


//-----------------------------------------------------------------------
//   class SliceFrom
//-----------------------------------------------------------------------


class SliceFrom : public Slice
{
 public:

  inline                SliceFrom

    ( idx_t               first );

};


//-----------------------------------------------------------------------
//   class SliceAll
//-----------------------------------------------------------------------


class SliceAll : public SliceFrom
{
 public:

  inline                SliceAll  ();

};


extern const SliceAll   ALL;


//-----------------------------------------------------------------------
//   class Begin
//-----------------------------------------------------------------------


class Begin {};

extern const Begin      BEGIN;


//-----------------------------------------------------------------------
//   class End
//-----------------------------------------------------------------------


class End {};

extern const End        END;


//-----------------------------------------------------------------------
//   Related functions
//-----------------------------------------------------------------------


inline SliceFromTo      slice

  ( idx_t                 first,
    idx_t                 last );

inline Slice            slice

  ( idx_t                 first,
    idx_t                 last,
    idx_t                 stride );

inline SliceTo          slice

  ( Begin                 first,
    idx_t                 last );

inline Slice            slice

  ( Begin                 first,
    idx_t                 last,
    idx_t                 stride );

inline SliceFrom        slice

  ( idx_t                 first,
    End                   last );

inline Slice            slice

  ( idx_t                 first,
    End                   last,
    idx_t                 stride );

inline const SliceAll&  slice

  ( Begin                 first,
    End                   last );

inline Slice            slice

  ( Begin                 first,
    End                   last,
    idx_t                 stride );

inline const SliceAll&  slice

  ( const SliceAll&       all );

inline Slice            slice

  ( const SliceAll&       all,
    idx_t                 stride );





//#######################################################################
//   implementation
//#######################################################################

//=======================================================================
//   class Slice
//=======================================================================

//-----------------------------------------------------------------------
//   contructors
//-----------------------------------------------------------------------


inline Slice::Slice () :

  first_  ( 0 ),
  last_   ( 0 ),
  stride_ ( 0 )

{}


inline Slice::Slice ( idx_t first, idx_t last, idx_t stride ) :

  first_  (  first ),
  last_   (   last ),
  stride_ ( stride )

{
  JEM_ASSERT_NOTHROW ( first >= 0 && first <= last && stride > 0 );
}


inline Slice::Slice ( const Slice& rhs ) :

  first_  ( rhs.first_  ),
  last_   ( rhs.last_   ),
  stride_ ( rhs.stride_ )

{}


//-----------------------------------------------------------------------
//   first
//-----------------------------------------------------------------------


inline idx_t Slice::first () const
{
  return first_;
}


//-----------------------------------------------------------------------
//   last
//-----------------------------------------------------------------------


inline idx_t Slice::last () const
{
  return last_;
}


inline idx_t Slice::last ( idx_t ubound ) const
{
  // The comparison is made with unsigned numbers to suppress
  // warnings about signed overflow.

  return ((uidx_t) last_ > (uidx_t) ubound) ? ubound : last_;
}


//-----------------------------------------------------------------------
//   stride
//-----------------------------------------------------------------------


inline idx_t Slice::stride () const
{
  return stride_;
}


//=======================================================================
//   class SliceFromTo
//=======================================================================


inline SliceFromTo::SliceFromTo ( idx_t first, idx_t last ) :

  Slice ( first, last )

{}


//=======================================================================
//   class SliceTo
//=======================================================================


inline SliceTo::SliceTo ( idx_t last ) :

  SliceFromTo ( 0, last )

{}


//=======================================================================
//   class SliceFrom
//=======================================================================


inline SliceFrom::SliceFrom ( idx_t first ) :

  Slice ( first, Slice::MAX_INDEX )

{}


//=======================================================================
//   class SliceAll
//=======================================================================


inline SliceAll::SliceAll () :

  SliceFrom ( 0 )

{}


//=======================================================================
//   related functions
//=======================================================================


inline SliceFromTo      slice

  ( idx_t first, idx_t last )

{
  return SliceFromTo ( first, last );
}


inline Slice            slice

  ( idx_t first, idx_t last, idx_t stride )

{
  return Slice ( first, last, stride );
}


inline SliceTo          slice

  ( Begin, idx_t last )

{
  return SliceTo ( last );
}


inline Slice            slice

  ( Begin, idx_t last, idx_t stride )

{
  return Slice ( 0, last, stride );
}


inline SliceFrom        slice

  ( idx_t first, End )

{
  return SliceFrom ( first );
}


inline Slice            slice

  ( idx_t first, End, idx_t stride )

{
  return Slice ( first, Slice::MAX_INDEX, stride );
}


inline const SliceAll&  slice

  ( Begin, End )

{
  return ALL;
}


inline Slice            slice

  ( Begin, End, idx_t stride )

{
  return Slice ( 0, Slice::MAX_INDEX, stride );
}


inline const SliceAll&  slice

  ( const SliceAll& all )

{
  return all;
}


inline Slice            slice

  ( const SliceAll&, idx_t stride )

{
  return Slice ( 0, Slice::MAX_INDEX, stride );
}


JEM_END_PACKAGE_BASE

#endif
