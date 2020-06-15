#include <jem/base/System.h>
#include <jem/base/Array.h>
#include <jem/base/array/tensor.h>
#include <jem/numeric/algebra/utilities.h>

#include "voigtUtilities.h"
#include "LargeDispUtilities.h"

using jem::System;
using jem::idx_t;
using jem::ALL;
using jem::END;
using jem::TensorIndex;
using jem::numeric::norm2;
using jem::io::endl;

//-----------------------------------------------------------------------
//    evalDeformationGradient
//-----------------------------------------------------------------------

void  evalDeformationGradient

  ( const Matrix&  f,
    const Vector&  u,
    const Matrix&  g )

{
  idx_t rank = g.size(0);

  for ( idx_t i = 0; i < rank; ++i )
  {
    for ( idx_t j = 0; j < rank; ++j )
    {
      f(i,j) = dot ( g(j,ALL) , u[slice(i,END,rank)] );
    }
    f(i,i) += 1.;
  }
}
  
//-----------------------------------------------------------------------
//    evalDeformationGradient
//-----------------------------------------------------------------------

void  evalDeformationGradient

  ( const Matrix&  f,
    const Vector&  u,
    const Matrix&  gii,
    const Matrix&  gij )

{
  idx_t rank = gii.size(0);

  for ( idx_t i = 0; i < rank; ++i )
  {
    for ( idx_t j = 0; j < rank; ++j )
    {
      const Matrix& g = ( i == j ) ? gii : gij;

      f(i,j) = dot ( g(j,ALL) , u[slice(i,END,rank)] );
    }
    f(i,i) += 1.;
  }
}


//-----------------------------------------------------------------------
//    getGreenLagrangeStrain
//-----------------------------------------------------------------------

void  getGreenLagrangeStrain

  ( const Vector&  eps,
    const Matrix&  f )

{
  idx_t rank = f.size(0);

  // Compute Green-Lagrange strain tensor: 1/2 * ( F^T*F - I )

  TensorIndex i,j,k;
  Matrix      tens( rank, rank );

  tens(i,j) = 0.5 * ( dot( f(k,i), f(k,j), k ) - where(i==j,1.,0.) );

  // Convert to vector (Voigt notation)
  
  voigtUtilities::tensor2VoigtStrain ( eps, tens );
}

//-----------------------------------------------------------------------
//    rotateNormalVector
//-----------------------------------------------------------------------

Vector  rotateNormalVector

  ( const Vector&  n0,
    const Matrix&  f )

{
  // idx_t rank = f.size(0);

  // Only implemented for 2D!!

  JEM_ASSERT ( f.size(0) == 2 );

  // Evaluate det(F)(F^T)^(-1)n

  Vector nUpdated(2);

  nUpdated[0] =   f(1,1)*n0[0] - f(1,0)*n0[1];
  nUpdated[1] = - f(0,1)*n0[0] + f(0,0)*n0[1] ;

  // Return normalized vector
  
  double l = norm2( nUpdated );
  
  nUpdated /= l;

  return nUpdated ;
};

//-----------------------------------------------------------------------
//    updateNormalMatrix
//-----------------------------------------------------------------------

void  updateNormalMatrix

  ( const Matrix&  nMat,
    const Vector&  nVec )

{
  JEM_ASSERT ( nVec.size() == 2 );
  JEM_ASSERT ( nMat.size( 0 ) == 2 );
  JEM_ASSERT ( nMat.size( 1 ) == 3 );

  nMat(0,0) = nVec[0];
  nMat(0,1) = 0.;
  nMat(0,2) = nVec[1];
  nMat(1,0) = 0.;
  nMat(1,1) = nVec[1];
  nMat(1,2) = nVec[0];
}

//-----------------------------------------------------------------------
//    updateRotationMatrix
//-----------------------------------------------------------------------

void  updateRotationMatrix

  ( const Matrix&  Q,
    const Vector&  nVec )

{
  Q(0,0) =   nVec[0];
  Q(0,1) =   nVec[1];
  Q(1,0) =   nVec[1];
  Q(1,1) = - nVec[0];
}

//-----------------------------------------------------------------------
//    updateMatrices
//-----------------------------------------------------------------------

void  updateMatrices

  ( const Matrix&  Q,
    const Matrix&  dPhidF,
    const Vector&  n0,
    const Matrix&  f )

{
  // idx_t rank = f.size(0);

  // Only implemented for 2D!!

  JEM_ASSERT ( f.size(0) == 2 );

  // Evaluate det(F)(F^T)^(-1)n

  double nBar0 =   f(1,1)*n0[0] - f(1,0)*n0[1];
  double nBar1 = - f(0,1)*n0[0] + f(0,0)*n0[1] ;

  // Normalize vector
  
  double l2 = nBar0 * nBar0 + nBar1 * nBar1;

  double l = sqrt( l2 );
  
  Vector n ( 2 );

  n[0] = nBar0 / l;
  n[1] = nBar1 / l;

  // Fill matrices

  // Vector n = rotateNormalVector ( n0, f );
  updateRotationMatrix ( Q, n );

  dPhidF(0,0) =   nBar0 * n0[1] / l2;
  dPhidF(0,1) = - nBar0 * n0[0] / l2;
  dPhidF(1,0) =   nBar1 * n0[1] / l2;
  dPhidF(1,1) = - nBar1 * n0[0] / l2;
}

