
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

#ifndef JIVE_UTIL_HASHITEMMAP_H
#define JIVE_UTIL_HASHITEMMAP_H

#include <jem/base/Clonable.h>
#include <jem/io/Serializable.h>
#include <jem/util/Flex.h>
#include <jive/util/import.h>
#include <jive/util/XItemMap.h>


JIVE_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class HashItemMap
//-----------------------------------------------------------------------


class HashItemMap : public XItemMap,
                    public Clonable,
                    public Serializable
{
 public:

  JEM_DECLARE_CLASS     ( HashItemMap, XItemMap );


                          HashItemMap    ();

                          HashItemMap

    ( const Self&           rhs );

  virtual void            readFrom

    ( ObjectInput&          in  );

  virtual void            writeTo

    ( ObjectOutput&         out )          const;

  virtual Ref<Object>     clone         () const;
  virtual idx_t           size          () const;
  virtual void            clear         ();

  virtual void            reserve

    ( idx_t                 count );

  virtual void            trimToSize    ();

  virtual idx_t           addItem       ();

  virtual idx_t           addItem

    ( idx_t                 itemID );

  virtual idx_t           addItems

    ( idx_t                 count );

  virtual void            reorderItems

    ( const Reordering&     reord );

  virtual idx_t           findItem

    ( idx_t                 itemID )       const;

  virtual idx_t           findItems

    ( const IdxVector&      iitems,
      const IdxVector&      itemIDs )      const;

  virtual idx_t           getItemID

    ( idx_t                 iitem )        const;

  virtual void            getItemIDs

    ( const IdxVector&      itemIDs,
      const IdxVector&      iitems )       const;


 protected:

  virtual                ~HashItemMap    ();


 private:

  class                   IndexMap_;

  IndexMap_*              indexMap_;
  jem::util::Flex<idx_t>  itemIDs_;
  idx_t                   nextID_;

};


JIVE_END_PACKAGE( util )

#endif
