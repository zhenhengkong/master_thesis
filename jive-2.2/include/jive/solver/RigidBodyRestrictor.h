
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

#ifndef JIVE_SOLVER_RIGIDBODYRESTRICTOR_H
#define JIVE_SOLVER_RIGIDBODYRESTRICTOR_H

#include <jive/solver/SparseRestrictor.h>


JIVE_BEGIN_PACKAGE( solver )


//-----------------------------------------------------------------------
//   class RigidBodyRestrictor
//-----------------------------------------------------------------------


class RigidBodyRestrictor : public SparseRestrictor
{
 public:

  JEM_DECLARE_CLASS       ( RigidBodyRestrictor, SparseRestrictor );

  static const char*        TYPE_NAME;
  static const idx_t        MIN_NODES;


                            RigidBodyRestrictor

    ( const String&           name,
      const Ref<DofSpace>&    dofs,
      const Properties&       globdat,
      Ref<AbstractMatrix>     matrix = NIL,
      Ref<Constraints>        cons   = NIL );

  virtual void              resetEvents           ();

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )                const;

  static Ref<Restrictor>    makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       params,
      const Properties&       globdat );

  static void               declare               ();


 protected:

  virtual                  ~RigidBodyRestrictor   ();


 private:

  void                      connect_              ();
  virtual void              update_               ();
  void                      invalidate_           ();

  static void               calcRotmat_

    ( const Matrix&           rotmat,
      const Vector&           xcoords,
      const Vector&           ycoords,
      const IdxVector&        iitems );


 private:

  Ref<DofSpace>             dofs_;
  Ref<AbstractMatrix>       matrix_;
  Ref<Constraints>          cons_;

  idx_t                     minNodes_;
  int                       procCount_;
  IdxVector                 dofTypes_;

};


JIVE_END_PACKAGE( solver )

#endif
