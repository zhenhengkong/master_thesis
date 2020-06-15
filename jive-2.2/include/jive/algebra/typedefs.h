
/*
 *  Copyright (C) 2016 DRG. All rights reserved.
 *
 *  This file is part of Jive, an object oriented toolkit for solving
 *  partial differential equations.
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
 *  This file is part of Jive, an object oriented toolkit for
 *  solving partial differential equations.
 *
 *  Jive version: 2.2
 *  Date:         Thu 28 Jan 10:31:15 CET 2016
 */

#ifndef JIVE_ALGEBRA_TYPEDEFS_H
#define JIVE_ALGEBRA_TYPEDEFS_H

#include <jive/defines.h>


//-----------------------------------------------------------------------
//   forward declarations
//-----------------------------------------------------------------------


namespace jem
{
  namespace mp
  {
    class Context;
  }
}


JIVE_BEGIN_PACKAGE( algebra )


class SparseMatrixObject;
class NullMatrixObject;
class MPMatrixObject;
class IdentMatrixObject;
class DiagMatrixObject;
class DenseMatrixObject;
class MatrixBuilder;
class FlexMatrixBuilder;
class SparseMatrixBuilder;
class LumpedMatrixBuilder;
class MPMatrixExtension;
class DiagMatrixExtension;
class MultiMatmulExtension;
class DirectMatrixExtension;
class SparseMatrixExtension;
class GramSchmidtFactory;
class ClassicalGramSchmidt;
class ModifiedGramSchmidt;
class IteratedGramSchmidt;


//-----------------------------------------------------------------------
//   typedefs
//-----------------------------------------------------------------------


typedef jem::mp::Context        MPContext;
typedef SparseMatrixObject      SparseMatrixObj;
typedef NullMatrixObject        NullMatrixObj;
typedef MPMatrixObject          MPMatrixObj;
typedef IdentMatrixObject       IdentMatrixObj;
typedef DiagMatrixObject        DiagMatrixObj;
typedef DenseMatrixObject       DenseMatrixObj;
typedef MatrixBuilder           MBuilder;
typedef FlexMatrixBuilder       FlexMBuilder;
typedef SparseMatrixBuilder     SparseMBuilder;
typedef LumpedMatrixBuilder     LumpedMBuilder;
typedef MPMatrixExtension       MPMatrixExt;
typedef DiagMatrixExtension     DiagMatrixExt;
typedef MultiMatmulExtension    MultiMatmulExt;
typedef DirectMatrixExtension   DirectMatrixExt;
typedef SparseMatrixExtension   SparseMatrixExt;
typedef GramSchmidtFactory      GSFactory;
typedef ClassicalGramSchmidt    ClassicalGS;
typedef ModifiedGramSchmidt     ModifiedGS;
typedef IteratedGramSchmidt     IteratedGS;


JIVE_END_PACKAGE( algebra )

#endif
