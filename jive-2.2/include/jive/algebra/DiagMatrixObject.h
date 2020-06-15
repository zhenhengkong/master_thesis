
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

#ifndef JIVE_ALGEBRA_DIAGMATRIXOBJECT_H
#define JIVE_ALGEBRA_DIAGMATRIXOBJECT_H

#include <jem/base/Clonable.h>
#include <jem/io/Serializable.h>
#include <jive/algebra/DiagMatrixExtension.h>
#include <jive/algebra/MultiMatmulExtension.h>
#include <jive/algebra/DirectMatrixExtension.h>
#include <jive/algebra/SparseMatrixExtension.h>
#include <jive/algebra/AbstractMatrix.h>


JIVE_BEGIN_PACKAGE( algebra )


//-----------------------------------------------------------------------
//   class DiagMatrixObject
//-----------------------------------------------------------------------


class DiagMatrixObject : public AbstractMatrix,
                         public DiagMatrixExt,
                         public MultiMatmulExt,
                         public DirectMatrixExt,
                         public SparseMatrixExt,
                         public Clonable,
                         public Serializable
{
 public:

  JEM_DECLARE_CLASS       ( DiagMatrixObject, AbstractMatrix );


  explicit                  DiagMatrixObject

    ( const String&           name = "" );

                            DiagMatrixObject

    ( const String&           name,
      const Vector&           diag );

  virtual void              readFrom

    ( ObjectInput&            in  );

  virtual void              writeTo

    ( ObjectOutput&           out )                const;

  virtual Ref<Object>       clone               () const;
  virtual Shape             shape               () const;
  inline idx_t              size                () const;

  virtual void              matmul

    ( const Vector&           lhs,
      const Vector&           rhs )                const;

  virtual void              multiMatmul

    ( Matrix&                 lhsVecs,
      IdxVector&              lhsTags,
      const Matrix&           rhsVecs,
      const IdxVector&        rhsTags )            const;

  virtual bool              hasTrait

    ( const String&           trait )              const;

  virtual void*             getExtByID

    ( ExtensionID             extID )              const;

  virtual double            getValue

    ( idx_t                   irow,
      idx_t                   jcol )               const;

  virtual void              getBlock

    ( const Matrix&           block,
      const IdxVector&        irows,
      const IdxVector&        jcols )              const;

  virtual void              getDiagonal

    ( const Vector&           diag )               const;

  virtual void              getRowScales

    ( const Vector&           rscales )            const;

  virtual SparseStruct      getStructure        () const;
  virtual SparseMatrix      toSparseMatrix      () const;
  virtual SparseMatrix      cloneSparseMatrix   () const;

  virtual void              setValues

    ( const Vector&           diag );

  virtual void              handleNewValues     ();

  virtual void              printTo

    ( PrintWriter&            out )                const;

  inline const Vector&      getValues           () const;


 protected:

  virtual                  ~DiagMatrixObject    ();


 protected:

  Vector                    values_;

};


//-----------------------------------------------------------------------
//   related types
//-----------------------------------------------------------------------


typedef DiagMatrixObject    DiagMatrixObj;





//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   size
//-----------------------------------------------------------------------


inline idx_t DiagMatrixObject::size () const
{
  return values_.size ();
}


//-----------------------------------------------------------------------
//   getValues
//-----------------------------------------------------------------------


inline const Vector& DiagMatrixObject::getValues () const
{
  return values_;
}


JIVE_END_PACKAGE( algebra )

#endif
