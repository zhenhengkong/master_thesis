/*
 *  Copyright (C) 2008 TU Delft. All rights reserved.
 *
 *  This class implements a mixed mode traction-seperation law
 *  for cohesive crack models with dummy stiffness (Turon et al. 2006). 
 *  For use in InterfaceModel (2D or 3D).
 *
 *  Author: F.P. van der Meer
 *  Date: June 2008
 *
 */

#include <jem/base/System.h>
#include <jem/base/array/utilities.h>
#include <jem/base/array/select.h>
#include <jem/base/array/operators.h>
#include <jem/base/array/tensor.h>
#include <jem/base/array/intrinsics.h>
#include <jem/io/PrintWriter.h>
#include <jem/util/Properties.h>
#include <jem/numeric/utilities.h>
#include <jem/numeric/algebra/utilities.h>
#include <jem/numeric/algebra/matmul.h>
#include <jem/numeric/algebra/MatmulChain.h>

#include "TuronCohesiveMat.h"

using namespace jem;
using namespace jem::io;

using jem::numeric::matmul;
using jem::numeric::norm2;
using jem::numeric::MatmulChain;

//=======================================================================
//   class TuronCohesiveMat
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------

const char*  TuronCohesiveMat::F2T_PROP      = "f2t";
const char*  TuronCohesiveMat::F6_PROP       = "f6";
const char*  TuronCohesiveMat::G_I_PROP      = "gI";
const char*  TuronCohesiveMat::G_II_PROP     = "gII";
const char*  TuronCohesiveMat::ETA_PROP      = "eta";
const char*  TuronCohesiveMat::DUMMY_PROP    = "dummy";

const double TuronCohesiveMat::OMEGA_MAX     = 1. - 1.e-12;
const double TuronCohesiveMat::EPS           = 1.e-10;

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------

TuronCohesiveMat::TuronCohesiveMat

  ( const idx_t        rank,
    const Properties&  globdat )

  : CohesiveMat ( rank, globdat )
  
{
  JEM_PRECHECK ( rank >= 1 && rank <= 3 );

  f2t_  = 0.;
  f6_   = 0.;
  gI_   = 0.;
  gII_  = 0.;
  eta_  = 0.;

  f2t2_ = 0.;
  f62_  = 0.;

  deltaN02_ = 0.;
  deltaS02_ = 0.;
  deltaN0F_ = 0.;
  deltaS0F_ = 0.;

  // some default values for optional input

  dummy_       = 1.e6;
}


TuronCohesiveMat::~TuronCohesiveMat ()
{} 

TuronCohesiveMat::Hist_::Hist_
  
  ( const double    dam,
    const idx_t     loading )

  : damage(dam), loading(loading), dissipation(0.), modeIIDiss(0.)

{}

//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void TuronCohesiveMat::configure

  ( const Properties& props,
    const Properties& globdat )

{
  using jem::maxOf;

  props.get  ( f2t_  ,   F2T_PROP,  0., maxOf( f2t_  ) );
  props.get  ( f6_   ,    F6_PROP,  0., maxOf( f6_   ) );
  props.get  ( gI_   ,   G_I_PROP,  0., maxOf( gI_   ) );
  props.get  ( gII_  ,  G_II_PROP,  0., maxOf( gII_  ) );
  props.find ( dummy_, DUMMY_PROP,  0., maxOf( dummy_) );

  props.find ( eta_  ,   ETA_PROP,  .5, maxOf( eta_  ) );

  // Initialize some values

  double deltaNF = 2. *  gI_ / f2t_;
  double deltaSF = 2. * gII_ /  f6_;
  double deltaN0 = f2t_ / dummy_;
  double deltaS0 =  f6_ / dummy_;

  deltaN02_ = deltaN0 * deltaN0;
  deltaS02_ = deltaS0 * deltaS0;
  deltaN0F_ = deltaN0 * deltaNF;
  deltaS0F_ = deltaS0 * deltaSF;

  f2t2_ = f2t_ * f2t_;
  f62_  = f6_  * f6_ ;
}

