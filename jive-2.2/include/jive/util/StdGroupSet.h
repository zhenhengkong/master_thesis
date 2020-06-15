
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

#ifndef JIVE_UTIL_STDGROUPSET_H
#define JIVE_UTIL_STDGROUPSET_H

#include <jem/base/Clonable.h>
#include <jem/io/Serializable.h>
#include <jive/util/XGroupSet.h>


JIVE_BEGIN_PACKAGE( util )


class XItemMap;


//-----------------------------------------------------------------------
//   class StdGroupSet
//-----------------------------------------------------------------------


class StdGroupSet : public XGroupSet,
                    public Clonable,
                    public Serializable
{
 public:

  JEM_DECLARE_CLASS       ( StdGroupSet, XGroupSet );


                            StdGroupSet     ();

                            StdGroupSet

    ( const String&           name,
      const String&           groupName,
      const Ref<ItemSet>&     items,
      const Ref<XItemMap>&    map = NIL );

                            StdGroupSet

    ( const Self&             rhs );

  virtual void              resetEvents     ();

  virtual void              readFrom

    ( ObjectInput&            in  );

  virtual void              writeTo

    ( ObjectOutput&           out )            const;

  virtual Ref<Object>       clone           () const;
  virtual idx_t             size            () const;
  virtual String            getItemName     () const;
  virtual ItemMap*          getItemMap      () const;
  virtual void              clear           ();

  virtual void              reserve

    ( idx_t                   count );

  virtual void              trimToSize      ();

  virtual idx_t             addGroup

    ( const IdxVector&        iitems );

  virtual idx_t             addGroup

    ( idx_t                   id,
      const IdxVector&        iitems );

  virtual idx_t             addGroups

    ( const IdxMatrix&        groups );

  virtual void              reorderGroups

    ( const Reordering&       reord );

  virtual void              setGroupMembers

    ( idx_t                   igroup,
      const IdxVector&        iitems );

  virtual idx_t             getGroupSize

    ( idx_t                   igroup )         const;

  virtual idx_t             getGroupMembers

    ( const IdxVector&        iitems,
      idx_t                   igroup  )        const;

  virtual void              getSomeGroupMembers

    ( const IdxVector&        iitems,
      const IdxVector&        index,
      idx_t                   igroup  )        const;

  virtual idx_t             getOneGroupMember

    ( idx_t                   index,
      idx_t                   igroup  )        const;

  virtual IdxVector         getMembersOf

    ( const IdxVector&        igroups )        const;

  virtual idx_t             maxGroupSize    () const;

  virtual idx_t             maxGroupSizeOf

    ( const IdxVector&        igroups )        const;

  virtual IdxVector         toVector        () const;
  virtual Topology          toMatrix        () const;
  virtual ItemSet*          getGroupedItems () const;


 protected:

  virtual                  ~StdGroupSet     ();

  virtual void              emitEvents_

    ( int                     events );


 private:

  void                      connect_        ();

  void                      itemsResized_

    ( idx_t                   count );

  void                      itemsReordered_

    ( const Reordering&       reord );


 private:

  class                     Data_;

  Ref<Data_>                data_;
  Ref<XItemMap>             map_;

};


JIVE_END_PACKAGE( util )

#endif
