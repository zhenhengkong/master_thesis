
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


#include <jem/base/Thread.h>
#include <jem/base/CString.h>
#include <jem/base/LogBuffer.h>
#include <jem/base/StringBuffer.h>
#include <jem/base/ClassTemplate.h>
#include <jem/base/SystemException.h>
#include <jem/base/CancelledException.h>
#include <jem/base/native/System.h>
#include <jem/io/IOException.h>
#include <jem/io/FileFlags.h>
#include <jem/io/FileInputStream.h>
#include <jem/io/FileOutputStream.h>
#include <jem/io/FileOpenException.h>
#include <jem/io/native/FileStream.h>
#include "native/WinProcess.h"

extern "C"
{
  #include <windows.h>
  #include <process.h>
}

#include "native/winclean.h"


JEM_DEFINE_CLASS( jem::WinProcess );


JEM_BEGIN_PACKAGE_BASE


using jem::io::FileFlags;
using jem::io::InputStream;
using jem::io::OutputStream;
using jem::io::WinFileStream;

typedef WinFileStream  WinFS;


//=======================================================================
//   private helper functions
//=======================================================================

//-----------------------------------------------------------------------
//   open2_
//-----------------------------------------------------------------------


static void       open2_

  ( Ref<WinFS>      fs[2],
    const String&   name,
    int             flags )

{
  using jem::io::IOException;
  using jem::io::FileOpenException;

  int  i = (flags & FileFlags::READ) ? 0 : 1;

  if ( name.size() == 0 )
  {
    fs[i] = newInstance<WinFS> ( *fs[i] );

    fs[i]->setInherit  ();

    return;
  }

  // Note: the WinFS objects must be constructed first, so that no
  // resources are leaked when an exception is thrown.

  if ( name == SpawnParams::PIPE )
  {
    SECURITY_ATTRIBUTES  secattr;
    HANDLE               hpipe[2];

    fs[0] = newInstance<WinFS> ();
    fs[1] = newInstance<WinFS> ();

    std::memset ( &secattr, 0x0, sizeof(secattr) );

    secattr.nLength              = sizeof(secattr);
    secattr.lpSecurityDescriptor = NULL;
    secattr.bInheritHandle       = TRUE;

    if ( ! CreatePipe( &hpipe[0], &hpipe[1], &secattr, 0 ) )
    {
      String  details = NativeSystem::strerror ();

      throw SystemException (
        JEM_FUNC,
        String::format ( "error opening pipe (%s)", details )
      );
    }

    fs[0]->setHandle ( hpipe[0], FileFlags::READ );
    fs[1]->setHandle ( hpipe[1], FileFlags::WRITE );

    i = (i + 1) % 2;

    fs[i]->setInherit ( false );
  }
  else
  {
    fs[i] = newInstance<WinFS> ( name.stripWhite(), flags );
  }
}


//=======================================================================
//   class WinProcess
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


WinProcess::WinProcess

  ( const Array<String>&  cline,
    const SpawnParams&    params,
    int                   options )

