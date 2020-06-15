
#include <jem/base/array/tensor.h>
#include <jem/base/array/operators.h>
#include <jem/base/Error.h>
#include <jem/numeric/algebra/matmul.h>
#include <jem/numeric/algebra/MatmulChain.h>
#include <jem/util/Properties.h>

#include "Material.h"
#include "HookeMaterial.h"
#include "DamageMaterial.h"
#include "NeoHookeMaterial.h"
#include "OrthotropicMaterial.h"
#include "MelroMaterial.h"
#include "VoglerMaterial.h"
#include "utilities.h"

using namespace jem;

using jem::numeric::matmul;
using jem::numeric::MatmulChain;

typedef MatmulChain<double,3>      MChain3;

//=======================================================================
//   class Material
//=======================================================================

Material::Material 

  ( const idx_t        rank,
    const Properties&  globdat )

  : rank_(rank), desperateMode_(false), viscous_(false)

{
}


Material::~Material()
{}

void   Material::commit()
{}

void   Material::cancel()
{}

void   Material::configure

  ( const Properties& props )

{}

void   Material::getConfig 

  ( const Properties& props ) const

{}

void   Material::updateThermStrain

  ( const Properties& globdat ) 

{}



//--------------------------------------------------------------------
//   allocPoints
//--------------------------------------------------------------------

void Material::allocPoints ( const idx_t count )
{}

void Material::allocPoints 

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

void Material::deallocPoints ( const idx_t count )
{}

//--------------------------------------------------------------------
//   despair
//--------------------------------------------------------------------

bool Material::despair ()
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

bool Material::endDespair ()
{
  desperateMode_ = false;

  return true;
}

//-----------------------------------------------------------------------
//   getHistoryCount
//-----------------------------------------------------------------------

idx_t Material::getHistoryCount () const
{
  return historyNames_.size();
}

//-----------------------------------------------------------------------
//   getHistoryNames
//-----------------------------------------------------------------------

StringVector Material::getHistoryNames 

  () const
{
  return historyNames_;
}

//-----------------------------------------------------------------------
//   getHistoryNames
//-----------------------------------------------------------------------

void Material::getHistoryNames 

  ( const StringVector&  hnames ) const
{
  JEM_ASSERT ( hnames.size() == getHistoryCount() );
  hnames = historyNames_;
}

//-----------------------------------------------------------------------
//   getHistory
//-----------------------------------------------------------------------

void Material::getHistory
 
  ( const Vector&  history,
    const idx_t    ipoint ) const

{
}

//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   newMaterial
//-----------------------------------------------------------------------


Ref<Material>         newMaterial

  ( const String&       name,
    const Properties&   conf,
    const Properties&   props,
    const Properties&   globdat )

{
  Properties     matProps = props.getProps ( name );
  Properties     matConf  = conf.makeProps ( name );

  Ref<Material>  mat;
  String         type;
  idx_t          dim;

  matProps.get ( type, "type" );
  matConf .set ( "type", type );

  matProps.get ( dim, "dim"   );
  matConf .set ( "dim", dim   );
  
  if      ( type == "Hooke" )
  {
    mat = newInstance<HookeMaterial> ( dim, globdat );
  }
  else if ( type == "NeoHooke" )
  {
    mat = newInstance<NeoHookeMaterial> ( dim, globdat );
  }
  else if ( type == "Orthotropic" )
  {
    mat = newInstance<OrthotropicMaterial> ( dim, globdat );
  }
  else if ( type == "Melro" )
  {
    mat = newInstance<MelroMaterial> ( dim, globdat );
  }
  else if ( type == "Damage" )
  {
    mat = newInstance<DamageMaterial> ( dim, globdat );
  }
  else if ( type == "Vogler" )
  {
    mat = newInstance<VoglerMaterial> ( dim, globdat );
  }
  else
  {
    matProps.propertyError (
      name,
      "invalid material type: " + type
    );
  }

  return mat;
}

//-----------------------------------------------------------------------
//   copy helper function
//-----------------------------------------------------------------------

Tuple<double,6> Material::fillFrom3D_

  ( const Vector&    v6 ) const

{
  Tuple<double,6> t6;
  t6[0] = v6[0];
  t6[1] = v6[1];
  t6[2] = v6[2];
  t6[3] = v6[3];
  t6[4] = v6[4];
  t6[5] = v6[5];
  return  t6;
}

Tuple<double,6> Material::fillFrom2D_

  ( const Vector&    v3,
    const double     zzval ) const

{
  Tuple<double,6> t6;
  t6[0] = v3[0];
  t6[1] = v3[1];
  t6[2] = zzval; 
  t6[3] = v3[2];
  t6[4] = 0.;
  t6[5] = 0.;
  return  t6;
}

void Material::reduceTo2D_

  ( const Vector&          v3,
    const double           zz,
    const Tuple<double,6>& v6 ) const 

{
    v3[0] = v6[0];  // xx
    v3[1] = v6[1];  // yy 
    v3[2] = v6[3];  // xy

    v3[0] += zz;    // zz->xx
    v3[1] += zz;    // zz->yy
}

void Material::reduceTo3D_

  ( const Vector&          v3,
    const Tuple<double,6>& v6 ) const 

{
  v3[0] = v6[0];
  v3[1] = v6[1];
  v3[2] = v6[2];
  v3[3] = v6[3];
  v3[4] = v6[4];
  v3[5] = v6[5];
}
