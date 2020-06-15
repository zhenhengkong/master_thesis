/*
 * 
 *  Copyright (C) 2009 TU Delft. All rights reserved.
 *  
 *  This pure virtual class implements functions for a softening material 
 *  model. Any softening material should be derived from Material
 *  as well as from Softening.
 *  
 *  Author: F.P. van der Meer, f.p.vandermeer@tudelft.nl
 *  Date: June 2009
 *
 */

#ifndef SOFTENING_H 
#define SOFTENING_H

class Softening 
{
 public:
  
  virtual double          maxAllowedLength () const = 0;

  virtual void            update

    ( Vector&               stress,
      Matrix&               stiff,
      const Vector&         strain,
      const idx_t           ip,
      const double          le ) = 0;
 
 protected:

  double                  maxLe_;

};

#endif