//-----------------------------------------------------------------------
//    getBMatrixLin2D
//-----------------------------------------------------------------------

void  getBMatrixLin2D

  ( const Matrix&  b,
    const Matrix&  f,
    const Matrix&  g )

{
  JEM_ASSERT   ( b.size(0) == 3 &&
                 g.size(0) == 2 &&
                 f.size(0) == 2 &&
                 f.size(1) == 2 &&
                 b.size(1) == 2 * g.size(1) );

  const idx_t  nodeCount = g.size (1);

  for ( idx_t inode = 0; inode < nodeCount; inode++ )
  {
    idx_t  i0 = 2 * inode;
    idx_t  i1 = i0 + 1;

    b(0,i0) = f(0,0) * g(0,inode);
    b(0,i1) = f(1,0) * g(0,inode);
    b(1,i0) = f(0,1) * g(1,inode);
    b(1,i1) = f(1,1) * g(1,inode);

    b(2,i0) = f(0,0) * g(1,inode) + f(0,1) * g(0,inode);
    b(2,i1) = f(1,1) * g(0,inode) + f(1,0) * g(1,inode);
  }
}

//-----------------------------------------------------------------------
//    getBMatrixLin3D
//-----------------------------------------------------------------------

void  getBMatrixLin3D

  ( const Matrix&  b,
    const Matrix&  f,
    const Matrix&  g )

{
  JEM_ASSERT   ( b.size(0) == 6 &&
                 g.size(0) == 3 &&
                 f.size(0) == 3 &&
                 f.size(1) == 3 &&
                 b.size(1) == 3 * g.size(1) );

  const idx_t  nodeCount = g.size (1);

  for ( idx_t inode = 0; inode < nodeCount; inode++ )
  {
    idx_t  i0 = 3 * inode;
    idx_t  i1 = i0 + 1;
    idx_t  i2 = i0 + 2;

    b(0,i0) = f(0,0) * g(0,inode);
    b(0,i1) = f(1,0) * g(0,inode);
    b(0,i2) = f(2,0) * g(0,inode);
    b(1,i0) = f(0,1) * g(1,inode);
    b(1,i1) = f(1,1) * g(1,inode);
    b(1,i2) = f(2,1) * g(1,inode);
    b(2,i0) = f(0,2) * g(2,inode);
    b(2,i1) = f(1,2) * g(2,inode);
    b(2,i2) = f(2,2) * g(2,inode);

    b(3,i0) = f(0,0) * g(1,inode) + f(0,1) * g(0,inode);
    b(3,i1) = f(1,0) * g(1,inode) + f(1,1) * g(0,inode);
    b(3,i2) = f(2,0) * g(1,inode) + f(2,1) * g(0,inode);

    b(4,i0) = f(0,1) * g(2,inode) + f(0,2) * g(1,inode);
    b(4,i1) = f(1,1) * g(2,inode) + f(1,2) * g(1,inode);
    b(4,i2) = f(2,1) * g(2,inode) + f(2,2) * g(1,inode);

    b(5,i0) = f(0,2) * g(0,inode) + f(0,0) * g(2,inode);
    b(5,i1) = f(1,2) * g(0,inode) + f(1,0) * g(2,inode);
    b(5,i2) = f(2,2) * g(0,inode) + f(2,0) * g(2,inode);
  }
}
 
//-----------------------------------------------------------------------
//    getBMatrixLin2D
//-----------------------------------------------------------------------

void  getBMatrixLin2D

  ( const Matrix&  b,
    const Matrix&  f,
    const Matrix&  gii,
    const Matrix&  gij )

{
  JEM_ASSERT   ( b.size(0) == 3 &&
                 gii.size(0) == 2 &&
                 f.size(0) == 2 &&
                 f.size(1) == 2 &&
                 b.size(1) == 2 * gii.size(1) );

  JEM_ASSERT   ( gij.size(0) == gii.size(0) &&
                 gij.size(1) == gii.size(1) );

  const idx_t  nodeCount = gii.size (1);

  for ( idx_t inode = 0; inode < nodeCount; inode++ )
  {
    idx_t  i0 = 2 * inode;
    idx_t  i1 = i0 + 1;

    b(0,i0) = f(0,0) * gii(0,inode);
    b(0,i1) = f(1,0) * gii(0,inode);
    b(1,i0) = f(0,1) * gii(1,inode);
    b(1,i1) = f(1,1) * gii(1,inode);

    b(2,i0) = f(0,0) * gij(1,inode) + f(0,1) * gij(0,inode);
    b(2,i1) = f(1,1) * gij(0,inode) + f(1,0) * gij(1,inode);
  }
}


