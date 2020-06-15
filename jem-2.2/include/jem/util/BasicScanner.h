
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

#ifndef JEM_UTIL_BASICSCANNER_H
#define JEM_UTIL_BASICSCANNER_H

#include <jem/base/StringBuffer.h>
#include <jem/io/ReadBuffer.h>


JEM_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class BasicScanner
//-----------------------------------------------------------------------


class BasicScanner
{
 public:

  typedef io::Reader      Reader;


  enum                    NumberType
  {
                            INTEGER,
                            FLOAT
  };


  explicit                BasicScanner

    ( const String&         str = "" );

  explicit                BasicScanner

    ( const Ref<Reader>&    in,
      idx_t                 bufsize = -1  );

  void                    setInput

    ( const String&         str );

  void                    skipWhite       ();
  void                    skipLine        ();

  inline bool             scan

    ( const char*           str,
      idx_t                 len );

  inline int              read            ();
  void                    readSymbol      ();

  void                    readString

    ( int                   delim );

  void                    readLine        ();
  inline void             readFloat       ();
  NumberType              readNumber      ();
  int                     tryReadNumber   ();

  inline void             unread

    ( int                   c );

  inline lint             getInteger      () const;
  inline double           getFloat        () const;
  bool                    isBoolToken     () const;
  inline String           getTokenString  () const;

  void                    setTokenString

    ( const String&         str );

  void                    parseError

    ( const String&         what )           const;


 public:

  Ref<Reader>             input;
  io::ReadBuffer          buffer;
  StringBuffer            token;
  lint                    lineno;


 private:

  void                    init_           ();

  void                    skipString_

    ( int                   delim );

  void                    readFraction_   ();
  void                    readExponent_   ();
  void                    setDecimalInt_  ();
  void                    setHexInt_      ();
  void                    setFloat_       ();


  union
  {
    lint                  ivalue_;
    double                xvalue_;
  };

};






//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   scan
//-----------------------------------------------------------------------


inline bool BasicScanner::scan

  ( const char*  str,
    idx_t        len )

{
  return buffer.scan ( *input, str, len );
}


//-----------------------------------------------------------------------
//   read
//-----------------------------------------------------------------------


inline int BasicScanner::read ()
{
  return buffer.read ( *input );
}


//-----------------------------------------------------------------------
//   readFloat
//-----------------------------------------------------------------------


inline void BasicScanner::readFloat ()
{
  readFraction_ ();
  setFloat_     ();
}


//-----------------------------------------------------------------------
//   unread
//-----------------------------------------------------------------------


inline void BasicScanner::unread ( int c )
{
  buffer.pushBack ( *input, c );
}


//-----------------------------------------------------------------------
//   getInteger
//-----------------------------------------------------------------------


inline lint BasicScanner::getInteger () const
{
  return ivalue_;
}


//-----------------------------------------------------------------------
//   getFloat
//-----------------------------------------------------------------------


inline double BasicScanner::getFloat () const
{
  return xvalue_;
}


//-----------------------------------------------------------------------
//   getTokenString
//-----------------------------------------------------------------------


inline String BasicScanner::getTokenString () const
{
  return token[ALL];
}


JEM_END_PACKAGE( util )

#endif
