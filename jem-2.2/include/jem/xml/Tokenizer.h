
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

#ifndef JEM_XML_TOKENIZER_H
#define JEM_XML_TOKENIZER_H

#include <jem/base/Object.h>
#include <jem/xml/import.h>
#include <jem/xml/Scanner.h>


JEM_BEGIN_PACKAGE( xml )


//-----------------------------------------------------------------------
//   class Tokenizer
//-----------------------------------------------------------------------


class Tokenizer : public Object
{
 public:

  typedef Tokenizer       Self;
  typedef Object          Super;


  static const int        NULL_TOKEN;
  static const int        EOF_TOKEN;
  static const int        COMMENT_TOKEN;
  static const int        START_TAG_TOKEN;
  static const int        TAG_END_TOKEN;
  static const int        EMPTY_TAG_END_TOKEN;
  static const int        END_TAG_TOKEN;
  static const int        SYMBOL_TOKEN;
  static const int        STRING_TOKEN;
  static const int        BOOLEAN_TOKEN;
  static const int        INTEGER_TOKEN;
  static const int        FLOAT_TOKEN;

  static const int        PARSE_COMMENTS;
  static const int        PARSE_SYMBOLS;
  static const int        PARSE_STRINGS;
  static const int        PARSE_BOOLEANS;
  static const int        PARSE_NUMBERS;
  static const int        PARSE_SIGNED_NUMBERS;


  explicit                Tokenizer

    ( const String&         in = String() );

  explicit                Tokenizer

    ( const Ref<Reader>&    in,
      idx_t                 bufsize = -1 );

  void                    setInput

    ( const String&         str );

  int                     nextToken       ();
  void                    pushBackToken   ();
  int                     getToken        () const;
  lint                    getLineNumber   () const;

  void                    setLineNumber

    ( lint                  lineno );

  String                  getTokenString  () const;
  String                  getComment      () const;
  String                  getTagName      () const;
  String                  getSymbol       () const;
  String                  getString       () const;
  bool                    getBoolean      () const;
  long                    getInteger      () const;
  double                  getFloat        () const;

  void                    setOptions

    ( int                   options );

  int                     getOptions      () const;

  bool                    readAttribute

    ( String&               name,
      String&               value );

  int                     skipAttributes  ();

  int                     readAttributes

    ( TextOutput&           out );

  void                    skipToEndTag

    ( const String&         tagName );

  String                  readToEndTag

    ( const String&         tagName );

  void                    readToEndTag

    ( TextOutput&           out,
      const String&         tagName );

  String                  readText        ();

  int                     readText

    ( TextOutput&           out );


 protected:

  virtual                ~Tokenizer       ();


 private:

  void                    init_           ();

  int                     readTag_        ();

  void                    endTagError_

    ( const String&         tagName )        const;


 private:

  Scanner                 scanner_;

  int                     token_;
  int                     pushedToken_;
  int                     options_;

};


JEM_END_PACKAGE( xml )

#endif
