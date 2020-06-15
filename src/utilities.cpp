
#include <jem/base/array/select.h>
#include <jem/base/array/utilities.h>
#include <jem/base/PrecheckException.h>
#include <jem/base/Error.h>
#include <jem/numeric/utilities.h>

#include "utilities.h"

extern "C"
{
  #include  <math.h>
}

using jem::ALL;

//-----------------------------------------------------------------------
//   constants
//-----------------------------------------------------------------------


const idx_t  STRAIN_COUNTS[4] = { 0, 1, 3, 6 };

const double PI               = 3.14159265;
const double RAD_120          = 0.666666667 * PI;
const double RAD_240          = 2 * RAD_120;

const double ONE_THIRD        = 0.33333333333;

const double EPS              = 1.e-16;


//-----------------------------------------------------------------------
//   get1DShapeGrads
//-----------------------------------------------------------------------


void              get1DShapeGrads

  ( const Matrix&   b,
    const Matrix&   g )

{
  JEM_ASSERT ( b.size(0) == 1 &&
                 g.size(0) == 1 &&
                 b.size(1) == g.size(1) );

  b = g;
}


//-----------------------------------------------------------------------
//   get2DShapeGrads
//-----------------------------------------------------------------------


void              get2DShapeGrads

  ( const Matrix&   b,
    const Matrix&   g )

{
  JEM_ASSERT ( b.size(0) == 3 &&
                 g.size(0) == 2 &&
                 b.size(1) == 2 * g.size(1) );

  const idx_t  nodeCount = g.size (1);


  b = 0.0;

  for ( idx_t inode = 0; inode < nodeCount; inode++ )
  {
    idx_t  i = 2 * inode;

    b(0,i + 0) = g(0,inode);
    b(1,i + 1) = g(1,inode);

    b(2,i + 0) = g(1,inode);
    b(2,i + 1) = g(0,inode);
  }
}


//-----------------------------------------------------------------------
//   get3DShapeGrads
//-----------------------------------------------------------------------


void              get3DShapeGrads

  ( const Matrix&   b,
    const Matrix&   g )

{
  JEM_ASSERT ( b.size(0) == 6 &&
               g.size(0) == 3 &&
               b.size(1) == 3 * g.size(1) );

  const idx_t  nodeCount = g.size (1);


  b = 0.0;

  for ( idx_t inode = 0; inode < nodeCount; inode++ )
  {
    idx_t  i = 3 * inode;

    b(0,i + 0) = g(0,inode);
    b(1,i + 1) = g(1,inode);
    b(2,i + 2) = g(2,inode);

    b(3,i + 0) = g(1,inode);
    b(3,i + 1) = g(0,inode);

    b(4,i + 1) = g(2,inode);
    b(4,i + 2) = g(1,inode);

    b(5,i + 2) = g(0,inode);
    b(5,i + 0) = g(2,inode);
  }
}


//-----------------------------------------------------------------------
//   getShapeGradsFunc
//-----------------------------------------------------------------------


ShapeGradsFunc getShapeGradsFunc ( idx_t rank )
{
  JEM_ASSERT ( rank >= 1 && rank <= 3 );


  if      ( rank == 1 )
  {
    return & get1DShapeGrads;
  }
  else if ( rank == 2 )
  {
    return & get2DShapeGrads;
  }
  else
  {
    return & get3DShapeGrads;
  }
}


// --------------------------------------------------------------------
//  get1DShapeFuncs
// --------------------------------------------------------------------

void                  get1DShapeFuncs

  ( const Matrix&       sfuncs,
    const Vector&       n )
{
  sfuncs(0,0) = n[0];
}

// --------------------------------------------------------------------
//  get2DShapeFuncs
// --------------------------------------------------------------------

void                  get2DShapeFuncs

  ( const Matrix&       s,
    const Vector&       n )
{
  JEM_ASSERT ( s.size(0) == 2 &&
               s.size(1) == 2 * n.size() );

  const idx_t  nodeCount = n.size ();

  s = 0.0;

  for ( idx_t inode = 0; inode < nodeCount; inode++ )
  {
    idx_t  i = 2 * inode;

    s(0,i + 0) = n[inode];
    s(1,i + 1) = n[inode];
  }
}

