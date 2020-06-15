/*
 * 
 *  Copyright (C) 2018 TU Delft. All rights reserved.
 *  
 *  This class implements an anisotropic Gasser material.
 *  The isotropic NeoHookean part is implemented following
 *  Belytschko Sec. 5.4
 *
 *  Author: F.P. van der Meer, f.p.vandermeer@tudelft.nl, Dean Howes
 *  Date: January 2018
 *
 */


#include <jem/base/limits.h>
#include <jem/base/Error.h>
#include <jem/base/PrecheckException.h>
#include <jem/base/array/operators.h>
#include <jem/util/Properties.h>
#include <jem/numeric/algebra/matmul.h>
#include <jem/numeric/algebra/LUSolver.h>
#include <jem/numeric/utilities.h>
#include <jem/base/System.h>


#include "utilities.h"
#include "voigtUtilities.h"
#include "GasserNLMaterial.h"
#include "Invariants.h"
#include "fun.h"


using namespace jem;
using namespace voigtUtilities;
using jem::numeric::matmul;
using jem::numeric::LUSolver;


//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  GasserNLMaterial::LAMBDA_PROP  = "lambda";
const char*  GasserNLMaterial::MU_PROP      = "mu";
const char*  GasserNLMaterial::BETA_PROP  = "beta";

const char*  GasserNLMaterial::K1_A_PROP   = "k1_a";
const char*  GasserNLMaterial::K2_A_PROP   = "k2_a";

const char*  GasserNLMaterial::KAPPA_PROP   = "kappa";
const char*  GasserNLMaterial::STATE_PROP   = "state";

const char*  GasserNLMaterial::KAPPAM0_PROP = "kappam0";
const char*  GasserNLMaterial::KAPPAMU_PROP = "kappamu";

const char*  GasserNLMaterial::KAPPAF0_PROP = "kappaf0";
const char*  GasserNLMaterial::KAPPAFU_PROP = "kappafu";


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


GasserNLMaterial::GasserNLMaterial 

  ( idx_t rank, const Properties& globdat )
    : NLMaterial ( rank, globdat )
{
  JEM_PRECHECK ( rank >= 1 && rank <= 3 );

  lambda_ = 1.0;
  mu_     = 1.0;
  beta_   = 1.0;

  k1_a_ = 1.0;
  k2_a_ = 1.0;

  kappa_ = 1.0;

  kappam0_ = 1.0;
  kappamu_ = 1.0;

  kappaf0_ = 1.0;
  kappafu_ = 1.0;

}


GasserNLMaterial::~GasserNLMaterial ()
{}

GasserNLMaterial::Hist_::Hist_ () 
  : 
    damagem ( 0. ),
    damagef ( 0. ),
    loadingm ( 0 ),
    loadingf ( 0 )
{
}



//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void GasserNLMaterial::configure ( const Properties& props )
{
  props.get ( lambda_, LAMBDA_PROP );
  props.get ( mu_, MU_PROP, 0.0, maxOf( mu_ ) );
  props.get ( beta_, BETA_PROP );

  props.get ( k1_a_, K1_A_PROP );
  props.get ( k2_a_, K2_A_PROP );

  props.get ( kappa_, KAPPA_PROP );

  props.get ( kappam0_, KAPPAM0_PROP );
  props.get ( kappamu_, KAPPAMU_PROP );

  props.get ( kappaf0_, KAPPAF0_PROP );
  props.get ( kappafu_, KAPPAFU_PROP );

  idx_t strCount = STRAIN_COUNTS[rank_];

  // read problem type, plane stress ...

  if ( rank_ == 2  )
  {
    props.get( stateString_, STATE_PROP );

    if      ( stateString_ == "PLANE_STRAIN" )
    {
      state_ = PlaneStrain;
    }
    else if ( stateString_ == "PLANE_STRESS" )
    {
      throw Error ( JEM_FUNC, "Plane stress not supported" );
    }
    else if ( stateString_ == "AXISYMMETRIC" )
    {
      state_ = AxiSymmetric;

      ++strCount;
    }
  }
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void GasserNLMaterial::getConfig ( const Properties& conf ) const
{
  conf.set ( LAMBDA_PROP, lambda_ );
  conf.set ( MU_PROP, mu_ );
  conf.set ( BETA_PROP, beta_ );

  conf.set ( K1_A_PROP, k1_a_ );    
  conf.set ( K2_A_PROP, k2_a_ );

  conf.set ( KAPPA_PROP, kappa_ );

  conf.set ( KAPPAM0_PROP, kappam0_ );
  conf.set ( KAPPAMU_PROP, kappamu_ );
  conf.set ( KAPPAF0_PROP, kappaf0_ );
  conf.set ( KAPPAFU_PROP, kappafu_ );


  if ( rank_ == 2 )
  {
    conf.set ( STATE_PROP, stateString_ );
  }
}

//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------

void GasserNLMaterial::update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      const Matrix&         defGrad,
      idx_t                 ipoint )

