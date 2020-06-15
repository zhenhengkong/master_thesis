
/*
 *  Copyright (C) 2016 DRG. All rights reserved.
 *
 *  This file is part of Jem, a general purpose programming toolkit.
 *
 *  Commercial License Usage
 *
 *  This file may be used under the terms of a commercial license
 *  provided with the software, or under the terms contained in a written
 *  agreement between you and DRG. For more information contact DRG at
 *  http://www.dynaflow.com.
 *
 *  GNU Lesser General Public License Usage
 *
 *  Alternatively, this file may be used under the terms of the GNU
 *  Lesser General Public License version 2.1 or version 3 as published
 *  by the Free Software Foundation and appearing in the file
 *  LICENSE.LGPLv21 and LICENSE.LGPLv3 included in the packaging of this
 *  file. Please review the following information to ensure the GNU
 *  Lesser General Public License requirements will be met:
 *  https://www.gnu.org/licenses/lgpl.html and
 *  http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 *
 *  Jem version: 2.2
 *  Date:        Thu 28 Jan 10:36:48 CET 2016
 */


#include <cmath>
#include <cstdlib>
#include <jem/base/assert.h>
#include <jem/base/MemCache.h>
#include <jem/io/DataInput.h>
#include <jem/io/DataOutput.h>
#include <jem/util/Random.h>


JEM_BEGIN_PACKAGE( util )


//=======================================================================
//   class Random
//=======================================================================

/*
  The Random class generates random numbers with a uniform
  distribution in the interval [0,1]. It is based on the subtractive
  method by Knuth. See also "Numerical Recipes in C", second edition,
  Chapter 7, page 283.
*/

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const int  Random::COEFFC_ = 56;
const int  Random::LARGE_  = 1000000000;
const int  Random::SEED_   = 131803398;


//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------


Random::Random ( int seed )
{
  coeffs_ = (int*) MemCache::alloc ( COEFFC_ * sizeof(int) );

  try
  {
    restart ( seed );
  }
  catch ( ... )
  {
    MemCache::dealloc ( coeffs_, COEFFC_ * sizeof(int) );
    throw;
  }
}


//-----------------------------------------------------------------------
//   destructor
//-----------------------------------------------------------------------


Random::~Random ()
{
  MemCache::dealloc ( coeffs_, COEFFC_ * sizeof(int) );

  coeffs_ = NULL;
}


//-----------------------------------------------------------------------
//   restart()
//-----------------------------------------------------------------------


void Random::restart ( int seed )
{
  int*       cf = coeffs_;

  const int  M  = COEFFC_ - 1;

  int        c0 = ::abs ( SEED_ - ::abs( seed ) ) % LARGE_;
  int        c1 = 1;

  cf[M] = c0;

  for ( int i = 1; i <= M - 1; i++ )
  {
    int  j = (21 * i) % M;

    cf[j] = c1;
    c1    = c0 - c1;

    if ( c1 < 0 )
    {
      c1 += LARGE_;
    }

    c0 = cf[j];
  }

  for ( int i = 0; i < 4; i++ )
  {
    for ( int j = 1; j <= M; j++)
    {
      cf[j] -= cf[1 + (j + 30) % M];

      if ( cf[j] < 0 )
      {
        cf[j] += LARGE_;
      }
    }
  }

  ipos_   = 0;
  jpos_   = 31;
  igauss_ = 0;
  xgauss_ = 0.0;
}


//-----------------------------------------------------------------------
//   next
//-----------------------------------------------------------------------


double Random::next ()
{
  int*  cf = coeffs_;

  ipos_++;
  jpos_++;

  if ( ipos_ >= COEFFC_ )
  {
    ipos_ = 1;
  }

  if ( jpos_ >= COEFFC_ )
  {
    jpos_ = 1;
  }

  int  n = cf[ipos_] - cf[jpos_];

  if ( n < 0 )
  {
    n += LARGE_;
  }

  cf[ipos_] = n;

  return (double) n * (1.0 / (double) LARGE_);
}


idx_t Random::next ( idx_t ubound )
{
  JEM_ASSERT ( ubound >= 0 );

  idx_t  n = (idx_t) ((double) ubound * next());

  if ( n >= ubound )
  {
    n = ubound - JEM_IDX_C(1);
  }

  return n;
}


//-----------------------------------------------------------------------
//   nextGaussian
//-----------------------------------------------------------------------


double Random::nextGaussian ()
{
  if ( ! igauss_ )
  {
    double  a, b, f, r;

    do
    {
      a = 2.0 * next() - 1.0;
      b = 2.0 * next() - 1.0;
      r =a * a + b * b;
    }
    while ( r >= 1.0 || r == 0.0 );

    f       = std::sqrt ( -2.0 * std::log( r ) / r );
    xgauss_ = a * f;
    igauss_ = 1;

    return b * f;
  }
  else
  {
    igauss_ = 0;

    return xgauss_;
  }
}


//-----------------------------------------------------------------------
//   readFrom
//-----------------------------------------------------------------------


void Random::readFrom ( io::DataInput& in )
{
  in.decode ( coeffs_, COEFFC_ );
  decode ( in, ipos_, jpos_, igauss_, xgauss_ );
}


//-----------------------------------------------------------------------
//   writeTo
//-----------------------------------------------------------------------


void Random::writeTo ( io::DataOutput& out ) const
{
  out.encode ( coeffs_, COEFFC_ );
  encode ( out, ipos_, jpos_, igauss_, xgauss_ );
}


JEM_END_PACKAGE( util )