// --------------------------------------------------------------------
//  get3DShapeFuncs
// --------------------------------------------------------------------

void                  get3DShapeFuncs

  ( const Matrix&       s,
    const Vector&       n )
{
  JEM_ASSERT ( s.size(0) == 3 &&
                 s.size(1) == 3 * n.size() );

  const idx_t  nodeCount = n.size ();

  s = 0.0;

  for ( idx_t inode = 0; inode < nodeCount; inode++ )
  {
    idx_t  i = 3 * inode;

    s(0,i + 0) = n[inode];
    s(1,i + 1) = n[inode];
    s(2,i + 2) = n[inode];
  }
}

// A function that returns a pointer to a function that computes the
// B-matrix given the number of spatial dimensions.

ShapeFuncsFunc   getShapeFuncsFunc

  ( idx_t               rank )

{
  JEM_ASSERT ( rank >= 1 && rank <= 3 );


  if      ( rank == 1 )
  {
    return & get1DShapeFuncs;
  }
  else if ( rank == 2 )
  {
    return & get2DShapeFuncs;
  }
  else
  {
    return & get3DShapeFuncs;
  }
}

//-----------------------------------------------------------------------
//   getGradUFunc
//-----------------------------------------------------------------------

GradUFunc getGradUFunc ( idx_t rank )
{
  JEM_ASSERT ( rank >= 1 && rank <= 3 );


  if      ( rank == 1 )
  {
    return & get1DGradU;
  }
  else if ( rank == 2 )
  {
    return & get2DGradU;
  }
  else
  {
    return & get3DGradU;
  }
}

//-----------------------------------------------------------------------
//   get1DGradU
//-----------------------------------------------------------------------

void get1DGradU

  ( const Matrix&       gradU,
    const Matrix&       gradN,
    const Vector&       elemDisp )

{
  using namespace jem;

  gradU(0,0) = dot ( elemDisp, gradN(0,ALL ) );
}

//-----------------------------------------------------------------------
//   get2DGradU
//-----------------------------------------------------------------------

void get2DGradU

  ( const Matrix&       gradU,
    const Matrix&       gradN,
    const Vector&       elemDisp )

{
  // gradU = du_i/dx_j,  size: ( rank,rank )
  // gradN = dN_i/dx_j,  size: ( rank,nodeCount )
  // elemDisp = u        numbering: (0x,0y,1x,1y...nx,ny)

  using namespace jem;

  Vector ux   ( elemDisp[slice(0,END,2)] );
  Vector uy   ( elemDisp[slice(1,END,2)] );
  Vector dNdx ( gradN(0,ALL)             );
  Vector dNdy ( gradN(1,ALL)             );

  gradU(0,0) = dot ( ux, dNdx );
  gradU(0,1) = dot ( ux, dNdy );
  gradU(1,0) = dot ( uy, dNdx );
  gradU(1,1) = dot ( uy, dNdy );
}

//-----------------------------------------------------------------------
//   get3DGradU
//-----------------------------------------------------------------------

void get3DGradU

  ( const Matrix&       gradU,
    const Matrix&       gradN,
    const Vector&       elemDisp )

{
  using namespace jem;

  Vector ux   ( elemDisp[slice(0,END,3)] );
  Vector uy   ( elemDisp[slice(1,END,3)] );
  Vector uz   ( elemDisp[slice(2,END,3)] );
  Vector dNdx ( gradN(0,ALL)             );
  Vector dNdy ( gradN(1,ALL)             );
  Vector dNdz ( gradN(2,ALL)             );

  gradU(0,0) = dot ( ux, dNdx );
  gradU(0,1) = dot ( ux, dNdy );
  gradU(0,2) = dot ( ux, dNdy );
  gradU(1,0) = dot ( uy, dNdx );
  gradU(1,1) = dot ( uy, dNdy );
  gradU(1,2) = dot ( uy, dNdy );
  gradU(2,0) = dot ( uy, dNdx );
  gradU(2,1) = dot ( uy, dNdy );
  gradU(2,2) = dot ( uy, dNdy );
}


