
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

#ifndef JIVE_SOLVER_STDITERATIVESOLVER_H
#define JIVE_SOLVER_STDITERATIVESOLVER_H

#include <jive/solver/IterativeSolver.h>


JIVE_BEGIN_PACKAGE( solver )


class Constrainer;


//-----------------------------------------------------------------------
//   class StdIterativeSolver
//-----------------------------------------------------------------------


class StdIterativeSolver : public IterativeSolver
{
 public:

  JEM_DECLARE_CLASS       ( StdIterativeSolver, IterativeSolver );

  static const double       MAX_RESIDUAL;
  static const char*        UPDATE_POLICIES[3];

  enum                      UpdatePolicy
  {
                              AUTO_UPDATE,
                              NEVER_UPDATE,
                              ALWAYS_UPDATE
  };


                            StdIterativeSolver

    ( const String&           name,
      Ref<AbstractMatrix>     matrix,
      Ref<VectorSpace>        vspace,
      Ref<Constraints>        cons = NIL );

  virtual void              improve

    ( const Vector&           lhs,
      const Vector&           rhs );

  virtual void              getInfo

    ( const Properties&       info )               const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )              const;

  virtual void              setMode

    ( int                     mode );

  virtual int               getMode             () const;

  virtual void              setPrecision

    ( double                  eps );

  virtual double            getPrecision        () const;
  virtual AbstractMatrix*   getMatrix           () const;
  virtual Constraints*      getConstraints      () const;

  virtual void              setMaxIterCount

    ( idx_t                   count );

  virtual idx_t             getMaxIterCount     () const;

  void                      setUpdatePolicy

    ( UpdatePolicy            policy );

  UpdatePolicy              getUpdatePolicy     () const;

  void                      getResidual

    ( const Vector&           r,
      const Vector&           lhs,
      const Vector&           rhs );

  static bool               decodeParams

    ( Ref<AbstractMatrix>&    matrix,
      Ref<VectorSpace>&       vspace,
      Ref<Constraints>&       cons,
      const Properties&       params );


 protected:

  virtual                  ~StdIterativeSolver  ();

  virtual void              updatePrecon_       ()       = 0;

  virtual void              solve_

    ( idx_t&                  iiter,
      double&                 error,
      double                  rscale,
      const Vector&           lhs,
      const Vector&           rhs )                      = 0;

  void                      testCancelled_      ();


 protected:

  Ref<AbstractMatrix>       matrix_;
  Ref<VectorSpace>          vspace_;
  Ref<Constrainer>          conman_;

  int                       mode_;
  idx_t                     maxIter_;
  double                    precision_;


 private:

  void                      checkMatrix_        ();


 private:

  UpdatePolicy              policy_;
  idx_t                     iiter_;
  idx_t                     iiter0_;
  double                    error_;
  idx_t                     cancelCount_;

};


JIVE_END_PACKAGE( solver )

#endif
