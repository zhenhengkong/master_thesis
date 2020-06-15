
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

#ifndef JIVE_UTIL_DENSEDOFSPACE_H
#define JIVE_UTIL_DENSEDOFSPACE_H

#include <jem/base/Clonable.h>
#include <jem/io/Serializable.h>
#include <jive/util/XDofSpace.h>


JIVE_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class DenseDofSpace
//-----------------------------------------------------------------------


class DenseDofSpace : public XDofSpace,
                      public Clonable,
                      public Serializable
{
 public:

  JEM_DECLARE_CLASS       ( DenseDofSpace, XDofSpace );


                            DenseDofSpace   ();

  explicit                  DenseDofSpace

    ( const Ref<ItemSet>&     items );

                            DenseDofSpace

    ( const Self&             rhs );

  virtual void              resetEvents     ();

  virtual void              readFrom

    ( ObjectInput&            in  );

  virtual void              writeTo

    ( ObjectOutput&           out )            const;

  virtual Ref<Object>       clone           () const;
  virtual idx_t             typeCount       () const;
  virtual idx_t             dofCount        () const;
  virtual void              clearDofs       ();
  virtual void              clearAll        ();

  virtual void              reserve

    ( idx_t                   dofCount );

  virtual void              trimToSize      ();

  virtual idx_t             addType

    ( const String&           name );

  virtual idx_t             findType

    ( const String&           name )           const;

  virtual String            getTypeName

    ( idx_t                   itype )          const;

  virtual StringVector      getTypeNames    () const;

  virtual idx_t             addDof

    ( idx_t                   iitem,
      idx_t                   itype );

  virtual void              addDofs

    ( const IdxVector&        iitems,
      const IdxVector&        itypes );

  virtual void              reorderDofs

    ( const Reordering&       reord );

  virtual idx_t             findDofIndex

    ( idx_t                   iitem,
      idx_t                   itype  )         const;

  virtual idx_t             findDofIndices

    ( const IdxVector&        idofs,
      const IdxVector&        iitems,
      const IdxVector&        itypes )         const;

  virtual idx_t             collectDofIndices

    ( const IdxVector&        idofs,
      const IdxVector&        iitems,
      const IdxVector&        itypes )         const;

  virtual idx_t             getDofsForItem

    ( const IdxVector&        idofs,
      const IdxVector&        itypes,
      idx_t                     iitem )          const;

  virtual idx_t             getDofsForType

    ( const IdxVector&        idofs,
      const IdxVector&        iitems,
      idx_t                   itype )          const;

  virtual void              decodeDofIndex

    ( idx_t&                  iitem,
      idx_t&                  itype,
      idx_t                   idof   )         const;

  virtual void              decodeDofIndices

    ( const IdxVector&        iitems,
      const IdxVector&        itypes,
      const IdxVector&        idofs  )         const;

  virtual SparseIdxMatrix   toMatrix        () const;
  virtual ItemSet*          getItems        () const;

  void                      setAllDofs

    ( const IdxVector&        iitems,
      const IdxVector&        itypes );

  void                      getAllDofs

    ( const IdxVector&        iitems,
      const IdxVector&        itypes )         const;


 protected:

  virtual                  ~DenseDofSpace   ();

  void                      emitEvents_

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

};


JIVE_END_PACKAGE( util )

#endif