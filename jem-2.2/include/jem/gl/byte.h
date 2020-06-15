
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

#ifndef JEM_GL_BYTE_H
#define JEM_GL_BYTE_H

#include <jem/defines.h>
#include <jem/gl/gl.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   conversion functions
//-----------------------------------------------------------------------


inline GLbyte   toByte    ( float     fval );
inline GLbyte   toByte    ( double    dval );
inline GLubyte  toUbyte   ( float     fval );
inline GLubyte  toUbyte   ( double    dval );
inline float    toFloat   ( GLbyte    bval );
inline float    toFloat   ( GLubyte   bval );
inline double   toDouble  ( GLbyte    bval );
inline double   toDouble  ( GLubyte   bval );

inline void     convert   ( GLbyte&   lhs, float    rhs );
inline void     convert   ( GLbyte&   lhs, double   rhs );
inline void     convert   ( GLubyte&  lhs, float    rhs );
inline void     convert   ( GLubyte&  lhs, double   rhs );
inline void     convert   ( float&    lhs, GLbyte   rhs );
inline void     convert   ( float&    lhs, GLubyte  rhs );
inline void     convert   ( double&   lhs, GLbyte   rhs );
inline void     convert   ( double&   lhs, GLubyte  rhs );




//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   toByte
//-----------------------------------------------------------------------


inline GLbyte toByte ( float  fval )
{
  int  i = (int) (255.0 * 0.5 * ((double) fval + 1.0)) - 128;

  if      ( i < -128 )
  {
    return (GLbyte) -128;
  }
  else if ( i >  127 )
  {
    return (GLbyte)  127;
  }
  else
  {
    return (GLbyte)    i;
  }
}


inline GLbyte toByte ( double dval )
{
  int  i = (int) (255.0 * 0.5 * (dval + 1.0)) - 128;

  if      ( i < -128 )
  {
    return (GLbyte) -128;
  }
  else if ( i >  127 )
  {
    return (GLbyte)  127;
  }
  else
  {
    return (GLbyte)    i;
  }
}


//-----------------------------------------------------------------------
//   toUbyte
//-----------------------------------------------------------------------


inline GLubyte toUbyte ( float  fval )
{
  int  i = (int) (255.0 * (double) fval);

  if      ( i < 0 )
  {
    return (GLubyte)   0;
  }
  else if ( i > 255 )
  {
    return (GLubyte) 255;
  }
  else
  {
    return (GLubyte)   i;
  }
}


inline GLubyte toUbyte ( double dval )
{
  int  i = (int) (255.0 * dval);

  if      ( i < 0 )
  {
    return (GLubyte)   0;
  }
  else if ( i > 255 )
  {
    return (GLubyte) 255;
  }
  else
  {
    return (GLubyte)   i;
  }
}


//-----------------------------------------------------------------------
//   toFloat
//-----------------------------------------------------------------------


inline float toFloat ( GLbyte bval )
{
  return (float) (2.0 * (((int) bval + 128) / 255.0) - 1.0);
}


inline float toFloat ( GLubyte bval )
{
  return (float) (bval / 255.0);
}


//-----------------------------------------------------------------------
//   toDouble
//-----------------------------------------------------------------------


inline double toDouble ( GLbyte bval )
{
  return (double) (2.0 * (((int) bval + 128) / 255.0) - 1.0);
}


inline double toDouble ( GLubyte bval )
{
  return (double) (bval / 255.0);
}


//-----------------------------------------------------------------------
//   convert
//-----------------------------------------------------------------------


inline void  convert ( GLbyte&   lhs, float    rhs )
{
  lhs = toByte ( rhs );
}

inline void  convert ( GLbyte&   lhs, double   rhs )
{
  lhs = toByte ( rhs );
}

inline void  convert ( GLubyte&  lhs, float    rhs )
{
  lhs = toUbyte ( rhs );
}

inline void  convert ( GLubyte&  lhs, double   rhs )
{
  lhs = toUbyte ( rhs );
}

inline void  convert ( float&    lhs, GLbyte   rhs )
{
  lhs = toFloat ( rhs );
}

inline void  convert ( float&    lhs, GLubyte  rhs )
{
  lhs = toFloat ( rhs );
}

inline void  convert ( double&   lhs, GLbyte   rhs )
{
  lhs = toDouble ( rhs );
}

inline void  convert ( double&   lhs, GLubyte  rhs )
{
  lhs = toDouble ( rhs );
}


JEM_END_PACKAGE( gl )

#endif
