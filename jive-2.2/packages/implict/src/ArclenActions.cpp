
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


#include <jive/implict/ArclenActions.h>


JIVE_BEGIN_PACKAGE( implict )


//=======================================================================
//   class ArclenActions
//=======================================================================


const char*  ArclenActions::GET_ARC_FUNC   = "GET_ARC_FUNC";
const char*  ArclenActions::GET_UNIT_LOAD  = "GET_UNIT_LOAD";


//=======================================================================
//   class ArclenParams
//=======================================================================


const char*  ArclenParams::IITER           = "arclen.iiter";
const char*  ArclenParams::ARC_FUNC        = "arclen.arcFunc";
const char*  ArclenParams::JACOBIAN10      = "arclen.jacobian10";
const char*  ArclenParams::JACOBIAN11      = "arclen.jacobian11";
const char*  ArclenParams::UNIT_LOAD       = "arclen.unitLoad";
const char*  ArclenParams::LOAD_SCALE      = "arclen.loadScale";
const char*  ArclenParams::OLD_LOAD_SCALE  = "arclen.oldLoadScale";
const char*  ArclenParams::LOAD_RESPONSE   = "arclen.loadResponse";


JIVE_END_PACKAGE( implict )