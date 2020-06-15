
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


#include <climits>
#include "native/WinTime.h"


JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class WinTime
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


double  WinTime::CPU_TICK_ = 0.0;


//-----------------------------------------------------------------------
//   initCPUTime
//-----------------------------------------------------------------------


void WinTime::initCPUTime ()
{
  static volatile long  dummy;

  double  t0 = getCPUTime ();
  lint    n  = JEM_LINT_C(2);
  int     k  = 0;


  CPU_TICK_ = 1.0e10;

  while ( k < 4 )
  {
    dummy = 123456789L;

    for ( lint i = 0; i < n; i++ )
    {
      dummy /= 13;
      dummy *= 16;
      dummy /= 3;
    }

    double  t  = getCPUTime ();
    double  dt = t - t0;

    if ( dt > 0.0 )
    {
      if ( dt < CPU_TICK_ )
      {
        CPU_TICK_ = dt;
      }

      t0 = t;
      n /= JEM_LINT_C(2);
      k += 1;

      if ( n < JEM_LINT_C(2) )
      {
        n = JEM_LINT_C(2);
      }
    }
    else
    {
      n *= JEM_LINT_C(2);

      if ( n > (JEM_LINT_MAX / 2) )
      {
        n = JEM_LINT_MAX / 2;
      }
    }
  }
}


JEM_END_PACKAGE_BASE
