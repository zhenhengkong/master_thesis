#include <jem/base/array/operators.h>
#include <jem/base/array/select.h>
#include <jem/base/array/tensor.h>
#include <jem/base/Error.h>
#include <jem/base/IllegalInputException.h>
#include <jem/base/System.h>
#include <jem/io/FileWriter.h>
#include <jem/numeric/algebra/MatmulChain.h>

#include <jive/geom/Geometries.h>
#include <jive/model/ModelFactory.h>

#include "SolidModel.h"
#include "TbFiller.h"
#include "Invariants.h"

#include <iostream>

using jem::io::FileWriter;
using jem::numeric::matmul;
using jem::numeric::MatmulChain;
using jive::geom::Geometries;

typedef MatmulChain<double,3>   MChain3;
typedef MatmulChain<double,2>   MChain2;
typedef MatmulChain<double,1>   MChain1;

//======================================================================
//   definition
//======================================================================

const char* SolidModel::DOF_NAMES[3]     = {"dx","dy","dz"};
const char* SolidModel::SHAPE_PROP       = "shape";
const char* SolidModel::MATERIAL_PROP    = "material";
const char* SolidModel::THICK_PROP       = "thickness";
const char* SolidModel::LARGE_DISP_PROP  = "largeDisp";
      idx_t SolidModel::nodesWritten_    = 0;
Ref<PrintWriter> SolidModel::nodeOut_    = NIL;

//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------

SolidModel::SolidModel

   ( const String&       name,
     const Properties&   conf,
     const Properties&   props,
     const Properties&   globdat ) : Super(name)
{
  using jive::util::joinNames;

  // create myTag_ (last part of myName_)
  
  StringVector names ( StringUtils::split( myName_, '.' ) );
  myTag_     = names [ names.size() - 1 ];

  Properties  myProps = props.getProps ( myName_ );
  Properties  myConf  = conf.makeProps ( myName_ );

  const String context = getContext();

  egroup_ = ElemGroup::get ( myConf, myProps, globdat, context );

  numElem_   = egroup_.size();
  ielems_    . resize( numElem_ );
  ielems_    = egroup_.getIndices ();
  elems_     = egroup_.getElements ( );
  nodes_     = elems_.getNodes     ( );
  rank_      = nodes_.rank         ( );
  numNode_   = nodes_.size         ( );
  strCount_  = STRAIN_COUNTS[rank_];

  // Make sure that the number of spatial dimensions (the rank of the
  // mesh) is valid.

  if ( rank_ < 1 || rank_ > 3 )
  {
    throw IllegalInputException (
      context,
      String::format (
        "invalid node rank: %d (should be 1, 2 or 3)", rank_
      )
    );
  }

  String shapeProp = joinNames ( myName_, SHAPE_PROP );
  shape_  = newInstance<CustomShape> ( shapeProp, conf, props );

  nodeCount_  = shape_->nodeCount   ();
  ipCount_    = shape_->ipointCount ();
  dofCount_   = rank_ * nodeCount_;

  // Make sure that the rank of the shape matches the rank of the
  // mesh.

  if ( shape_->globalRank() != rank_ )
  {
    throw IllegalInputException (
      context,
      String::format (
        "shape has invalid rank: %d (should be %d)",
        shape_->globalRank (),
        rank_
      )
    );
  }

  // Make sure that each element has the same number of nodes as the
  // shape object.

  elems_.checkSomeElements (
    context,
    ielems_,
    shape_->nodeCount  ()
  );

  dofs_ = XDofSpace::get ( nodes_.getData(), globdat );
  
  dofTypes_.resize( rank_ );

  for( idx_t i = 0; i < rank_; i++)
  {
    dofTypes_[i] = dofs_->addType ( DOF_NAMES[i]);
  }

  dofs_->addDofs (
    elems_.getUniqueNodesOf ( ielems_ ),
    dofTypes_
  );

  // Compute the total number of integration points.

  idx_t  ipCount = shape_->ipointCount() * egroup_.size();

  // Create a material model object.

  material_ = newMaterial ( MATERIAL_PROP, myConf, myProps, globdat );

  material_-> allocPoints  ( ipCount );

  softening_ = dynamicCast<Softening> ( material_ );

  getShapeGrads_ = getShapeGradsFunc ( rank_ );

  thickness_ = 1.;

  if ( rank_ == 2 )
  {
    myProps.find( thickness_, THICK_PROP );
    myConf.set  ( THICK_PROP, thickness_ );
  }

  largeDisp_ = false;
  myProps.find(   largeDisp_, LARGE_DISP_PROP );
  myConf.set  (   LARGE_DISP_PROP, largeDisp_ );
  shape_->setLargeDisp ( largeDisp_ );

  crackBandMethod_ = false;
}

SolidModel::~SolidModel()
{}

//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------

void SolidModel::configure

  ( const Properties&  props,
    const Properties&  globdat )

