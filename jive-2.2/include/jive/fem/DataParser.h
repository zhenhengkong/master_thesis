
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

#ifndef JIVE_FEM_DATAPARSER_H
#define JIVE_FEM_DATAPARSER_H

#include <jive/util/DataParser.h>
#include <jive/fem/typedefs.h>


JIVE_BEGIN_PACKAGE( fem )


class XNodeSet;
class XElementSet;
class XBoundarySet;


//-----------------------------------------------------------------------
//   class DataParser
//-----------------------------------------------------------------------


class DataParser : public util::DataParser
{
 public:

  typedef DataParser        Self;
  typedef util::DataParser  Super;


  explicit                  DataParser

    ( const Ref<MPContext>&   mpx = NIL );

  void                      addNodesParser

    ( const XNodeSet&         nodes,
      idx_t                   rank          = -1 );

  void                      addElementsParser

    ( const XElementSet&      elems,
      idx_t                   nodesPerElem  = -1 );

  void                      addBoundsParser

    ( const XBoundarySet&     bounds,
      idx_t                   maxLocalIndex = -1 );


 protected:

  virtual                  ~DataParser  ();

};


JIVE_END_PACKAGE( fem )

#endif
