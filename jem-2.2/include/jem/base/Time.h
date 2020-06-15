
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

#ifndef JEM_BASE_TIME_H
#define JEM_BASE_TIME_H

#include <jem/base/TypeTraits.h>


JEM_BEGIN_PACKAGE_BASE


//-----------------------------------------------------------------------
//   class Time
//-----------------------------------------------------------------------


class Time
{
 public:

  inline                 Time             ();

  explicit               Time

    ( lint                 sec,
      lint                 nsec = 0 );

  explicit               Time

    ( double               sec );

  inline                 Time

    ( const Time&          rhs );

  void                  operator +=

    ( const Time&         rhs );

  void                  operator -=

    ( const Time&         rhs );

  inline void           swap

    ( Time&               rhs );

  inline lint           sec               () const;
  inline lint           nsec              () const;
  inline double         toDouble          () const;

  String                toString          () const;

  String                toString

    ( const String&       format )           const;

  static inline Time    zero              ();
  static Time           now               ();
  static Time           resolution        ();


 private:

  void                  normalize_        ();

  static void           initResolution_   ();


 private:

  static const lint     NANOS_PER_SEC_;

  static lint           tick_;
  static lint           nanoTick_;

  lint                  sec_;
  lint                  nsec_;

};


//-----------------------------------------------------------------------
//   Related functions
//-----------------------------------------------------------------------


Time                    operator +

  ( const Time&           lhs,
    const Time&           rhs );

Time                    operator -

  ( const Time&           lhs,
    const Time&           rhs );

inline bool             operator ==

  ( const Time&           lhs,
    const Time&           rhs );

inline bool             operator !=

  ( const Time&           lhs,
    const Time&           rhs );

bool                    operator <

  ( const Time&           lhs,
    const Time&           rhs );

inline bool             operator <=

  ( const Time&           lhs,
    const Time&           rhs );

inline bool             operator >

  ( const Time&           lhs,
    const Time&           rhs );

inline bool             operator >=

  ( const Time&           lhs,
    const Time&           rhs );

template <class Input>

  inline void           decode

  ( Input&                in,
    Time&                 t );

template <class Output>

  inline void           encode

  ( Output&               out,
    const Time&           t );

template <class Output>

  inline void           print

  ( Output&               out,
    const Time&           t );

inline void             swap

  ( Time&                 lhs,
    Time&                 rhs );





//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class Time
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


inline Time::Time () :

  sec_  ( JEM_LINT_C(0) ),
  nsec_ ( JEM_LINT_C(0) )

{}


inline Time::Time ( const Time& rhs ) :

  sec_  ( rhs.sec_  ),
  nsec_ ( rhs.nsec_ )

{}


//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


inline void Time::swap ( Time& rhs )
{
  jem::swap ( sec_,  rhs.sec_  );
  jem::swap ( nsec_, rhs.nsec_ );
}


//-----------------------------------------------------------------------
//   sec & nsec
//-----------------------------------------------------------------------


inline lint Time::sec () const
{
  return sec_;
}


inline lint Time::nsec () const
{
  return nsec_;
}


//-----------------------------------------------------------------------
//   toDouble
//-----------------------------------------------------------------------


inline double Time::toDouble () const
{
  return ((double) sec_ + 1.0e-9 * (double) nsec_);
}


//-----------------------------------------------------------------------
//   zero
//-----------------------------------------------------------------------


inline Time Time::zero ()
{
  return Time ( JEM_LINT_C(0), JEM_LINT_C(0) );
}


//=======================================================================
//   Related functions
//=======================================================================

//-----------------------------------------------------------------------
//   comparison operators
//-----------------------------------------------------------------------


inline bool     operator ==

  ( const Time&   lhs,
    const Time&   rhs )

{
  return (lhs.sec() == rhs.sec() && lhs.nsec() == rhs.nsec());
}


inline bool     operator !=

  ( const Time&   lhs,
    const Time&   rhs )

{
  return !( lhs == rhs );
}


inline bool     operator <=

  ( const Time&   lhs,
    const Time&   rhs )

{
  return !( rhs < lhs );
}


inline bool     operator >

  ( const Time&   lhs,
    const Time&   rhs )

{
  return ( rhs < lhs );
}


inline bool     operator >=

  ( const Time&   lhs,
    const Time&   rhs )

{
  return !( lhs < rhs );
}


//-----------------------------------------------------------------------
//   decode
//-----------------------------------------------------------------------


template <class Input>

  inline void   decode

  ( Input&        in,
    Time&         t )

{
  lint  sec, nsec;

  decode ( in, sec, nsec );

  t = Time ( sec, nsec );
}


//-----------------------------------------------------------------------
//   encode
//-----------------------------------------------------------------------


template <class Output>

  inline void   encode

  ( Output&       out,
    const Time&   t )

{
  encode ( out, t.sec(), t.nsec() );
}


//-----------------------------------------------------------------------
//   print
//-----------------------------------------------------------------------


template <class Output>

  inline void   print

  ( Output&       out,
    const Time&   t )

{
  print ( out, t.toString() );
}


//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


inline void swap ( Time& lhs, Time& rhs )
{
  lhs.swap( rhs );
}


//=======================================================================
//   class TypeTraits<Time>
//=======================================================================


template <>

  class TypeTraits<Time> : public BasicTypeTraits

{
 public:

  static const bool     IS_SERIALIZABLE  = true;
  static const bool     HAS_TRIVIAL_COPY = true;
  static const bool     HAS_TRIVIAL_DTOR = true;


  static String         whatis    ()
  {
    return String ( "a time" );
  }

  static String         typeName  ()
  {
    return String ( "jem::Time" );
  }

  static lint           hashValue

    ( const Time&         t )

  {
    return ( t.sec() ^ t.nsec() );
  }

};


JEM_END_PACKAGE_BASE

#endif
