
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

#ifndef JEM_BASE_LIMITS_H
#define JEM_BASE_LIMITS_H

#include <jem/defines.h>


JEM_BEGIN_PACKAGE_BASE


//-----------------------------------------------------------------------
//   class Limits & specializations
//-----------------------------------------------------------------------


template <class T>  class Limits {};


template <>

  class Limits<byte>

{
 public:

  static const byte     MIN_VALUE;
  static const byte     MAX_VALUE;

};


template <>

  class Limits<char>

{
 public:

  static const char     MIN_VALUE;
  static const char     MAX_VALUE;

};


template <>

  class Limits<short>

{
 public:

  static const short    MIN_VALUE;
  static const short    MAX_VALUE;

};


template <>

  class Limits<unsigned short>

{
 public:

  static const
    unsigned short      MIN_VALUE;
  static const
    unsigned short      MAX_VALUE;

};

template <>

  class Limits<int>

{
 public:

  static const int      MIN_VALUE;
  static const int      MAX_VALUE;

};


template <>

  class Limits<unsigned int>

{
 public:

  static const
    unsigned int        MIN_VALUE;
  static const
    unsigned int        MAX_VALUE;

};

template <>

  class Limits<lint>

{
 public:

  static const lint     MIN_VALUE;
  static const lint     MAX_VALUE;

};


template <>

  class Limits<ulint>

{
 public:

  static const ulint    MIN_VALUE;
  static const ulint    MAX_VALUE;

};

template <>

  class Limits<float>

{
 public:

  static const float    MIN_VALUE;
  static const float    MAX_VALUE;
  static const float    TINY_VALUE;
  static const float    EPSILON;

};


template <>

  class Limits<double>

{
 public:

  static const double   MIN_VALUE;
  static const double   MAX_VALUE;
  static const double   TINY_VALUE;
  static const double   EPSILON;

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


template <class T>

  inline T              minOf ()

{
  return Limits<T>::MIN_VALUE;
}


template <class T>

  inline T              minOf ( const T& )

{
  return Limits<T>::MIN_VALUE;
}


template <class T>

  inline T              maxOf ()

{
  return Limits<T>::MAX_VALUE;
}


template <class T>

  inline T              maxOf ( const T& )

{
  return Limits<T>::MAX_VALUE;
}


inline bool             isTiny ( float x )
{
  return ( x < Limits<float>::TINY_VALUE &&
          -x < Limits<float>::TINY_VALUE );
}


inline bool             isTiny ( double x )
{
  return ( x < Limits<double>::TINY_VALUE &&
          -x < Limits<double>::TINY_VALUE );
}


JEM_END_PACKAGE_BASE

#endif
