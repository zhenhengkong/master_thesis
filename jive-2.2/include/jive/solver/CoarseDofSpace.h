
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

#ifndef JIVE_SOLVER_COARSEDOFSPACE_H
#define JIVE_SOLVER_COARSEDOFSPACE_H

#include <jive/util/DofSpace.h>
#include <jive/solver/import.h>
#include <jive/solver/typedefs.h>


JIVE_BEGIN_PACKAGE( solver )


class Restrictor;


//-----------------------------------------------------------------------
//   class CoarseDofSpace
//-----------------------------------------------------------------------


class CoarseDofSpace : public DofSpace
{
 public:

  JEM_DECLARE_CLASS       ( CoarseDofSpace, DofSpace );

  static const char*        DOF_TYPE_NAME;


  explicit                  CoarseDofSpace

    ( Ref<Restrictor>         rt );

  inline void               update            ();
  inline bool               updated           () const;

  virtual idx_t             typeCount         () const;
  virtual idx_t             dofCount          () const;
  idx_t                     globalDofCount    () const;

  virtual void              resetEvents       ();

  virtual idx_t             findType

    ( const String&           name )             const;

  virtual String            getTypeName

    ( idx_t                   itype )            const;

  virtual StringVector      getTypeNames      () const;

  virtual idx_t             findDofIndex

    ( idx_t                   iitem,
      idx_t                   itype )            const;

  virtual idx_t             findDofIndices

    ( const IdxVector&        idofs,
      const IdxVector&        iitems,
      const IdxVector&        itypes )           const;

  virtual idx_t             collectDofIndices

    ( const IdxVector&        idofs,
      const IdxVector&        iitems,
      const IdxVector&        itypes )           const;

  virtual idx_t             getDofsForItem

    ( const IdxVector&        idofs,
      const IdxVector&        itypes,
      idx_t                   iitem )            const;

  virtual idx_t             getDofsForType

    ( const IdxVector&        idofs,
      const IdxVector&        iitems,
      idx_t                   itype )            const;

  virtual void              decodeDofIndex

    ( idx_t&                  iitem,
      idx_t&                  itype,
      idx_t                   idof   )           const;

  virtual void              decodeDofIndices

    ( const IdxVector&        iitems,
      const IdxVector&        itypes,
      const IdxVector&        idofs  )           const;

  virtual SparseIdxMatrix   toMatrix          () const;
  virtual ItemSet*          getItems          () const;

  MPContext*                getMPContext      () const;
  inline bool               isDistributed     () const;
  inline Restrictor*        getRestrictor     () const;
  inline idx_t              getMyFirstDof     () const;


 protected:

  virtual                  ~CoarseDofSpace    ();

  virtual void              emitEvents_

    ( int                     events );


 private:

  void                      connect_          ();
  void                      update_           ();
  void                      invalidate_       ();


 private:

  static const int          UPDATED_;
  static const int          DISTRIBUTED_;

  Ref<Restrictor>           rtor_;
  Ref<ItemSet>              items_;

  int                       status_;
  idx_t                     firstDof_;

};




//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------


inline void CoarseDofSpace::update ()
{
  if ( ! (status_ & UPDATED_) )
  {
    update_ ();
  }
}


//-----------------------------------------------------------------------
//   updated
//-----------------------------------------------------------------------


inline bool CoarseDofSpace::updated () const
{
  return ((status_ & UPDATED_) != 0);
}


//-----------------------------------------------------------------------
//   isDistributed
//-----------------------------------------------------------------------


inline bool CoarseDofSpace::isDistributed () const
{
  return ((status_ & DISTRIBUTED_) != 0);
}


//-----------------------------------------------------------------------
//   getRestrictor
//-----------------------------------------------------------------------


inline Restrictor* CoarseDofSpace::getRestrictor () const
{
  return rtor_.get ();
}


//-----------------------------------------------------------------------
//   getMyFirstDof
//-----------------------------------------------------------------------


inline idx_t CoarseDofSpace::getMyFirstDof () const
{
  return firstDof_;
}


JIVE_END_PACKAGE( solver )

#endif
