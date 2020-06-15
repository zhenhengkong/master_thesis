
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

#ifndef JIVE_SOLVER_SKYLINELU_H
#define JIVE_SOLVER_SKYLINELU_H

#include <jive/solver/import.h>
#include <jive/solver/DirectSolver.h>


JIVE_BEGIN_PACKAGE( solver )


class Constrainer;
class SparseLU;
class SkylineSolver;


//-----------------------------------------------------------------------
//   class SkylineLU
//-----------------------------------------------------------------------


class SkylineLU : public DirectSolver
{
 public:

  JEM_DECLARE_CLASS       ( SkylineLU, DirectSolver );

  static const char*        TYPE_NAME;


                            SkylineLU

    ( const String&           name,
      Ref<AbstractMatrix>     matrix,
      Ref<Constraints>        cons );

  virtual void              clear             ();

  virtual void              improve

    ( const Vector&           lhs,
      const Vector&           rhs );

  virtual void              getInfo

    ( const Properties&       info )             const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       conf )             const;

  virtual void              setMode

    ( int                     mode );

  virtual int               getMode           () const;

  virtual void              setPrecision

    ( double                  eps );

  virtual double            getPrecision      () const;
  virtual AbstractMatrix*   getMatrix         () const;
  virtual Constraints*      getConstraints    () const;

  virtual void              getNullSpace

    ( Matrix&                 nspace );

  virtual void              setZeroThreshold

    ( double                  eps );

  virtual double            getZeroThreshold  () const;

  virtual void              setMaxZeroPivots

    ( idx_t                   maxPivots );

  virtual idx_t             getMaxZeroPivots  () const;

  static Ref<Solver>        makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       params,
      const Properties&       globdat );

  static void               declare           ();


 protected:

  virtual                  ~SkylineLU         ();


 private:

  void                      connect_          ();
  void                      toSparseLU_       ();

  void                      solveEvent_

    ( double                  err );

  void                      pivotEvent_

    ( double                  piv );

  void                      factorEvent_

    ( idx_t                   progress );


 private:

  Ref<DirectSolver>         solver_;
  Ref<SkylineSolver>        skyline_;
  Ref<SparseLU>             sparseLU_;

};


JIVE_END_PACKAGE( solver )

#endif