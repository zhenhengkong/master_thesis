#include <jem/base/array/operators.h>
#include <jem/base/array/select.h>
#include <jem/base/array/tensor.h>
#include <jem/base/Error.h>
#include <jem/base/IllegalInputException.h>
#include <jem/base/System.h>
#include <jem/io/FileWriter.h>
#include <jem/numeric/algebra/MatmulChain.h>

#include <jive/geom/Geometries.h>
#include <jive/geom/IShapeFactory.h>
#include <jive/model/ModelFactory.h>

#include "NLSolidModel.h"
#include "TbFiller.h"
#include "Invariants.h"
#include "LargeDispUtilities.h"

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

const char* NLSolidModel::DOF_NAMES[3]     = {"dx","dy","dz"};
const char* NLSolidModel::SHAPE_PROP       = "shape";
const char* NLSolidModel::MATERIAL_PROP    = "material";
const char* NLSolidModel::THICK_PROP       = "thickness";
const char* NLSolidModel::LARGE_DISP_PROP  = "largeDisp";
      idx_t NLSolidModel::nodesWritten_    = 0;
Ref<PrintWriter> NLSolidModel::nodeOut_    = NIL;

//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------

NLSolidModel::NLSolidModel

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

  shape_  = jive::geom::IShapeFactory::newInstance(
    joinNames (myName_, SHAPE_PROP ),
    conf,
    props );

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

  material_ = newNLMaterial ( MATERIAL_PROP, myConf, myProps, globdat );

  material_-> allocPoints  ( ipCount );

  getShapeGrads_ = getShapeGradsFunc ( rank_ );

  thickness_ = 1.;

  if ( rank_ == 2 )
  {
    myProps.find( thickness_, THICK_PROP );
    myConf.set  ( THICK_PROP, thickness_ );
  }
}

NLSolidModel::~NLSolidModel()
{}

//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------

void NLSolidModel::configure

  ( const Properties&  props,
    const Properties&  globdat )

