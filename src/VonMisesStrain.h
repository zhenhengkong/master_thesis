#ifndef VON_MISES_STRAIN_H
#define VON_MISES_STRAIN_H


#include <jive/Array.h>

using jem::idx_t;
using jem::Tuple;
using jive::Vector;

//=======================================================================
//   class VonMisesStrain
//=======================================================================

class VonMisesStrain
{

public:

            VonMisesStrain ();


  double    operator () 

    ( const double I1, 
      const double J2 )           const;

  template  <idx_t N> 
  void      getDeriv    

    (       Tuple<double,N>& dvMdStrain,
      const Tuple<double,N>& dI1dStrain, 
      const Tuple<double,N>& dJ2dStrain,
      const double           I1,
      const double           J2 )          const;

  void      getGrads

    (       double&          dvMdI1,
            double&          dvMdJ2,
      const double           I1,
      const double           J2 )          const;

  void      init

    ( double          k,
      double          nu );


private:

  double    a_; 
  double    b_;
  double    c_; 
};

#endif
