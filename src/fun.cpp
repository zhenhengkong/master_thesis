#include <jem/base/System.h>
#include <jem/base/array/select.h>
#include <jem/base/array/utilities.h>
#include <jem/base/PrecheckException.h>
#include <jem/base/Error.h>
#include <jem/numeric/utilities.h>
#include <jem/base/limits.h>
#include <jem/base/array/operators.h>
#include <jem/util/Properties.h>
#include <jem/numeric/algebra/matmul.h>
#include <jem/numeric/algebra/LUSolver.h>

#include "fun.h"
#include "voigtUtilities.h"
#include "Invariants.h"
#include "utilities.h"
#include "NeoHookeNLMaterial.h"

using namespace jem;
using namespace voigtUtilities;
using jem::numeric::matmul;
using jem::numeric::LUSolver;
using jem::ALL;

const double PI               = 3.14159265;
const double RAD_120          = 0.666666667 * PI;
const double RAD_240          = 2 * RAD_120;

const double ONE_THIRD        = 0.33333333333;

const double EPS              = 1.e-16;


void    getInvariant 

  ( Matrix   a,
    Vector&  b)
{
  b[0] = a(0,0) + a(1,1) + a(2,2);
 
  b[1] = a(0,0)*a(1,1) + a(1,1)*a(2,2) + a(0,0)*a(2,2) - 
         a(0,1)*a(1,0) - a(1,2)*a(2,1) + a(0,2)*a(2,0); 
 
  b[2] =  - a(0,2)*a(1,1)*a(2,0) + a(0,1)*a(1,2)*a(2,0) +
          a(0,2)*a(1,0)*a(2,1) - a(0,0)*a(1,2)*a(2,1) -
          a(0,1)*a(1,0)*a(2,2) + a(0,0)*a(1,1)*a(2,2); 
}  

void getgradeI1   
                   
  ( Matrix& a, 
    Matrix& I1)
    
{
  I1(0,0)= 1.;
  I1(0,1)= 0.; 
  I1(0,2)= 0.;
  I1(1,0)= 0.;
  I1(1,1)= 1.;
  I1(1,2)= 0.;
  I1(2,0)= 0.;
  I1(2,1)= 0.;
  I1(2,2)= 1.;
}

void getgradeI2   
                   
  ( Matrix& a, 
    Matrix& I2)
{
  I2(0,0)= a(1,1) + a(2,2);
  I2(0,1)=-a(1,0); 
  I2(0,2)=-a(2,0);
  I2(1,0)=-a(0,1);
  I2(1,1)= a(0,0) + a(2,2);
  I2(1,2)=-a(2,1);
  I2(2,0)=-a(0,2);
  I2(2,1)=-a(1,2);
  I2(2,2)= a(1,1) + a(0,0);
}

void getgradeI3   
                   
  ( Matrix& a, 
    Matrix& I3)
{
  I3(0,0)=-a(1,2)*a(2,1) + a(1,1)*a(2,2);
  I3(0,1)= a(1,2)*a(2,0) - a(1,0)*a(2,2);
  I3(0,2)=-a(1,1)*a(2,0) + a(1,0)*a(2,1);
  I3(1,0)= a(0,2)*a(1,1) - a(0,1)*a(2,2);
  I3(1,1)=-a(0,2)*a(2,0) + a(0,0)*a(2,2);
  I3(1,2)= a(0,1)*a(2,0) - a(0,0)*a(2,1);
  I3(2,0)=-a(0,2)*a(1,1) + a(0,1)*a(1,2);
  I3(2,1)= a(0,2)*a(1,0) - a(0,0)*a(1,2);
  I3(2,2)=-a(0,1)*a(1,0) + a(0,0)*a(1,1);   
}





