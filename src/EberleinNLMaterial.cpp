/*
 * 
 *  Copyright (C) 2018 TU Delft. All rights reserved.
 *  
 *  This class implements an anisotropic Eberlein material.
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
#include "EberleinNLMaterial.h"

using namespace jem;
using namespace voigtUtilities;
using jem::numeric::matmul;
using jem::numeric::LUSolver;


//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  EberleinNLMaterial::LAMBDA_PROP  = "lambda";
const char*  EberleinNLMaterial::MU_PROP      = "mu";
const char*  EberleinNLMaterial::BETA_PROP  = "beta";
const char*  EberleinNLMaterial::C_A_PROP   = "c_a";
const char*  EberleinNLMaterial::K1_A_PROP   = "k1_a";
const char*  EberleinNLMaterial::K2_A_PROP   = "k2_a";
const char*  EberleinNLMaterial::C_10_PROP   = "c_10";
const char*  EberleinNLMaterial::STATE_PROP   = "state";

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


EberleinNLMaterial::EberleinNLMaterial 

  ( idx_t rank, const Properties& globdat )
    : NLMaterial ( rank, globdat )
{
  JEM_PRECHECK ( rank >= 1 && rank <= 3 );

  lambda_ = 1.0;
  mu_     = 1.0;
  beta_   = 1.0;
  c_a_ = 1.0;
  k1_a_ = 1.0;
  k2_a_ = 1.0;
  c_10_ = 1.0;
}


EberleinNLMaterial::~EberleinNLMaterial ()
{}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void EberleinNLMaterial::configure ( const Properties& props )
{
  props.get ( lambda_, LAMBDA_PROP );
  props.get ( mu_, MU_PROP, 0.0, maxOf( mu_ ) );
  props.get ( beta_, BETA_PROP );
  props.get ( c_a_, C_A_PROP );
  props.get ( k1_a_, K1_A_PROP );
  props.get ( k2_a_, K2_A_PROP );
  props.get ( c_10_, C_10_PROP );

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


void EberleinNLMaterial::getConfig ( const Properties& conf ) const
{
  conf.set ( LAMBDA_PROP, lambda_ );
  conf.set ( MU_PROP, mu_ );
  conf.set ( BETA_PROP, beta_ );
  conf.set ( C_A_PROP, c_a_ );
  conf.set ( K1_A_PROP, k1_a_ );    
  conf.set ( K2_A_PROP, k2_a_ );
  conf.set ( C_10_PROP, c_10_ );

  if ( rank_ == 2 )
  {
    conf.set ( STATE_PROP, stateString_ );
  }
}

//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------

void EberleinNLMaterial::update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      const Matrix&         defGrad,
      idx_t                 ipoint )

{
  //System::out() << "strain: " << strain << endl;
  //System::out() << "defGrad: " << defGrad << endl;

  idx_t strCount = STRAIN_COUNTS[rank_];

  //System::out() << "strCount: " << strCount << endl;


  Matrix  C   ( rank_, rank_);  
  Matrix  S  ( rank_, rank_);

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
 	  a02[1] = -sin( beta_ );
 }
  else 
  {
    a02[0] = 0.;
    a02[1] = cos( beta_ );
    a02[2] = -sin( beta_ );
  }

  A1 = matmul( a01, a01);
  A2 = matmul( a02, a02);

  voigt2TensorStrain ( C, strain );

  C *= 2.;
  for ( idx_t i = 0; i < rank_; ++i )
  {
    C(i,i) += 1.;                       
  }

  double jac = jem::numeric::det( defGrad );	

  C *= pow(jac, -0.6666666666666666);   

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

  System::out() << "I4: " << I4 << endl;
  System::out() << "I6: " << I6 << endl;

  double lnJ = log(jac);

  C *= pow(jac, 0.6666666666666666);     //Cbar returned to C

  double d;

  Matrix Ci ( rank_, rank_ );

  Ci = C;

  LUSolver::invert( Ci, d );      //C^-1

  double H1;
  double H2;

    if (I4 < 1)
  {
    H1 = 0;
  }
  else
  {
    H1 = 1;
  }

  if (I6 < 1) 
  {
    H2 = 0;
  }
  else
  {
    H2 = 1;
  }


  //////////////////STRESS UPDATE////////////////////////

 

  S = ( lambda_ * lnJ - mu_ ) * Ci;

  for ( idx_t i = 0; i < rank_; ++i )
  {
  	S(i,i) += mu_;
  }

  Matrix S_f ( rank_, rank_ );

  S_f =  H1 * 2. * k1_a_ * exp( k2_a_ * pow((I4 - 1.), 2.)) * (I4 - 1.) * A1;
  S_f += H2 * 2. * k1_a_ * exp( k2_a_ * pow((I6 - 1.), 2.)) * (I6 - 1.) * A2;

  Array <double,4> P (3,3,3,3);

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
  					P(i,j,k,l) = 0.5;
  				}
  				else
  				{
  					P(i,j,k,l) = 0.;
  				}

  				if (i == l && j == k)
  				{
  					P(i,j,k,l) += 0.5;
  				}
  				else
  				{
  					P(i,j,k,l) += 0.;
  				}

  				P(i,j,k,l) -= 0.3333333333333333 * Ci(i,j) * C(k,l);

  				S(i,j)  +=  pow(jac, -0.6666666666666666) * P(i,j,k,l) * S_f(k,l);

  			}
  		}
  	}
  }

  tensor2VoigtStress ( stress, S );


  ////////////////////////STIFFNESS MATRIX/////////////////////////////////////
  ////////////////////////////C-hat_f//////////////////////////////////////////

  double delta1;
  double delta2;

  delta1 = H1 * 4. * pow(jac, -1.3333333333333333) * k1_a_ * (2. * k2_a_ * pow((I4 - 1.), 2.) + 1.) * exp( k2_a_ * pow((I4 - 1.), 2.));
  delta2 = H2 * 4. * pow(jac, -1.3333333333333333) * k1_a_ * (2. * k2_a_ * pow((I6 - 1.), 2.) + 1.) * exp( k2_a_ * pow((I6 - 1.), 2.)); 

  Array <double,4> Df (3,3,3,3);

  Array <double,4> AA1 (3,3,3,3);
  Array <double,4> AA2 (3,3,3,3);

  for ( idx_t i = 0; i < rank_; ++i )
  {
  	for ( idx_t j = 0; j < rank_; ++j )
  	{
  		for ( idx_t k = 0; k < rank_; ++k )
  		{
  			for ( idx_t l = 0; l < rank_; ++l )
  			{
  				AA1(i,j,k,l) = delta1 * A1(i,j) * A1(k,l);
  				AA2(i,j,k,l) = delta2 * A2(i,j) * A2(k,l);
  			}
  		}
  	}
  }

  Df = AA1 + AA2;

  ////////////////////NH, DF1,DF2 & DF3 STIFFNESSES//////////////////////////////
  
  double muTangent = mu_ - lambda_ * lnJ;

  for ( idx_t iv = 0; iv < strCount; ++iv )
  {
  	for ( idx_t jv = 0; jv < strCount; ++jv )
  	{
  		idx_t i, j, k, l;

  		tensorIndex ( i, j, iv, rank_ );
  		tensorIndex ( k, l, jv, rank_ );

  		///////////////////////NeoHooke//////////////////////////////////

  		stiff(iv,jv)  = lambda_ * Ci(i,j) * Ci(k,l);
  		stiff(iv,jv) += muTangent * ( Ci(i,k) * Ci(j,l) + Ci(i,l) * Ci(k,j) );

  		///////////////////////Df1//////////////////////////////////

  		for ( idx_t i = 0; i < rank_; ++i )
  		{
  			for ( idx_t j = 0; j < rank_; ++j )
  			{
  				for ( idx_t k = 0; k < rank_; ++k )
  				{
  					for ( idx_t l = 0; l < rank_; ++l )
  					{
  						for ( idx_t m = 0; m < rank_; ++m )
  						{
  							for ( idx_t n = 0; n < rank_; ++n )
  							{
  								for ( idx_t o = 0; o < rank_; ++o )
  								{
  									for ( idx_t p = 0; p < rank_; ++p )
  									{
  										stiff(iv,jv) += P(i,j,m,n) * Df(m,n,o,p) * P(o,p,k,l);
  									}
  								}
  							}
  						}
  					}
  				}
  			}
  		}

   	    ////////////////////////Df2///////////////////////////////

  		double Tr_S_f;

  		Tr_S_f = 0;

  		for ( idx_t i = 0; i < rank_; ++i )
  		{
  			Tr_S_f += pow(jac, - 0.6666666666666666) * S_f(i,i);
  		}

  		Tr_S_f *= 0.6666666666666666;

  		stiff(iv,jv) += Tr_S_f * 0.5 * ( Ci(i,k) * Ci(j,l) + Ci(i,l) * Ci(j,k) );
  		stiff(iv,jv) -= Tr_S_f * 0.3333333333333333 * Ci(i,j) * Ci(k,l);

        //////////////////////////Df3/////////////////////////////

  		stiff(iv,jv) -= 0.6666666666666666 * ( Ci(i,j) * S_f(k,l) + S_f(i,j) * Ci(k,l) );

  	}
  }





}

//-----------------------------------------------------------------------
//   clone
//-----------------------------------------------------------------------

Ref<NLMaterial> EberleinNLMaterial::clone () const

{
  // use default copy constructor

  return newInstance<EberleinNLMaterial> ( *this );
}

