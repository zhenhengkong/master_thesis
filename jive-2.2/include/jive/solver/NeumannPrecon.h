
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

#ifndef JIVE_SOLVER_NEUMANNPRECON_H
#define JIVE_SOLVER_NEUMANNPRECON_H

#include <jive/solver/Preconditioner.h>


JIVE_BEGIN_PACKAGE( solver )


class SchurMatrix;


//-----------------------------------------------------------------------
//   class NeumannPrecon
//-----------------------------------------------------------------------


class NeumannPrecon : public Preconditioner
{
 public:

  JEM_DECLARE_CLASS       ( NeumannPrecon, Preconditioner );

  static const char*        TYPE_NAME;


                            NeumannPrecon

    ( const String&           name,
      Ref<Preconditioner>     inner,
      Ref<VectorExchanger>    vex  = NIL,
      Ref<SchurMatrix>        smat = NIL );

  virtual void              resetEvents       ();

  virtual Shape             shape             () const;
  virtual void              update            ();

  virtual void              matmul

    ( const Vector&           lhs,
      const Vector&           rhs )              const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       conf )             const;

  virtual bool              hasTrait

    ( const String&           trait )            const;

  virtual Constraints*      getConstraints    () const;

  static Ref<Precon>        makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       params,
      const Properties&       globdat );

  static void               declare           ();


 protected:

  virtual                  ~NeumannPrecon     ();


 private:

  void                      connect_          ();
  void                      update_           ();
  void                      invalidate_       ();
  void                      valuesChanged_    ();
  void                      structChanged_    ();


 private:

  static const int          NEW_VALUES_;
  static const int          NEW_STRUCT_;

  Ref<Preconditioner>       inner_;
  Ref<VectorExchanger>      exchanger_;
  Ref<SchurMatrix>          schurMat_;

  Vector                    weights_;
  Vector                    totalRhs_;
  Vector                    totalLhs_;

  int                       events_;

};


JIVE_END_PACKAGE( solver )

#endif
