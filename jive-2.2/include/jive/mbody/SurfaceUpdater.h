
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

#ifndef JIVE_MBODY_SURFACEUPDATER_H
#define JIVE_MBODY_SURFACEUPDATER_H

#include <jem/base/Object.h>
#include <jem/base/WeakRef.h>
#include <jive/Array.h>
#include <jive/mbody/import.h>


namespace jive
{
  namespace util
  {
    class GroupSet;
    class XGroupSet;
  }
}


JIVE_BEGIN_PACKAGE( mbody )


class BodySet;
class XBodySet;


//-----------------------------------------------------------------------
//   class SurfaceUpdater
//-----------------------------------------------------------------------


class SurfaceUpdater : public Object
{
 public:

  typedef Object            Super;
  typedef SurfaceUpdater    Self;


                            SurfaceUpdater

    ( const XBodySet&         sbodies,
      const BodySet&          rbodies,
      const Ref<ShapeTable>&  shapes );

  void                      update            ();


 protected:

  virtual                  ~SurfaceUpdater    ();


 private:

  void                      newSizeHandler_   ();
  void                      newTopoHandler_   ();

  void                      newOrderHandler_

    ( const Reordering&       reord );


 private:

  jem::WeakRef
    <util::XGroupSet>       sbodies_;
  jem::WeakRef
    <util::GroupSet>        rbodies_;
  jem::WeakRef
    <ShapeTable>            shapes_;

  bool                      updated_;

};


JIVE_END_PACKAGE( mbody )

#endif