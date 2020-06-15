
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


#include <jem/pragmas.h>
#include <jem/numeric/sparse/matmul.h>


JEM_BEGIN_PACKAGE( numeric )


//-----------------------------------------------------------------------
//   matmul (matrix - vector)
//-----------------------------------------------------------------------


void                            matmul

  ( const Array<double>&          r,
    const SparseMatrix<double>&   a,
    const Array<double>&          b )

{
  JEM_ASSERT2 ( a.size(0) == r.size() &&
                a.size(1) == b.size(),
                "sparse matrix/vector shape mismatch" );

  const idx_t  rowCount = r.size ();

  double       x;
  idx_t        irow;
  idx_t        i, k;


  i = 0;

  if ( a.isContiguous() && b.isContiguous() )
  {
    const idx_t*  JEM_RESTRICT  offsets = a.getOffsetPtr ();
    const idx_t*  JEM_RESTRICT  indices = a.getIndexPtr  ();
    const double* JEM_RESTRICT  values  = a.getValuePtr  ();
    const double* JEM_RESTRICT  rhs     = b.addr         ();


    for ( irow = 0; irow < rowCount; irow++ )
    {
      x = 0.0;
      k = offsets[irow + 1];

JEM_NOPREFETCH( rhs )

      for ( ; i < k; i++ )
      {
        x += values[i] * rhs[indices[i]];
      }

      r[irow] = x;
    }
  }
  else
  {
    const Array<idx_t>&   offsets = a.getRowOffsets    ();
    const Array<idx_t>&   indices = a.getColumnIndices ();
    const Array<double>&  values  = a.getValues        ();


    for ( irow = 0; irow < rowCount; irow++ )
    {
      x = 0.0;
      k = offsets[irow + 1];

      for ( ; i < k; i++ )
      {
        x += values[i] * b[indices[i]];
      }

      r[irow] = x;
    }
  }
}


//-----------------------------------------------------------------------
//   matmul (vector - matrix)
//-----------------------------------------------------------------------


void                            matmul

  ( const Array<double>&          r,
    const Array<double>&          a,
    const SparseMatrix<double>&   b )

{
  JEM_ASSERT2 ( b.size(0) == a.size() &&
                b.size(1) == r.size(),
                "sparse matrix/vector shape mismatch" );

  const idx_t  rowCount = b.size (0);
  const idx_t  colCount = b.size (1);

  double       x;
  idx_t        irow;
  idx_t        i, k;


  i = 0;

  if ( r.isContiguous() && b.isContiguous() )
  {
    const idx_t*  JEM_RESTRICT  offsets = b.getOffsetPtr ();
    const idx_t*  JEM_RESTRICT  indices = b.getIndexPtr  ();
    const double* JEM_RESTRICT  values  = b.getValuePtr  ();
    double*       JEM_RESTRICT  lhs     = r.addr         ();


    for ( k = 0; k < colCount; k++ )
    {
      lhs[k] = 0.0;
    }

    for ( irow = 0; irow < rowCount; irow++ )
    {
      x = a[irow];
      k = offsets[irow + 1];

JEM_NOPREFETCH( lhs )
JEM_IVDEP

      for ( ; i < k; i++ )
      {
        lhs[indices[i]] += values[i] * x;
      }
    }
  }
  else
  {
    const Array<idx_t>&   offsets  = b.getRowOffsets    ();
    const Array<idx_t>&   indices  = b.getColumnIndices ();
    const Array<double>&  values   = b.getValues        ();


    r = 0.0;

    for ( irow = 0; irow < rowCount; irow++ )
    {
      x = a[irow];
      k = offsets[irow + 1];

JEM_IVDEP

      for ( ; i < k; i++ )
      {
        r[indices[i]] += values[i] * x;
      }
    }
  }
}


JEM_END_PACKAGE( numeric )
