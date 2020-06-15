
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

#ifndef JIVE_SOLVER_RESTRICTORFACTORY_H
#define JIVE_SOLVER_RESTRICTORFACTORY_H

#include <jive/util/Factory.h>
#include <jive/solver/typedefs.h>


JIVE_BEGIN_PACKAGE( solver )


class Restrictor;


//-----------------------------------------------------------------------
//   class RestrictorFactory
//-----------------------------------------------------------------------


class RestrictorFactory : public util::Factory
{
 public:

  typedef RestrictorFactory   Self;
  typedef util::Factory       Super;

  typedef Ref<Restrictor>   (*Constructor)

    ( const String&             name,
      const Properties&         conf,
      const Properties&         props,
      const Properties&         params,
      const Properties&         globdat );


  static void                 declare

    ( const String&             type,
      Constructor               ctor );

  static bool                 exists

    ( const String&             type );

  static StringVector         listKnownTypes  ();

  static Ref<Restrictor>      newInstance

    ( const String&             type,
      const String&             name,
      const Properties&         conf,
      const Properties&         props,
      const Properties&         params,
      const Properties&         globdat );

  static Ref<Restrictor>      newInstance

    ( const String&             name,
      const Properties&         conf,
      const Properties&         props,
      const Properties&         params,
      const Properties&         globdat );


 private:

  class                       CtorMap_;

};


JIVE_END_PACKAGE( solver )

#endif
