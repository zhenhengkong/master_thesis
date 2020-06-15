
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

#ifndef JIVE_SOLVER_SPARSEILUD_H
#define JIVE_SOLVER_SPARSEILUD_H

#include <jive/solver/SparseIFactor.h>


JIVE_BEGIN_PACKAGE( solver )


//-----------------------------------------------------------------------
//   class SparseILUd
//-----------------------------------------------------------------------


class SparseILUd : public SparseIFactor
{
 public:

  JEM_DECLARE_CLASS       ( SparseILUd, SparseIFactor );

  static const char*        TYPE_NAME;


                            SparseILUd

    ( const String&           name,
      Ref<AbstractMatrix>     matrix,
      Ref<Constraints>        cons = NIL );

  virtual void              getInfo

    ( const Properties&       info )             const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )            const;

  void                      setMinSize

    ( idx_t                   msize );

  inline idx_t              getMinSize        () const;

  void                      setMaxFill

    ( double                  mfill );

  inline double             getMaxFill        () const;

  void                      setQuality

    ( double                  qlty );

  inline double             getQuality        () const;

  static Ref<Precon>        makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       params,
      const Properties&       globdat );

  static void               declare           ();


 protected:

  virtual                  ~SparseILUd        ();

  virtual void              factor_

    ( const SparseMatrix&     matrix,
      const BoolVector&       mask,
      const IdxVector&        iperm );

  virtual void              solve_

    ( const Vector&           lhs,
      const Vector&           rhs  )             const;


 private:

  class                     Solver_;

  idx_t                     minSize_;
  double                    maxFill_;
  double                    quality_;

  Solver_*                  solver_;

};



//#######################################################################
//  Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getMinSize
//-----------------------------------------------------------------------


inline idx_t SparseILUd::getMinSize () const
{
  return minSize_;
}


//-----------------------------------------------------------------------
//   getMaxFill
//-----------------------------------------------------------------------


inline double SparseILUd::getMaxFill () const
{
  return maxFill_;
}


//-----------------------------------------------------------------------
//   getQuality
//-----------------------------------------------------------------------


inline double SparseILUd::getQuality () const
{
  return quality_;
}


JIVE_END_PACKAGE( solver )

#endif
