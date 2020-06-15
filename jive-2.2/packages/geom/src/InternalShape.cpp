
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
#include <jive/geom/InternalShape.h>


JEM_DEFINE_CLASS( jive::geom::InternalShape );


JIVE_BEGIN_PACKAGE( geom )


//=======================================================================
//   class InternalShape
//=======================================================================

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


InternalShape::InternalShape ( const String& name ) :

  Super ( name )

{}


InternalShape::~InternalShape ()
{}


//-----------------------------------------------------------------------
//   getContext
//-----------------------------------------------------------------------


String InternalShape::getContext () const
{
  return NamedObject::makeContext ( "internal shape", myName_ );
}


//-----------------------------------------------------------------------
//   globalRank
//-----------------------------------------------------------------------


idx_t InternalShape::globalRank () const
{
  return localRank ();
}


//-----------------------------------------------------------------------
//   getBoundaryTopology
//-----------------------------------------------------------------------


util::Topology InternalShape::getBoundaryTopology () const
{
  const idx_t  bndCount = boundaryCount();

  idx_t        i, n;

  for ( i = n = 0; i < bndCount; i++ )
  {
    n += getBoundaryNodes(i).size();
  }

  IdxVector  offsets ( bndCount + 1 );
  IdxVector  indices ( n );

  offsets[0] = 0;

  for ( i = 0; i < bndCount; i++ )
  {
    IdxVector  nodes = getBoundaryNodes (i);

    offsets[i + 1] = offsets[i] + nodes.size();

    indices[slice(offsets[i],offsets[i + 1])] = nodes;
  }

  return Topology (
    jem::shape ( bndCount, nodeCount() ),
    offsets,
    indices
  );
}


//-----------------------------------------------------------------------
//   getBoundaryCoords
//-----------------------------------------------------------------------


void InternalShape::getBoundaryCoords

  ( const Matrix&  x,
    idx_t          ibnd,
    const Matrix&  c ) const

{
  IdxVector    inodes ( getBoundaryNodes(ibnd) );

  const idx_t  m      = c.size     (0);
  const idx_t  n      = inodes.size ();


  for ( idx_t j = 0; j < n; j++ )
  {
    idx_t  k = inodes[j];

    for ( idx_t i = 0; i < m; i++ )
    {
      x(i,j) = c(i,k);
    }
  }
}


//-----------------------------------------------------------------------
//   getNodeGradients
//-----------------------------------------------------------------------


void InternalShape::getVertexGradients

  ( const Cubix&   g,
    const Matrix&  c ) const

{
  Matrix       u = getVertexCoords ();

  const idx_t  n = u.size (1);

  for ( idx_t i = 0; i < n; i++ )
  {
    evalShapeGradients ( g[i], u[i], c );
  }
}


JIVE_END_PACKAGE( geom )
