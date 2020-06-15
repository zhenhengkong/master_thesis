
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
#include <jem/base/limits.h>
#include <jem/util/CppScanner.h>


JEM_BEGIN_PACKAGE( util )


//=======================================================================
//   class CppScanner
//=======================================================================

//-----------------------------------------------------------------------
//   skipCppWhite
//-----------------------------------------------------------------------


void CppScanner::skipCppWhite ()
{
  int  c = read ();

  while ( c >= 0 )
  {
    if ( std::isspace(c) )
    {
      if ( c == '\n' )
      {
        lineno++;
      }
    }
    else if ( c == '/' )
    {
      int  next = read ();

      if ( next == '/' )
      {
        skipLine ();
      }
      else if ( next == '*' )
      {
        skipComment_ ( false );
      }
      else
      {
        unread ( next );
        break;
      }
    }
    else
    {
      break;
    }

    c = read ();
  }

  unread ( c );
}


//-----------------------------------------------------------------------
//   readComment
//-----------------------------------------------------------------------


void CppScanner::readComment ()
{
  skipComment_ ( true );
}


//-----------------------------------------------------------------------
//   skipComment_
//-----------------------------------------------------------------------


void CppScanner::skipComment_ ( bool store )
{
  lint  line  = lineno;
  int   level = 1;

  int   c[2];


  c[0] = c[1] = -1;

  while ( level > 0 )
  {
    c[0] = read ();

    if ( c[0] < 0 )
    {
      parseError (
        String::format (
          "unterminated comment block starting at line %d", line
        )
      );
    }

    if ( store )
    {
      token += (char) c[0];
    }

    if ( c[0] == '\n' )
    {
      lineno++;
    }

    if      ( c[1] =='/' && c[0] == '*' )
    {
      c[1] = -1;

      if ( level >= maxOf( level ) )
      {
        parseError ( "too many nested comment blocks" );
      }

      level++;
    }
    else if ( c[1] == '*' && c[0] == '/' )
    {
      c[1] = -1;

      level--;
    }
    else
    {
      c[1] = c[0];
    }
  }
}


JEM_END_PACKAGE( util )
