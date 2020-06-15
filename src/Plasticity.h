/*
 * 
 *  Copyright (C) 2009 TU Delft. All rights reserved.
 *  
 *  This pure virtual class implements functions for a material 
 *  model with permanent deformation. 
 *  Additional functionality is are related to the dissipation-based 
 *  arclength method.
 *  
 *  Author: F.P. van der Meer, f.p.vandermeer@tudelft.nl
 *  Date: November 2014
 *
 */

#ifndef PLASTICITY_H 
#define PLASTICITY_H

class Plasticity 
{
 public:
  
  virtual void            getDissipationStress
    
    ( const Vector&         sstar,
      const Vector&         strain,
      const idx_t           ipoint ) const = 0;
};

#endif
