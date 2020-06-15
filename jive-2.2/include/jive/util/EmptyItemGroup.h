
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

#ifndef JIVE_UTIL_EMPTYITEMGROUP_H
#define JIVE_UTIL_EMPTYITEMGROUP_H

#include <jem/base/Clonable.h>
#include <jem/io/Serializable.h>
#include <jive/util/ItemGroup.h>


JIVE_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class EmptyItemGroup
//-----------------------------------------------------------------------


class EmptyItemGroup : public ItemGroup,
                       public Clonable,
                       public Serializable
{
 public:

  JEM_DECLARE_CLASS     ( EmptyItemGroup, ItemGroup );


                          EmptyItemGroup  ();

  explicit                EmptyItemGroup

    ( const Ref<ItemSet>&   items );

  virtual void            readFrom

    ( ObjectInput&          in  );

  virtual void            writeTo

    ( ObjectOutput&         out )            const;

  virtual Ref<Object>     clone           () const;
  virtual idx_t           size            () const;
  virtual IdxVector       getIDs          () const;
  virtual IdxVector       getIndices      () const;

  virtual idx_t           getIndex

    ( idx_t                 i )              const;

  virtual ItemSet*        getItems        () const;

  virtual bool            contains

    ( idx_t                 iitem )          const;

  virtual void            filter

    ( const IdxVector&      iitems )         const;


 protected:

  virtual                ~EmptyItemGroup  ();


 private:

  Ref<ItemSet>            items_;

};


JIVE_END_PACKAGE( util )

#endif