{
  using jem::io::FileInputStream;
  using jem::io::FileOutputStream;

  const idx_t          argc = cline.size ();

  STARTUPINFOA         startup;
  PROCESS_INFORMATION  procinfo;
  BOOL                 result;
  StringBuffer         command;

  Ref<WinFS>           fin [2];
  Ref<WinFS>           fout[2];
  Ref<WinFS>           ferr[2];


  // Catenate the command-line arguments.

  for ( idx_t i = 0; i < argc; i++ )
  {
    if ( i > 0 )
    {
      command += ' ';
    }

    if ( ! (options & OMIT_QUOTES) )
    {
      command += '\"';
    }

    command += cline[i];

    if ( ! (options & OMIT_QUOTES) )
    {
      command += '\"';
    }
  }

  command += '\0';

  // Open the standard input, output and error files for the child
  // process.

  fin [0] = WinFS::getStdin  ();
  fout[1] = WinFS::getStdout ();
  ferr[1] = WinFS::getStderr ();

  open2_ ( fin,  params.stdInput,  FileFlags::READ );
  open2_ ( fout, params.stdOutput, FileFlags::WRITE );

  if ( params.stdError == SpawnParams::STD_OUT )
  {
    ferr[0] = fout[0];
    ferr[1] = fout[1];
  }
  else
  {
    open2_ ( ferr, params.stdOutput, FileFlags::WRITE );
  }

  // Spawn the child process.

  std::memset ( &startup, 0x0, sizeof(startup) );

  startup.cb         = sizeof(startup);
  startup.dwFlags    = STARTF_USESTDHANDLES;
  startup.hStdInput  = fin [0]->getHandle ();
  startup.hStdOutput = fout[1]->getHandle ();
  startup.hStdError  = ferr[1]->getHandle ();

  result = CreateProcessA (
    NULL,                    // App name.
    command.addr (),         // Command line.
    NULL,                    // Process attributes.
    NULL,                    // Thread attributes.
    TRUE,                    // Inherit handles?
    CREATE_NO_WINDOW,        // Creation flags.
    NULL,                    // Environment variables.
    NULL,                    // Working directory.
    & startup,               // Startup info.
    & procinfo               // Process info.
  );

  if ( ! result )
  {
    String  details = NativeSystem::strerror ();

    throw SystemException (
      JEM_FUNC,
      String::format (
        "error executing `%s\' (%s)",
        cline[0],
        details
      )
    );
  }

  if ( cline.size() > 0 )
  {
    LogBuffer::pushBack (
      JEM_FUNC,
      String::format (
        "started `%s\'",
        cline[0]
      )
    );
  }

  // Release the file handles of the child process.

  fin [0]->release ();
  fout[1]->release ();
  ferr[1]->release ();

  if ( ! CloseHandle( procinfo.hThread ) )
  {
    CloseHandle ( procinfo.hProcess );

    String  details = NativeSystem::strerror ();

    throw SystemException (
      JEM_FUNC,
      String::format (
        "error closing thread handle (%s)", details
      )
    );
  }

  // Initialize the input, output and error streams of the parent
  // process.

  if ( fin[1] != NIL )
  {
    stdin_ = newInstance<FileOutputStream> ( fin[1] );
  }

  if ( fout[0] != NIL )
  {
    stdout_ = newInstance<FileInputStream> ( fout[0] );
  }

  if      ( ferr[0] == fout[0] )
  {
    stderr_ = stdout_;
  }
  else if ( ferr[0] != NIL )
  {
    stderr_ = newInstance<FileInputStream> ( ferr[0] );
  }

  hproc_  = (void*) procinfo.hProcess;
  iproc_  = (lint)  procinfo.dwProcessId;
  status_ = 0;
}


WinProcess::~WinProcess ()
{
  if ( hproc_ )
  {
    kill_       ();
    CloseHandle ( (HANDLE) hproc_ );

    hproc_ = NULL;
    iproc_ = 0;
  }
}


//-----------------------------------------------------------------------
//   kill
//-----------------------------------------------------------------------


void WinProcess::kill ()
{
  if ( ! hproc_ )
  {
    return;
  }

  // Check whether the process is still running.

  wait_ ( false );

  if ( ! hproc_ )
  {
    return;
  }

  if ( kill_() )
  {
    wait_ ();
    return;
  }

  // Termination of the process failed. Check again whether the
  // process is still running, and if so, do a last attempt to
  // kill it.

  wait_ ( false );

  if ( ! hproc_ )
  {
    return;
  }

  if ( kill_() )
  {
    wait_ ();
  }
  else
  {
    String  details = NativeSystem::strerror ();

    throw SystemException (
      JEM_FUNC,
      String::format (
        "error killing child process (%s)", details
      )
    );
  }
}


//-----------------------------------------------------------------------
//   wait
//-----------------------------------------------------------------------


int WinProcess::wait ()
{
  if ( hproc_ )
  {
    wait_ ();
  }

  return status_;
}


//-----------------------------------------------------------------------
//   isAlive
//-----------------------------------------------------------------------


bool WinProcess::isAlive ()
{
  if ( hproc_ )
  {
    wait_ ( false );
  }

  return (hproc_ != NULL);
}


//-----------------------------------------------------------------------
//   getStdin
//-----------------------------------------------------------------------


Ref<OutputStream> WinProcess::getStdin () const
{
  return stdin_;
}


//-----------------------------------------------------------------------
//   getStdout
//-----------------------------------------------------------------------


Ref<InputStream> WinProcess::getStdout () const
{
  return stdout_;
}


