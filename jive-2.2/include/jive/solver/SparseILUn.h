
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

#ifndef JIVE_SOLVER_SPARSEILUN_H
#define JIVE_SOLVER_SPARSEILUN_H

#include <jive/solver/SparseIFactor.h>


JIVE_BEGIN_PACKAGE( solver )


//-----------------------------------------------------------------------
//   class SparseILUn
//-----------------------------------------------------------------------


class SparseILUn : public SparseIFactor
{
 public:

  JEM_DECLARE_CLASS       ( SparseILUn, SparseIFactor );

  static const char*        TYPE_NAME;


                            SparseILUn

    ( const String&           name,
      Ref<AbstractMatrix>     matrix,
      Ref<Constraints>        cons = NIL );

  virtual void              getInfo

    ( const Properties&       info )             const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )            const;

  void                      setMaxFill

    ( idx_t                   mfill );

  inline idx_t              getMaxFill        () const;

  static Ref<Precon>        makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       params,
      const Properties&       globdat );

  static void               declare           ();


 protected:

  virtual                  ~SparseILUn        ();

  virtual void              factor_

    ( const SparseMatrix&     matrix,
      const BoolVector&       mask,
      const IdxVector&        iperm );

  virtual void              solve_

    ( const Vector&           lhs,
      const Vector&           rhs  )             const;


 private:

  class                     Solver_;

  idx_t                     maxFill_;
  Solver_*                  solver_;

};



//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getMaxFill
//-----------------------------------------------------------------------


inline idx_t SparseILUn::getMaxFill () const
{
  return maxFill_;
}


JIVE_END_PACKAGE( solver )

#endif
