/*
 *  Copyright (C) 2012 TU Delft. All rights reserved.
 *  
 *  This class implements a mixed mode traction-seperation law
 *  for cohesive crack models with dummy stiffness (Turon et al. 2006), 
 *  with friction in the damaged part (Alfano and Sacco, 2006).
 *  For use in InterfaceModel.
 *  
 *  Author: F.P. van der Meer
 *  Date: December 2012
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

#include "AlfanoTuronCoheMat.h"

using namespace jem;
using namespace jem::io;

using jem::numeric::matmul;
using jem::numeric::norm2;
using jem::numeric::MatmulChain;

//=======================================================================
//   class AlfanoTuronCoheMat
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------

const char*  AlfanoTuronCoheMat::F2T_PROP      = "f2t";
const char*  AlfanoTuronCoheMat::F6_PROP       = "f6";
const char*  AlfanoTuronCoheMat::G_I_PROP      = "gI";
const char*  AlfanoTuronCoheMat::G_II_PROP     = "gII";
const char*  AlfanoTuronCoheMat::ETA_PROP      = "eta";
const char*  AlfanoTuronCoheMat::DUMMY_PROP    = "dummy";
const char*  AlfanoTuronCoheMat::MU_PROP       = "mu";
const char*  AlfanoTuronCoheMat::DILATANT_PROP = "dilatancy";

const double AlfanoTuronCoheMat::OMEGA_MAX     = 1. - 1.e-12;
const double AlfanoTuronCoheMat::EPS           = 1.e-10;

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------

AlfanoTuronCoheMat::AlfanoTuronCoheMat

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
  mu_   = 0.;
  psi_  = 0.;

  f2t2_ = 0.;
  f62_  = 0.;

  deltaN02_ = 0.;
  deltaS02_ = 0.;
  deltaN0F_ = 0.;
  deltaS0F_ = 0.;

  // some default values for optional input

  dummy_       = 1.e6;
}

AlfanoTuronCoheMat::Hist_::Hist_
  
  ( const idx_t     rank,
    const double    dam,
    const idx_t     loading ) 

  : damage(dam), loading(loading), uplastic(rank)

{
  uplastic    = 0.;
  frictDiss   = 0.;
  dissipation = 0.;
}

AlfanoTuronCoheMat::Hist_::Hist_
  
  ( const Hist_&     h )  

  : damage(h.damage), 
    loading(h.loading), 
    dissipation(h.dissipation), 
    uplastic(h.uplastic.size()),
    frictDiss(h.frictDiss)
{
  uplastic = h.uplastic;  // deep copy
}

AlfanoTuronCoheMat::~AlfanoTuronCoheMat ()
{} 

//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void AlfanoTuronCoheMat::configure

  ( const Properties& props,
    const Properties& globdat )

{
  using jem::maxOf;

  props.get  ( f2t_  ,      F2T_PROP,  0., maxOf( f2t_  ) );
  props.get  ( f6_   ,       F6_PROP,  0., maxOf( f6_   ) );
  props.get  ( gI_   ,      G_I_PROP,  0., maxOf( gI_   ) );
  props.get  ( gII_  ,     G_II_PROP,  0., maxOf( gII_  ) );
  props.find ( dummy_,    DUMMY_PROP,  0., maxOf( dummy_) );

  props.find ( eta_  ,      ETA_PROP,  .5, maxOf( eta_  ) );
  props.find ( mu_   ,       MU_PROP,  0., maxOf( mu_   ) );
  props.find ( psi_  , DILATANT_PROP,  0., maxOf( psi_  ) );

  // Initialize auxiliary parameters

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


void AlfanoTuronCoheMat::getConfig

  ( const Properties& conf , 
    const Properties& globdat ) const

{
  conf.set (      F2T_PROP,   f2t_ );
  conf.set (       F6_PROP,    f6_ );
  conf.set (      G_I_PROP,    gI_ );
  conf.set (     G_II_PROP,   gII_ );
  conf.set (      ETA_PROP,   eta_ );
  conf.set (       MU_PROP,    mu_ );
  conf.set (    DUMMY_PROP, dummy_ );
  conf.set ( DILATANT_PROP,   psi_ );
}

//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------

void  AlfanoTuronCoheMat::update

    ( Vector&               traction,
      Matrix&               stiff,
      const Vector&         jump,
      idx_t                 mpoint )
{
  Vector   dddu ( rank_ );

  double damage = updateTuron_  ( traction, stiff, dddu, jump, mpoint );
  updateAlfano_ ( traction, stiff, dddu, damage, jump, mpoint );

  latestHist_ = &newHist_;
}

//-----------------------------------------------------------------------
//   updateTuron_
//-----------------------------------------------------------------------

double  AlfanoTuronCoheMat::updateTuron_

    ( Vector&               traction,
      Matrix&               stiff,
      Vector&               dddu,
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

  // if ( jem::numeric::abs ( hisDam - preHist_[mpoint].damage ) == 0.0 )
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
  // NB: no interpenatration penalty here, that is done in the friction part

  stiff = 0.;

  stiff(0,0) = ( 1. - damage ) * dummy_;

  for ( idx_t i = 1; i < rank_; ++i )
  {
    stiff(i,i) = ( 1. - damage ) * dummy_;
  }

  traction = matmul( stiff, jump );

  // compute the tangent stiffness matrix

  if ( loading )
  {
    double H  = deltaF * delta0;
           H /= ( deltaF - delta0 ) * delta * delta * delta;

    TensorIndex i,j;

    Vector dtdd   ( - dummy_ * jump ); 

    dddu = H * jumpCorr;

    // stiff(i,j) -= dummy_ * H * jumpCorr[i] * jumpCorr[j];

    // apocryphal term

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
       tmp     = jumpCorr[0] / deltaS / deltaS;
       dBdd[0] = - 2. * tmp * B * B;

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
            dddu += dddB * dBdd;

    stiff(i,j)   += dtdd[i] * dddu[j];
  }
  else
  {
    dddu = 0.;
  }

  // compute dissipation per unit surface (incremental, Euler backward)
  
  double dG  = .5 * ( hisDam - preHist_[mpoint].damage );
         dG *= deltaF * dummy_ * delta0;

  double  G  = preHist_[mpoint].dissipation + dG;

  // update history variables

  newHist_[mpoint].damage      = hisDam;
  newHist_[mpoint].loading     = loading;
  newHist_[mpoint].dissipation = G;

  return damage;
} 

//-----------------------------------------------------------------------
//   updateAlfano_
//-----------------------------------------------------------------------

bool  AlfanoTuronCoheMat::updateAlfano_

    ( Vector&               traction,
      Matrix&               stiff,
      Vector&               dddu,
      const double          damage,
      const Vector&         jump,
      idx_t                 mpoint )

{
  TensorIndex i,j;
  SliceFromTo sh  = slice(1,rank_);
  bool   tension  = jump[0] >= 0.;
  bool   friction = false;
  double dG       = 0.;

  Vector newplas  ( newHist_[mpoint].uplastic ); // shallow copy
  Vector preplas  ( preHist_[mpoint].uplastic );

  Matrix tmpM     ( rank_, rank_ );
  Vector tmpV     ( rank_ );

  if ( damage == 0.0 )
  {
    return friction;
  }

  if ( tension )
  {
    // update "plastic" deformation, no tractions or stiffness

    newplas[sh] = jump[sh];
  }
  else
  {
    Vector   tElas ( rank_ );

    tElas = dummy_ * ( jump - preplas );

    double tsh = norm2(tElas[sh]);  // for 2d: use abs for efficiency?
    double tfr = - mu_ * tElas[0];

    double phi = - tfr + tsh; 

    if ( phi > -1.e-10 && tsh > 0.0 )
    {
      friction = true;

      // traction

      Vector   ww    ( rank_ );
      Vector   tFric ( rank_ );

      ww          = tElas / tsh;
      // ww[0]       = 0.;
      ww[0]       = psi_;

      double pfac = 1. / ( 1. - mu_*psi_ );
      double dlam = phi * pfac / dummy_;
      newplas     = preplas + dlam * ww;

      tFric       = dummy_ * ( jump - newplas );
      traction   += damage * tFric;

      // linearization

      if ( 1 )//psi_ > 0. )
      {
        // stiff(i,j) += where( i == j, damage * dummy_, 0. );

        double fac1  = damage * dummy_ * pfac;
        stiff(0,0)  += fac1 * ( 1. - 2. * mu_ * psi_ );
        stiff(0,sh) -= fac1 * psi_ * ww[sh];
        stiff(sh,0) -= fac1 * mu_  * ww[sh];
        stiff(1,1)  -= fac1 * mu_  * psi_;

        if ( rank_ == 3 )
        {
          stiff(2,2)  -= fac1 * mu_ * psi_;
          double fac2  = fac1 * mu_ * tElas[0] / tsh;

          stiff(1,1)  -= fac2 * ww[2] * ww[2];
          stiff(1,2)  += fac2 * ww[1] * ww[2];
          stiff(2,1)  += fac2 * ww[1] * ww[2];
          stiff(2,2)  -= fac2 * ww[1] * ww[1];
        }
        dG  = damage * dlam * tfr; // TODO: dG with psi
      }
      else
      {
        stiff(0,0)  += damage * dummy_;
        stiff(sh,0) -= damage * mu_ * dummy_ * ww[sh];

        if ( rank_ == 3 )
        {
          double cd = -damage * mu_ * dummy_ * tElas[0] / tsh;

          stiff(1,1) += cd*ww[2]*ww[2];
          stiff(1,2) -= cd*ww[1]*ww[2];
          stiff(2,1) -= cd*ww[1]*ww[2];
          stiff(2,2) += cd*ww[1]*ww[1];
        }
        dG  = damage * dlam * tfr;
      }
      stiff(i,j)  += tFric[i] * dddu[j];
    }
    else
    {
      traction   += damage * tElas;
      stiff(i,j) += where( i == j, damage * dummy_, 0. );
      stiff(i,j) += tElas[i] * dddu[j];
    }
  }
  newHist_[mpoint].frictDiss = preHist_[mpoint].frictDiss + dG;

  return friction;
}

//-----------------------------------------------------------------------
//   elasticUpdate (regular)
//-----------------------------------------------------------------------

void  AlfanoTuronCoheMat::elasticUpdate

    ( Vector&               traction,
      Matrix&               stiff,
      const Vector&         jump )
{
  TensorIndex i,j;

  traction = dummy_ * jump;

  stiff(i,j) = where ( i == j, dummy_, 0. );
}

//-----------------------------------------------------------------------
//   getDissForce
//-----------------------------------------------------------------------

void  AlfanoTuronCoheMat::getDissForce

    ( Vector&       floc,
      const Vector& jump,
      const idx_t   mpoint )
{
  TensorIndex i,j;

  Vector   newplas   ( newHist_[mpoint].uplastic ); 
  Vector   dddu      ( rank_ );
  Vector   traction  ( rank_ );
  Vector   ftraction ( rank_ );
  Matrix   tmpstf    ( rank_, rank_ );
  Matrix   fstiff    ( rank_, rank_ );

  fstiff    = 0.;
  ftraction = 0.;

  double damage = updateTuron_  ( traction, tmpstf, dddu, jump, mpoint );
  bool friction = updateAlfano_ ( ftraction, fstiff, dddu, damage, jump, mpoint );
  
  // fstiff    =  \pder{\omega\bt^f}{\bdel}
  // ftraction =  \omega\bt^f

  // compute \pder{\bdel^p}{\bdel} from fstiff

  tmpstf       = fstiff / ( dummy_ * damage );
  tmpstf(i,j) += where ( i == j, 1., 0. );

  // compute local contribution to 'force vector'

  floc[i]  = damage * dot ( newplas[j], fstiff(j,i), j );

  if ( friction )
  {
    floc[i] += dot ( ftraction[j], tmpstf(j,i), j ); 
  }
}

// --------------------------------------------------------------------
//  commit
// --------------------------------------------------------------------

void  AlfanoTuronCoheMat::commit()

{
  newHist_.swap ( preHist_ );

  latestHist_ = &preHist_;
}

// --------------------------------------------------------------------
//  allocPoints
// --------------------------------------------------------------------

void  AlfanoTuronCoheMat::allocPoints 

    ( idx_t   count, 
      double  dam   )

{
  allocPoints_ ( count, dam, 0 );
}

// --------------------------------------------------------------------
//  allocPoints
// --------------------------------------------------------------------

void  AlfanoTuronCoheMat::allocPoints_

    ( const idx_t   count, 
      const double  dam,
      const idx_t   loading )

{
  Hist_ hist ( rank_, dam, loading );

  preHist_.pushBack ( hist, count );
  newHist_.pushBack ( hist, count );

//   for ( idx_t i = 0; i < count; ++i )
//   {
//     preHist_.pushBack ( Hist_(rank_,dam,loading) );
//     newHist_.pushBack ( Hist_(rank_,dam,loading) );
//   }
}

// --------------------------------------------------------------------
//  deallocPoints
// --------------------------------------------------------------------

void AlfanoTuronCoheMat::deallocPoints ( idx_t count )
{
  preHist_.popBack ( count );
  newHist_.popBack ( count );
}

// --------------------------------------------------------------------
//  getDelta0
// --------------------------------------------------------------------

double  AlfanoTuronCoheMat::getDelta0_ ( const double beta ) const

{
  double B       = beta * beta;
         B      /= 1. + 2.*B - 2.*beta;
  double Beta    = ::pow( B , eta_ );
 
  double delta02 = deltaN02_ + ( deltaS02_ - deltaN02_ ) * Beta;

  return sqrt( delta02 );
}

