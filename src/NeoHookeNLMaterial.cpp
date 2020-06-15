/*
 * 
 *  Copyright (C) 2018 TU Delft. All rights reserved.
 *  
 *  This class implements an isotropic NeoHookean material.
 *  Its update function computes PK2 stress from the Green-Lagrange
 *  strain tensor and evaluates the associated gradient
 *  Following Belytschko Sec. 5.4
 *  
 *  Author: F.P. van der Meer, f.p.vandermeer@tudelft.nl
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
#include "NeoHookeNLMaterial.h"
#include "Invariants.h"
#include "fun.h"

using namespace jem;
using namespace voigtUtilities;
using jem::numeric::matmul;
using jem::numeric::LUSolver;

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  NeoHookeNLMaterial::LAMBDA_PROP  = "lambda";
const char*  NeoHookeNLMaterial::MU_PROP      = "mu";
const char*  NeoHookeNLMaterial::STATE_PROP   = "state";
const char*  NeoHookeNLMaterial::KAPPA0_PROP  = "kappa0";
const char*  NeoHookeNLMaterial::KAPPAU_PROP  = "kappau";

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


NeoHookeNLMaterial::NeoHookeNLMaterial 

  ( idx_t rank, const Properties& globdat )
    : NLMaterial ( rank, globdat )
{
  JEM_PRECHECK ( rank >= 1 && rank <= 3 );

  lambda_ = 1.0;
  mu_     = 1.0;
  kappa0_ = 1.0;
  kappau_ = 1.0;
}


NeoHookeNLMaterial::~NeoHookeNLMaterial ()
{}



NeoHookeNLMaterial::Hist_::Hist_ () 
  : 
    damagem ( 0. ),
    loadingm ( 0 )
{
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void NeoHookeNLMaterial::configure ( const Properties& props )
{
  props.get ( lambda_, LAMBDA_PROP );
  props.get ( mu_, MU_PROP, 0.0, maxOf( mu_ ) );
  props.get ( kappa0_, KAPPA0_PROP );
  props.get ( kappau_, KAPPAU_PROP );

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


void NeoHookeNLMaterial::getConfig ( const Properties& conf ) const
{
  conf.set ( LAMBDA_PROP, lambda_ );
  conf.set ( MU_PROP, mu_ );
  conf.set ( KAPPA0_PROP, kappa0_ );
  conf.set ( KAPPAU_PROP, kappau_ );

  if ( rank_ == 2 )
  {
    conf.set ( STATE_PROP, stateString_ );
  }
}
//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------

void NeoHookeNLMaterial::update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      const Matrix&         defGrad,
      idx_t                 ipoint )

{
  idx_t loadingm;

  idx_t strCount = STRAIN_COUNTS[rank_];

  Matrix  C  ( rank_, rank_ );  
  Matrix  S  ( rank_, rank_ );
  Matrix  S0 ( rank_, rank_ );
  Matrix deb ( rank_, rank_ ); 



  double omega  ;
  double deomega  ;

    
  voigt2TensorStrain ( C, strain );
    
  getparameters ( omega, deomega, C, deb, kappa0_, kappau_, rank_ );

  if ( omega < preHist_[ipoint].damagem )
  {
    deomega = 0.;
    omega = preHist_[ipoint].damagem;
    loadingm = 0;
  }
  else
  {
    loadingm = 1;
  }


//System::out() << 'A' << omega << endl;
//System::out() << 'B' << deomega << endl;

 
  C *= 2.;
  for ( idx_t i = 0; i < rank_; ++i )
  {
    C(i,i) += 1;
  }

  double d;

  LUSolver::invert ( C, d );

  double jac = sqrt(d);

  double lnJ = log(jac);

  S = ( lambda_ * lnJ - mu_ ) * C;

  for ( idx_t i = 0; i < rank_; ++i )
  {
    S(i,i) += mu_;
  }

  
  S0 = S;
  S *= (1.0 - omega);


//System::out() << 'A' << S0 << endl;
//System::out() << 'B' << S << endl;

  tensor2VoigtStress ( stress, S );

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
      stiff(iv,jv) *= (1.0 - omega); 
      stiff(iv,jv) -= deomega * deb(i,j) * S0(k,l);

    }
  }

// System::out() << 'A' << omega << endl;

  newHist_[ipoint].loadingm = loadingm;
  newHist_[ipoint].damagem = omega;


//System::out() << 'S' << stiff << endl;
}

// --------------------------------------------------------------------
//  commit
// --------------------------------------------------------------------

void  NeoHookeNLMaterial::commit()
{
  newHist_.swap ( preHist_ );
}

//-----------------------------------------------------------------------
//   allocPoints
//-----------------------------------------------------------------------

void NeoHookeNLMaterial::allocPoints ( idx_t count )
{
  // NB: this version of pushBack calls the copy constructor
  // this does not work when Hist_ contains Arrays

  preHist_.pushBack ( Hist_(), count );
  newHist_.pushBack ( Hist_(), count );
}


//-----------------------------------------------------------------------
//   clone
//-----------------------------------------------------------------------

Ref<NLMaterial> NeoHookeNLMaterial::clone () const

{
  // use default copy constructor

  return newInstance<NeoHookeNLMaterial> ( *this );
}

