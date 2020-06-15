
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

#ifndef JIVE_SOLVER_STDCONSTRAINER_H
#define JIVE_SOLVER_STDCONSTRAINER_H

#include <jive/solver/import.h>
#include <jive/solver/Constrainer.h>


JIVE_BEGIN_PACKAGE( solver )


//-----------------------------------------------------------------------
//   class StdConstrainer
//-----------------------------------------------------------------------


class StdConstrainer : public Constrainer
{
 public:

  JEM_DECLARE_CLASS       ( StdConstrainer, Constrainer );

  static const char*        TYPE_NAME;


                            StdConstrainer

    ( const String&           name,
      Ref<Constraints>        cons,
      Ref<AbstractMatrix>     input,
      Ref<SparseMatrixObj>    output = NIL );

  virtual void              update          ();

  virtual bool              updated         () const;

  virtual void              getRhs

    ( const Vector&           g,
      const Vector&           f )              const;

  virtual void              getLhs

    ( const Vector&           x,
      const Vector&           y )              const;

  virtual void              initLhs

    ( const Vector&           x,
      const Vector&           y )              const;

  virtual Constraints*      getConstraints  () const;
  virtual AbstractMatrix*   getInputMatrix  () const;
  virtual AbstractMatrix*   getOutputMatrix () const;

  static Ref<Constrainer>   makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      Ref<Constraints>        cons,
      Ref<AbstractMatrix>     matrix );

  static void               declare         ();


 protected:

  virtual                  ~StdConstrainer  ();


 private:

  void                      updateRhs_      ();
  void                      updateValues_   ();
  void                      updateStruct_   ();


 private:

  class                     Data_;
  class                     Utils_;

  Ref<Data_>                data_;

};


JIVE_END_PACKAGE( solver )

#endif
