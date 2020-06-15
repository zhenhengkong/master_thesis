
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

#ifndef JEM_GL_ARRAY_H
#define JEM_GL_ARRAY_H

#include <jem/base/array/Array.h>
#include <jem/gl/gl.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   Array types
//-----------------------------------------------------------------------


typedef Array<idx_t>        IdxVector;
typedef Array<idx_t,2>      IdxMatrix;

typedef Array<GLbyte>       GLbyteVector;
typedef Array<GLbyte,2>     GLbyteMatrix;
typedef Array<GLubyte>      GLubyteVector;
typedef Array<GLubyte,2>    GLubyteMatrix;
typedef Array<GLfloat>      GLfloatVector;
typedef Array<GLfloat,2>    GLfloatMatrix;
typedef Array<GLdouble>     GLdoubleVector;
typedef Array<GLdouble,2>   GLdoubleMatrix;


//-----------------------------------------------------------------------
//   Array conversion functions
//-----------------------------------------------------------------------


void                        convert

  ( const GLbyteVector&       lhs,
    const GLfloatVector&      rhs );

void                        convert

  ( const GLbyteMatrix&       lhs,
    const GLfloatMatrix&      rhs );

void                        convert

  ( const GLbyteVector&       lhs,
    const GLdoubleVector&     rhs );

void                        convert

  ( const GLbyteMatrix&       lhs,
    const GLdoubleMatrix&     rhs );

void                        convert

  ( const GLubyteVector&      lhs,
    const GLfloatVector&      rhs );

void                        convert

  ( const GLubyteMatrix&      lhs,
    const GLfloatMatrix&      rhs );

void                        convert

  ( const GLubyteVector&      lhs,
    const GLdoubleVector&     rhs );

void                        convert

  ( const GLubyteMatrix&      lhs,
    const GLdoubleMatrix&     rhs );


template <class T, int N>

  inline Array<GLbyte,N>    toBytes

  ( const Array<T,N>&         rhs );


template <class T, int N>

  inline Array<GLubyte,N>   toUbytes

  ( const Array<T,N>&         rhs );





//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   toBytes
//-----------------------------------------------------------------------


template <class T, int N>

  inline Array<GLbyte,N>    toBytes

  ( const Array<T,N>&         rhs )

{
  Array<GLbyte,N>  lhs ( rhs.shape() );

  convert ( lhs, rhs );

  return lhs;
}


template <class T, int N>

  inline Array<GLubyte,N>   toUbytes

  ( const Array<T,N>&         rhs )

{
  Array<GLubyte,N>  lhs ( rhs.shape() );

  convert ( lhs, rhs );

  return lhs;
}


JEM_END_PACKAGE( gl )

#endif
