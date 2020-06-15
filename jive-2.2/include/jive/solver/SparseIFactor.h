
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

#ifndef JIVE_SOLVER_SPARSEIFACTOR_H
#define JIVE_SOLVER_SPARSEIFACTOR_H

#include <jive/SparseMatrix.h>
#include <jive/solver/import.h>
#include <jive/solver/Preconditioner.h>


JIVE_BEGIN_PACKAGE( solver )


//-----------------------------------------------------------------------
//   class SparseIFactor
//-----------------------------------------------------------------------


class SparseIFactor : public Preconditioner
{
 public:

  JEM_DECLARE_CLASS       ( SparseIFactor, Preconditioner );

  typedef VectorExchanger   Exchanger;

  static const int          REORDER;
  static const int          LAST_OPTION;
  static const double       ZERO_THRESHOLD;


                            SparseIFactor

    ( const String&           name,
      Ref<AbstractMatrix>     matrix,
      Ref<Constraints>        cons = NIL );

  virtual void              resetEvents       ();
  virtual Shape             shape             () const;
  virtual void              update            ();

  virtual void              matmul

    ( const Vector&           lhs,
      const Vector&           rhs )              const;

  virtual void              configure

    ( const Properties&       props );

  virtual void              getConfig

    ( const Properties&       props )            const;

  virtual bool              hasTrait

    ( const String&           trait )            const;

  virtual Constraints*      getConstraints    () const;

  void                      setOptions

    ( int                     options );

  inline int                getOptions        () const;

  void                      setDropTol

    ( double                  tol );

  inline double             getDropTol        () const;

  void                      setDiagShift

    ( double                  dshift );

  inline double             getDiagShift      () const;

  void                      setZeroThreshold

    ( double                  eps );

  inline double             getZeroThreshold  () const;

  void                      setExchangeMode

    ( int                     xmode );

  inline int                getExchangeMode   () const;

  static bool               decodeParams

    ( Ref<AbstractMatrix>&    matrx,
      Ref<Constraints>&       cons,
      const Properties&       params );


 protected:

  virtual                  ~SparseIFactor     ();

  virtual void              factor_

    ( const SparseMatrix&     matrix,
      const BoolVector&       mask,
      const IdxVector&        iperm )                  = 0;

  virtual void              solve_

    ( const Vector&           lhs,
      const Vector&           rhs  )             const = 0;

  void                      setParam_

    ( int&                    param,
      int                     value );

  void                      setParam_

    ( lint&                   param,
      lint                    value );

  void                      setParam_

    ( double&                 param,
      double                  value );


 protected:

  static const int          NEW_VALUES_;
  static const int          NEW_STRUCT_;
  static const int          NEW_CONFIG_;

  int                       options_;
  idx_t                     maxFill_;
  double                    droptol_;
  double                    dshift_;
  double                    zeroThreshold_;
  bool                      symmetric_;
  int                       events_;


 private:

  void                      connect_          ();
  void                      update_           ();
  void                      valuesChanged_    ();
  void                      structChanged_    ();


 private:

  Ref<AbstractMatrix>       matrix_;
  Ref<Constraints>          cons_;
  Ref<Exchanger>            exchanger_;

  IdxVector                 dofPerm_;
  IdxVector                 borderDofs_;
  Vector                    borderDiag_;

  int                       xmode_;

};



//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getOptions
//-----------------------------------------------------------------------


inline int SparseIFactor::getOptions () const
{
  return options_;
}


//-----------------------------------------------------------------------
//   getDropTol
//-----------------------------------------------------------------------


inline double SparseIFactor::getDropTol () const
{
  return droptol_;
}


//-----------------------------------------------------------------------
//   getDiagShift
//-----------------------------------------------------------------------


inline double SparseIFactor::getDiagShift () const
{
  return dshift_;
}


//-----------------------------------------------------------------------
//   getExchangeMode
//-----------------------------------------------------------------------


inline int SparseIFactor::getExchangeMode () const
{
  return xmode_;
}


//-----------------------------------------------------------------------
//   getZeroThreshold
//-----------------------------------------------------------------------


inline double SparseIFactor::getZeroThreshold () const
{
  return zeroThreshold_;
}


JIVE_END_PACKAGE( solver )

#endif
