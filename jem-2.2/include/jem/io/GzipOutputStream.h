
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

#ifndef JEM_IO_GZIPOUTPUTSTREAM_H
#define JEM_IO_GZIPOUTPUTSTREAM_H

#include <jem/base/utilities.h>
#include <jem/io/OutputStream.h>


JEM_BEGIN_PACKAGE( io )


class Deflator;


//-----------------------------------------------------------------------
//   class GzipOutputStream
//-----------------------------------------------------------------------


class GzipOutputStream : public OutputStream
{
 public:

  JEM_DECLARE_CLASS   ( GzipOutputStream, OutputStream );


  explicit              GzipOutputStream

    ( Ref<OutputStream>   out,
      int                 level   = -1,
      idx_t               bufsize = -1 );

  virtual void          close             ();
  virtual void          flush             ();

  virtual void          write

    ( const void*         buf,
      idx_t               n );

  virtual void          writeNoThrow

    ( const void*         buf,
      idx_t               n );


 protected:

  virtual              ~GzipOutputStream  ();


 private:

  void                  finish_

    ( ThrowMode           tm = CAN_THROW );


 private:

  Ref<OutputStream>     output_;
  Ref<Deflator>         deflator_;

};


JEM_END_PACKAGE( io )

#endif
