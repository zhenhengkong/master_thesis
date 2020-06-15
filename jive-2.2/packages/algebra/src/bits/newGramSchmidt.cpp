
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


#include <jem/util/Properties.h>
#include <jive/algebra/GramSchmidtFactory.h>
#include <jive/algebra/MixedGramSchmidt.h>


JIVE_BEGIN_PACKAGE( algebra )


//-----------------------------------------------------------------------
//   newGramSchmidt
//-----------------------------------------------------------------------


Ref<GramSchmidt>            newGramSchmidt

  ( const String&             name,
    const Properties&         conf,
    const Ref<VectorSpace>&   vspace )

{
  Properties  subConf = conf.makeProps ( name );

  subConf.set ( GSFactory::TYPE_PROP, MixedGS::TYPE_NAME );

  return MixedGS::makeNew ( name, conf, Properties(), vspace );
}


Ref<GramSchmidt>            newGramSchmidt

  ( const String&             name,
    const Properties&         conf,
    const Properties&         props,
    const Ref<VectorSpace>&   vspace )

{
  if ( props.contains( name ) )
  {
    return GSFactory::newInstance ( name,  conf,
                                    props, vspace );
  }
  else
  {
    return newGramSchmidt ( name, conf, vspace );
  }
}


JIVE_END_PACKAGE( algebra )