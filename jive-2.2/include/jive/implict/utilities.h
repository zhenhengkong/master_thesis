
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

#ifndef JIVE_IMPLICT_UTILITIES_H
#define JIVE_IMPLICT_UTILITIES_H

#include <jem/base/Object.h>
#include <jive/implict/import.h>


JIVE_BEGIN_PACKAGE( implict )


//-----------------------------------------------------------------------
//   utility functions
//-----------------------------------------------------------------------


Ref<MatrixBuilder>          newMatrixBuilder

  ( const String&             name,
    const String&             context,
    const Properties&         conf,
    const Properties&         props,
    const Properties&         globdat,
    Ref<DofSpace>             dofs   = NIL );

Ref<Solver>                 newSolver

  ( const String&             name,
    const String&             context,
    const Properties&         conf,
    const Properties&         props,
    const Properties&         params,
    const Properties&         globdat );

Properties                  newSolverParams

  ( const Properties&         globdat,
    Ref<AbstractMatrix>       globmat,
    Ref<AbstractMatrix>       precon = NIL,
    const Ref<DofSpace>&      dofs   = NIL );


JIVE_END_PACKAGE( implict )

#endif
