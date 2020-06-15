
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

#ifndef JIVE_SCHURSOLVER_H
#define JIVE_SCHURSOLVER_H

#include <jive/solver/Solver.h>


JIVE_BEGIN_PACKAGE( solver )


class SchurMatrix;


//-----------------------------------------------------------------------
//   class SchurSolver
//-----------------------------------------------------------------------


class SchurSolver : public Solver
{
 public:

  JEM_DECLARE_CLASS       ( SchurSolver, Solver );

  static const char*        TYPE_NAME;


  explicit                  SchurSolver

    ( const String&           name,
      Ref<Solver>             solver,
      Ref<SchurMatrix>        matrix,
      Ref<VectorSpace>        vspace,
      Ref<VectorExchanger>    vex );

  virtual void              improve

   ( const Vector&            lhs,
     const Vector&            rhs );

  virtual void              getInfo

    ( const Properties&       info )           const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       conf )           const;

  virtual void              setMode

    ( int                     mode );

  virtual int               getMode         () const;

  virtual void              setPrecision

    ( double                  eps );

  virtual double            getPrecision    () const;
  virtual AbstractMatrix*   getMatrix       () const;
  virtual Constraints*      getConstraints  () const;

  static Ref<Solver>        makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       params,
      const Properties&       globdat );

  static void               declare         ();


 protected:

  virtual                  ~SchurSolver     ();


 private:

  Ref<Solver>               solver_;
  Ref<SchurMatrix>          matrix_;
  Ref<VectorSpace>          vspace_;
  Ref<VectorExchanger>      exchanger_;

  int                       myMode_;

};


JIVE_END_PACKAGE( solver )

#endif