//-----------------------------------------------------------------------
//   solveLinearEqua
//-----------------------------------------------------------------------

void                  solveLinearEqua

  ( double&       ans,
    const double  a,
    const double  b )
{
  ans = -b/a;
}

//-----------------------------------------------------------------------
//   solveQuadEqua
//-----------------------------------------------------------------------

void                  solveQuadEqua

  ( Vector&       ans,
    const double  a,
    const double  b,
    const double  c )
{
  if ( jem::numeric::abs(a) < EPS )
  {
    ans.resize( 1 );
    ans[0] = - c / b;
  }
  else
  {
    double d = b * b - 4.0 * a * c;
    
    if ( d < 0 )
    {
      using namespace jem;
      throw Error (
      JEM_FUNC,
      " imaginary principal values !!! "
      );  
    }
    else
    {
      ans.resize( 2 );
      ans[0] = 0.5 * (-b + sqrt( d )) / a;
      ans[1] = 0.5 * (-b - sqrt( d )) / a;
    }
  }
}

//-----------------------------------------------------------------------
//   solveCubicEqua
//-----------------------------------------------------------------------

idx_t                 solveCubicEqua

  ( Tuple<double,3>& ans, 
    const double     a,
    const double     b,
    const double     c,
    const double     d)

{  
  ans = NAN;

  if ( jem::numeric::abs(a) < EPS )
  {
    if ( jem::numeric::abs(b) < EPS )
    {
      ans[0] = - d / c;

      return 1;
    }
    else
    {
      double D = c * c - 4.0 * b * d;
      
      if ( D < 0.0 )
      {        
        return 0;
      }
      else
      {
        D      = sqrt(D);

        ans[0] = 0.5 * (-c + D) / b;
        ans[1] = 0.5 * (-c - D) / b;
       
        return 2;
      }
    }
  }
  else
  {
    double kk  = 1.0 / a;
    
    double aa  = b * kk;
    double bb  = c * kk;
    double cc  = d * kk;

    double aa3 = aa / 3.0;

    double p, q, r;
    double phi, help;
   
    q  = ( aa * aa - 3.0 * bb ) / 9.0;
    r  = ( 2.0 * aa * aa * aa - 9.0 * aa * bb + 27.0 * cc ) / 54.0;
 
    help = r / sqrt( q * q * q );
   
    if ( jem::numeric::abs (help) > 1.0 )
    {
      help = ( help < 0 ) ? -1.0 : 1.0; // prevent rounding errors
    }
   
    phi = acos ( help );
    p   = sqrt ( q    );

    ans[0] = -2.0 * p * cos ( phi / 3.0 )           - aa3;
    ans[1] = -2.0 * p * cos ( phi / 3.0 + RAD_120 ) - aa3;
    ans[2] = -2.0 * p * cos ( phi / 3.0 - RAD_120 ) - aa3;

    return 3;
  }
}
  
//-----------------------------------------------------------------------
//   invert2x2
//-----------------------------------------------------------------------

void  invert2x2

  ( const Matrix& m )

{
  // no check for size and singularity!

  double d = m(0,0) * m(1,1) - m(0,1) * m(1,0);
  double s = 1. / d;

  double t = m(0,0);

  m(0,0) =  m(1,1) * s;
  m(0,1) = -m(0,1) * s;
  m(1,0) = -m(1,0) * s;
  m(1,1) =  t      * s;
}


//-----------------------------------------------------------------------
//   evalMcAuley
//-----------------------------------------------------------------------

double                evalMcAuley

  ( const double x )
{
  return (x > 0 ? x : 0.0);
}

//-----------------------------------------------------------------------
//    evalHeaviside
//-----------------------------------------------------------------------

double               evalHeaviside

  ( const double x )
{
  return (x < 0 ? -1.0 : 1.0);
} 

