
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

#ifndef JIVE_MODEL_IMPLICITMATRIX_H
#define JIVE_MODEL_IMPLICITMATRIX_H

#include <jem/util/Properties.h>
#include <jive/algebra/AbstractMatrix.h>
#include <jive/model/import.h>


JIVE_BEGIN_PACKAGE( model )


class Model;


//-----------------------------------------------------------------------
//   class ImplicitMatrix
//-----------------------------------------------------------------------


class ImplicitMatrix : public AbstractMatrix
{
 public:

  JEM_DECLARE_CLASS       ( ImplicitMatrix, AbstractMatrix );

  static const int          SYMMETRIC;


                            ImplicitMatrix

    ( const String&           name,
      const Ref<Model>&       model,
      const Ref<DofSpace>&    dofs,
      const Properties&       globdat,
      int                     traits = 0 );

  virtual Shape             shape           () const;

  virtual void              matmul

    ( const Vector&           lhs,
      const Vector&           rhs )            const;

  virtual bool              hasTrait

    ( const String&           trait )          const;

  bool                      takeAction

    ( const String&           action,
      const Properties&       params,
      const Properties&       globdat );

  void                      setEpsilon

    ( double                  eps );

  double                    getEpsilon      () const;


 protected:

  virtual                  ~ImplicitMatrix  ();


 private:

  void                      update_         ();
  void                      invalidate_     ();
  void                      sizeChanged_    ();


 private:

  int                       traits_;

  Ref<Model>                model_;
  Ref<DofSpace>             dofs_;
  Ref<VectorSpace>          vspace_;

  Properties                globdat_;
  Properties                params_;

  Vector                    fint_;
  Vector                    fint0_;
  Vector                    state0_;

  double                    eps_;
  double                    scale_;
  bool                      updated_;

};


JIVE_END_PACKAGE( model )

#endif
