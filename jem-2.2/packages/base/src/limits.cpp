
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


#include <cfloat>
#include <climits>
#include <jem/base/limits.h>


JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class Limits
//=======================================================================


const byte    Limits<byte>           :: MIN_VALUE  = 0;
const byte    Limits<byte>           :: MAX_VALUE  = UCHAR_MAX;

const char    Limits<char>           :: MIN_VALUE  = CHAR_MIN;
const char    Limits<char>           :: MAX_VALUE  = CHAR_MAX;

const short   Limits<short>          :: MIN_VALUE  = SHRT_MIN;
const short   Limits<short>          :: MAX_VALUE  = SHRT_MAX;

const unsigned
  short       Limits<unsigned short> :: MIN_VALUE  = 0;
const unsigned
  short       Limits<unsigned short> :: MAX_VALUE  = USHRT_MAX;

const int     Limits<int>            :: MIN_VALUE  = INT_MIN;
const int     Limits<int>            :: MAX_VALUE  = INT_MAX;

const unsigned
  int         Limits<unsigned int>   :: MIN_VALUE  = 0;
const unsigned
  int         Limits<unsigned int>   :: MAX_VALUE  = UINT_MAX;

const lint    Limits<lint>           :: MIN_VALUE  = JEM_LINT_MIN;
const lint    Limits<lint>           :: MAX_VALUE  = JEM_LINT_MAX;

const ulint   Limits<ulint>          :: MIN_VALUE  = JEM_ULINT_C(0);
const ulint   Limits<ulint>          :: MAX_VALUE  = JEM_ULINT_MAX;

const float   Limits<float>          :: MIN_VALUE  = FLT_MIN;
const float   Limits<float>          :: MAX_VALUE  = FLT_MAX;
const float   Limits<float>          :: TINY_VALUE = 1.0e-30F;
const float   Limits<float>          :: EPSILON    = FLT_EPSILON;

const double  Limits<double>         :: MIN_VALUE  = DBL_MIN;
const double  Limits<double>         :: MAX_VALUE  = DBL_MAX;
const double  Limits<double>         :: TINY_VALUE = 1.0e-300;
const double  Limits<double>         :: EPSILON    = DBL_EPSILON;


JEM_END_PACKAGE_BASE
