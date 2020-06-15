
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

#ifndef JIVE_MP_IMPORT_H
#define JIVE_MP_IMPORT_H

#include <jive/defines.h>


//-----------------------------------------------------------------------
//   forward declarations
//-----------------------------------------------------------------------


namespace jem
{
  class Clonable;

  namespace io
  {
    class DataInput;
    class DataOutput;
    class ObjectInput;
    class ObjectOutput;
    class Serializable;
  }

  namespace mp
  {
    class Context;
  }

  namespace util
  {
    class Dictionary;
  }
}

namespace jive
{
  namespace util
  {
    class EventMask;
    class EventSource;
    class ItemSet;
    class GroupSet;
    class XTable;
    class Database;
    class DofSpace;
    class XDofSpace;
    class Reordering;
    class MemberSet;
    class XMemberSet;
    class ItemSetSkin;
  }
}


JIVE_BEGIN_PACKAGE( mp )


//-----------------------------------------------------------------------
//   using declarations
//-----------------------------------------------------------------------


using jem::Clonable;
using jem::io::DataInput;
using jem::io::DataOutput;
using jem::io::ObjectInput;
using jem::io::ObjectOutput;
using jem::io::Serializable;
using jem::mp::Context;
using jem::util::Dictionary;
using jive::util::EventMask;
using jive::util::EventSource;
using jive::util::ItemSet;
using jive::util::ItemSetSkin;
using jive::util::GroupSet;
using jive::util::XTable;
using jive::util::Database;
using jive::util::DofSpace;
using jive::util::XDofSpace;
using jive::util::Reordering;
using jive::util::MemberSet;
using jive::util::XMemberSet;


JIVE_END_PACKAGE( mp )

#endif
