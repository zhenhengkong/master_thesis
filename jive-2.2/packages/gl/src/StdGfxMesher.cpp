
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


#include <jem/base/ClassTemplate.h>
#include <jem/base/IllegalInputException.h>
#include <jive/fem/ElementGroup.h>
#include <jive/gl/StdShapeMapper.h>
#include <jive/gl/GfxMesher2D.h>
#include <jive/gl/GfxMesher3D.h>
#include <jive/gl/StdGfxMesher.h>


JEM_DEFINE_CLASS( jive::gl::StdGfxMesher );


JIVE_BEGIN_PACKAGE( gl )


using jem::newInstance;


//=======================================================================
//   class StdGfxMesher
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


StdGfxMesher::StdGfxMesher ( Ref<ShapeMapper> mapper ) :

  shapeMapper_ ( mapper )

{
  if ( shapeMapper_ == NIL )
  {
    shapeMapper_ = newInstance<StdShapeMapper> ();
  }
}


StdGfxMesher::~StdGfxMesher ()
{}


//-----------------------------------------------------------------------
//   makeGfxMesh
//-----------------------------------------------------------------------


void StdGfxMesher::makeGfxMesh

  ( GfxMesh&             mesh,
    const ElementGroup&  group,
    const ShapeTable&    shapes )

{
  JEM_PRECHECK ( group != NIL );

  ElementSet   elems  = group.getElements ();
  NodeSet      nodes  = elems.getNodes    ();

  GfxMesher*   mesher = 0;
  const idx_t  rank   = nodes.rank        ();


  if      ( rank == 2 )
  {
    if ( mesher2d_ == NIL )
    {
      mesher2d_ = newInstance<GfxMesher2D> ( shapeMapper_ );
    }

    mesher = mesher2d_.get ();
  }
  else if ( rank == 3 )
  {
    if ( mesher3d_ == NIL )
    {
      mesher3d_ = newInstance<GfxMesher3D> ( shapeMapper_ );
    }

    mesher = mesher3d_.get ();
  }
  else
  {
    throw jem::IllegalInputException (
      JEM_FUNC,
      String::format (
        "mesh has %d dimensions but should have "
        "two or three dimensions",
        rank
      )
    );
  }

  mesher->makeGfxMesh ( mesh, group, shapes );
}


JIVE_END_PACKAGE( gl )
