
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

#ifndef JEM_XML_DOMTREE_H
#define JEM_XML_DOMTREE_H

#include <jem/xml/DOMElement.h>


JEM_BEGIN_PACKAGE( xml )


//-----------------------------------------------------------------------
//   class DOMTree
//-----------------------------------------------------------------------


class DOMTree
{
 public:

  static const int          QUOTE_STRINGS;
  static const int          CLONE_DEEP;


  static void               print

    ( PrintWriter&            out,
      const DOMElement&       root,
      int                     options = 0 );

  static Ref<DOMElement>    clone

    ( const DOMElement&       root,
      int                     options = 0 );


 private:

  static void               printAttribs_

    ( PrintWriter&            out,
      const Properties&       attribs,
      int                     options );

  static void               printElement_

    ( PrintWriter&            out,
      const DOMElement&       elem,
      int                     options );

};


JEM_END_PACKAGE( xml )

#endif
