#include <jem/base/assert.h>
#include <jem/base/tuple/operators.h>
#include <jem/base/RuntimeException.h>
#include <jem/base/Array.h>
#include <jem/numeric/utilities.h>
#include <jem/base/System.h>

#include "VonMisesStrain.h"

using namespace jem;

// -------------------------------------------------------------------
//  constructor
// -------------------------------------------------------------------

VonMisesStrain::VonMisesStrain

()

{
}

// -------------------------------------------------------------------
//  init
// -------------------------------------------------------------------

void VonMisesStrain::init

  ( double  k,
    double  nu )

{
  a_ = (k - 1.0) / (1.0 - 2.0 * nu);
  b_ = 0.5 / k;
  c_ = 6.0 * k / (1.0 + 2.0 * nu + nu * nu); 
  // NB, factor 1/3 in J2 definition wrt Peerlings et al (1998)
}

// ---------------------------------------------------------------------
//  double  operator() (I1, J2)
// ---------------------------------------------------------------------

double VonMisesStrain::operator ()

  ( const double  I1,
    const double  J2 )             const

{
  const double  d = a_ * a_ * I1 * I1 + c_ * J2;

  if ( d < 0 )
  {
    throw RuntimeException ( JEM_FUNC,
      " invalid parameters for von Mises equivalent strain !!! "
    );
  }

  return b_ * (a_ * I1 + ::sqrt( d ));
}

// ---------------------------------------------------------------------
//  getDeriv
// ---------------------------------------------------------------------

template<idx_t N>
void VonMisesStrain::getDeriv

    (       Tuple<double,N>& dvMdStrain,
      const Tuple<double,N>& dI1dStrain, 
      const Tuple<double,N>& dJ2dStrain, 
      const double           I1,
      const double           J2 )            const

{
  double  d = a_ * a_ * I1 * I1 + c_ * J2;

  d = ::sqrt(d);
    
  if ( d == 0.0 )
  {
    d = 1.0; 
  }
  
  const double  e      = b_ / d ;
  
  const double  deqdI1 = a_ * b_  + e * a_ * a_ * I1; 
  const double  deqdJ2 = e * c_ * 0.5;

  dvMdStrain = deqdI1 * dI1dStrain + deqdJ2 * dJ2dStrain;
}

// ---------------------------------------------------------------------
//  getGrads
// ---------------------------------------------------------------------

void VonMisesStrain::getGrads

    (       double& dvMdI1,
            double& dvMdJ2,
      const double  I1,
      const double  J2 ) const

{
  double  d = a_ * a_ * I1 * I1 + c_ * J2;

  d = ::sqrt(d);
    
  if ( d == 0.0 )
  {
    d = 1.0; 
  }
  
  const double  e      = b_ / d ;
  
  dvMdI1 = a_ * b_  + e * a_ * a_ * I1; 
  dvMdJ2 = e * c_ * 0.5;
}

