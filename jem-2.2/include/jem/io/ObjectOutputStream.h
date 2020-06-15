
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

#ifndef JEM_IO_OBJECTOUTPUTSTREAM_H
#define JEM_IO_OBJECTOUTPUTSTREAM_H

#include <jem/io/ObjectOutput.h>
#include <jem/io/ObjectStreamParams.h>
#include <jem/io/OutputStream.h>


JEM_BEGIN_PACKAGE( io )


//-----------------------------------------------------------------------
//   class ObjectOutputStream
//-----------------------------------------------------------------------


class ObjectOutputStream : public  OutputStream,
                           public  ObjectOutput,
                           private ObjectStreamParams
{
 public:

  JEM_DECLARE_CLASS     ( ObjectOutputStream, OutputStream );


  explicit                ObjectOutputStream

    ( Ref<OutputStream>     out );

  virtual void            close               ();
  virtual void            flush               ();

  virtual void            write

    ( const void*           buf,
      idx_t                 n );

  virtual void            writeNoThrow

    ( const void*           buf,
      idx_t                 n );

  virtual void            reset               ();

  virtual void            encode

    ( bool                  b );

  virtual void            encode

    ( byte                  b );

  virtual void            encode

    ( char                  c );

  virtual void            encode

    ( short                 s );

  virtual void            encode

    ( int                   i );

  virtual void            encode

    ( lint                  l );

  virtual void            encode

    ( float                f );

  virtual void            encode

    ( double                d );

  virtual void            encode

    ( const bool*           buf,
      idx_t                 len );

  virtual void            encode

    ( const byte*           buf,
      idx_t                 len );

  virtual void            encode

    ( const char*           buf,
      idx_t                 len );

  virtual void            encode

    ( const short*          buf,
      idx_t                 len );

  virtual void            encode

    ( const int*            buf,
      idx_t                 len );

  virtual void            encode

    ( const lint*           buf,
      idx_t                 len );

  virtual void            encode

    ( const float*          buf,
      idx_t                 len );

  virtual void            encode

    ( const double*         buf,
      idx_t                 len );

  virtual void            encodeObject

    ( const Ref<Object>&    obj );

  virtual void            encodeReference

    ( const Ref<Object>&    obj );


 protected:

  virtual                ~ObjectOutputStream  ();


 private:

  class                   Data_;

  Ref<Data_>              data_;

};


JEM_END_PACKAGE( io )

#endif
