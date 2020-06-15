
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

#ifndef JIVE_FEMODEL_MISC_LOADMODEL_H
#define JIVE_FEMODEL_MISC_LOADMODEL_H

#include <jem/io/Serializable.h>
#include <jive/model/Model.h>
#include <jive/femodel/import.h>


JIVE_BEGIN_PACKAGE( femodel )


//-----------------------------------------------------------------------
//   class LoadModel
//-----------------------------------------------------------------------


class LoadModel : public Model, public Serializable
{
 public:

  JEM_DECLARE_CLASS       ( LoadModel, Model );

  static const char*        TYPE_NAME;


                            LoadModel ();

                            LoadModel

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );

  virtual void              readFrom

    ( ObjectInput&            in );

  virtual void              writeTo

    ( ObjectOutput&           out )        const;

  virtual void              configure

    ( const Properties&       props,
      const Properties&       globdat );

  virtual void              getConfig

    ( const Properties&       props,
      const Properties&       globdat )    const;

  virtual bool              takeAction

    ( const String&           action,
      const Properties&       params,
      const Properties&       globdat );

  static Ref<Model>         makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );


 protected:

  virtual                  ~LoadModel ();


 private:

  void                      getExtVector_

    ( const Vector&           vec,
      double                  scale,
      const Properties&       globdat )    const;


 private:

  Ref<Element>              element_;
  Ref<ElemIter>             elemIter_;

  Ref<XDofSpace>            dofs_;
  StringVector              dofNames_;
  IdxVector                 dofTypes_;

  jem::Array
    < Ref<Function> >       loads_;

};


JIVE_END_PACKAGE( femodel )

#endif
