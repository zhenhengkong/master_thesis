
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

#ifndef JIVE_SOLVER_DENSERESTRICTOR_H
#define JIVE_SOLVER_DENSERESTRICTOR_H

#include <jive/algebra/SparseMatrixExtension.h>
#include <jive/solver/Restrictor.h>
#include <jive/solver/LocalRestrictor.h>


JIVE_BEGIN_PACKAGE( solver )


//-----------------------------------------------------------------------
//   class DenseRestrictor
//-----------------------------------------------------------------------


class DenseRestrictor : public Restrictor,
                        public SparseMatrixExt,
                        public LocalRestrictor
{
 public:

  JEM_DECLARE_CLASS       ( DenseRestrictor, Restrictor );


  virtual Shape             shape               () const;
  void                      update              ();

  virtual void              matmul

    ( const Vector&           lhs,
      const Vector&           rhs )                const;

  virtual void              transmul

    ( const Vector&           lhs,
      const Vector&           rhs )                const;

  virtual void              getBasis

    ( const Vector&           v,
      idx_t                   j )                  const;

  virtual void*             getExtByID

    ( ExtensionID             extID )              const;

  virtual SparseStruct      getStructure        () const;
  virtual SparseMatrix      toSparseMatrix      () const;
  virtual SparseMatrix      cloneSparseMatrix   () const;


 protected:

                            DenseRestrictor

    ( const String&           name = "" );

  virtual                  ~DenseRestrictor     ();

  virtual void              update_             () = 0;


 protected:

  Matrix                    basis_;
  int                       events_;

};


JIVE_END_PACKAGE( solver )

#endif
