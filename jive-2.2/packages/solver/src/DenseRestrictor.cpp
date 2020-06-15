
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


#include <jem/base/limits.h>
#include <jem/base/Array.h>
#include <jem/base/ClassTemplate.h>
#include <jem/numeric/algebra/matmul.h>
#include <jem/numeric/algebra/utilities.h>
#include <jive/util/error.h>
#include <jive/algebra/utilities.h>
#include <jive/solver/DenseRestrictor.h>


JEM_DEFINE_CLASS( jive::solver::DenseRestrictor );


JIVE_BEGIN_PACKAGE( solver )


using jive::util::sizeError;
using jive::algebra::denseToSparse;


//=======================================================================
//   class DenseRestrictor
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


DenseRestrictor::DenseRestrictor ( const String& name ) :

  Super ( name )

{
  events_ = ~0x0;
}


DenseRestrictor::~DenseRestrictor ()
{}


//-----------------------------------------------------------------------
//   shape
//-----------------------------------------------------------------------


AbstractMatrix::Shape DenseRestrictor::shape () const
{
  if ( events_ )
  {
    const_cast<Self*> ( this ) -> update ();
  }

  return Shape ( basis_.size(1), basis_.size(0) );
}


//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------


void DenseRestrictor::update ()
{
  using jem::isTiny;
  using jem::numeric::norm2;

  // This is to avoid infinite recursions.

  if ( ! events_ )
  {
    return;
  }

  events_ = 0;

  update_ ();

  // Normalize the restriction operator and remove zero vectors.

  idx_t  jcol = 0;

  for ( idx_t j = 0; j < basis_.size(1); j++ )
  {
    double  s = norm2 ( basis_[j] );

    if ( ! isTiny( s ) )
    {
      basis_[j] *= (1.0 / s);

      if ( jcol < j )
      {
        basis_[jcol] = basis_[j];
      }

      jcol++;
    }
  }

  if ( jcol < basis_.size(1) )
  {
    basis_.reshape ( basis_.size(0), jcol );
  }

  resetEvents ();
}


//-----------------------------------------------------------------------
//   matmul
//-----------------------------------------------------------------------


void DenseRestrictor::matmul

  ( const Vector&  lhs,
    const Vector&  rhs ) const

{
  if ( events_ )
  {
    const_cast<Self*> ( this ) -> update ();
  }

  if ( rhs.size() != basis_.size(0) )
  {
    sizeError ( JEM_FUNC, "right-hand vector",
                rhs.size(), basis_.size(0) );
  }

  if ( lhs.size() != basis_.size(1) )
  {
    sizeError ( JEM_FUNC, "left-hand vector",
                lhs.size(), basis_.size(1) );
  }

  jem::numeric::matmul ( lhs, basis_.transpose(), rhs );
}


//-----------------------------------------------------------------------
//   transmul
//-----------------------------------------------------------------------


void DenseRestrictor::transmul

  ( const Vector&  lhs,
    const Vector&  rhs ) const

{
  if ( events_ )
  {
    const_cast<Self*> ( this ) -> update ();
  }

  if ( rhs.size() != basis_.size(1) )
  {
    sizeError ( JEM_FUNC, "right-hand vector",
                rhs.size(), basis_.size(1) );
  }

  if ( lhs.size() != basis_.size(0) )
  {
    sizeError ( JEM_FUNC, "left-hand vector",
                lhs.size(), basis_.size(0) );
  }

  jem::numeric::matmul ( lhs, basis_, rhs );
}


//-----------------------------------------------------------------------
//   getBasis
//-----------------------------------------------------------------------


void DenseRestrictor::getBasis

  ( const Vector&  v,
    idx_t            j ) const

{
  using jive::util::indexError;

  if ( events_ )
  {
    const_cast<Self*> ( this ) -> update ();
  }

  if ( v.size() != basis_.size(0) )
  {
    sizeError  ( JEM_FUNC, "basis vector",
                 v.size(), basis_.size(0) );
  }

  if ( j < 0 || j >= basis_.size(1) )
  {
    indexError ( JEM_FUNC, "basis vector", j, basis_.size(1) );
  }

  v = basis_[j];
}


//-----------------------------------------------------------------------
//   getExtByID
//-----------------------------------------------------------------------


void* DenseRestrictor::getExtByID ( ExtensionID extID ) const
{
  if ( extID == LocalRestrictor::ID )
  {
    LocalRestrictor*  ext = const_cast<Self*> ( this );

    return ext;
  }

  if ( extID == SparseMatrixExt::ID )
  {
    SparseMatrixExt*  ext = const_cast<Self*> ( this );

    return ext;
  }

  return NULL;
}


//-----------------------------------------------------------------------
//   getStructure
//-----------------------------------------------------------------------


SparseStruct DenseRestrictor::getStructure () const
{
  if ( events_ )
  {
    const_cast<Self*> ( this ) -> update ();
  }

  return denseToSparse ( basis_.size(1), basis_.size(0) );
}


//-----------------------------------------------------------------------
//   toSparseMatrix
//-----------------------------------------------------------------------


SparseMatrix DenseRestrictor::toSparseMatrix () const
{
  if ( events_ )
  {
    const_cast<Self*> ( this ) -> update ();
  }

  return denseToSparse ( basis_.transpose() );
}


//-----------------------------------------------------------------------
//   cloneSparseMatrix
//-----------------------------------------------------------------------


SparseMatrix DenseRestrictor::cloneSparseMatrix () const
{
  if ( events_ )
  {
    const_cast<Self*> ( this ) -> update ();
  }

  return denseToSparse ( basis_.transpose().clone() );
}


JIVE_END_PACKAGE( solver )
