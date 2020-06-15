
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

#ifndef JIVE_UTIL_ARRAYITEMGROUP_H
#define JIVE_UTIL_ARRAYITEMGROUP_H

#include <jem/base/Clonable.h>
#include <jem/io/Serializable.h>
#include <jive/util/XItemGroup.h>

namespace jem
{
  namespace util
  {
    class BitSet;
  }
}


JIVE_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class ArrayItemGroup
//-----------------------------------------------------------------------


class ArrayItemGroup : public XItemGroup,
                       public Clonable,
                       public Serializable
{
 public:

  JEM_DECLARE_CLASS     ( ArrayItemGroup, XItemGroup );


                          ArrayItemGroup ();

  explicit                ArrayItemGroup

    ( const Ref<ItemSet>&   items );

                          ArrayItemGroup

    ( const IdxVector&      iitems,
      const Ref<ItemSet>&   items );

  virtual void            readFrom

    ( ObjectInput&          in  );

  virtual void            writeTo

    ( ObjectOutput&         out )              const;

  virtual Ref<Object>     clone             () const;

  virtual idx_t           size              () const;
  virtual IdxVector       getIndices        () const;

  virtual idx_t           getIndex

    ( idx_t                 i )                const;

  virtual ItemSet*        getItems          () const;
  virtual void            clear             ();

  virtual void            append

    ( const IdxVector&      iitems );

  virtual void            reorder

    ( const Reordering&     reord );

  virtual bool            contains

    ( idx_t                 iitem  )           const;

  virtual void            filter

    ( const IdxVector&      iitems )           const;


 protected:

  virtual                ~ArrayItemGroup    ();


 private:

  void                    initMask_         () const;
  void                    freeMask_         ();
  void                    connect_          ();

  void                    checkIndices_

    ( const IdxVector&      iitems )           const;

  void                    itemsResized_

    ( idx_t                 count );

  void                    itemsReordered_

    ( const Reordering&     reord );


 private:

  Ref<ItemSet>            items_;
  IdxVector               iitems_;

  IdxVector*              imask_;
  jem::util::BitSet*      bmask_;

};


JIVE_END_PACKAGE( util )

#endif
