
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

#ifndef JIVE_SOLVER_IMPORT_H
#define JIVE_SOLVER_IMPORT_H

#include <jive/defines.h>


//-----------------------------------------------------------------------
//   forward declarations
//-----------------------------------------------------------------------


namespace jem
{
  class Clonable;

  namespace io
  {
    class Writer;
    class PrintWriter;
    class ObjectInput;
    class ObjectOutput;
    class Serializable;
  }
}

namespace jive
{
  namespace util
  {
    class   Table;
    class   ItemSet;
    class   DofSpace;
    class   Constraints;
  }

  namespace mp
  {
    class   BorderSet;
    class   VectorExchanger;
  }

  namespace algebra
  {
    class   VectorSpace;
    class   GramSchmidt;
    class   AbstractMatrix;
    class   ConstrainedMatrix;
    class   MPMatrixExtension;
    class   SparseMatrixObject;
    class   SparseMatrixExtension;
    class   MultiMatmulExtension;

    typedef MPMatrixExtension      MPMatrixExt;
    typedef MultiMatmulExtension   MultiMatmulExt;
    typedef SparseMatrixObject     SparseMatrixObj;
    typedef SparseMatrixExtension  SparseMatrixExt;
  }
}


JIVE_BEGIN_PACKAGE( solver )


//-----------------------------------------------------------------------
//   using declarations
//-----------------------------------------------------------------------


using jem::Clonable;
using jem::io::Writer;
using jem::io::PrintWriter;
using jem::io::ObjectInput;
using jem::io::ObjectOutput;
using jem::io::Serializable;
using jive::util::Table;
using jive::util::ItemSet;
using jive::util::DofSpace;
using jive::util::Constraints;
using jive::mp::BorderSet;
using jive::mp::VectorExchanger;
using jive::algebra::VectorSpace;
using jive::algebra::GramSchmidt;
using jive::algebra::AbstractMatrix;
using jive::algebra::ConstrainedMatrix;
using jive::algebra::MPMatrixExt;
using jive::algebra::MultiMatmulExt;
using jive::algebra::SparseMatrixObj;
using jive::algebra::SparseMatrixExt;


JIVE_END_PACKAGE( solver )

#endif