
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

#ifndef JEM_IO_OBJECTINPUTSTREAM_H
#define JEM_IO_OBJECTINPUTSTREAM_H

#include <jem/io/ObjectInput.h>
#include <jem/io/InputStream.h>
#include <jem/io/ObjectStreamParams.h>


JEM_BEGIN_PACKAGE( io )


class TextOutput;


//-----------------------------------------------------------------------
//   class ObjectInputStream
//-----------------------------------------------------------------------


class ObjectInputStream : public  InputStream,
                          public  ObjectInput,
                          private ObjectStreamParams
{
 public:

  JEM_DECLARE_CLASS     ( ObjectInputStream, InputStream );


  explicit                ObjectInputStream

    ( Ref<InputStream>      in );

  virtual void            close             ();

  virtual idx_t           poll

    ( const Time&           timeout );

  virtual idx_t            read

    ( void*                 buf,
      idx_t                 n );

  virtual bool            decodeBool        ();
  virtual byte            decodeByte        ();
  virtual char            decodeChar        ();
  virtual short           decodeShort       ();
  virtual int             decodeInt         ();
  virtual lint            decodeLong        ();
  virtual float           decodeFloat       ();
  virtual double          decodeDouble      ();

  virtual void            decode

    ( bool*                 buf,
      idx_t                 len );

  virtual void            decode

    ( byte*                 buf,
      idx_t                 len );

  virtual void            decode

    ( char*                 buf,
      idx_t                 len );

  virtual void            decode

    ( short*                buf,
      idx_t                 len );

  virtual void            decode

    ( int*                  buf,
      idx_t                 len );

  virtual void            decode

    ( lint*                 buf,
      idx_t                 len );

  virtual void            decode

    ( float*                buf,
      idx_t                 len );

  virtual void            decode

    ( double*               buf,
      idx_t                 len );

  virtual Ref<Object>     decodeObject      ();

  virtual void            decodeReference

    ( const Ref<Object>&    obj );


  void                    printTrace

    ( TextOutput&           out )              const;


 protected:

  virtual                ~ObjectInputStream ();


 private:

  void                    decodeClass_      ();
  Ref<Object>             decodeObject_     ();


 private:

  class                   Data_;

  Ref<Data_>              data_;

};


JEM_END_PACKAGE( io )

#endif
