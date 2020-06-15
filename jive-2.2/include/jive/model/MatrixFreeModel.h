
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

#ifndef JIVE_MODEL_MATRIXFREEMODEL_H
#define JIVE_MODEL_MATRIXFREEMODEL_H

#include <jem/io/Serializable.h>
#include <jive/model/import.h>
#include <jive/model/Model.h>


JIVE_BEGIN_PACKAGE( model )


class ImplicitMatrix;


//-----------------------------------------------------------------------
//   class MatrixFreeModel
//-----------------------------------------------------------------------


class MatrixFreeModel : public Model,
                        public Serializable
{
 public:

  JEM_DECLARE_CLASS       ( MatrixFreeModel, Model );

  static const char*        TYPE_NAME;


                            MatrixFreeModel ();

                            MatrixFreeModel

    ( const String&           name,
      const Ref<DofSpace>&    dofs,
      const Ref<Model>&       child   );

                            MatrixFreeModel

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );

  virtual void              readFrom

    ( ObjectInput&            in );

  virtual void              writeTo

    ( ObjectOutput&           out )            const;

  virtual Model*            findModel

    ( const String&           name )           const;

  virtual void              configure

    ( const Properties&       props,
      const Properties&       globdat );

  virtual void              getConfig

    ( const Properties&       props,
      const Properties&       globdat )        const;

  virtual bool              takeAction

    ( const String&           action,
      const Properties&       params,
      const Properties&       globdat );

  static Ref<Model>         makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );

  static void               declare         ();


 protected:

  virtual                  ~MatrixFreeModel ();


 private:

  Ref<DofSpace>             dofs_;
  Ref<Model>                child_;
  Ref<ImplicitMatrix>       matrix_;

  double                    eps_;
  bool                      symmetric_;

};


JIVE_END_PACKAGE( model )

#endif
