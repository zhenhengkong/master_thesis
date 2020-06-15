
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

#ifndef JEM_XML_DOMPARSER_H
#define JEM_XML_DOMPARSER_H

#include <jem/xml/Parser.h>


JEM_BEGIN_PACKAGE( xml )


class DOMElement;


//-----------------------------------------------------------------------
//   class DOMParser
//-----------------------------------------------------------------------


class DOMParser : public Parser
{
 public:

  typedef DOMParser         Self;
  typedef Parser            Super;


  static const char*        PARSE_ATTRIBS;
  static const char*        PARSE_TO_UPPER;
  static const char*        PARSE_TO_LOWER;
  static const char*        SKIP_CONTENTS;


                            DOMParser      ();

  virtual void              reset          ();

  virtual void              parse

    ( State&                  state );

  virtual void              takeAction

    ( const String&           action,
      const Properties&       params );

  Ref<DOMElement>           getRootElement ();


 protected:

  virtual                  ~DOMParser      ();


 private:

  class                     Work_;
  class                     Element_;


  void                      setOption_

    ( const char*             name,
      int                     value,
      const Properties&       params );

  Ref<Element_>             newElement_

    ( State&                  state,
      Work_&                  work,
      const Ref<Element_>&    parent );

  void                      readAttribs_

    ( State&                  state,
      Work_&                  work,
      Element_&               elem  );

  void                      parseAttribs_

    ( State&                  state,
      Work_&                  work,
      Element_&               elem  );

  static Ref<Object>        parseAttrib_

    ( Work_&                  work,
      const String&           value );


 private:

  static const int          PARSE_ATTRIBS_;
  static const int          PARSE_TO_UPPER_;
  static const int          PARSE_TO_LOWER_;
  static const int          SKIP_CONTENTS_;


  Ref<DOMElement>           root_;
  int                       options_;

};


JEM_END_PACKAGE( xml )

#endif
