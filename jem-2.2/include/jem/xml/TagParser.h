
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

#ifndef JEM_XML_TAGPARSER_H
#define JEM_XML_TAGPARSER_H

#include <jem/xml/Parser.h>
#include <jem/xml/AttributeParser.h>


JEM_BEGIN_PACKAGE( xml )


//-----------------------------------------------------------------------
//   class TagParser
//-----------------------------------------------------------------------


class TagParser : public    Parser,
                  protected AttributeParser
{
 public:

  typedef TagParser       Self;
  typedef Parser          Super;


  explicit                TagParser

    ( const String&         attribs = "" );

  virtual void            reset      ();

  virtual void            parse

    ( State&                state  );

  virtual void            takeAction

    ( const String&         action,
      const Properties&     params );

  virtual void            parseContents

    ( State&                state  );

  virtual void            handleEmptyContents

    ( State&                state  );

  virtual void            finish

    ( State&                state  );


  const String&           getTagName () const;


 protected:

  virtual                ~TagParser  ();


 protected:

  bool                    isLenient_;


 private:

  enum                    State_
  {
                            START_,
                            FINISH_
  };


  State_                  state_;
  String                  tagName_;

};


JEM_END_PACKAGE( xml )

#endif