//-----------------------------------------------------------------------
//   getStderr
//-----------------------------------------------------------------------


Ref<InputStream> WinProcess::getStderr () const
{
  return stderr_;
}


//-----------------------------------------------------------------------
//   exec
//-----------------------------------------------------------------------


void WinProcess::exec

  ( const Array<String>&  cline,
    int                   options )

{
  const idx_t  argc = cline.size ();

  char*        args;
  char**       argv;
  idx_t        size;


  // Copy the arguments. Note that each arguments is quoted to make
  // sure that embedded spaces are handles correctly.

  size = sizeof(char*) * (size_t) (argc + 1);

  for ( idx_t i = 0; i < argc; i++ )
  {
    size += cline[i].size() + 1;

    if ( ! (options & OMIT_QUOTES) )
    {
      size += 2;
    }
  }

  args  = (char*)  MemCache::alloc ( (size_t) size );
  argv  = (char**) args;

  args += sizeof(char*) * (size_t) (argc + 1);

  for ( idx_t i = 0; i < argc; i++ )
  {
    argv[i] = args;

    if ( ! (options & OMIT_QUOTES) )
    {
      args[0] = '\"';
      args++;
    }

    std::memcpy ( args, cline[i].addr(), (size_t) cline[i].size() );

    args += cline[i].size ();

    if ( ! (options & OMIT_QUOTES) )
    {
      args[0] = '\"';
      args++;
    }

    args[0] = '\0';
    args++;
  }

  argv[argc] = NULL;

  ::_execvp ( argv[0], argv );

  // An error occurred if this point is reached.

  MemCache::dealloc ( argv, (size_t) size );

  String  details = NativeSystem::strerror ();

  throw SystemException (
    JEM_FUNC,
    String::format (
      "error executing `%s\' (%s)",
      cline[0],
      details
    )
  );
}


//-----------------------------------------------------------------------
//   getExecPath
//-----------------------------------------------------------------------


String WinProcess::getExecPath ()
{
  char     buf[512];
  String   path;
  DWORD    n;

  n = GetModuleFileNameA ( NULL, buf, (DWORD) (sizeof(buf) - 1) );

  if ( n > 0 )
  {
    path = String ( buf, n );
  }

  return path;
}


//-----------------------------------------------------------------------
//   kill_
//-----------------------------------------------------------------------


bool WinProcess::kill_ ()
{
  if ( TerminateProcess( (HANDLE) hproc_, (UINT) KILLED ) )
  {
    return true;
  }

  if ( GetLastError() == ERROR_ACCESS_DENIED )
  {
    // Try again with another handle with explicit TERMINATE priviliges.

    HANDLE  hproc2 = OpenProcess ( PROCESS_TERMINATE, 0,
                                   (DWORD) iproc_ );

    if ( hproc2 )
    {
      BOOL  result = TerminateProcess ( hproc2, (UINT) KILLED );

      CloseHandle ( hproc2 );

      return (result != 0);
    }
  }

  return false;
}


//-----------------------------------------------------------------------
//   wait_
//-----------------------------------------------------------------------


void WinProcess::wait_ ( bool block )
{
  DWORD  millis = block ? INFINITE : 0;
  DWORD  result = WaitForSingleObject( (HANDLE) hproc_, millis );

  if ( result == WAIT_TIMEOUT )
  {
    return;
  }

  if ( result != WAIT_OBJECT_0 )
  {
    String  details = NativeSystem::strerror ();

    throw SystemException (
      JEM_FUNC,
      String::format (
        "failed waiting for child process (%s)", details
      )
    );
  }

  if ( ! GetExitCodeProcess( (HANDLE) hproc_, &result ) )
  {
    String  details = NativeSystem::strerror ();

    throw SystemException (
      JEM_FUNC,
      String::format (
        "failed to get process exit code (%s)", details
      )
    );
  }

  status_ = (int) result;

  if ( ! CloseHandle( (HANDLE) hproc_ ) )
  {
    String  details = NativeSystem::strerror ();

    throw SystemException (
      JEM_FUNC,
      String::format (
        "error closing child process handle (%s)", details
      )
    );
  }

  hproc_ = NULL;
  iproc_ = 0;
}


JEM_END_PACKAGE_BASE
