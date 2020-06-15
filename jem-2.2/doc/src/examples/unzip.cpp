
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


#include <jem/base/System.h>
#include <jem/io/FileName.h>
#include <jem/io/FileWriter.h>
#include <jem/io/GzipFileReader.h>

using namespace jem;
using namespace jem::io;

int unzip ( int argc, char** argv )
{
  const int    N = 128;

  Ref<Reader>  input;
  Ref<Writer>  output;
  String       fname;
  char         buf[N];
  idx_t        k;

  for ( int i = 1; i < argc; i++ )
  {
    fname = argv[i];
    k     = FileName::findSuffix ( fname );

    if ( k < 0 || fname[slice(k,END)] != ".gz" )
    {
      continue;
    }

    input  = newInstance<GzipFileReader> ( fname );
    output = newInstance<FileWriter> ( fname[slice(BEGIN,k)] );
    k      = input->read ( buf, N );

    while ( k > 0 )
    {
      output->write   ( buf, k );
      k = input->read ( buf, N );
    }
  }

  return 0;
}

int main ( int argc, char** argv )
{
  return System::exec ( & unzip, argc, argv );
}