//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void TuronCohesiveMat::getConfig

  ( const Properties& conf , 
    const Properties& globdat ) const

{
  conf.set (   F2T_PROP,   f2t_ );
  conf.set (    F6_PROP,    f6_ );
  conf.set (   G_I_PROP,    gI_ );
  conf.set (  G_II_PROP,   gII_ );
  conf.set (   ETA_PROP,   eta_ );
  conf.set ( DUMMY_PROP, dummy_ );
}

//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------

void  TuronCohesiveMat::update

    ( Vector&               traction,
      Matrix&               stiff,
      const Vector&         jump,
      idx_t                 mpoint )
{
  idx_t  loading;
  double damage;
  double beta;

  bool   tension = jump[0] > 0;

  // compute equivalent displacement jump

  Vector jumpCorr( rank_ ) ;

  jumpCorr     = jump;
  jumpCorr[0] *= tension;

  double delta  = norm2 ( jumpCorr );
  double deltaS = norm2 ( jump[slice(1,END)] );

  // update the damage

  if ( deltaS + jumpCorr[0] > 0 )
  {
    beta = deltaS / ( deltaS + jumpCorr[0] );
  }
  else
  {
    beta = 0.;
  }
    
  double B     = beta * beta;
         B    /= 1. + 2.*B - 2.*beta;
  double Beta  = ::pow( B , eta_ );

  double delta0  = deltaN02_ + ( deltaS02_ - deltaN02_ ) * Beta;
         delta0  = sqrt( delta0 );
  double deltaF  = deltaN0F_ + ( deltaS0F_ - deltaN0F_ ) * Beta;
         deltaF /= delta0;

  double hisDam = ( delta - delta0 ) / ( deltaF - delta0 );

  // TODO: OPT 

  // if ( abs ( hisDam - preHist_[mpoint].damage ) == 0.0 )
  if ( jem::numeric::abs ( hisDam - preHist_[mpoint].damage ) < 1.e-14 )
  {
    loading = preHist_[mpoint].loading;
  }
  else if ( hisDam > preHist_[mpoint].damage )
  {
    loading = 1;
  }
  else
  {
    loading = 0;
    hisDam = preHist_[mpoint].damage;
  }

  if ( hisDam > 0. )
  {
    if ( loading )
    {
      damage = hisDam * deltaF / delta;
    }
    else
    {
      damage  = hisDam * deltaF;
      damage /= delta0 + hisDam * ( deltaF - delta0 );
    }
  }
  else
  {
    hisDam = damage =  0.;
  }

  if ( hisDam >= OMEGA_MAX )
  {
    hisDam = damage = OMEGA_MAX;
    loading = 0;
  }

  // compute secant stiffness and traction

  stiff = 0;

  stiff(0,0) = ( 1. - damage * tension ) * dummy_;

  for ( idx_t i = 1; i < rank_; ++i )
  {
    stiff(i,i) = ( 1. - damage ) * dummy_;
  }

  traction = matmul( stiff, jump );

  // compute the tangent stiffness matrix

  if ( loading )
  {
    double H  = deltaF * delta0 ;
           H /= ( deltaF - delta0 ) * delta * delta * delta;

    TensorIndex i,j;

    stiff(i,j) -= dummy_ * H * jumpCorr[i] * jumpCorr[j];

    // apocryphal term

    Vector dtdd   ( - dummy_ * jumpCorr );

    double   tmp  = deltaF - delta0;
             tmp *= tmp * delta;
    double dddd0  = - deltaF * ( deltaF - delta ) / tmp;
    double ddddF  = - delta0 * ( delta - delta0 ) / tmp;

    double dBedB  = eta_ * ::pow( B, eta_-1. );

    double dd0dB  = deltaS02_ - deltaN02_;
           dd0dB /= 2. * delta0; 
    double ddFdB  = deltaS0F_ - deltaN0F_ - deltaF * dd0dB;
           ddFdB /= delta0;

    Vector  dBdd  ( rank_ );

    if ( deltaS > EPS )
    {
             tmp  = jumpCorr[0] / deltaS / deltaS;
         dBdd[0]  = - 2. * tmp * B * B;

      if (   rank_ == 2 )
      {
         dBdd[1]  = - dBdd[0] * jumpCorr[0] / jumpCorr[1];
      }
      else
      {
         dBdd[1]  = 2 * tmp * tmp * B * B * jump[1];
         dBdd[2]  = 2 * tmp * tmp * B * B * jump[2];
      }
    }
    else
    {
      dBdd[0] = 0.;
      dBdd[slice(1,END)] = 1. / jumpCorr[0] / jumpCorr[0];
    }

    double  dddB  = dddd0 * dd0dB + ddddF * ddFdB;
            dddB *= dBedB;
    Vector  dddd  ( dddB * dBdd );

    stiff(i,j)   += dtdd[i] * dddd[j];
  }

  // compute dissipation per unit surface (incremental, Euler backward)
  
  double dG  = .5 * ( hisDam - preHist_[mpoint].damage );
         dG *= deltaF * dummy_ * delta0;

//   if ( dG < 0. )
//   {
//     System::warn() << "dg smaller than zero! " << deltaF << " " << dummy_
//       << " " << delta0 << " " <<  hisDam << " " 
//       << preHist_[mpoint].damage << endl;
//   }

  double  G  = preHist_[mpoint].dissipation + dG;

  // update history variables

  newHist_[mpoint].damage = hisDam;
  newHist_[mpoint].loading = loading;
  newHist_[mpoint].dissipation = G;
  newHist_[mpoint].modeIIDiss = preHist_[mpoint].modeIIDiss + dG*B; 

  latestHist_ = &newHist_;
} 