{
  Properties  myProps  = props.findProps ( myName_ );
  Properties  matProps = myProps.findProps ( MATERIAL_PROP );

  material_->configure ( matProps );
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void NLSolidModel::getConfig 

  ( const Properties& conf,
    const Properties& globdat ) const

{
  Properties  myConf  = conf.makeProps ( myName_ );
  Properties  matConf = myConf.makeProps ( MATERIAL_PROP );

  material_->getConfig ( matConf );
}


//-----------------------------------------------------------------------
//   takeAction
//-----------------------------------------------------------------------


bool NLSolidModel::takeAction

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


void NLSolidModel::getMatrix_

  ( Ref<MatrixBuilder>  mbuilder,
    const Vector&       force,
    const Vector&       disp ) const

{
  Matrix      stiff      ( strCount_, strCount_ );
  Matrix      coords     ( rank_, nodeCount_ );

  Matrix      elemMat    ( dofCount_, dofCount_  );
  Vector      elemForce  ( dofCount_ );
  Vector      elemDisp   ( dofCount_ );

  Cubix       grads      ( rank_, nodeCount_, ipCount_ );
  Vector      strain     ( strCount_ );
  Vector      stress     ( strCount_ );

  Matrix      defGrad    ( rank_, rank_ );
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

    shape_->getShapeGradients ( grads, ipWeights, coords );

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

      Matrix grad  ( grads(ALL,ALL,ip) );
      getStrain_ ( strain, defGrad, b, grad, elemDisp );

      // Get the tangent stiffness matrix and the stress vector
      // from the Material given the current strain

      material_->update ( stress, stiff, strain, defGrad, ipoint++ );

      // System::out() << " stress " << stress << endl;
      // System::out() << " strain " << strain << endl;
      // System::out() << " stiff  " << endl << stiff << endl;
     
      // Compute the element force vector.

      elemForce += ipWeights[ip] * mc1.matmul ( bt, stress );

      // Compute the stiffness matrix
      
      elemMat   += ipWeights[ip] * mc3.matmul ( bt, stiff, b );

      // Add element stiffness matrix large displacements

      addElemMatLargeD_ ( elemMat, stress, grad, ipWeights[ip] );
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

void NLSolidModel::getMatrix2_

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


void NLSolidModel::getThermForce_

  ( const Vector&       fth0,
    const Vector&       disp )   const

{
  Matrix      coords     ( rank_, nodeCount_     );
  Matrix      b          ( strCount_, dofCount_  );
  Matrix      stiff      ( strCount_, strCount_  );
  Matrix      bt         = b.transpose         ( );
  Matrix      dt         = stiff.transpose     ( );
  Cubix       grads      ( rank_, nodeCount_, ipCount_ );
  Matrix      defGrad    ( rank_, rank_ );

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

    shape_->getShapeGradients ( grads, ipWeights, coords );

    elemDisp   = select ( disp, idofs );

    ipWeights *= thickness_;

    elemForce = 0.0;

    for ( idx_t ip = 0; ip < ipCount_; ip++ )
    {
      Matrix grad  ( grads(ALL,ALL,ip) );
      getStrain_ ( strain, defGrad, b, grad, elemDisp );

      material_->update ( stress, stiff, strain, defGrad, ipoint );
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


void NLSolidModel::getDissForce_

  ( const Ref<Plasticity> p,
    const Vector&         fstar,
    const Vector&         disp )   const

{
  Matrix      coords     ( rank_, nodeCount_ );
  Matrix      b          ( strCount_, dofCount_  );
  Matrix      bt         = b.transpose ();
  idx_t       ipoint     = 0;
  Cubix       grads      ( rank_, nodeCount_, ipCount_ );
  Matrix      defGrad    ( rank_, rank_ );

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

    shape_->getShapeGradients ( grads, ipWeights, coords );

    elemDisp   = select ( disp, idofs );

    ipWeights *= thickness_;

    elemForce = 0.0;

    for ( idx_t ip = 0; ip < ipCount_; ip++ )
    {
      // get dissipation stress F^T*sigma+D^T*eps^p

      Matrix grad  ( grads(ALL,ALL,ip) );
      getStrain_ ( strain, defGrad, b, grad, elemDisp );

      matmul ( strain, b, elemDisp );

      p->getDissipationStress ( sstar, strain, ipoint++ );

      elemForce += ipWeights[ip] * matmul ( bt, sstar );
    }
    // Add the element force vector to the global force vector.

    fstar[idofs] += elemForce;
  }
}

//-----------------------------------------------------------------------
//   getDissipation_
//-----------------------------------------------------------------------

void NLSolidModel::getDissipation_

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


bool NLSolidModel::getTable_

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
//   getStrain_
//-----------------------------------------------------------------------


void NLSolidModel::getStrain_

   ( const Vector& strain,
     const Matrix& defGrad,
     const Matrix& b,
     const Matrix& grad,
     const Vector& disp )    const

{
  evalDeformationGradient ( defGrad, disp, grad );
  getGreenLagrangeStrain  ( strain, defGrad );
  if ( rank_ == 2 )
  {
    getBMatrixLin2D       ( b, defGrad, grad );
  }
  else
  {
    getBMatrixLin3D       ( b, defGrad, grad );
  }
}

//-----------------------------------------------------------------------
//   getStress_
//-----------------------------------------------------------------------


void NLSolidModel::getStress_

  ( XTable&        table,
    const Vector&  weights,
    const Vector&  disp )

{
  IdxVector   ielems     = egroup_.getIndices  ();

  Matrix     ndNStress  ( nodeCount_, strCount_ );  // nodal normal stress
  Vector     ndWeights  ( nodeCount_ );
  Matrix     stiff      ( strCount_,  strCount_ );

  Matrix     defGrad    ( rank_, rank_ );
  Cubix      grads      ( rank_, nodeCount_, ipCount_ );
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

    shape_->getShapeGradients ( grads, ipWeights, coords );

    elemDisp = select ( disp, idofs );

    Matrix     sfuncs     = shape_->getShapeFunctions ();

    // Iterate over the integration points.

    for ( idx_t ip = 0; ip < ipCount_; ip++ )
    {
      Matrix grad  ( grads(ALL,ALL,ip) );
      getStrain_ ( strain, defGrad, b, grad, elemDisp );

      material_->update ( nStressIp, stiff, strain, defGrad, ipoint );

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

void NLSolidModel::writeElements_

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

void NLSolidModel::writeNodes_

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
//    initWriter_
//-----------------------------------------------------------------------

Ref<PrintWriter>  NLSolidModel::initWriter_

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

void NLSolidModel::writeDisplacements_

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


void NLSolidModel::getXOutTable_

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
  Cubix        grads      ( rank_, nodeCount_, ipCount_ );
  Matrix       defGrad    ( rank_, rank_ );

  Vector       elemDisp   ( dofCount_ );

  IdxVector    inodes     ( nodeCount_ );
  IdxVector    idofs      ( dofCount_  );

  const bool   tri6 = ( shape_->getGeometry() == Geometries::TRIANGLE
                      && nodeCount_ == 6 );

  // tell TbFiller which types are available to write

  TbFiller   tbFiller   ( rank_ );

  Slice      iistrain   = tbFiller.announce ( "strain.tensor" );
  Slice      iistress   = tbFiller.announce ( "stress.tensor" );
  Slice      iihistory  = tbFiller.announce ( hisNames );

  Vector     ipValues   ( tbFiller.typeCount() );

  Vector     strain     ( ipValues[iistrain]   );
  Vector     stress     ( ipValues[iistress]   );
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

    shape_->getShapeGradients ( grads, ipWeights, coords );

    elemDisp = select ( disp, idofs );

    // Iterate over the integration points.
    // Gather all data, no matter which is asked, to keep code neat
    // The option to specify output is primarily for disk size, not CPU time

    for ( idx_t ip = 0; ip < ipCount; ip++, ++ipoint )
    {
      Matrix grad  ( grads(ALL,ALL,ip) );
      getStrain_ ( strain, defGrad, b, grad, elemDisp );

      material_->update ( stress, stiff, strain, defGrad, ipoint );
      material_-> getHistory ( history, ipoint );

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
//   newNLSolidModel
//-----------------------------------------------------------------------


static Ref<Model>     newNLSolidModel

  ( const String&       name,
    const Properties&   conf,
    const Properties&   props,
    const Properties&   globdat )

{
  return newInstance<NLSolidModel> ( name, conf, props, globdat );
}


//-----------------------------------------------------------------------
//   declareNLSolidModel
//-----------------------------------------------------------------------


void declareNLSolidModel ()
{
  using jive::model::ModelFactory;

  ModelFactory::declare ( "NLSolid", & newNLSolidModel );
}

