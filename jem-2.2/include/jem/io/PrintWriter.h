
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

#ifndef JEM_IO_PRINTWRITER_H
#define JEM_IO_PRINTWRITER_H

#include <jem/io/Writer.h>
#include <jem/io/NumberFormat.h>


JEM_BEGIN_PACKAGE( io )


//-----------------------------------------------------------------------
//   class PrintWriter
//-----------------------------------------------------------------------


class PrintWriter : public Writer
{
 public:

  JEM_DECLARE_CLASS     ( PrintWriter, Writer );

  static const int        PAGE_WIDTH;
  static const int        INDENT_WIDTH;

  static const int        FLUSH_LINES;
  static const int        NO_WORD_WRAP;
  static const int        NUMERIC_BOOLS;


  explicit                PrintWriter

    ( const Ref<Writer>&    out,
      idx_t                 bufsize = -1 );

                          PrintWriter

    ( const Self&           rhs );

  virtual Ref<Writer>     dup             ();
  virtual void            close           ();
  virtual void            flush           ();

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

  void                    flushBuffer     ();

  virtual void            printBool

    ( bool                  b );

  virtual void            printInt

    ( lint                  i );

  virtual void            printFloat

    ( double                d );

  void                    printLine       ();
  void                    printSpace      ();

  void                    printSpaces

    ( idx_t                 count );

  void                    printBlankLine  ();

  void                    printBlankLines

    ( idx_t                 count );

  void                    pushSettings    ();
  void                    popSettings     ();

  int                     incrIndentLevel ();
  int                     decrIndentLevel ();

  void                    setOptions

    ( int                   options );

  void                    setPageWidth

    ( int                   width );

  void                    setIndentWidth

    ( int                   width );

  int                     getOptions      () const;
  int                     getPageWidth    () const;
  int                     getIndentWidth  () const;
  int                     getIndentLevel  () const;

  int                     getMarginWidth  () const;
  lint                    getLineNumber   () const;
  lint                    getColumnNumber () const;
  bool                    atStartOfLine   () const;

  template <class T>
    inline Self&          operator <<

    ( const T&              obj );


 public:

  NumberFormat            nformat;


 protected:

  virtual                ~PrintWriter     ();


 private:

  class                   Settings_;
  class                   Data_;
  friend class            Data_;

  Data_*                  data_;

};


//-----------------------------------------------------------------------
//   class Fill
//-----------------------------------------------------------------------


class Fill
{
 public:

  inline Fill             operator  ()

    ( int                   end )      const;


 public:

  const char              marker;
  const int               endCol;

};


extern const Fill         dots;
extern const Fill         spaces;


//-----------------------------------------------------------------------
//   related classes
//-----------------------------------------------------------------------


struct                    Indent      {};
extern const Indent       indent;

struct                    Outdent     {};
extern const Outdent      outdent;

struct                    BlankLine   {};
extern const BlankLine    blankLine;


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


inline Fill               fill

  ( char                    ch,
    int                     end = 48 );

inline void               print

  ( PrintWriter&            out,
    const Space&            sp );

inline void               print

  ( PrintWriter&            out,
    const EndLine&          eol );

inline void               print

  ( PrintWriter&            out,
    const Indent&           id );

inline void               print

  ( PrintWriter&            out,
    const Outdent&          od );

inline void               print

  ( PrintWriter&            out,
    const BlankLine&        bl );

void                      print

  ( PrintWriter&            out,
    const Fill&             fill );




//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class PrintWriter
//=======================================================================


template <class T>

  inline PrintWriter& PrintWriter::operator <<

  ( const T&  obj )

{
  print ( *this, obj );
  return *this;
}


//=======================================================================
//   class Fill
//=======================================================================


inline Fill Fill::operator () ( int end ) const
{
  Fill  f = { marker, end };

  return f;
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   fill
//-----------------------------------------------------------------------


inline Fill             fill

  ( char                  ch,
    int                   end )

{
  Fill  f = { ch, end };

  return f;
}


//-----------------------------------------------------------------------
//   print
//-----------------------------------------------------------------------


inline void             print

  ( PrintWriter&          out,
    const Space&          sp )

{
  out.printSpace ();
}


inline void             print

  ( PrintWriter&          out,
    const EndLine&        eol )

{
  out.printLine ();
}


inline void             print

  ( PrintWriter&          out,
    const Indent&         id )

{
  out.incrIndentLevel ();
}


inline void             print

  ( PrintWriter&          out,
    const Outdent&        od )

{
  out.decrIndentLevel ();
}


inline void             print

  ( PrintWriter&          out,
    const BlankLine&      bl )

{
  out.printBlankLine ();
}


JEM_END_PACKAGE( io )

#endif