{
  idx_t loadingm;
  idx_t loadingf;

  idx_t strCount = STRAIN_COUNTS[rank_];

  Matrix  S  ( rank_, rank_);
  Matrix  C   ( rank_, rank_); 

  Matrix  deb ( rank_, rank_);
  Matrix  S0  ( rank_, rank_); 

  double omegam ;
  double deomegam ;

  double omegaf ;
  double deomegaf ;  


  Vector a01 ( rank_ ); 
  Vector a02 ( rank_ );

  if ( rank_ == 2 )
 {
    a01[0] = cos( beta_ );
    a01[1] = sin( beta_ );
 }
  else 
  {
    a01[0] = 0.;
    a01[1] = cos( beta_ );
    a01[2] = sin( beta_ );
  }

 if ( rank_ == 2 )
 {
 	  a02[0] = cos( beta_ );
 	  a02[1] = -sin( beta_ );
 }
  else 
  {
    a02[0] = 0.;
    a02[1] = cos( beta_ );
    a02[2] = -sin( beta_ );
  }

  Matrix A1 ( rank_, rank_ );
  Matrix A2 ( rank_, rank_ );

  A1 = matmul( a01, a01);
  A2 = matmul( a02, a02);


//  System::out() << "A " << A1 << endl;

//  System::out() << "B " << A2 << endl;



  double I4 = 0;
  double I6 = 0;

  voigt2TensorStrain ( C, strain );

  getparameters2 ( omegam, deomegam, omegaf, deomegaf, C, deb, kappam0_, kappamu_, kappaf0_, kappafu_,rank_ );

  if ( omegam < preHist_[ipoint].damagem )
  {
    deomegam = 0.;
    omegam = preHist_[ipoint].damagem;
    loadingm = 0;
  }
  else
  {
    loadingm = 1;
  }

  if ( omegaf < preHist_[ipoint].damagef )
  {
    deomegaf = 0.;
    omegaf = preHist_[ipoint].damagef;
    loadingf = 0;
  }
  else
  {
    loadingf = 1;
  }


//  System::out() << 'A' << omegam << endl;


//  System::out() << 'C' << omegaf << endl;



   C *= 2.;
  for ( idx_t i = 0; i < rank_; ++i )
  {
    C(i,i) += 1;                       
  }

  double jac = jem::numeric::det( defGrad );	

  C *= pow(jac, - 0.6666666666666666);   

//  System::out() << "C: " << C << endl;

  for (idx_t i = 0; i < rank_; ++i)
  {
  	for (idx_t j = 0; j < rank_; ++j)
    {
      I4 += C(i,j) * A1(i,j);
      I6 += C(i,j) * A2(i,j);
    }  
  }	

//  System::out() << "I4: " << I4 << endl;
//  System::out() << "I6: " << I6 << endl;

  double X1;
  double X2;

  if (I4 < 1)
  {
  	X1 = 0;
  }
  else
  {
  	X1 = 1;
  }

  if (I6 < 1) 
  {
  	X2 = 0;
  }
  else
  {
  	X2 = 1;
  }


  Matrix Fbar (rank_,rank_);

  Fbar = pow(jac, - 0.3333333333333333) * defGrad;



  double lnJ = log(jac);

  Matrix bl ( rank_, rank_ );     

  bl = matmul(defGrad , defGrad.transpose());   

  Matrix bbar ( rank_, rank_ );

  bbar = pow(jac, -0.6666666666666666) * matmul(defGrad , defGrad.transpose()); 


  Vector a1 ( rank_ );    

  a1 = matmul(Fbar, a01);   

  Matrix outer_p1( rank_, rank_ );

  outer_p1 = matmul( a1, a1 );


  Vector a2 ( rank_ );      

  a2 = matmul(Fbar, a02); 

  Matrix outer_p2( rank_, rank_ );

  outer_p2 = matmul( a2, a2 );



  //////////////////STRESS UPDATE////////////////////////

  S = 0.;

  for ( idx_t i = 0; i < rank_; ++i )
  {
    S(i,i) = lambda_ * lnJ - mu_;
  }

  S += mu_ * bl;

  S0 = S;
  S *= (1.0 - omegam);


 
   Matrix I ( rank_, rank_ );
   
   I = 0.;                            // added
 
  for ( idx_t i = 0; i < rank_; ++i )
  {
    I(i,i) = 1;
  }



  Matrix H1 ( rank_, rank_ );

  H1 = X1 * kappa_ * I + ( X1 * ( 1. - ( 3. * kappa_ ) ) * A1 );

  Matrix H2 ( rank_, rank_ );

  H2 = X2 * kappa_ * I + ( X2 * ( 1. - ( 3. * kappa_ ) ) * A2 );


  Matrix h1 ( rank_, rank_);
  Matrix h11 ( rank_, rank_);

  h11 = matmul( Fbar, H1);

  h1 = matmul( h11, Fbar.transpose() );


  Matrix h2 ( rank_, rank_);
  Matrix h22 ( rank_, rank_);

  h22 = matmul( Fbar, H2);

  h2 = matmul( h22, Fbar.transpose() );



  double E1 = 0;

  for ( idx_t i = 0; i < rank_; ++i )
  {
    E1 += h1(i,i);
  }

  if ( E1 < 1.0)
  {
     E1 = 1.0;
  }

  E1 -= X1 * 1.;

  double E2 = 0;

  for ( idx_t i = 0; i < rank_; ++i )
  {
    E2 += h2(i,i);
  }

  if ( E2 < 1.0)
  {
     E2 = 1.0;
  }


  E2 -= X2 * 1.;



  double psi1_1;

  psi1_1 = k1_a_ * E1 * exp( k2_a_ * pow( E1, 2 ) );

  double psi1_2;

  psi1_2 = k1_a_ * E2 * exp( k2_a_ * pow( E2, 2 ) );

//  System::out() << "A " << E1 << endl;

//  System::out() << "B " << E2 << endl;



  Matrix Sf_tilda ( rank_, rank_ );

  Sf_tilda = 2. * psi1_1 * h1;
  Sf_tilda += 2. * psi1_2 * h2;


//  System::out() << "A " << Sf_tilda << endl;

//  System::out() << "B " << lnJ << endl;






  Array <double,4> P (3,3,3,3) ;
  P = 0.; 

  Matrix S1  ( rank_, rank_);
  S1 = 0.;


  for ( idx_t i = 0; i < rank_; ++i )
  {
    for ( idx_t j = 0; j < rank_; ++j )
    {
      for ( idx_t k = 0; k < rank_; ++k )
      {
        for ( idx_t l = 0; l < rank_; ++l )
        {
          if (i == k && j == l)
          {
            P(i,j,k,l) += 0.5;
          }

          if (i == l && j == k)
          {
            P(i,j,k,l) += 0.5;
          }

          P(i,j,k,l) -= 0.3333333333333333 * I(i,j) * I(k,l);

          S1(i,j) += P(i,j,k,l) * Sf_tilda(k,l);

//          S(i,j) += P(i,j,k,l) * Sf_tilda(k,l);
        }
      }
    }
  }



  S += (1.0 - omegaf) * S1; 

//  System::out() << "A " << S1 << endl;

  S ( rank_-1, rank_-1 ) = 0.;

//  System::out() << "A " << S << endl;

   tensor2VoigtStress ( stress, S );


//  System::out() << "C: " << S << endl;

   ////////////////////////STIFFNESS MATRIX/////////////////////////////////////

   Array <double,4> I_unit_2 (3,3,3,3);
   Array <double,4> I_unit_3 (3,3,3,3);
   Array <double,4> I_unit_4 (3,3,3,3);

   I_unit_2 = 0;
   I_unit_3 = 0;
   I_unit_4 = 0;

   for ( idx_t i = 0; i < rank_; ++i )
   {
    for ( idx_t j = 0; j < rank_; ++j )
    {
      for ( idx_t k = 0; k < rank_; ++k )
      {
        for ( idx_t l = 0; l < rank_; ++l )
        {
          if (i == j && k == l)
          {
            I_unit_2(i,j,k,l) += 1;
          }
          if (i == k && j ==l)
          {
            I_unit_3(i,j,k,l) += 1;
          }
          if (i == l && k == j)
          {
            I_unit_4(i,j,k,l) += 1;
          }
        }
      }
    }
  }


  double psi2_1;

  psi2_1 = k1_a_ * ( 1. + ( 2. * k2_a_ * pow( E1, 2) ) ) * exp( k2_a_ * pow( E1, 2) ); 

  double psi2_2;

  psi2_2 = k1_a_ * ( 1. + ( 2. * k2_a_ * pow( E2, 2) ) ) * exp( k2_a_ * pow( E2, 2) ); 

  Matrix ddot1 ( rank_, rank_ );
  Matrix ddot2 ( rank_, rank_ );



  for ( idx_t i = 0; i < rank_; ++i )
  {
    for ( idx_t j = 0; j < rank_; ++j )
    {
      for ( idx_t k = 0; k < rank_; ++k )
      {
        for ( idx_t l = 0; l < rank_; ++l )
        {
          ddot1(i,j) = P(i,j,k,l) * h1(k,l);
          ddot2(i,j) = P(i,j,k,l) * h2(k,l);
        }
      }
    }
  }




  for ( idx_t iv = 0; iv < strCount; ++iv )
  {
    for ( idx_t jv = 0; jv < strCount; ++jv )
    {
      idx_t i, j, k, l;

      tensorIndex ( i, j, iv, rank_ );
      tensorIndex ( k, l, jv, rank_ );

      ///////////////////////NeoHooke//////////////////////////////////

      stiff(iv,jv) = lambda_ * I_unit_2(i,j,k,l);
      
      stiff(iv,jv) += mu_ * ( I_unit_3(i,j,k,l) + I_unit_4(i,j,k,l) );

      stiff(iv,jv) *= (1.0 - omegam);         
      stiff(iv,jv) -= deomegam * deb(i,j) * S0(k,l); 


      //////////////////////Gasser Fibre Dispersion/////////////////////

//      for ( idx_t i = 0; i < rank_; ++i )
//      {
//        for ( idx_t j = 0; j < rank_; ++j )
//        {
//          for ( idx_t k = 0; k < rank_; ++k )
//          {
//            for ( idx_t l = 0; l < rank_; ++l )
//            {


      stiff(iv,jv) += (1.0 - omegaf) * X1 * 4. * pow(jac, -1.3333333333333333) * psi2_1 * ddot1(i,j) * ddot1(k,l);
      stiff(iv,jv) += (1.0 - omegaf) * X2 * 4. * pow(jac, -1.3333333333333333) * psi2_2 * ddot2(i,j) * ddot2(k,l);

      stiff(iv,jv)  -= deomegaf * deb(i,j) * S1(k,l);






//  System::out() << "C: " << stiff << endl;

//            }
//          }
//        }
//      }
    }
  }

  newHist_[ipoint].loadingm = loadingm;
  newHist_[ipoint].loadingf = loadingf;
  newHist_[ipoint].damagem = omegam;
  newHist_[ipoint].damagef = omegaf;    
}

// --------------------------------------------------------------------
//  commit
// --------------------------------------------------------------------

void  GasserNLMaterial::commit()
{
  newHist_.swap ( preHist_ );
}

//-----------------------------------------------------------------------
//   allocPoints
//-----------------------------------------------------------------------

void GasserNLMaterial::allocPoints ( idx_t count )
{
  // NB: this version of pushBack calls the copy constructor
  // this does not work when Hist_ contains Arrays

  preHist_.pushBack ( Hist_(), count );
  newHist_.pushBack ( Hist_(), count );
}


//-----------------------------------------------------------------------
//   clone
//-----------------------------------------------------------------------

Ref<NLMaterial> GasserNLMaterial::clone () const

{
  // use default copy constructor

  return newInstance<GasserNLMaterial> ( *this );
}