//-----------------------------------------------------------------------
//   elasticUpdate (regular)
//-----------------------------------------------------------------------

void  TuronCohesiveMat::elasticUpdate

    ( Vector&               traction,
      Matrix&               stiff,
      const Vector&         jump )
{
  TensorIndex i,j;

  traction = dummy_ * jump;

  stiff(i,j) = where ( i == j, dummy_, 0. );
}

// --------------------------------------------------------------------
//  commit
// --------------------------------------------------------------------

void  TuronCohesiveMat::commit()

{
  newHist_.swap ( preHist_ );

  latestHist_ = &preHist_;
}

// --------------------------------------------------------------------
//  allocPoints
// --------------------------------------------------------------------

void  TuronCohesiveMat::allocPoints 

    ( idx_t   count, 
      double  dam   )

{
  allocPoints_ ( count, dam, 0 );
}

// --------------------------------------------------------------------
//  allocPoints
// --------------------------------------------------------------------

void  TuronCohesiveMat::allocPoints_

    ( const idx_t   count, 
      const double  dam,
      const idx_t   loading )

{

  // not yet allocated

  preHist_.pushBack ( Hist_(dam,loading), count );
  newHist_.pushBack ( Hist_(dam,loading), count );
}

// --------------------------------------------------------------------
//  deallocPoints
// --------------------------------------------------------------------

void TuronCohesiveMat::deallocPoints ( idx_t count )
{
  preHist_.popBack ( count );
  newHist_.popBack ( count );
}

// --------------------------------------------------------------------
//  getDelta0
// --------------------------------------------------------------------

double  TuronCohesiveMat::getDelta0_ ( const double beta ) const

{
  double B       = beta * beta;
         B      /= 1. + 2.*B - 2.*beta;
  double Beta    = ::pow( B , eta_ );
 
  double delta02 = deltaN02_ + ( deltaS02_ - deltaN02_ ) * Beta;

  return sqrt( delta02 );
}