{
  Properties  myProps  = props.findProps ( myName_ );
  Properties  matProps = myProps.findProps ( MATERIAL_PROP );

  material_->configure ( matProps );

  Properties  params;
  Properties  locOutProps;

  crackBandMethod_ = ( softening_ != NIL && !material_->isViscous() );

  if ( crackBandMethod_ )
  {
    initCharLength_();
  }

  // read properties related to writing of local stress/strain history
  doLocalOutput_ = false;

  if ( myProps.find ( locOutProps, "localOutput" ) )
  {
    // get element numbers

    locDoAll_ = false;
    locOutProps.find ( locDoAll_, "doAll" );

    ArrayBuffer<idx_t> iebuf;
    ArrayBuffer<idx_t> ipbuf;

    if ( locDoAll_)
    {
      doLocalOutput_ = true;
      for ( idx_t i = 0; i < numElem_; ++i )
      {
        for ( idx_t j = 0; j < ipCount_; ++j )
        {
          iebuf.pushBack ( i );
          ipbuf.pushBack ( j );
        }
      }
      locIpNumbers_.ref ( ipbuf.toArray() );
    }
    else
    {
      locOutProps.get ( locElems_, "ielems" );

      if ( locElems_.size() > 0 )
      {
        if ( locOutProps.find ( locIpNumbers_, "ipnumbers" ) )
        {
          JEM_PRECHECK ( locIpNumbers_.size() == locElems_.size() );
        }
        else
        {
          // default: use first ip (works for Gauss1 integration)
          locIpNumbers_ . resize ( locElems_.size() );
          locIpNumbers_ = 0;
        }

        // find element indices for lookup from ielems_

        for ( idx_t i = 0; i < locElems_.size(); ++i )
        {
          for ( idx_t j = 0; j < ielems_.size(); ++j )
          {
            if ( ielems_[j] == locElems_[i] )
            {
              doLocalOutput_ = true;
              iebuf.pushBack ( j );
              ipbuf.pushBack ( locIpNumbers_[i] );
            }
          }
        }
      }
    }
    locIes_.ref ( iebuf.toArray() );
    locIps_.ref ( ipbuf.toArray() );
  }
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void SolidModel::getConfig 

  ( const Properties& conf,
    const Properties& globdat ) const

{
  Properties  myConf  = conf.makeProps ( myName_ );
  Properties  matConf = myConf.makeProps ( MATERIAL_PROP );

  material_->getConfig ( matConf );

  Properties  locOutConf = myConf.makeProps ( "localOutput" );

  if ( locDoAll_ )
  {
    locOutConf.set ( "doAll", true );
  }
  else
  {
    locOutConf.set ( "ielems", locElems_ );
    locOutConf.set ( "ipnumbers", locIpNumbers_ );
  }
}


//-----------------------------------------------------------------------
//   takeAction
//-----------------------------------------------------------------------


bool SolidModel::takeAction

  ( const String&      action,
    const Properties&  params,
    const Properties&  globdat )

{
  using jive::model::Actions;
  using jive::model::ActionParams;

  if ( action == Actions::GET_MATRIX0 
    || action == Actions::GET_INT_VECTOR )
  {
    Ref<MatrixBuilder>  mbuilder;

    Vector  disp;
    Vector  force;

    // Get the current displacements.

    StateVector::get ( disp, dofs_, globdat );

    // Get the matrix builder and the internal force vector.

    params.find( mbuilder, ActionParams::MATRIX0 );
    params.get ( force,    ActionParams::INT_VECTOR );

    getMatrix_ ( mbuilder, force, disp );

    return true;
  }

  if ( action == Actions::GET_MATRIX2 )
  {
    Ref<MatrixBuilder> mbuilder;

    params.get ( mbuilder, ActionParams::MATRIX2 );
    
    getMatrix2_( *mbuilder );

    return true;
  }

  if ( action == "GET_DISSIPATION" )
  {
    getDissipation_ ( params );

    return false;
  }

  if ( action == "DESPAIR" )
  {
    return material_->despair();
  }

  if ( action == "END_DESPAIR" )
  {
    material_->endDespair();

    return true;
  }

  if ( action == Actions::COMMIT )
  {
    material_->commit ();

    if ( doLocalOutput_ )
    {
      Vector  disp;

      StateVector::get ( disp, dofs_, globdat );

      writeLocalOutput_ ( disp, globdat );
    }

    return true;
  }
  
  // actions from ThermalModule

  if ( action == ThermNames::APPLY_TEMP )
  {
    material_->updateThermStrain ( params );
  }

  if ( action == ThermNames::GET_FORCE )
  {
    Vector  disp;
    Vector  fth0;

    StateVector::getOld ( disp, dofs_, globdat );
    globdat.get ( fth0, SolverNames::DISSIPATION_FORCE );

    getThermForce_ ( fth0, disp );

    return true;
  }

  if ( action == SolverNames::GET_DISS_FORCE )
  {
    Ref<Plasticity> p = dynamicCast<Plasticity> ( material_ );

    if ( p == NIL ) return false;

    Vector disp;
    Vector fDiss;

    StateVector::getOld ( disp, dofs_, globdat );
    globdat.get ( fDiss, SolverNames::DISSIPATION_FORCE );

    getDissForce_ ( p, fDiss, disp );

    return true;
  }

  if ( action == SolverNames::SET_STEP_SIZE )
  {
    double             dt;
    params.get       ( dt, SolverNames::STEP_SIZE );

    material_->setDT ( dt );
    return true;
  }

  if ( action == Actions::GET_TABLE )
  {
    return getTable_ ( params, globdat );
  }

  if ( action == "WRITE_XOUTPUT" )
  {
    if ( dw_.amFirstWriter ( this ) )
    {
      writeDisplacements_ ( params, globdat );
    }
  }

  return false;
}


//-----------------------------------------------------------------------
//   getMatrix_
//-----------------------------------------------------------------------


void SolidModel::getMatrix_

  ( Ref<MatrixBuilder>  mbuilder,
    const Vector&       force,
    const Vector&       disp ) const

{
  Matrix      stiff      ( strCount_, strCount_ );
  Matrix      coords     ( rank_, nodeCount_ );

  Matrix      elemMat    ( dofCount_, dofCount_  );
  Vector      elemForce  ( dofCount_ );
  Vector      elemDisp   ( dofCount_ );

  Vector      strain     ( strCount_ );
  Vector      stress     ( strCount_ );

  Matrix      b          ( strCount_, dofCount_  );
  Matrix      bt         = b.transpose ();

  IdxVector   inodes     ( nodeCount_ );
  IdxVector   idofs      ( dofCount_  );

  Vector      ipWeights  ( ipCount_   );

  MChain1     mc1;
  MChain3     mc3;

  idx_t         ipoint = 0;

  // Iterate over all elements assigned to this model.

  for ( idx_t ie = 0; ie < numElem_; ie++ )
  {
    // Get the global element index.

    idx_t  ielem = ielems_[ie];

    // Get the element coordinates and DOFs.

    elems_.getElemNodes  ( inodes, ielem    );
    nodes_.getSomeCoords ( coords, inodes );
    dofs_->getDofIndices ( idofs,  inodes, dofTypes_ );

    // Compute the spatial derivatives of the element shape
    // functions.

    shape_->setGradsForIntegration ( ipWeights, coords, ie );

    // for 2D: multiply ipWeights with thickness

    ipWeights *= thickness_;

    // Get the displacements at the element nodes.

    elemDisp = select ( disp, idofs );

    // Assemble the element matrix.

    elemMat   = 0.0;
    elemForce = 0.0;

    for ( idx_t ip = 0; ip < ipCount_; ip++ )
    {     
      // Compute the B-matrix for this integration point.
      // Compute the strain vector of this integration point

      shape_->getStrain ( strain, b, elemDisp, ip, ie );

      // Get the tangent stiffness matrix and the stress vector
      // from the Material given the current strain

      if ( crackBandMethod_ )
      {
        double le = charLength_[ielem];
        softening_->update ( stress, stiff, strain, ipoint++, le );
      }
      else
      {
        material_->update ( stress, stiff, strain, ipoint++ );
      }

      // System::out() << " stress " << stress << endl;
      // System::out() << " strain " << strain << endl;
      // System::out() << " stiff  " << endl << stiff << endl;
     
      // Compute the element force vector.

      elemForce += ipWeights[ip] * mc1.matmul ( bt, stress );

      // Compute the stiffness matrix
      
      elemMat   += ipWeights[ip] * mc3.matmul ( bt, stiff, b );

      // Add element stiffness matrix large displacements

      if ( largeDisp_ )
      {
        shape_->addElemMatLargeD 
          ( elemMat, stress, ipWeights[ip], ip, ie );
      }
    }

    // Add the element matrix to the global stiffness matrix.
    if ( mbuilder != NIL )
    {
      mbuilder->addBlock ( idofs, idofs, elemMat );
    }

    // Add the element force vector to the global force vector.

    select ( force, idofs ) += elemForce;
  }
}

//-----------------------------------------------------------------------
//   getMatrix2_
//-----------------------------------------------------------------------

// compute the mass matrix 
// current implementation: consistent mass matrix

void SolidModel::getMatrix2_

    ( MatrixBuilder&          mbuilder )
{
  Matrix      coords     ( rank_, nodeCount_ );
  Matrix      elemMat    ( dofCount_, dofCount_ );

  Matrix      R          ( rank_, rank_ );

  Matrix      sfuncs     = shape_->getShapeFunctions ();
  Matrix      N          ( rank_, rank_ * nodeCount_ );
  Matrix      Nt         = N.transpose ( ); 

  IdxVector   inodes     ( nodeCount_ );
  IdxVector   idofs      ( dofCount_  );

  Vector      ipWeights  ( ipCount_   );

  MChain3     mc3;

  double      rho = 0.0024;

  R = 0.0;
 
  for ( idx_t i = 0; i < rank_ ; i++ )
  {
    R(i,i) = rho;
  }

  // Iterate over all elements assigned to this model.

  for ( idx_t ie = 0; ie < numElem_; ie++ )
  {
    // Get the global element index.

    idx_t  ielem = ielems_[ie];

    // Get the element coordinates and DOFs.

    elems_.getElemNodes  ( inodes, ielem    );
    nodes_.getSomeCoords ( coords, inodes );
    dofs_->getDofIndices ( idofs,  inodes, dofTypes_ );

    // Assemble the element matrix and the internal force vector.

    elemMat   = 0.0;

    for ( idx_t ip = 0; ip < ipCount_; ip++ )
    {
      // compute matrix of shape function N       

      getShapeFuncs_ ( N, sfuncs(ALL,ip) );

      // Add the contribution of this integration point.

      elemMat   += ipWeights[ip] * mc3.matmul ( Nt, R, N );
    }

    // Add the element secant matrix to the global stiffness matrix.

    mbuilder.addBlock ( idofs, idofs, elemMat );

  }
}

//-----------------------------------------------------------------------
//   getThermForce_
//-----------------------------------------------------------------------


void SolidModel::getThermForce_

  ( const Vector&       fth0,
    const Vector&       disp )   const

{
  Matrix      coords     ( rank_, nodeCount_     );
  Matrix      b          ( strCount_, dofCount_  );
  Matrix      stiff      ( strCount_, strCount_  );
  Matrix      bt         = b.transpose         ( );
  Matrix      dt         = stiff.transpose     ( );

  Vector      strain     ( strCount_ );
  Vector      stress     ( strCount_ );
  Vector      ipWeights  ( ipCount_  );

  MChain2     mc2;

  IdxVector   inodes     ( nodeCount_ );
  IdxVector   idofs      ( dofCount_  );
  Vector      epsth      ( strCount_  );
  Vector      elemForce  ( dofCount_  );
  Vector      elemDisp   ( dofCount_ );

  idx_t ipoint = 0;
  // for linear material: get stiffness once

  material_->getThermStrain ( epsth );

  for ( idx_t ie = 0; ie < numElem_; ++ie )
  {
    idx_t ielem = ielems_[ie];

    // Get the element coordinates and DOFs.

    elems_.getElemNodes  ( inodes, ielem             );
    nodes_.getSomeCoords ( coords, inodes            );
    dofs_->getDofIndices ( idofs , inodes, dofTypes_ );

    shape_->setGradsForIntegration ( ipWeights, coords, ie );

    elemDisp   = select ( disp, idofs );

    ipWeights *= thickness_;

    elemForce = 0.0;

    for ( idx_t ip = 0; ip < ipCount_; ip++ )
    {
      shape_->getStrain ( strain, b, elemDisp, ip, ie );

      if ( crackBandMethod_ )
      {
        double le = charLength_[ielem];
        softening_->update ( stress, stiff, strain, ipoint, le );
      }
      else
      {
        material_->update ( stress, stiff, strain, ipoint );
      }
      ++ipoint;

      elemForce += ipWeights[ip] * mc2.matmul ( bt, dt, epsth );
    }

    // Add the element force vector to the global force vector.

    select ( fth0, idofs ) += elemForce;
  }
}

//-----------------------------------------------------------------------
//   getDissForce_
//-----------------------------------------------------------------------


void SolidModel::getDissForce_

  ( const Ref<Plasticity> p,
    const Vector&         fstar,
    const Vector&         disp )   const

{
  Matrix      coords     ( rank_, nodeCount_ );
  Matrix      b          ( strCount_, dofCount_  );
  Matrix      bt         = b.transpose ();
  idx_t       ipoint     = 0;

  Vector      ipWeights  ( ipCount_ );

  IdxVector   inodes     ( nodeCount_ );
  IdxVector   idofs      ( dofCount_  );
  Vector      strain     ( strCount_  );
  Vector      elemForce  ( dofCount_  );
  Vector      elemDisp   ( dofCount_  );
  Vector      sstar      ( strCount_  );

  for ( idx_t ie = 0; ie < numElem_; ++ie )
  {
    idx_t ielem = ielems_[ie];

    elems_.getElemNodes  ( inodes, ielem             );
    nodes_.getSomeCoords ( coords, inodes            );
    dofs_->getDofIndices ( idofs , inodes, dofTypes_ );

    shape_->setGradsForIntegration ( ipWeights, coords, ie );

    elemDisp   = select ( disp, idofs );

    ipWeights *= thickness_;

    elemForce = 0.0;

    for ( idx_t ip = 0; ip < ipCount_; ip++ )
    {
      // get dissipation stress F^T*sigma+D^T*eps^p

      shape_->getStrain ( strain, b, elemDisp, ip, ie );

      matmul ( strain, b, elemDisp );

      p->getDissipationStress ( sstar, strain, ipoint++ );

      elemForce += ipWeights[ip] * matmul ( bt, sstar );
    }
    // Add the element force vector to the global force vector.

    fstar[idofs] += elemForce;
  }
}

//-----------------------------------------------------------------------
//   writeLocalOutput_
//-----------------------------------------------------------------------

void SolidModel::writeLocalOutput_

  ( const Vector&       disp,
    const Properties&   globdat )

{
  idx_t       it;

  Matrix      stiff      ( strCount_, strCount_ );
  Matrix      coords     ( rank_, nodeCount_ );
  Vector      elemDisp   ( dofCount_ );
  Vector      strain     ( strCount_ );
  Vector      stress     ( strCount_ );

  Matrix      b          ( strCount_, dofCount_  );

  IdxVector   inodes     ( nodeCount_ );
  IdxVector   idofs      ( dofCount_  );

  Vector      ipWeights  ( ipCount_   );

  if ( locOut_ == NIL )
  {
    initLocOutWriter_ ();
  }

  globdat.get ( it, Globdat::TIME_STEP );

  *locOut_ << "newXOutput " << it << " (strain, stress, dissipation, history)\n";

  // Iterate over all ips for which output is to be written

  for ( idx_t i = 0; i < locIes_.size(); ++i )
  {
    idx_t ie = locIes_[i];
    idx_t ip = locIps_[i];
    idx_t ielem = ielems_[ie];
    idx_t ipoint = ipCount_ * ie + ip;

    elems_.getElemNodes  ( inodes, ielem    );
    nodes_.getSomeCoords ( coords, inodes );
    dofs_->getDofIndices ( idofs,  inodes, dofTypes_ );

    shape_->setGradsForIntegration ( ipWeights, coords, ie );

    elemDisp = select ( disp, idofs );

    shape_->getStrain ( strain, b, elemDisp, ip, ie );

    if ( crackBandMethod_ )
    {
      double le = charLength_[ielem];
      softening_->update ( stress, stiff, strain, ipoint, le );
    }
    else
    {
      material_->update ( stress, stiff, strain, ipoint );
    }
    for ( idx_t j = 0; j < strCount_; ++j ) 
    { 
      *locOut_ << strain[j] << " ";
    }
    for ( idx_t j = 0; j < strCount_; ++j ) 
    { 
      *locOut_ << stress[j] << " ";
    }
    *locOut_ << material_->giveDissipation ( ipoint );
    *locOut_ << endl;
  }
  locOut_->flush();
}

//-----------------------------------------------------------------------
//   initCharLength_
//-----------------------------------------------------------------------

void SolidModel::initCharLength_ ()

{
  IdxVector   inodes     (            nodeCount_ );
  Matrix      coords     ( rank_,     nodeCount_ );

  // maimi07i for triangles
  // double      fac   = 2. / sqrt ( sqrt(3.) ); 

  // my own expression 6/pi/3^.25
  double      fac   = 1.4512;

  double      maxLe = softening_->maxAllowedLength();

  idx_t       ielem;

  charLength_.resize ( max(ielems_)+1 );

  for ( idx_t ie = 0; ie < numElem_; ++ie )
  {
    ielem   = ielems_[ie];

    Vector    ipWeights ( ipCount_ );

    elems_.getElemNodes  ( inodes, ielem  );
    nodes_.getSomeCoords ( coords, inodes );

    shape_->getIntegrationWeights ( ipWeights, coords );

    double area = sum ( ipWeights );
    double le   = fac * sqrt ( area );

    if ( le > maxLe )
    {
      System::warn() << "characteristic length of element " <<
        ielem << " results in local snapback!\n" <<
        "changed from " << le << " to " << maxLe  << endl;

      charLength_[ielem] = maxLe;
    }
    else
    {
      charLength_[ielem] = le;
    }
  }
}

//-----------------------------------------------------------------------
//   getDissipation_
//-----------------------------------------------------------------------

void SolidModel::getDissipation_

  ( const Properties&  params )

{
  const idx_t  nodeCount  = shape_->nodeCount   ();
  const idx_t  ipCount    = shape_->ipointCount ();
  const idx_t  ielemCount = ielems_.size        ();

  IdxVector    inodes    (        nodeCount );
  Matrix       coords    ( rank_, nodeCount );
  Vector       ipWeights (          ipCount );

  idx_t  ipoint      = 0;
  double dissipation = 0.;

  // bulk damage

  for ( idx_t ie = 0; ie < ielemCount; ++ie )
  {
    idx_t ielem = ielems_[ie];

    elems_.getElemNodes  ( inodes, ielem   );
    nodes_.getSomeCoords ( coords, inodes );

    // get the correct shape and then the number of idx_t points

    shape_->getIntegrationWeights ( ipWeights, coords );

    for ( idx_t ip = 0; ip < ipCount; ++ip )
    {
      dissipation += ipWeights[ip] * material_->giveDissipation ( ipoint++ );
    }
  }
  params.set ( myTag_, dissipation );
}

//-----------------------------------------------------------------------
//   getTable_
//-----------------------------------------------------------------------


bool SolidModel::getTable_

  ( const Properties&  params,
    const Properties&  globdat )

{
  using jive::model::Actions;
  using jive::model::ActionParams;
  using jive::model::StateVector;

  String       contents;
  Ref<XTable>  table;
  Vector       weights;
  String       name;

  Vector       disp;

  StateVector::get ( disp, dofs_, globdat );

  // Get the table, the name of the table, and the table row weights
  // from the action parameters.

  params.get ( table,   ActionParams::TABLE );
  params.get ( name,    ActionParams::TABLE_NAME );
  params.get ( weights, ActionParams::TABLE_WEIGHTS );

  // Stress value are computed in the nodes.

  if ( name == "stress" &&
       table->getRowItems() == nodes_.getData() )
  {
    getStress_ ( *table, weights, disp );

    return true;
  }
  else if ( name == "xoutTable" )
  {
    params.get ( contents, "contentString" );

    getXOutTable_ ( table, weights, contents, disp );

    return true;
  }
  return false;
}

//-----------------------------------------------------------------------
//   getStress_
//-----------------------------------------------------------------------


void SolidModel::getStress_

  ( XTable&        table,
    const Vector&  weights,
    const Vector&  disp )

{
  IdxVector   ielems     = egroup_.getIndices  ();

  Matrix     ndNStress  ( nodeCount_, strCount_ );  // nodal normal stress
  Vector     ndWeights  ( nodeCount_ );
  Matrix     stiff      ( strCount_,  strCount_ );

  Matrix     coords     ( rank_,     nodeCount_ );
  Matrix     b          ( strCount_, dofCount_  );

  Vector     nStressIp  ( strCount_ );    // normal stress vector at idx_t.pt.
  Vector     strain     ( strCount_ );
  Vector     elemDisp   ( dofCount_ );

  IdxVector  inodes     ( nodeCount_ );
  IdxVector  idofs      ( dofCount_  );
  IdxVector  jcols      ( strCount_  );

  jcols.resize ( strCount_ );

  // Add the columns for the stress components to the table.

  switch ( strCount_ )
  {
  case 1:

    jcols[0] = table.addColumn ( "xx" );

    break;

  case 3:

    jcols[0] = table.addColumn ( "xx" );
    jcols[1] = table.addColumn ( "yy" );
    jcols[2] = table.addColumn ( "xy" );

    break;

  case 6:

    jcols[0] = table.addColumn ( "xx" );
    jcols[1] = table.addColumn ( "yy" );
    jcols[2] = table.addColumn ( "zz" );
    jcols[3] = table.addColumn ( "xy" );
    jcols[4] = table.addColumn ( "yz" );
    jcols[5] = table.addColumn ( "xz" );

    break;

  default:

    throw Error (
      JEM_FUNC,
      "unexpected number of stress components: " +
      String ( strCount_ )
    );
  }

  idx_t         ipoint = 0;

  Vector      ipWeights ( ipCount_ );

  for ( idx_t ie = 0; ie < numElem_; ie++ )
  {
    // Get the global element index.

    idx_t  ielem = ielems[ie];

    ndNStress  = 0.0;
    ndWeights  = 0.0;

    elems_.getElemNodes  ( inodes, ielem );
    dofs_->getDofIndices ( idofs,  inodes,  dofTypes_ );

    nodes_.getSomeCoords ( coords, inodes );

    shape_->setGradsForIntegration ( ipWeights, coords, ie );

    elemDisp = select ( disp, idofs );

    Matrix     sfuncs     = shape_->getShapeFunctions ();

    // Iterate over the integration points.

    for ( idx_t ip = 0; ip < ipCount_; ip++ )
    {
      shape_->getStrain ( strain, b, elemDisp, ip, ie );

      if ( crackBandMethod_ )
      {
        double le = charLength_[ielem];
        softening_->update ( nStressIp, stiff, strain, ipoint, le );
      }
      else
      {
        material_->update ( nStressIp, stiff, strain, ipoint );
      }

      ndNStress += matmul ( sfuncs(ALL,ip), nStressIp );

      ndWeights += sfuncs(ALL,ip);

      ++ipoint; 
    }

    select ( weights, inodes ) += ndWeights;

    // Add the stresses to the table.

    table.addBlock ( inodes, jcols[slice(0,strCount_)],   ndNStress );
  }
}

//-----------------------------------------------------------------------
//    writeElements_
//-----------------------------------------------------------------------

void SolidModel::writeElements_

  ( const Properties&  params,
    const Properties&  globdat )

{
  IdxVector  inodes ( nodeCount_ );

  if ( elemOut_ == NIL )
  {
    // open file and write connectivity of all elements

    elemOut_ = initWriter_ ( params, "elems" );

    // write connectivity of elements

    for ( idx_t ie = 0; ie < numElem_; ++ie )
    {
      idx_t ielem = ielems_[ie];

      elems_.getElemNodes ( inodes, ielem );

      *elemOut_ << ielem << " ";

      for ( idx_t in = 0; in < nodeCount_; ++in )
      {
        *elemOut_ << inodes[in] << ' ';
      }
      *elemOut_ << '\n';
    }
  }
  elemOut_->flush();
}

//-----------------------------------------------------------------------
//    writeNodes_
//-----------------------------------------------------------------------

void SolidModel::writeNodes_

  ( const Properties&  params,
    const Properties&  globdat )

{
  if ( nodeOut_ == NIL )
  {
    // all nodes are written in one file, nodeOut_ is static

    String       prepend;
    String       filename;

    if ( params.find( prepend, "prepend" ) )
    {
      filename = prepend + String(".all.nodes");
    }
    else
    {
      filename = "all.nodes";
    }
    nodeOut_ = newInstance<PrintWriter>(
               newInstance<FileWriter> ( filename ) );
  }

  Vector      pCoords    ( rank_ );

  // quasi nodes

  while ( nodesWritten_ < nodes_.size() )
  {
    nodes_.getNodeCoords( pCoords, nodesWritten_ );

    *nodeOut_ << nodesWritten_++ <<  " ";

    for ( idx_t j = 0; j < rank_; ++j )
    {
      *nodeOut_ << pCoords[j] << " ";
    }

    *nodeOut_ << "\n";
  }

  nodeOut_->flush();
}

//-----------------------------------------------------------------------
//    initLocOutWriter_
//-----------------------------------------------------------------------

void  SolidModel::initLocOutWriter_ ()

{
  Matrix      coords     ( rank_, nodeCount_ );
  Matrix      ipcoords   ( rank_, ipCount_ );
  IdxVector   inodes     ( nodeCount_ );

  // open file

  locOut_ = initWriter_ ( Properties(), "locOut" );

  // write coordinate data

  *locOut_ << "number of points, dim:\n" << locIes_.size() << " " << rank_ 
    << "\ncoords of points for which history is written:\n";

  for ( idx_t i = 0; i < locIes_.size(); i++ )
  {
    idx_t ie = locIes_[i];
    idx_t ip = locIpNumbers_[i];
    idx_t ielem = ielems_[ie];

    elems_.getElemNodes  ( inodes, ielem    );
    nodes_.getSomeCoords ( coords, inodes );
    shape_->getGlobalIntegrationPoints ( ipcoords, coords );

    for ( idx_t j = 0; j < rank_; ++j )
    {
      locOut_->printFloat ( ipcoords(j,ip) ); 
      locOut_->printSpace ();
    }
    locOut_->printLine ();
  }
}

//-----------------------------------------------------------------------
//    initWriter_
//-----------------------------------------------------------------------

Ref<PrintWriter>  SolidModel::initWriter_

  ( const Properties&  params, 
    const String       name )  const

{
  // Open file for output

  StringVector fileName;
  String       prepend;

  if ( params.find( prepend, "prepend" ) )
  { 
    fileName.resize(3);

    fileName[0] = prepend;
    fileName[1] = myTag_;
    fileName[2] = name;
  }
  else
  {
    fileName.resize(2);

    fileName[0] = myTag_;
    fileName[1] = name;
  }

  return newInstance<PrintWriter>( newInstance<FileWriter> ( 
         StringUtils::join( fileName, "." ) ) );
}

//-----------------------------------------------------------------------
//    writeDisplacements_
//-----------------------------------------------------------------------

void SolidModel::writeDisplacements_

  ( const Properties&  params,
    const Properties&  globdat )

{
  Vector      dispi      ( rank_ );
  IdxVector   idofs      ( rank_ );
  Vector      disp;
  idx_t         it;

  if ( dispOut_ == NIL )
  {
    dispOut_ = initWriter_ ( params, "disp" );
  }

  globdat.get ( it, Globdat::TIME_STEP );

  *dispOut_ << "newXOutput " << it << '\n';

  StateVector::get ( disp, dofs_, globdat );

  // regular nodes

  for ( idx_t inode = 0; inode < nodes_.size(); ++inode )
  {
    dofs_->getDofsForItem ( idofs,  dofTypes_,  inode );
    dispi = select ( disp, idofs );

    *dispOut_ << inode << " ";

    for ( idx_t j = 0; j < rank_; ++j )
    {
      *dispOut_ << dispi[j] << " ";
    }
    *dispOut_ << '\n';
  }
  dispOut_->flush();
}

//-----------------------------------------------------------------------
//   getXOutTable_
//-----------------------------------------------------------------------


void SolidModel::getXOutTable_

  ( Ref<XTable>        table,
    const Vector&      weights,
    const String&      contents,
    const Vector&      disp )

{
  Vector       ndWeights  ( nodeCount_ );
  StringVector hisNames   = material_->getHistoryNames ();

  Matrix       coords     ( rank_,     nodeCount_ );
  Matrix       b          ( strCount_, dofCount_  );
  Matrix       stiff      ( strCount_, strCount_  );

  Vector       elemDisp   ( dofCount_ );

  IdxVector    inodes     ( nodeCount_ );
  IdxVector    idofs      ( dofCount_  );

  const bool   tri6 = ( shape_->getGeometry() == Geometries::TRIANGLE
                      && nodeCount_ == 6 );

  // tell TbFiller which types are available to write

  TbFiller   tbFiller   ( rank_ );

  Slice      iistrain   = tbFiller.announce ( "strain.tensor" );
  Slice      iistress   = tbFiller.announce ( "stress.tensor" );
  Slice      iipstress  = tbFiller.announce ( "pstress.diag" );
  Slice      iihistory  = tbFiller.announce ( hisNames );

  Vector     ipValues   ( tbFiller.typeCount() );

  Vector     strain     ( ipValues[iistrain]   );
  Vector     stress     ( ipValues[iistress]   );
  Vector     pstress    ( ipValues[iipstress]  );
  Vector     history    ( ipValues[iihistory]  );

  // Let TbFiller find out which columns of ndValues to write to 
  // which columns of the table (based on filter in input file)

  IdxVector  i2table;
  IdxVector  jcols;

  tbFiller . setFilter   ( contents );
  tbFiller . prepareTable( i2table, jcols, table );

  Matrix     ndValuesOut ( nodeCount_, i2table.size() );
  Vector     ipValuesOut ( i2table.size() );

  // fill table in loop over elements

  idx_t      ipCount;
  idx_t      ipoint = 0;

  Vector     ipWeights;

  // Add the columns for the stress components to the table.

  const idx_t nel = ielems_.size();

  Matrix      sfuncs     = shape_->getShapeFunctions ();

  for ( idx_t ie = 0; ie < nel; ++ie )
  {
    idx_t ielem = ielems_[ie];

    ndValuesOut = 0.;
    ndWeights   = 0.;

    elems_.getElemNodes  ( inodes, ielem );
    dofs_->getDofIndices ( idofs,  inodes,  dofTypes_ );

    ipCount  = shape_->ipointCount ();

    ipWeights.resize ( ipCount   );

    nodes_.getSomeCoords ( coords, inodes );

    shape_->setGradsForIntegration ( ipWeights, coords, ie );

    elemDisp = select ( disp, idofs );

    // Iterate over the integration points.
    // Gather all data, no matter which is asked, to keep code neat
    // The option to specify output is primarily for disk size, not CPU time

    for ( idx_t ip = 0; ip < ipCount; ip++, ++ipoint )
    {
      shape_->getStrain ( strain, b, elemDisp, ip, ie );

      if ( crackBandMethod_ )
      {
        double le = charLength_[ielem];
        softening_->update ( stress, stiff, strain, ipoint, le );
      }
      else
      {
        material_->update ( stress, stiff, strain, ipoint );
      }
      material_-> getHistory ( history, ipoint );

      StressInvariants inv ( material_->fill3DStress ( stress ) );
      Vec3 pstr3 = inv.getSortedPrincipalValues ();

      for ( idx_t j = 0; j < pstress.size(); ++j ) pstress[j] = pstr3[j];

      ipValuesOut  = ipValues[i2table];
      ndValuesOut += matmul ( sfuncs(ALL,ip), ipValuesOut ); 
      ndWeights   += sfuncs(ALL,ip);
    }

    if ( tri6 ) TbFiller::permTri6 ( ndWeights, ndValuesOut );

    select ( weights, inodes ) += ndWeights;

    // Add the stresses to the table.

    table->addBlock ( inodes, jcols, ndValuesOut );
  }
}

//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   newSolidModel
//-----------------------------------------------------------------------


static Ref<Model>     newSolidModel

  ( const String&       name,
    const Properties&   conf,
    const Properties&   props,
    const Properties&   globdat )

{
  return newInstance<SolidModel> ( name, conf, props, globdat );
}


//-----------------------------------------------------------------------
//   declareSolidModel
//-----------------------------------------------------------------------


void declareSolidModel ()
{
  using jive::model::ModelFactory;

  ModelFactory::declare ( "Solid", & newSolidModel );
}

