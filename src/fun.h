

#include <jive/Array.h>
#include <jem/base/Tuple.h>


using jive::Vector;
using jive::Matrix;
using jive::IntVector;
using jem::Tuple;
using jem::idx_t;



void                 getInvariant 

  ( Matrix  a,
    Vector& b);

void                 getgradeI1

  ( Matrix& a, 
    Matrix& I1);  

void                 getgradeI2

  ( Matrix& a, 
    Matrix& I2);  

void                 getgradeI3

  ( Matrix& a, 
    Matrix& I3);  


// Solving cubic equation
// a*x^3 + b*x^2 + c*x + d = 0
// only for functions with real roots
// returns the number of roots


idx_t                 solveCubicEquation

  ( Vector&          ans,
    const double     a,
    const double     b,
    const double     c,
    const double     d);

void                  getOmegaandDeomega  
    (  double&    omega,
       double&    deomega,
       double&    epsBar,      
       double&    kappa0_,
       double&    kappau_);

void                  getparameters
    (  double&  omega, 
       double&  deomega,
       Matrix&  S,
       Matrix&  debdeb,
       double&  kappa0_,
       double&  kappau_,
       idx_t&   rank_);

void                  getparameters2
    (  double&  omegam, 
       double&  deomegam,
       double&  omegaf,       
       double&  deomegaf,
       Matrix&  S,
       Matrix&  debdeb,
       double&  kappam0_,
       double&  kappamu_,
       double&  kappaf0_,
       double&  kappafu_,
       idx_t&   rank_);





















