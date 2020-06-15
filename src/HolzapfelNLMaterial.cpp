/*
 * 
 *  Copyright (C) 2018 TU Delft. All rights reserved.
 *  
 *  This class implements an anisotropic Holzapfel material.
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
#include "HolzapfelNLMaterial.h"
#include "Invariants.h"
#include "fun.h"

using namespace jem;
using namespace voigtUtilities;
using jem::numeric::matmul;
using jem::numeric::LUSolver;


//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  HolzapfelNLMaterial::LAMBDA_PROP  = "lambda";
const char*  HolzapfelNLMaterial::MU_PROP      = "mu";
const char*  HolzapfelNLMaterial::BETA_PROP  = "beta";
const char*  HolzapfelNLMaterial::K1_A_PROP   = "k1_a";
const char*  HolzapfelNLMaterial::K2_A_PROP   = "k2_a";

const char*  HolzapfelNLMaterial::STATE_PROP   = "state";

const char*  HolzapfelNLMaterial::KAPPAM0_PROP = "kappam0";
const char*  HolzapfelNLMaterial::KAPPAMU_PROP = "kappamu";

const char*  HolzapfelNLMaterial::KAPPAF0_PROP = "kappaf0";
const char*  HolzapfelNLMaterial::KAPPAFU_PROP = "kappafu";


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


HolzapfelNLMaterial::HolzapfelNLMaterial 

  ( idx_t rank, const Properties& globdat )
    : NLMaterial ( rank, globdat )
{
  JEM_PRECHECK ( rank >= 1 && rank <= 3 );

  lambda_ = 1.0;
  mu_     = 1.0;
  beta_   = 1.0;
  k1_a_ = 1.0;
  k2_a_ = 1.0;

  kappam0_ = 1.0;
  kappamu_ = 1.0;

  kappaf0_ = 1.0;
  kappafu_ = 1.0;
}


HolzapfelNLMaterial::~HolzapfelNLMaterial ()
{}

HolzapfelNLMaterial::Hist_::Hist_ () 
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


void HolzapfelNLMaterial::configure ( const Properties& props )
{
  props.get ( lambda_, LAMBDA_PROP );
  props.get ( mu_, MU_PROP, 0.0, maxOf( mu_ ) );
  props.get ( beta_, BETA_PROP );
  props.get ( k1_a_, K1_A_PROP );
  props.get ( k2_a_, K2_A_PROP );


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


void HolzapfelNLMaterial::getConfig ( const Properties& conf ) const
{
  conf.set ( LAMBDA_PROP, lambda_ );
  conf.set ( MU_PROP, mu_ );
  conf.set ( BETA_PROP, beta_ );
  conf.set ( K1_A_PROP, k1_a_ );    
  conf.set ( K2_A_PROP, k2_a_ );

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

void HolzapfelNLMaterial::update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      const Matrix&         defGrad,
      idx_t                 ipoint )

{
  idx_t loadingm;
  idx_t loadingf;

  idx_t strCount = STRAIN_COUNTS[rank_];

  Matrix  C   ( rank_, rank_);  
  Matrix  S2  ( rank_, rank_);
  Matrix  deb ( rank_, rank_);
  Matrix  S0  ( rank_, rank_); 



  double omegam ;
  double deomegam ;

  double omegaf ;
  double deomegaf ;  


  Vector a01 ( rank_ ); 
  Vector a02 ( rank_ );

  Matrix A1 ( rank_, rank_ );
  Matrix A2 ( rank_, rank_ );

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
    a02[1] = - sin( beta_ );
  }
  else 
  {
    a02[0] = 0.;
    a02[1] = cos( beta_ );
    a02[2] = - sin( beta_ );
  }

  A1 = matmul( a01, a01);
  A2 = matmul( a02, a02);

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

  C *= 2.;
  for ( idx_t i = 0; i < rank_; ++i )
  {
    C(i,i) += 1;                       
  }

  double jac = jem::numeric::det( defGrad );	

  C *= pow(jac, - 0.6666666666666666);   

  double I4 = 0;
  double I6 = 0;

  for (idx_t i = 0; i < rank_; ++i)
  {
    for (idx_t j = 0; j < rank_; ++j)
    {
      I4 += C(i,j) * A1(i,j);
      I6 += C(i,j) * A2(i,j);
    }  
  }
  Matrix Fbar (rank_,rank_);

  Fbar = pow(jac, - 0.3333333333333333) * defGrad;

  Vector a1 ( rank_ );    

  a1 = matmul(Fbar, a01);   

  Matrix outer_p1( rank_, rank_ );

  outer_p1 = matmul( a1, a1 );   

  double sumdiag1 = 0;

  for ( idx_t i = 0; i < rank_; ++i )
  {
    sumdiag1 += outer_p1(i,i);
  }

  Matrix ddot1 ( rank_, rank_ );                        

  ddot1 = 0;

  for ( idx_t i = 0; i < rank_; ++i )
  {
    ddot1(i,i) += sumdiag1 * 0.3333333333333333;    
  }

  Matrix dev_a1 (rank_, rank_);

  dev_a1 = outer_p1 - ddot1;

  Vector a2 ( rank_ );      

  a2 = matmul(Fbar, a02); 

  Matrix outer_p2( rank_, rank_ );

  outer_p2 = matmul( a2, a2 );   

  double sumdiag2 = 0;

  for ( idx_t i = 0; i < rank_; ++i )
  {
    sumdiag2 += outer_p2(i,i);
  }

  Matrix ddot2 ( rank_, rank_ );                      

  ddot2 = 0;

  for ( idx_t i = 0; i < rank_; ++i )
  {
    ddot2(i,i) += sumdiag2 * 0.3333333333333333;  
  }

  Matrix dev_a2 (rank_, rank_);

  dev_a2 = outer_p2 - ddot2;

  double phi4;

  phi4 = k1_a_ * (I4 - 1) * exp( k2_a_ * pow((I4 - 1), 2));

  double phi6;

  phi6 = k1_a_ * (I6 - 1) * exp( k2_a_ * pow((I6 - 1), 2));

  double lnJ = log(jac);

  C *= pow(jac, 0.6666666666666666);    

  double d;

  LUSolver::invert( C, d );     

  double det; 

  LUSolver::invert( defGrad, det );      



  S2 = ( lambda_ * lnJ - mu_ ) * C;

  for ( idx_t i = 0; i < rank_; ++i )
  {
    S2(i,i) += mu_;
  }


  S0  = S2;
  S2 *= (1.0 - omegam);


  double H1;
  double H2;

  if (I4 <= 1)
  {
    H1 = 0.;
  }
  else
  {
    H1 = 1.;
  }

  if (I6 <= 1) 
  {
    H2 = 0.;
  }
  else
  {
    H2 = 1.;
  }

  Matrix  sig ( rank_, rank_);

  sig = H1 * 2. * phi4 * dev_a1;

  sig += H2 * 2. * phi6 * dev_a2;            

  Matrix S ( rank_, rank_ );

  S = jac * matmul( defGrad , sig );


  Matrix S1  ( rank_, rank_);
  Matrix S11 ( rank_, rank_);

  S1  = matmul( S, defGrad.transpose() );

  S11 = S1;

  S2 += (1.0 - omegaf) * S1;   

  S2 ( rank_-1, rank_-1 ) = 0.;

  tensor2VoigtStress ( stress, S2 );

  double muTangent = mu_ - lambda_ * lnJ;


  for ( idx_t iv = 0; iv < strCount; ++iv )
  {
    for ( idx_t jv = 0; jv < strCount; ++jv )
    {
      idx_t i, j, k, l;

      tensorIndex ( i, j, iv, rank_ );
      tensorIndex ( k, l, jv, rank_ );

      stiff(iv,jv)  = lambda_ * C(i,j) * C(k,l);    
      stiff(iv,jv) += muTangent * ( C(i,k) * C(j,l) + C(i,l) * C(k,j) );

      stiff(iv,jv) *= (1.0 - omegam);         
      stiff(iv,jv) -= deomegam * deb(i,j) * S0(k,l); 


      stiff(iv,jv)  += (1.0 - omegaf) * 2. * k1_a_ * H1 * a01[i] * a01[j] * a01[k] * a01[l];     
      stiff(iv,jv)  += (1.0 - omegaf) * 2. * k1_a_ * H2 * a02[i] * a02[j] * a02[k] * a02[l];     

      stiff(iv,jv)  -= deomegaf * deb(i,j) * S11(k,l);

    } 
  }

//System::out() << 'A' << stiff << endl;

  newHist_[ipoint].loadingm = loadingm;
  newHist_[ipoint].loadingf = loadingf;
  newHist_[ipoint].damagem = omegam;
  newHist_[ipoint].damagef = omegaf;
}

// --------------------------------------------------------------------
//  commit
// --------------------------------------------------------------------

void  HolzapfelNLMaterial::commit()
{
  newHist_.swap ( preHist_ );
}

//-----------------------------------------------------------------------
//   allocPoints
//-----------------------------------------------------------------------

void HolzapfelNLMaterial::allocPoints ( idx_t count )
{
  // NB: this version of pushBack calls the copy constructor
  // this does not work when Hist_ contains Arrays

  preHist_.pushBack ( Hist_(), count );
  newHist_.pushBack ( Hist_(), count );
}


//-----------------------------------------------------------------------
//   clone
//-----------------------------------------------------------------------

Ref<NLMaterial> HolzapfelNLMaterial::clone () const

{
  // use default copy constructor

  return newInstance<HolzapfelNLMaterial> ( *this );
}

