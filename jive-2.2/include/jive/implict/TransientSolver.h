
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

#ifndef JIVE_IMPLICT_TRANSIENTSOLVER_H
#define JIVE_IMPLICT_TRANSIENTSOLVER_H

#include <jive/implict/import.h>
#include <jive/implict/SolverModule.h>


JIVE_BEGIN_PACKAGE( implict )


//-----------------------------------------------------------------------
//   class TransientSolver
//-----------------------------------------------------------------------


class TransientSolver : public SolverModule
{
 public:

  JEM_DECLARE_CLASS       ( TransientSolver, SolverModule );


  virtual Status            init

    ( const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );

  virtual Status            run

    ( const Properties&       globdat );

  virtual void              shutdown

    ( const Properties&       globdat );

  virtual void              configure

    ( const Properties&       props,
      const Properties&       globdat );

  virtual void              getConfig

    ( const Properties&       props,
      const Properties&       globdat )          const;

  virtual bool              takeAction

    ( const String&           action,
      const Properties&       params,
      const Properties&       globdat );

  virtual void              advance

    ( const Properties&       globdat );

  virtual void              solve

    ( const Properties&       info,
      const Properties&       globdat );

  virtual void              cancel

    ( const Properties&       globdat );

  virtual bool              commit

    ( const Properties&       globdat );

  virtual void              setPrecision

    ( double                  eps );

  virtual double            getPrecision      () const;


 protected:

                            TransientSolver

    ( const String&           name,
      Ref<SolverModule>       solver );

  virtual                  ~TransientSolver   ();

  virtual Ref<Model>        newTransModel_

    ( const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat )                = 0;


 protected:

  Ref<SolverModule>         solver_;
  Ref<Model>                tmodel_;

};


JIVE_END_PACKAGE( implict )

#endif
