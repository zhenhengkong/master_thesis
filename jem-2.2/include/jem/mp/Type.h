
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

#ifndef JEM_MP_TYPE_H
#define JEM_MP_TYPE_H

#include <jem/defines.h>


namespace jem
{
  namespace util
  {
    template <class U, class V> class Pair;
  }
}


JEM_BEGIN_PACKAGE( mp )


//-----------------------------------------------------------------------
//   enum Type & related stuff
//-----------------------------------------------------------------------


enum Type
{
  NO_TYPE,
  BYTE,
  CHAR,
  SHORT,
  INT,
  LINT,
  FLOAT,
  DOUBLE,
  INT_PAIR,
  DOUBLE_INT
};


enum
{
  TYPE_COUNT = DOUBLE_INT + 1
};


extern const int TYPE_SIZES[TYPE_COUNT];


typedef util::Pair<int,int>     IntPair;
typedef util::Pair<double,int>  DoubleInt;


inline int    sizeOf   ( Type  t );

const char*   typeName ( int   t );

inline Type   getType  ( byte*      );
inline Type   getType  ( char*      );
inline Type   getType  ( short*     );
inline Type   getType  ( int*       );
inline Type   getType  ( lint*      );
inline Type   getType  ( float*     );
inline Type   getType  ( double*    );
inline Type   getType  ( IntPair*   );
inline Type   getType  ( DoubleInt* );





//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   sizeOf
//-----------------------------------------------------------------------


inline int sizeOf ( Type t )
{
  return TYPE_SIZES[t];
}


//-----------------------------------------------------------------------
//   getType
//-----------------------------------------------------------------------


inline Type getType ( byte* )
{
  return BYTE;
}


inline Type getType ( char* )
{
  return CHAR;
}


inline Type getType ( short* )
{
  return SHORT;
}


inline Type getType ( int* )
{
  return INT;
}


inline Type getType ( lint* )
{
  return LINT;
}


inline Type getType ( float* )
{
  return FLOAT;
}


inline Type getType ( double* )
{
  return DOUBLE;
}


inline Type getType ( IntPair* )
{
  return INT_PAIR;
}


inline Type getType ( DoubleInt* )
{
  return DOUBLE_INT;
}


JEM_END_PACKAGE( mp )

#endif
