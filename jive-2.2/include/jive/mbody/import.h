
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

#ifndef JIVE_MBODY_IMPORT_H
#define JIVE_MBODY_IMPORT_H

#include <jive/defines.h>


//-----------------------------------------------------------------------
//   forward declarations
//-----------------------------------------------------------------------


namespace jive
{
  namespace util
  {
    class ItemSet;
    class Reordering;
    class Database;
    class DatabaseSkin;
    class IntDBColumn;
    class FloatDBColumn;
    class ItemSetSkin;
    class ItemGroupSkin;
  }

  namespace geom
  {
    class ShapeTable;
  }

  namespace fem
  {
    class NodeSet;
    class ElementSet;
  }
}


JIVE_BEGIN_PACKAGE( mbody )


//-----------------------------------------------------------------------
//   using declarations
//-----------------------------------------------------------------------


using jive::util::ItemSet;
using jive::util::Reordering;
using jive::util::Database;
using jive::util::DatabaseSkin;
using jive::util::IntDBColumn;
using jive::util::FloatDBColumn;
using jive::util::ItemSetSkin;
using jive::util::ItemGroupSkin;
using jive::geom::ShapeTable;
using jive::fem::NodeSet;
using jive::fem::ElementSet;


JIVE_END_PACKAGE( mbody )

#endif