//-----------------------------------------------------------------------
//    getBMatrixNonlin2D
//-----------------------------------------------------------------------

void  getBMatrixNonlin2D

  ( const Matrix&  b,
    const Matrix&  g )

{
  JEM_ASSERT   ( b.size(0) == 4 &&
                 g.size(0) == 2 &&
                 b.size(1) == 2 * g.size(1) );

  const idx_t  nodeCount = g.size (1);

  b = 0.0;

  for ( idx_t inode = 0; inode < nodeCount; inode++ )
  {
    idx_t  i0 = 2 * inode;
    idx_t  i1 = i0 + 1;

    b(0,i0) = g(0,inode);
    b(1,i0) = g(1,inode);

    b(2,i1) = g(0,inode);
    b(3,i1) = g(1,inode);
  }
}


//-----------------------------------------------------------------------
//    addElemMatLargeD_
//-----------------------------------------------------------------------

void addElemMatLargeD_

  ( const Matrix& k,
    const Vector& tau,
    const Matrix& g,
    const double  w )

{
  idx_t rank = g.size(0);
  idx_t nn   = g.size(1);

  JEM_ASSERT    ( k.size(0) == rank*nn &&
                  k.size(1) == rank*nn );

  Matrix   t  ( rank, rank );

  voigtUtilities::voigt2TensorStress( t, tau );

  JEM_ASSERT    ( t.size(0) == rank &&
                  t.size(1) == rank );

  Matrix   btb ( nn, nn );

  TensorIndex in,jn,it,jt;

  btb(in,jn) = dot( g(jt,in), 
                  dot( t(it,jt), g(it,jn), it ), 
                  jt );

  for ( idx_t in = 0; in < nn; ++in )
  {
    for ( idx_t jn = 0; jn < nn; ++jn )
    {
      for ( idx_t ix = 0; ix < rank; ++ix )
      {
        k( in*rank+ix, jn*rank+ix ) += w*btb(in,jn);
      }
    }
  }
}

//-----------------------------------------------------------------------
//    addElemMatLargeD_
//-----------------------------------------------------------------------

void addElemMatLargeD_

  ( const Matrix& k,
    const Vector& tau,
    const Matrix& gii,
    const Matrix& gij,
    const double  w )

{
  idx_t rank = gii.size(0);
  idx_t nn   = gii.size(1);

  JEM_ASSERT    ( gij.size(0) == rank &&
                  gij.size(1) == nn );

  JEM_ASSERT    ( k.size(0) == rank*nn &&
                  k.size(1) == rank*nn );

  Matrix   t  ( rank, rank );

  voigtUtilities::voigt2TensorStress( t, tau );

  Matrix   btb ( nn, nn );

  TensorIndex in,jn;

  btb = 0.;
  for ( idx_t it = 0; it < rank; ++it )
  {
    for ( idx_t jt = 0; jt < rank; ++jt )
    {
      if ( it == jt )
      {
        btb(in,jn) += gii(jt,in) * t(it,jt) * gii(it,jn);
      }
      else
      {
        btb(in,jn) += gij(jt,in) * t(it,jt) * gij(it,jn);
      }
    }
  }

  for ( idx_t in = 0; in < nn; ++in )
  {
    for ( idx_t jn = 0; jn < nn; ++jn )
    {
      for ( idx_t ix = 0; ix < rank; ++ix )
      {
        k( in*rank+ix, jn*rank+ix ) += w*btb(in,jn);
      }
    }
  }
}

//-----------------------------------------------------------------------
//    getNominalStress
//-----------------------------------------------------------------------

void   getNominalStress

  ( const Matrix& stre,
    const Vector& pk2,
    const Matrix& f )

{
  JEM_ASSERT( f.size(1)    == 2 && 
              pk2.size()   == 3 &&
              stre.size(0) == 2 &&
              stre.size(1) == 2 );

  stre(0,0) = f(0,0)*pk2[0] + f(0,1)*pk2[2];
  stre(0,1) = f(0,0)*pk2[2] + f(0,1)*pk2[1];
  stre(1,0) = f(1,0)*pk2[0] + f(1,1)*pk2[2];
  stre(1,1) = f(1,0)*pk2[2] + f(1,1)*pk2[1];
}


//-----------------------------------------------------------------------
//    getNominalStiff
//-----------------------------------------------------------------------

void   getNominalStiff

  ( const Cubix & nK,
    const Matrix& K,
    const Matrix& f )

{
  idx_t nDof = nK.size(2);

  for ( idx_t i = 0; i < nDof; ++i ) 
  {
    Matrix  thisNK = nK(ALL,ALL,i);
    getNominalStress ( thisNK, K(ALL,i), f );
  }
}


