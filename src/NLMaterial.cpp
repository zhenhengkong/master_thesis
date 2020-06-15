
#include <jem/base/array/tensor.h>
#include <jem/base/array/operators.h>
#include <jem/base/Error.h>
#include <jem/numeric/algebra/matmul.h>
#include <jem/numeric/algebra/MatmulChain.h>
#include <jem/util/Properties.h>

#include "NLMaterial.h"
#include "NeoHookeNLMaterial.h"
#include "HolzapfelNLMaterial.h"
#include "GasserNLMaterial.h"
#include "utilities.h"
//#include "NeoHookeNLMaterialFailure.h"

using namespace jem;

using jem::numeric::matmul;
using jem::numeric::MatmulChain;

typedef MatmulChain<double,3>      MChain3;

//=======================================================================
//   class NLMaterial
//=======================================================================

NLMaterial::NLMaterial 

  ( const idx_t        rank,
    const Properties&  globdat )

  : rank_(rank), desperateMode_(false), viscous_(false)

{
}


NLMaterial::~NLMaterial()
{}

void   NLMaterial::commit()
{}

void   NLMaterial::cancel()
{}

void   NLMaterial::configure

  ( const Properties& props )

{}

void   NLMaterial::getConfig 

  ( const Properties& props ) const

{}

void   NLMaterial::updateThermStrain

  ( const Properties& globdat ) 

{}

double   NLMaterial::giveDissipation

  ( const idx_t ipoint ) const

{ 
  return 0.;
}



//--------------------------------------------------------------------
//   allocPoints
//--------------------------------------------------------------------

void NLMaterial::allocPoints ( const idx_t count )
{}

void NLMaterial::allocPoints 

  ( const idx_t      count,
    const Matrix&    transfer,
    const IntVector& oldPoints )

{
  // default implementation: ignore history input

  allocPoints ( count );
}

//--------------------------------------------------------------------
//   allocPoints
//--------------------------------------------------------------------

void NLMaterial::deallocPoints ( const idx_t count )
{}

//--------------------------------------------------------------------
//   despair
//--------------------------------------------------------------------

bool NLMaterial::despair ()
{
  desperateMode_ = true;

  idx_t np = pointCount();

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

bool NLMaterial::endDespair ()
{
  desperateMode_ = false;

  return true;
}

//-----------------------------------------------------------------------
//   getHistoryCount
//-----------------------------------------------------------------------

idx_t NLMaterial::getHistoryCount () const
{
  return historyNames_.size();
}

//-----------------------------------------------------------------------
//   getHistoryNames
//-----------------------------------------------------------------------

StringVector NLMaterial::getHistoryNames 

  () const
{
  return historyNames_;
}

//-----------------------------------------------------------------------
//   getHistoryNames
//-----------------------------------------------------------------------

void NLMaterial::getHistoryNames 

  ( const StringVector&  hnames ) const
{
  JEM_ASSERT ( hnames.size() == getHistoryCount() );
  hnames = historyNames_;
}

//-----------------------------------------------------------------------
//   getHistory
//-----------------------------------------------------------------------

void NLMaterial::getHistory
 
  ( const Vector&  history,
    const idx_t    ipoint ) const

{
}

//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   newNLMaterial
//-----------------------------------------------------------------------


Ref<NLMaterial>         newNLMaterial

  ( const String&       name,
    const Properties&   conf,
    const Properties&   props,
    const Properties&   globdat )

{
  Properties     matProps = props.getProps ( name );
  Properties     matConf  = conf.makeProps ( name );

  Ref<NLMaterial>  mat;
  String         type;
  idx_t          dim;

  matProps.get ( type, "type" );
  matConf .set ( "type", type );

  matProps.get ( dim, "dim"   );
  matConf .set ( "dim", dim   );
  
  if ( type == "NeoHooke" )
  {
    mat = newInstance<NeoHookeNLMaterial> ( dim, globdat );
  }
  else if ( type == "Holzapfel" )
  {
    mat = newInstance<HolzapfelNLMaterial> ( dim, globdat );
  }

  else if ( type == "Gasser" )
  {
    mat = newInstance<GasserNLMaterial> ( dim, globdat );
  }
//  else if ( type == "NeoHookeFailure" )
//  {
//    mat = newInstance<NeoHookeNLMaterialFailure> ( dim, globdat );
//  }
  else
  {
    matProps.propertyError (
      name,
      "invalid material type: " + type
    );
  }

  return mat;
}

