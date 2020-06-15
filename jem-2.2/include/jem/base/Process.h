
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

#ifndef JEM_BASE_PROCESS_H
#define JEM_BASE_PROCESS_H

#include <jem/base/Object.h>
#include <jem/base/array/Array.h>


JEM_BEGIN_PACKAGE_BASE


namespace io
{
  class InputStream;
  class OutputStream;
}


class SpawnParams;


//-----------------------------------------------------------------------
//   class Process
//-----------------------------------------------------------------------


class Process : public Object
{
 public:

  JEM_DECLARE_CLASS       ( Process, Object );

  typedef io::InputStream   Input;
  typedef io::OutputStream  Output;

  static const int          KILLED;
  static const int          OMIT_QUOTES;


  virtual void              kill         ()       = 0;
  virtual int               wait         ()       = 0;
  virtual bool              isAlive      ()       = 0;

  virtual Ref<Output>       getStdin     () const = 0;
  virtual Ref<Input>        getStdout    () const = 0;
  virtual Ref<Input>        getStderr    () const = 0;

  static void               exec

    ( const Array<String>&    cline,
      int                     options = 0 );

  static Ref<Process>       spawn

    ( const Array<String>&    cline,
      int                     options = 0 );

  static Ref<Process>       spawn

    ( const Array<String>&    cline,
      const SpawnParams&      params,
      int                     options = 0 );

  static String             getExecPath  ();


 protected:

  virtual                  ~Process      ();

};


//-----------------------------------------------------------------------
//   class SpawnParams
//-----------------------------------------------------------------------


class SpawnParams
{
 public:

  static const char*        INHERIT;
  static const char*        DEV_NULL;
  static const char*        PIPE;
  static const char*        STD_OUT;


 public:

  String                    stdInput;
  String                    stdOutput;
  String                    stdError;

};


JEM_END_PACKAGE_BASE

#endif
