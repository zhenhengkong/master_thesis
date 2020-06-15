
#include <jem/util/Properties.h>
#include <jem/base/Error.h>

#include "CohesiveMat.h"
#include "TuronCohesiveMat.h"
#include "AlfanoTuronCoheMat.h"

using namespace jem;
// using jem::Error;

//=======================================================================
//   class CohesiveMat
//=======================================================================

CohesiveMat::CohesiveMat 

  ( const idx_t        rank,
    const Properties&  globdat )

  : rank_ ( rank ), desperateMode_ (false)

{}

CohesiveMat::~CohesiveMat()
{}

void   CohesiveMat::commit()
{}

void   CohesiveMat::cancel()
{}

void   CohesiveMat::configure

  ( const Properties&     props,
    const Properties&     globdat )

{}

void   CohesiveMat::getConfig

  ( const Properties& props , 
    const Properties& globdat ) const

{}

void CohesiveMat::allocPoints ( idx_t count, double dam )
{}

void CohesiveMat::deallocPoints ( idx_t count )
{}

// //--------------------------------------------------------------------
// //   isFailure
// //--------------------------------------------------------------------
// 
// bool CohesiveMat::isFailure 
// 
//     ( const Vector& stress , 
//       const Ref<Material> bulkMat )  const
// {
//   return evalFailure( stress, bulkMat ) > 1.;
// }
// 
// //--------------------------------------------------------------------
// //   evalFailure
// //--------------------------------------------------------------------
// 
// double CohesiveMat::evalFailure 
// 
//     ( const Vector& stress , 
//       const Ref<Material> bulkMat )  const
// {
//   // if evalFailure in not implemented, call isFailure
// 
//   return 0.;
// }

//--------------------------------------------------------------------
//   ipointCount
//--------------------------------------------------------------------

idx_t  CohesiveMat::ipointCount () const
{
  return 0;
}

//--------------------------------------------------------------------
//   despair
//--------------------------------------------------------------------

bool CohesiveMat::despair ()
{
  desperateMode_ = true;

  idx_t np = ipointCount();

  hasSwitched_.resize ( np );
  useSecant_  .resize ( np );

  useSecant_ = false;

  for ( idx_t ip = 0; ip < np; ++ip )
  {
    hasSwitched_[ip] = ( isLoading(ip) != wasLoading(ip) );
  }

  return np > 0;
}

//--------------------------------------------------------------------
//   endDespair
//--------------------------------------------------------------------

bool CohesiveMat::endDespair ()
{
  desperateMode_ = false;

  return true;
}

//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   newCohesiveMat
//-----------------------------------------------------------------------


Ref<CohesiveMat>   newCohesiveMat

  ( const String&       name,
    const Properties&   conf,
    const Properties&   props,
    const Properties&   globdat )

{
  Properties     matProps = props.getProps ( name );
  Properties     matConf  = conf.makeProps ( name );

  Ref<CohesiveMat>  mat;
  String                 type;
  idx_t                  dim;

  matProps.get ( type, "type" );
  matConf .set ( "type", type );

  matProps.get ( dim, "dim"   );
  matConf .set ( "dim", dim   );
  
  if ( type == "Turon" )
  {
    mat = newInstance<TuronCohesiveMat> ( dim, globdat );
  }
  else if ( type == "AlfanoTuron" )
  {
    mat = newInstance<AlfanoTuronCoheMat> ( dim, globdat );
  }
  else
  {
    matProps.propertyError (
      name,
      "invalid cohesive material type: " + type
    );
  }

  return mat;
}
