
/*
 *  Copyright (C) 2016 DRG. All rights reserved.
 *
 *  This file is part of Jive, an object oriented toolkit for solving
 *  partial differential equations.
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
 *  This file is part of Jive, an object oriented toolkit for
 *  solving partial differential equations.
 *
 *  Jive version: 2.2
 *  Date:         Thu 28 Jan 10:31:15 CET 2016
 */

#ifndef JIVE_UTIL_DBASESPARSER_H
#define JIVE_UTIL_DBASESPARSER_H

#include <jem/xml/TagParser.h>
#include <jive/util/import.h>
#include <jive/util/typedefs.h>


JIVE_BEGIN_PACKAGE( util )


class ItemSet;
class Database;
class DBaseParser;
class ParserScope;


//-----------------------------------------------------------------------
//   class DBasesParser
//-----------------------------------------------------------------------


class DBasesParser : public jem::xml::TagParser
{
 public:

  JEM_DECLARE_CLASS       ( DBasesParser, jem::xml::TagParser );


  explicit                  DBasesParser

    ( const Ref<MPContext>&   mpx       = NIL,
      const Ref<ItemSet>&     items     = NIL,
      const Ref<Dictionary>&  dbases    = NIL,
      const Ref<Dictionary>&  rowGroups = NIL,
      const String&           filter    = "*" );

  virtual void              reset         ();

  virtual void              parseContents

    ( State&                  state     );

  virtual void              handleEmptyContents

    ( State&                  state     );

  virtual void              finish

    ( State&                  state     );

  virtual void              takeAction

    ( const String&           action,
      const Properties&       params    );

  void                      setDatabases

    ( const Ref<ItemSet>&     items,
      const Ref<Dictionary>&  dbases    );

  void                      setRowGroups

    ( const Ref<Dictionary>&  rowGroups );

  void                      setNameFilter

    ( const String&           pattern   );


 protected:

  virtual                  ~DBasesParser  ();

  virtual void              handleAttribute

    ( State&                  state,
      const String&           name,
      const String&           value,
      idx_t                   index );


 private:

  void                      newDatabase_  ();


 private:

  static const char*        ATTRIBS_;
  static const idx_t        NAME_ATTRIB_;

  Ref<ParserScope>          scope_;
  Ref<ItemSet>              rowItems_;
  Ref<Database>             dbase_;
  Ref<Dictionary>           dbases_;
  Ref<DBaseParser>          dbaseParser_;

  String                    dbaseName_;
  String                    nameFilter_;

};


JIVE_END_PACKAGE( util )

#endif
