
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

#ifndef JEM_ALGEBRA_MPMATRIXOBJECT_H
#define JEM_ALGEBRA_MPMATRIXOBJECT_H

#include <jem/base/Clonable.h>
#include <jem/io/Serializable.h>
#include <jive/algebra/AbstractMatrix.h>
#include <jive/algebra/MPMatrixExtension.h>
#include <jive/algebra/DiagMatrixExtension.h>
#include <jive/algebra/MultiMatmulExtension.h>


JIVE_BEGIN_PACKAGE( algebra )


//-----------------------------------------------------------------------
//   class MPMatrix
//-----------------------------------------------------------------------


class MPMatrixObject : public  AbstractMatrix,
                       public  MPMatrixExt,
                       public  MultiMatmulExt,
                       private DiagMatrixExt,
                       public  Clonable,
                       public  Serializable
{
 public:

  JEM_DECLARE_CLASS       ( MPMatrixObject, AbstractMatrix );


                            MPMatrixObject    ();

                            MPMatrixObject

    ( Ref<AbstractMatrix>     inner,
      Ref<VectorExchanger>    vex );

                            MPMatrixObject

    ( const Self&             rhs );

  virtual void              resetEvents       ();

  virtual void              readFrom

    ( ObjectInput&            in  );

  virtual void              writeTo

    ( ObjectOutput&           out )              const;

  virtual Ref<Object>       clone             () const;
  virtual Shape             shape             () const;

  virtual void              matmul

    ( const Vector&           lhs,
      const Vector&           rhs )              const;

  virtual void              multiMatmul

    ( Matrix&                 lhsVecs,
      IdxVector&              lhsTags,
      const Matrix&           rhsVecs,
      const IdxVector&        rhsTags )          const;

  virtual bool              hasTrait

    ( const String&           trait )            const;

  virtual void              printTo

    ( PrintWriter&            out )              const;

  virtual void*             getExtByID

    ( ExtensionID             extID )            const;

  virtual VectorExchanger*  getExchanger      () const;
  virtual int               getExchangeMode   () const;

  void                      setExchangeMode

    ( int                     mode );

  inline AbstractMatrix*    getInner          () const;


 protected:

  virtual                  ~MPMatrixObject    ();


 private:

  virtual void              getDiagonal

    ( const Vector&           diag )             const;

  virtual void              getRowScales

    ( const Vector&           rscales )          const;


 private:

  void                      connect_          ();
  void                      valuesChanged_    ();
  void                      structChanged_    ();


 private:

  Ref<AbstractMatrix>       inner_;
  Ref<VectorExchanger>      exchanger_;
  int                       xmode_;

};


//-----------------------------------------------------------------------
//   related types
//-----------------------------------------------------------------------


typedef MPMatrixObject      MPMatrixObj;




//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getInner
//-----------------------------------------------------------------------


inline AbstractMatrix* MPMatrixObject::getInner () const
{
  return inner_.get ();
}


JIVE_END_PACKAGE( algebra )

#endif
