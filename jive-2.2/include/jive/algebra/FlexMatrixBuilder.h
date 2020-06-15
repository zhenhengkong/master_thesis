
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

#ifndef JIVE_ALGEBRA_FLEXMATRIXBUILDER_H
#define JIVE_ALGEBRA_FLEXMATRIXBUILDER_H

#include <jive/algebra/typedefs.h>
#include <jive/algebra/MatrixBuilder.h>


JIVE_BEGIN_PACKAGE( algebra )


//-----------------------------------------------------------------------
//   class FlexMatrixBuilder
//-----------------------------------------------------------------------


class FlexMatrixBuilder : public MatrixBuilder
{
 public:

  JEM_DECLARE_CLASS       ( FlexMatrixBuilder, MatrixBuilder );

  static const char*        TYPE_NAME;

  static const int          AUTO_SHAPE0;
  static const int          AUTO_SHAPE1;
  static const int          AUTO_RESHAPE;
  static const int          DROP_ZEROES;


  explicit                  FlexMatrixBuilder

    ( const String&           name = "",
      Ref<SparseMatrixObj>    mat  = NIL );

  virtual void              clear             ();

  virtual void              scale

    ( double                  factor );

  virtual void              setToZero         ();

  virtual void              reserve

    ( idx_t                   nzCount );

  virtual void              shapeHint

    ( idx_t                   rowCount,
      idx_t                   colCount );

  virtual void              trimToSize        ();
  virtual void              updateMatrix      ();

  virtual void              setMultiplier

    ( double                  x );

  virtual double            getMultiplier     () const;

  virtual void              setData

    ( const idx_t*            irows,
      idx_t                   icount,
      const idx_t*            jcols,
      idx_t                   jcount,
      const double*           values );

  virtual void              addData

    ( const idx_t*            irows,
      idx_t                   icount,
      const idx_t*            jcols,
      idx_t                   jcount,
      const double*           values );

  virtual idx_t             eraseData

    ( const idx_t*            irows,
      idx_t                   icount,
      const idx_t*            jcols,
      idx_t                   jcount );

  virtual void              getData

    ( double*                 buf,
      const idx_t*            irows,
      idx_t                   icount,
      const idx_t*            jcols,
      idx_t                   jcount )           const;

  virtual AbstractMatrix*   getMatrix         () const;
  SparseMatrixObj*          getSparseMatrix   () const;

  void                      setShape

    ( idx_t                   rowCount,
      idx_t                   colCount );

  int                       setOptions

    ( int                     options );

  int                       getOptions        () const;

  inline void               trackDofSpace

    ( const Ref<DofSpace>&    dofs );

  void                      trackDofSpaces

    ( const Ref<DofSpace>&    rowSpace,
      const Ref<DofSpace>&    colSpace );

  static Ref<MBuilder>      makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       params,
      const Properties&       globdat );

  static void               declare           ();


 protected:

  virtual                  ~FlexMatrixBuilder ();


 private:

  void                      newDofCount_      ();


 private:

  class                     Data_;

  Ref<Data_>                data_;

};


//-----------------------------------------------------------------------
//   related types
//-----------------------------------------------------------------------


typedef FlexMatrixBuilder   FlexMBuilder;


//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   trackDofSpace
//-----------------------------------------------------------------------


inline void FlexMatrixBuilder::trackDofSpace

  ( const Ref<DofSpace>&  dofs )

{
  trackDofSpaces ( dofs, dofs );
}


JIVE_END_PACKAGE( algebra )

#endif