idx_t                 solveCubicEquation

  ( Vector&          ans, 
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


void getOmegaandDeomega  
    (   double&    omega,
        double&    deomega,
        double&    epsBar,      
        double&    kappa0_,
        double&    kappau_)
{   
 
  omega = kappau_ * (epsBar - kappa0_) / (epsBar * (kappau_ - kappa0_) ); 

  deomega  = kappau_ * kappa0_ / ( epsBar * epsBar * ( kappau_ - kappa0_ )); 

  if ( omega < 0.0)
    {
      omega   = 0.0;
      deomega = 0.0;
    }
  else if ( omega > 1.0)
    { 
        omega   = 1.0;
        deomega = 0.0; 
    }
}



void getparameters
    (  double&  omega, 
       double&  deomega,
       Matrix&  S,
       Matrix&  debdeb,
       double&  kappa0_,
       double&  kappau_,
       idx_t&   rank_)
{

  Vector invs(3); 
  Vector princ(3);
  Matrix dI1 (3,3);
  Matrix dI2 (3,3); 
  Matrix dI3 (3,3); 
  Matrix C (3,3);
  Matrix deb (3,3);

  C = 0.0;

 for ( idx_t i = 0; i < rank_ ; i++ )  
    {
       for ( idx_t j = 0; j < rank_ ; j++ )
         
           {
            
               C(i,j) = S(i,j);

           }   
     } 


  getInvariant  ( C, invs );
  getgradeI1 ( C, dI1 );  
  getgradeI2 ( C, dI2 );  
  getgradeI3 ( C, dI3 );  


const idx_t  n  = solveCubicEquation ( princ, 1.0, -invs[0], invs[1], -invs[2] );


for ( idx_t i = n; i < 3; ++i )
{
    princ[i] = 0.0;
}

double epsBar = 0.0;

for (idx_t i=0; i<3; i++)
{
   if (princ[i] >0 )
      epsBar += princ[i]*princ[i];
}

//System::out() << 'A' << epsBar << endl;

epsBar = sqrt(epsBar);



double I1 = invs[0];
double I2 = invs[1];

       deb = 0.0;

for ( idx_t i = 0; i < 3; i++)
{
    
    double vi = princ[i];

    if ( vi > 0.) 
    {
   
       double tmp = 1.0 / ( 3.0 * vi * vi - 2.0 * I1 * vi + I2);


       for ( idx_t j = 0; j < 3; j++) 
       {
           
            for ( idx_t k = 0; k < 3; k++)
            {

                 double dEidEj = tmp * ( vi *vi * dI1(j,k) - vi * dI2(j,k) + dI3(j,k) );
                 
                 deb(j,k) += vi * dEidEj;    

           }
       }
     }
}
  

  if ( epsBar > 0.0) 
  {
  deb *= ( 1.0 / epsBar);
  }




 for ( idx_t i = 0; i < rank_ ; i++ )  
    {
       for ( idx_t j = 0; j < rank_ ; j++ )
         
           {
            
               debdeb(i,j) = deb(i,j);

           }   
     } 





//System::out() << 'B' << deb << endl;



  getOmegaandDeomega  ( omega, deomega, epsBar, kappa0_, kappau_);

     
//  System::out() << 'A' << omega << endl;
//System::out() << 'B' << deomega << endl;


}


void  getparameters2
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
       idx_t&   rank_)
{

  Vector invs(3); 
  Vector princ(3);
  Matrix dI1 (3,3);
  Matrix dI2 (3,3); 
  Matrix dI3 (3,3); 
  Matrix C (3,3);
  Matrix deb (3,3);

  C = 0.0;

 for ( idx_t i = 0; i < rank_ ; i++ )  
    {
       for ( idx_t j = 0; j < rank_ ; j++ )
         
           {
            
               C(i,j) = S(i,j);

           }   
     } 

//  System::out() << 'A' << C << endl;

  getInvariant  ( C, invs );
  getgradeI1 ( C, dI1 );  
  getgradeI2 ( C, dI2 );  
  getgradeI3 ( C, dI3 );  


const idx_t  n  = solveCubicEquation ( princ, 1.0, -invs[0], invs[1], -invs[2] );


for ( idx_t i = n; i < 3; ++i )
{
    princ[i] = 0.0;
}

double epsBar = 0.0;


for (idx_t i=0; i<3; i++)
{
   if (princ[i] >0 )
      epsBar += princ[i]*princ[i];
}

epsBar = sqrt(epsBar);


double I1 = invs[0];
double I2 = invs[1];

       deb = 0.0;

for ( idx_t i = 0; i < 3; i++)
{
    
    double vi = princ[i];

    if ( vi > 0.) 
    {
   
       double tmp = 1.0 / ( 3.0 * vi * vi - 2.0 * I1 * vi + I2);


       for ( idx_t j = 0; j < 3; j++) 
       {
           
            for ( idx_t k = 0; k < 3; k++)
            {

                 double dEidEj = tmp * ( vi *vi * dI1(j,k) - vi * dI2(j,k) + dI3(j,k) );
                 
                 deb(j,k) += vi * dEidEj;    

           }
       }
     }
}

  
  if ( epsBar > 0.0) 
  {
  deb *= ( 1.0 / epsBar);
  }


 for ( idx_t i = 0; i < rank_ ; i++ )  
    {
       for ( idx_t j = 0; j < rank_ ; j++ )
         
           {
            
               debdeb(i,j) = deb(i,j);

           }   
     } 




  getOmegaandDeomega  ( omegam, deomegam, epsBar, kappam0_, kappamu_);

  getOmegaandDeomega  ( omegaf, deomegaf, epsBar, kappaf0_, kappafu_);


//  System::out() << 'A' << omegam << endl;

//  System::out() << 'B' << deomegam << endl;

//  System::out() << 'C' << omegaf << endl;

//  System::out() << 'D' << deomegaf << endl;



}





























  
