
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

#ifndef JEM_NUMERIC_NUMBERTRAITS_H
#define JEM_NUMERIC_NUMBERTRAITS_H

#include <jem/base/limits.h>


JEM_BEGIN_PACKAGE( numeric )


//-----------------------------------------------------------------------
//   class NumberTraitsBase
//-----------------------------------------------------------------------


template <class T>

  class NumberTraitsBase

{
 public:

  static T          one    ()
  {
    return T ( 1 );
  }

  static T          zero   ()
  {
    return T ();
  }

  static bool       isTiny ( T  x )
  {
    return ( x == zero() );
  }

  static void       scale  ( T& x, double s )
  {
    x = T ( x * s );
  }

  static void       madd   ( T& x, T y, T z )
  {
    x += y * z;
  }

};


//-----------------------------------------------------------------------
//   class NumberTraits
//-----------------------------------------------------------------------


template <class T>

  class NumberTraits : public NumberTraitsBase<T> {};


//-----------------------------------------------------------------------
//   class NumberTraits<bool>
//-----------------------------------------------------------------------


template <>

  class NumberTraits<bool>

{
 public:

  static bool       one    ()
  {
    return true;
  }

  static bool       zero   ()
  {
    return false;
  }

  static bool       isTiny ( bool  x )
  {
    return (x == false);
  }

  static void       scale  ( bool& x, double s )
  {
    if ( x && jem::isTiny( s ) )
    {
      x = false;
    }
  }

  static void       madd   ( bool& x, bool y, bool z )
  {
    x = x || ( y && z );
  }

};


//-----------------------------------------------------------------------
//   class NumberTraits<float>
//-----------------------------------------------------------------------


template <>

  class NumberTraits<float> : public NumberTraitsBase<float>

{
 public:

  static float      one    ()
  {
    return 1.0F;
  }

  static float      zero   ()
  {
    return 0.0F;
  }

  static bool       isTiny ( float x )
  {
    return jem::isTiny ( x );
  }

};


//-----------------------------------------------------------------------
//   class NumberTraits<double>
//-----------------------------------------------------------------------


template <>

  class NumberTraits<double> : public NumberTraitsBase<double>

{
 public:

  static double     one    ()
  {
    return 1.0;
  }

  static double     zero   ()
  {
    return 0.0;
  }

  static bool       isTiny ( double x )
  {
    return jem::isTiny ( x );
  }

};


//-----------------------------------------------------------------------
//   NumberTraits< std::complex<T> >
//-----------------------------------------------------------------------

#ifdef JEM_STD_COMPLEX


template <class T>

  class NumberTraits< std::complex<T> >
{
 public:

  typedef std::complex<T>   Complex;


  static Complex    one    ()
  {
    return Complex ( NumberTraits<T>::one() );
  }

  static Complex    zero   ()
  {
    return Complex ();
  }

  static bool       isTiny ( const Complex&  z )
  {
    return NumberTraits<T>::isTiny ( std::abs( z ) );
  }

  static void       scale  ( Complex&        z,
                             double          s )
  {
    z *= T(s);
  }

  static void       madd   ( Complex&        x,
                             const Complex&  y,
                             const Complex&  z )
  {
    x += y * z;
  }

};


#endif

JEM_END_PACKAGE( numeric )

#endif
