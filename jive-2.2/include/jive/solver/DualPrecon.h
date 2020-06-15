
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

#ifndef JIVE_SOLVER_DUALPRECON_H
#define JIVE_SOLVER_DUALPRECON_H

#include <jive/solver/Preconditioner.h>


JIVE_BEGIN_PACKAGE( solver )


//-----------------------------------------------------------------------
//   class DualPrecon
//-----------------------------------------------------------------------


class DualPrecon : public Preconditioner
{
 public:

  JEM_DECLARE_CLASS       ( DualPrecon, Preconditioner );

  static const char*        TYPE_NAME;

  enum                      Operation
  {
                              ADD,
                              MULTIPLY,
                              MULTIPLY2
  };

  static const char*        OPERATIONS[3];


                            DualPrecon

    ( const String&           name,
      Ref<AbstractMatrix>     matrix,
      Ref<AbstractMatrix>     precon0,
      Ref<AbstractMatrix>     precon1,
      Ref<Constraints>        cons = NIL );

  virtual void              resetEvents     ();
  virtual Shape             shape           () const;
  virtual void              update          ();

  virtual void              matmul

    ( const Vector&           lhs,
      const Vector&           rhs )            const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )          const;

  virtual bool              hasTrait

    ( const String&           trait )          const;

  virtual Constraints*      getConstraints  () const;
  Operation                 getOperation    () const;

  void                      setOperation

    ( Operation               operation );

  static Ref<Precon>        makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       params,
      const Properties&       globdat );

  static void               declare         ();


 protected:

  virtual                  ~DualPrecon      ();


 private:

  void                      connect_        ();

  void                      valuesChanged_  ();
  void                      structChanged_  ();


 private:

  Ref<AbstractMatrix>       matrix_;
  Ref<AbstractMatrix>       precon_[2];

  Operation                 operation_;

};


JIVE_END_PACKAGE( solver )

#endif