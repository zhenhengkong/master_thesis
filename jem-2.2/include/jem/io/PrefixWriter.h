
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

#ifndef JEM_IO_PREFIXWRITER_H
#define JEM_IO_PREFIXWRITER_H

#include <jem/io/WriteBuffer.h>


JEM_BEGIN_PACKAGE( io )


//-----------------------------------------------------------------------
//   class PrefixWriter
//-----------------------------------------------------------------------


class PrefixWriter : public Writer
{
 public:

  JEM_DECLARE_CLASS     ( PrefixWriter, Writer );

  static const int        FLUSH_LINES;


                          PrefixWriter

    ( const Ref<Writer>&    out,
      const String&         prefix,
      int                   options = FLUSH_LINES );

                          PrefixWriter

    ( const Self&           rhs );

  virtual Ref<Writer>     dup           ();
  virtual void            close         ();
  virtual void            flush         ();

  virtual void            write

    ( int                   c );

  virtual void            write

    ( int                   c,
      idx_t                 n );

  virtual void            write

    ( const char*           buf,
      idx_t                 n );

  virtual void            writeNoThrow

    ( const char*           buf,
      idx_t                 n );

  void                    setPrefix

    ( const String&         prefix );

  inline String           getPrefix     () const;

  void                    setLineNumber

    ( lint                  lineno );

  inline lint             getLineNumber () const;


 protected:

  virtual                ~PrefixWriter  ();


 private:

  void                    writePrefix_  ();
  void                    writeLineno_  ();
  void                    writeTime_    ();
  void                    writeDate_    ();


 private:

  Ref<Writer>             output_;
  WriteBuffer             buffer_;
  String                  prefix_;
  lint                    lineno_;
  bool                    atStart_;
  bool                    flushLines_;
  bool                    simplePrefix_;

};



//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getPrefix
//-----------------------------------------------------------------------


inline String PrefixWriter::getPrefix () const
{
  return prefix_;
}


//-----------------------------------------------------------------------
//   getLineNumber
//-----------------------------------------------------------------------


inline lint PrefixWriter::getLineNumber () const
{
  return lineno_;
}


JEM_END_PACKAGE( io )

#endif
