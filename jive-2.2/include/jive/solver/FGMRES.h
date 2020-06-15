
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

#ifndef JIVE_SOLVER_FGMRES_H
#define JIVE_SOLVER_FGMRES_H

#include <jive/solver/StdIterativeSolver.h>


JIVE_BEGIN_PACKAGE( solver )


//-----------------------------------------------------------------------
//   class FGMRES
//-----------------------------------------------------------------------


class FGMRES : public StdIterativeSolver
{
 public:

  JEM_DECLARE_CLASS       ( FGMRES, StdIterativeSolver );

  static const char*        TYPE_NAME;
  static const idx_t        RESTART_ITER;


  explicit                  FGMRES

    ( const String&           name,
      Ref<AbstractMatrix>     matrix,
      Ref<VectorSpace>        vspace,
      Ref<Constraints>        cons   = NIL,
      Ref<AbstractMatrix>     precon = NIL,
      Ref<GramSchmidt>        grams  = NIL );

  virtual void              getInfo

    ( const Properties&       info )               const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )              const;

  void                      setRestartIterCount

    ( idx_t                   count );

  idx_t                     getRestartIterCount () const;

  static Ref<Solver>        makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       params,
      const Properties&       globdat );

  static void               declare             ();


 protected:

  virtual                  ~FGMRES              ();

  virtual void              updatePrecon_       ();

  virtual void              solve_

    ( idx_t&                  iiter,
      double&                 error,
      double                  rscale,
      const Vector&           lhs,
      const Vector&           rhs );


 private:

  void                      reset_              ();

  void                      store_

    ( const Vector&           v,
      const Vector&           z,
      idx_t                   i );


 private:

  Ref<GramSchmidt>          grams_;
  Ref<AbstractMatrix>       precon_;

  idx_t                     restartIter_;

  Matrix                    vVectors_;
  Matrix                    zVectors_;

};


JIVE_END_PACKAGE( solver )

#endif
