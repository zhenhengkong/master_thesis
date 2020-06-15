
/*
 *  Copyright (C) 2016 DRG. All rights reserved.
 *
 *  This file is part of Jem, a general purpose programming toolkit.
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
 *
 *  Jem version: 2.2
 *  Date:        Thu 28 Jan 10:36:48 CET 2016
 */


#include <jem/util/Pair.h>
#include <jem/mp/Type.h>


JEM_BEGIN_PACKAGE( mp )


//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const int  TYPE_SIZES[TYPE_COUNT] =
{
  0,
  sizeof(byte),
  sizeof(char),
  sizeof(short),
  sizeof(int),
  sizeof(lint),
  sizeof(float),
  sizeof(double),
  sizeof(IntPair),
  sizeof(DoubleInt),
};


//-----------------------------------------------------------------------
//   function typeName
//-----------------------------------------------------------------------


const char* typeName ( int t )
{
  const char*  name = "";

  switch ( t )
  {
  case NO_TYPE    : name = "none";             break;
  case BYTE       : name = "byte";             break;
  case CHAR       : name = "char";             break;
  case SHORT      : name = "short";            break;
  case INT        : name = "int";              break;
  case LINT       : name = "lint";             break;
  case FLOAT      : name = "float";            break;
  case DOUBLE     : name = "double";           break;
  case INT_PAIR   : name = "Pair<int>";        break;
  case DOUBLE_INT : name = "Pair<double,int>"; break;

  default         : name = "unknown type";
  }

  return name;
}


JEM_END_PACKAGE( mp )
