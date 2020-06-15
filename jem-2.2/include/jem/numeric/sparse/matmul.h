
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

#ifndef JEM_NUMERIC_SPARSE_MATMUL_H
#define JEM_NUMERIC_SPARSE_MATMUL_H

#include <jem/numeric/sparse/SparseMatrix.h>
#include <jem/numeric/sparse/SparseFilter.h>


JEM_BEGIN_PACKAGE( numeric )


//-----------------------------------------------------------------------
//   matmul
//-----------------------------------------------------------------------


template <class T>

  void                          matmul

  ( const Array<T>&               r,
    const SparseMatrix<T>&        a,
    const Array<T>&               b );

template <class T>

  void                          matmul

  ( const Array<T>&               r,
    const Array<T>&               a,
    const SparseMatrix<T>&        b );

void                            matmul

  ( const Array<double>&          r,
    const SparseMatrix<double>&   a,
    const Array<double>&          b );

void                            matmul

  ( const Array<double>&          r,
    const Array<double>&          a,
    const SparseMatrix<double>&   b );


template <class T>

  Array<T>                      matmul

  ( const SparseMatrix<T>&        a,
    const Array<T>&               b );

template <class T>

  Array<T>                      matmul

  ( const Array<T>&               a,
    const SparseMatrix<T>&        b );

template <class T>

  inline SparseMatrix<T>        matmul

  ( const SparseMatrix<T>&        a,
    const SparseMatrix<T>&        b );

template <class T, class F>

  inline SparseMatrix<T>        matmul

  ( const SparseMatrix<T>&        a,
    const SparseMatrix<T>&        b,
    const SparseFilter<T,F>&      filter );




//#######################################################################
//   Implementation
//#######################################################################


JEM_END_PACKAGE( numeric )

#include <jem/numeric/sparse/matmul.tcc>

#endif
