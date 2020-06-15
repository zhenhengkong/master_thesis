
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

#ifndef JIVE_GEOM_NAMES_H
#define JIVE_GEOM_NAMES_H

#include <jive/defines.h>


JIVE_BEGIN_PACKAGE( geom )


//-----------------------------------------------------------------------
//   class PropertyNames
//-----------------------------------------------------------------------


class PropertyNames
{
 public:

  static const char*    BASIS;
  static const char*    EXTRACTOR;
  static const char*    IROWS;
  static const char*    ISCHEME;
  static const char*    JCOLS;
  static const char*    SHAPE;
  static const char*    SHAPE_FUNCS;
  static const char*    SHAPE_SETS;
  static const char*    TYPE;
  static const char*    VALUES;
  static const char*    WEIGHTS;

};


//-----------------------------------------------------------------------
//   related types
//-----------------------------------------------------------------------


typedef PropertyNames   PropNames;


JIVE_END_PACKAGE( geom )

#endif