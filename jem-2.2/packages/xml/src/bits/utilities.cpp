
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


#include <cctype>
#include <jem/base/StringBuffer.h>
#include <jem/io/PrintWriter.h>
#include <jem/xml/utilities.h>


JEM_BEGIN_PACKAGE( xml )


//-----------------------------------------------------------------------
//   makeTag
//-----------------------------------------------------------------------


String makeTag ( const String& str, bool firstCap )
{
  const char*  s = str.addr ();
  const idx_t  n = str.size ();

  idx_t        i = 0;

  while ( i < n && std::isspace( s[i] ) )
  {
    i++;
  }

  if ( i == n )
  {
    return String ();
  }
  else
  {
    StringBuffer  buf;

    char   c0 = s[i];
    char*  p  = buf.xalloc ( n - i );
    idx_t  j  = 0;

    while ( i < n )
    {
      p[j++] = (char) std::toupper ( s[i++] );

      while ( i < n && ! std::isspace( s[i] ) )
      {
        p[j++] = s[i++];
      }

      while ( i < n && std::isspace( s[i] ) )
      {
        i++;
      }
    }

    if ( ! firstCap )
    {
      p[0] = (char) std::tolower ( c0 );
    }

    buf.commit ( j );

    return buf.toString ();
  }
}


JEM_END_PACKAGE( xml )
