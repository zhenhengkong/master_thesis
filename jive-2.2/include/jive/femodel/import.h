
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

#ifndef JIVE_FEMODEL_IMPORT_H
#define JIVE_FEMODEL_IMPORT_H

#include <jive/defines.h>


//-----------------------------------------------------------------------
//   forward declarations
//-----------------------------------------------------------------------


namespace jem
{
  namespace io
  {
    class ObjectInput;
    class ObjectOutput;
    class Serializable;
  }

  namespace numeric
  {
    class Function;
  }
}

namespace jive
{
  namespace util
  {
    class Table;
    class XTable;
    class XDofSpace;
    class ItemGroup;
  }

  namespace algebra
  {
    class MatrixBuilder;

    typedef MatrixBuilder     MBuilder;
  }

  namespace model
  {
    class Model;
  }

  namespace fem
  {
    class NodeSet;
    class NodeGroup;
    class Element;
    class ElementSet;
    class ElementGroup;
    class InternalElement;
    class ElementIterator;

    typedef InternalElement   IElement;
    typedef ElementIterator   ElemIter;
  }
}


JIVE_BEGIN_PACKAGE( femodel )


//-----------------------------------------------------------------------
//   using declarations
//-----------------------------------------------------------------------


using jem::io::ObjectInput;
using jem::io::ObjectOutput;
using jem::io::Serializable;
using jem::numeric::Function;

using jive::util::Table;
using jive::util::XTable;
using jive::util::XDofSpace;
using jive::util::ItemGroup;
using jive::algebra::MBuilder;
using jive::model::Model;
using jive::fem::NodeSet;
using jive::fem::NodeGroup;
using jive::fem::Element;
using jive::fem::ElementSet;
using jive::fem::ElementGroup;
using jive::fem::IElement;
using jive::fem::ElemIter;


JIVE_END_PACKAGE( femodel )

#endif
