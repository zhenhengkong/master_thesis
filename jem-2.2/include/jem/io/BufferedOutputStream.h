
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

#ifndef JEM_IO_BUFFEREDOUTPUTSTREAM_H
#define JEM_IO_BUFFEREDOUTPUTSTREAM_H

#include <jem/io/FilterOutputStream.h>


JEM_BEGIN_PACKAGE( io )


//-----------------------------------------------------------------------
//   class BufferedOutputStream
//-----------------------------------------------------------------------


class BufferedOutputStream : public FilterOutputStream
{
 public:

  typedef
    BufferedOutputStream  Self;
  typedef
    FilterOutputStream    Super;


  explicit                BufferedOutputStream

    ( Ref<OutputStream>     out,
      idx_t                 bufsize = -1 );

                          BufferedOutputStream

    ( const Self&           rhs );

  virtual
    Ref<OutputStream>     dup                   ();
  virtual void            close                 ();
  virtual void            flush                 ();

  virtual void            write

    ( const void*           buf,
      idx_t                 n );

  virtual void            writeNoThrow

    ( const void*           buf,
      idx_t                 n );

  void                    flushBuffer           ();


 protected:

  virtual                ~BufferedOutputStream  ();


 private:

  void                    init_

    ( idx_t                 bufsize );


 private:

  static const byte       CANARY_VALUE_;

  byte*                   buffer_;
  idx_t                   bufsize_;
  idx_t                   last_;

};


JEM_END_PACKAGE( io )

#endif