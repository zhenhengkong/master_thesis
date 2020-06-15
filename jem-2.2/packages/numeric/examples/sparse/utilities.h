
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

#ifndef UTILITIES_H
#define UTILITIES_H

#include <jem/base/Array.h>
#include <jem/numeric/sparse/SparseMatrix.h>


//-----------------------------------------------------------------------
//   toDenseMatrix
//-----------------------------------------------------------------------


template <class T>

  jem::Array<T,2> toDenseMatrix

  ( const jem::numeric::SparseMatrix<T>&  sparse )

{
  using jem::idx_t;

  const idx_t        rowCount = sparse.size(0);

  jem::Array<idx_t>  offsets  = sparse.getRowOffsets    ();
  jem::Array<idx_t>  indices  = sparse.getColumnIndices ();
  jem::Array<T>      values   = sparse.getValues        ();

  jem::Array<T,2>    dense    ( sparse.shape() );

  idx_t              irow;
  idx_t              i, n;


  dense = T ();

  for ( i = 0, irow = 0; irow < rowCount; irow++ )
  {
    n = offsets[irow + 1];

    for ( ; i < n; i++ )
    {
      dense(irow,indices[i]) = values[i];
    }
  }

  return dense;
}


#endif