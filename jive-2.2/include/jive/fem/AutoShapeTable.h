
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

#ifndef JIVE_FEM_AUTOSHAPETABLE_H
#define JIVE_FEM_AUTOSHAPETABLE_H

#include <jem/io/Serializable.h>
#include <jive/geom/StdShapeTable.h>
#include <jive/fem/import.h>
#include <jive/fem/ShapeArray.h>


JIVE_BEGIN_PACKAGE( fem )


class ElementSet;
class ElementGroup;


//-----------------------------------------------------------------------
//   class AutoShapeTable
//-----------------------------------------------------------------------


class AutoShapeTable : public geom::StdShapeTable,
                       public Serializable
{
 public:

  JEM_DECLARE_CLASS       ( AutoShapeTable, geom::StdShapeTable );

  static const char*        TYPE_NAME;


  virtual void              readFrom

    ( ObjectInput&            in );

  virtual void              writeTo

    ( ObjectOutput&           out )              const;

  virtual void              configure

    ( const Properties&       props,
      const Properties&       globdat );

  virtual void              getConfig

    ( const Properties&       conf )             const;


  static void               declare           ();

  static Ref<ShapeTable>    makeNew

    ( const String&           name,
      const Properties&       conf,
      const Properties&       props,
      const Ref<ItemSet>&     items,
      const Properties&       globdat );


 protected:

                            AutoShapeTable    ();

                            AutoShapeTable

    ( const String&           name,
      const ElementSet&       elems,
      idx_t                   maxPrec );

                            AutoShapeTable

    ( const String&           name,
      const ElementGroup&     belems,
      const ElementGroup&     felems,
      idx_t                     maxPrec );

  virtual                  ~AutoShapeTable    ();

  virtual void              update_           ();

  virtual bool              makeIShapes_

    ( PrintWriter&            log,
      const ShapeVector&      shapes,
      idx_t                   nodeCount )        = 0;

  virtual bool              makeBShapes_

    ( PrintWriter&            log,
      const ShapeVector&      shapes,
      idx_t                   nodeCount )        = 0;

  virtual bool              makeFShapes_

    ( PrintWriter&            log,
      const ShapeVector&      shapes,
      idx_t                   nodeCount );


 private:

  void                      connect_          ();
  void                      invalidate_       ();


 private:

  Ref<ItemGroup>            belems_;
  Ref<ItemGroup>            felems_;

  String                    bname_;
  String                    fname_;

};


JIVE_END_PACKAGE( fem )

#endif