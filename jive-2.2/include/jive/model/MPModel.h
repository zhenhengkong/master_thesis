
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

#ifndef JIVE_MODEL_MPMODEL_H
#define JIVE_MODEL_MPMODEL_H

#include <jem/io/Serializable.h>
#include <jem/util/Flex.h>
#include <jive/model/import.h>
#include <jive/model/Model.h>


JIVE_BEGIN_PACKAGE( model )


//-----------------------------------------------------------------------
//   class MPModel
//-----------------------------------------------------------------------


class MPModel : public Model,
                public Serializable
{
 public:

  JEM_DECLARE_CLASS       ( MPModel, Model );

  static const char*        TYPE_NAME;


                            MPModel         ();

                            MPModel

    ( const String&           name,
      const Ref<Model>&       child,
      Ref<TableExchanger>     tex = NIL,
      Ref<VectorExchanger>    vex = NIL );

                            MPModel

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

  void                      clear           ();

  void                      addTable

    ( const String&           tname );

  bool                      eraseTable

    ( const String&           tname );

  void                      addVector

    ( const String&           action,
      const String&           vname );

  bool                      eraseVector

    ( const String&           action );

  void                      setDefaults     ();

  static Ref<Model>         makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Properties&       globdat );

  static void               declare         ();


 protected:

  virtual                  ~MPModel         ();


 private:

  Ref<ItemSet>              items_;
  Ref<DofSpace>             dofs_;
  Ref<Model>                child_;

  Ref<TableExchanger>       tex_;
  Ref<VectorExchanger>      vex_;

  jem::util::Flex<String>   tabNames_;
  jem::util::Flex<String>   vecActions_;
  jem::util::Flex<String>   vecNames_;

};


JIVE_END_PACKAGE( model )

#endif