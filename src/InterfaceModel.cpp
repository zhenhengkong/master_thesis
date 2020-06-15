/*
 *
 * Model for interface elements: 
 *   - assembly of stiffness and internal force vector
 *   - output
 * 
 * Frans van der Meer, June 2008
 *
 */

#include "InterfaceModel.h"

#include <jem/base/array/operators.h>
#include <jem/base/array/select.h>
#include <jem/base/Error.h>
#include <jem/base/Float.h>
#include <jem/base/IllegalInputException.h>
#include <jem/base/System.h>
#include <jem/numeric/algebra/utilities.h>
#include <jem/util/StringUtils.h>
#include <jive/model/ModelFactory.h>
#include <jive/model/Actions.h>
#include <jive/model/StateVector.h>
#include <jive/geom/BoundaryShape.h>
#include <jive/geom/BShapeFactory.h>
#include <jive/geom/BoundaryTriangle.h>
#include <jive/fem/Globdat.h>

#include "models.h"
#include "SolverNames.h"

using jem::numeric::norm2;
using jem::util::StringUtils;
using jive::model::StateVector;
using jive::geom::BShapeFactory;
using jive::geom::BShape;
using jive::fem::Globdat;

//-----------------------------------------------------------------------
//   static constants
//-----------------------------------------------------------------------


const char* InterfaceModel::DOF_NAMES[3]  = {"dx","dy","dz"};
const char* InterfaceModel::SHAPE_PROP    = "shape";
const char* InterfaceModel::COHEMAT_PROP  = "coheMat";
const char* InterfaceModel::AXI_PROP      = "axisymmetric";


//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------

InterfaceModel::InterfaceModel

   ( const String&       name,
     const Properties&   conf,
     const Properties&   props,
     const Properties&   globdat ) : Super(name)
{
  Ref<BShape> bshape = NIL;

  using jive::util::joinNames;

  // create myTag_ (last part of myName_)
  
  StringVector names ( StringUtils::split( myName_, '.' ) );
  myTag_     = names [ names.size() - 1 ];

  Properties  myProps = props.getProps ( myName_ );
  Properties  myConf  = conf.makeProps ( myName_ );
  Properties  shProps = myProps.getProps ( SHAPE_PROP );

  const String context = getContext();

  egroup_ = ElemGroup::get ( myConf, myProps, globdat, context );

  elems_      = egroup_.getElements ( );
  elemCount_  = egroup_.size        ( );

  ielems_.resize ( elemCount_ );
  ielems_ = egroup_.getIndices ( );

  // ielems_.ref ( egroup_.getIndices  ( ) );
  nodes_      = elems_.getNodes     ( );
  rank_       = nodes_.rank         ( );

  try
  {
    bshape = BShapeFactory::newInstance
     ( joinNames (myName_, SHAPE_PROP ), conf, props );
  }
  catch ( const jem::Exception& ex )
  {
    myConf.set ( SHAPE_PROP, shProps );

    bshape = makeBTriangleNC ( shProps );

    if ( bshape == NIL ) throw;
  }

  shape_ = newInstance<InterfaceShape> ( "interface", bshape );

  // NB: Make sure you know what you're doing when 
  // the rank of the shape doesn't match the rank of the mesh

  qRank_     = shape_->globalRank  ( );
  nodeCount_ = shape_->nodeCount   ( );
  ipCount_   = shape_->ipointCount ( );

  // Make sure that each element has the same number of nodes as the
  // shape object.

  elems_.checkSomeElements 
    ( context,
      ielems_,
      nodeCount_ );

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

  getShapeFuncs_ = getShapeFuncsFunc ( rank_ );

  axisym_ = false;
  myProps.find( axisym_, AXI_PROP );
  myConf.set  ( AXI_PROP, axisym_ );

  if ( axisym_ )
  {
    JEM_PRECHECK ( rank_ == 2 && qRank_ == 2 );
  }

  // Create a coheMat object.

  coheMat_ = newCohesiveMat 
           ( COHEMAT_PROP, myConf, myProps, globdat );
}

InterfaceModel::~InterfaceModel()
{}

//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------

void InterfaceModel::configure

  ( const Properties&  props,
    const Properties&  globdat )

{
  Properties  myProps  = props.findProps ( myName_ );
  Properties  matProps = myProps.findProps ( COHEMAT_PROP );

  coheMat_->configure ( matProps, globdat );

  coheMat_->allocPoints  ( ipCount_ * elemCount_ );
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void InterfaceModel::getConfig 

  ( const Properties& conf,
    const Properties& globdat ) const

{
  Properties  myConf  = conf.makeProps ( myName_ );
  Properties  matConf = myConf.makeProps ( COHEMAT_PROP );

  coheMat_->getConfig ( matConf, globdat );

  if ( rank_ == 2 )
  {
  }
}


//-----------------------------------------------------------------------
//   takeAction
//-----------------------------------------------------------------------


bool InterfaceModel::takeAction

  ( const String&      action,
    const Properties&  params,
    const Properties&  globdat )

{
  using jive::model::Actions;
  using jive::model::ActionParams;

  if ( action == Actions::GET_MATRIX0 ||
       action == Actions::GET_INT_VECTOR )
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

  if ( action == Actions::COMMIT )
  {
    coheMat_->commit ();
    return true;
  }

  if ( action == "WRITE_XOUTPUT" )
  {
    if ( xOut_ == NIL )
    {
      initWriter_ ( params );
      writeGeom_();
    }
    writeOutput_ ( globdat );
    return true;
  }

  if ( action == "GET_DISSIPATION" )
  {
    getDissipation_ ( params );
    return true;
  }

  if ( action == "DESPAIR" )
  {
    return coheMat_->despair();
  }

  if ( action == "END_DESPAIR" )
  {
    coheMat_->endDespair();

    return true;
  }

  return false;
}

//-----------------------------------------------------------------------
//   initWriter_
//-----------------------------------------------------------------------

void InterfaceModel::initWriter_  

  ( const Properties&  params )

{
  // Open file for writeXOutput

  StringVector fileName;
  String       prepend;
  
  if ( params.find( prepend, "prepend" ) )
  { 
    fileName.resize(3);

    fileName[0] = prepend;
    fileName[1] = myTag_;
    fileName[2] = "dat";
  }
  else
  {
    fileName.resize(2);

    fileName[0] = myTag_;
    fileName[1] = "dat";
  }

  xOut_ = newInstance<PrintWriter>(
          newInstance<FileWriter> ( StringUtils::join( fileName, "." ) ) );
}

//-----------------------------------------------------------------------
//   writeGeom_
//-----------------------------------------------------------------------

void InterfaceModel::writeGeom_  ()  const

{
  bool        gauss1     = false;
  bool        gauss3     = false;

  // write nodal coordinates when 1pt integration

  if ( ipCount_ == 1 )
  {
    gauss1 = true;
  }

  Matrix      coords     ( qRank_, nodeCount_ );
              coords     = 0.;

  Matrix      ipCoords   ( qRank_, ipCount_ );

  IntVector   inodes     ( nodeCount_ );

  *xOut_ << "ipCoords" << '\n';
  *xOut_ << rank_ << '\n';

  if ( ( nodeCount_ == 6 || nodeCount_ == 12 ) && ipCount_ == 3 
       && shape_->getBShape()->getName() != "BTriangleNC3" )
  {
    gauss3 = true;
    System::warn() << myName_ << " Writing gauss point data instead of " 
      << "nodal values." << endl;
  }

  for ( idx_t ie = 0; ie < elemCount_; ie++ )
  {
    // Get the element coordinates and DOFs.

    elems_.getElemNodes  ( inodes, ielems_[ie] );
    nodes_.getSomeCoords ( coords( slice(0,rank_) , ALL ), 
                           inodes );

    // take node coordinates in case of triangle with Gauss3 integration

    if ( gauss3 || gauss1 )
    {
      idx_t i = nodeCount_ == 3 ? 1 : 2;
      ipCoords.ref ( coords ( ALL, slice(0,nodeCount_/2,i) ) );
    }
    else
    {
      shape_->getGlobalIntegrationPoints ( ipCoords, coords );
    }

    for ( idx_t ip = 0; ip < ipCoords.size( 1 ); ++ip )
    {
      *xOut_ << ie << " " << ip << " ";
      for ( idx_t j = 0; j < rank_; ++j )
      {
        *xOut_ << ipCoords( j, ip ) << " ";
      }
      *xOut_ << '\n';
    }
  }
  xOut_->flush();
}

//-----------------------------------------------------------------------
//   checkGeom_
//-----------------------------------------------------------------------


void InterfaceModel::checkGeom_ () const

{
  Matrix      normals    ( qRank_, nodeCount_ / 2 );
  Matrix      coords     ( qRank_, nodeCount_ );
  IntVector   inodes     ( nodeCount_ );

  for ( idx_t ie = 0; ie < elemCount_; ie++ )
  {
    elems_.getElemNodes  ( inodes, ielems_[ie] );
    nodes_.getSomeCoords ( coords( slice(0,rank_) , ALL ), 
                           inodes );

    // check assumption used in getTransMatrix_ 

    if ( qRank_ == 3 )
    {
      shape_->getVertexNormals ( normals, coords );
      
      for ( idx_t in = 0; in < nodeCount_/2; ++in )
      {
        if ( normals ( 2, in ) < 1.-1.e-10 )
        {
          System::warn() << "Interface element normal not in positive"
            "z-direction! " << ielems_[ie] << endl;
        }
      }
    }
  }
}

//-----------------------------------------------------------------------
//   getMatrix_
//-----------------------------------------------------------------------


void InterfaceModel::getMatrix_

  ( Ref<MatrixBuilder>  mbuilder,
    const Vector&       force,
    const Vector&       disp ) const

{
  const idx_t dofCount   = rank_ * nodeCount_;

  Matrix      stiff      ( rank_,     rank_ );

  Matrix      coords     ( qRank_, nodeCount_ );
              coords     = 0.;
  Matrix      ipcoords   ( 2, ipCount_ );

  Matrix      elemMat    ( dofCount, dofCount  );
  Vector      elemForce  ( dofCount  );
  Vector      elemDisp   ( dofCount  );

  Vector      jump       ( rank_     );
  Vector      traction   ( rank_     );
  Vector      trac       ( rank_     );
  Vector      normal     ( rank_     );

  Matrix      sfuncs     = shape_->getShapeFunctions ();
  Matrix      negSfuncs  ( sfuncs.size(0), sfuncs.size(1) );
              negSfuncs  = - sfuncs;

  Matrix      N          ( rank_, rank_*nodeCount_  );
  Matrix      Nt         = N.transpose ();

  Matrix      normals    ( rank_, nodeCount_/2 );
  Matrix      transMat   ( rank_, rank_ );
  Matrix      transMatT  = transMat.transpose ();

  IntVector   inodes     ( nodeCount_ );
  IntVector   idofs      ( dofCount   );

  Vector      ipWeights  ( ipCount_   );

  MChain1     mc1;
  MChain2     mc2;
  MChain3     mc3;

  idx_t       ipoint = 0;

  // Iterate over all elements assigned to this model.

  for ( idx_t ie = 0; ie < elemCount_; ie++ )
  {
    // Get the element coordinates and DOFs.

    elems_.getElemNodes  ( inodes, ielems_[ie] );
    nodes_.getSomeCoords ( coords( slice(0,rank_) , ALL ), 
                           inodes );
    dofs_->getDofIndices ( idofs,  inodes, dofTypes_ );
    shape_->getIntegrationWeights ( ipWeights, coords );

    if ( qRank_ == 2 ) 
    {
      // for line elements: get the normal 

      shape_->getVertexNormals ( normals, coords );
      normal = normals ( ALL , 0 );
    }

    if ( axisym_ )
    {
      shape_->getGlobalIntegrationPoints ( ipcoords, coords );

      for ( idx_t ip = 0; ip < ipCount_; ++ip )
      { 
        ipWeights[ip] *= ipcoords(1,ip);
      }
    }

    getTransMatrix_ ( transMat, normal );

    // Get the displacements at the element nodes.

    elemDisp = disp[idofs];

    // Assemble the element matrix.

    elemMat   = 0.0;
    elemForce = 0.0;

    for ( idx_t ip = 0; ip < ipCount_; ip++ )
    {
      // assemble N-matrix

      getShapeFuncs_( N(ALL,slice(0,dofCount/2)), sfuncs(ALL,ip) );

      getShapeFuncs_( N(ALL,slice(dofCount/2,END)), negSfuncs(ALL,ip) );

      // Compute the displacement jump (in local {n,s}-frame)

      jump = mc2.matmul ( transMat, N, elemDisp );

      // Get the tangent stiffness matrix and the traction

      coheMat_->update ( traction, stiff, jump, ipoint++ );

      // transform traction and stiffness to global {x,y}-frame

      matmul ( trac, transMatT, traction );
      stiff    = mc3.matmul ( transMatT, stiff, transMat );

      // Compute the element force vector

      elemForce += ipWeights[ip] * mc1.matmul ( Nt, trac );

      // Compute the stiffness matrix
      
      elemMat   += ipWeights[ip] * mc3.matmul ( Nt, stiff, N );
    }

    // Add the element matrix to the global stiffness matrix.

    if ( mbuilder != NIL )
    {
      mbuilder->addBlock ( idofs, idofs, elemMat );
    }

    // Add the element force vector to the global force vector.

    force[idofs] += elemForce;

    if ( jem::Float::isNaN( sum( elemForce ) ) ||
         jem::Float::isNaN( sum( elemMat   ) ) )
    { 
      System::out() << "Interf something's wrong in element " << ie << endl;
    }
  }
}

//-----------------------------------------------------------------------
//   writeOutput_
//-----------------------------------------------------------------------

void InterfaceModel::writeOutput_

  ( const Properties&    globdat  ) const

{
  const idx_t dofCount   = rank_ * nodeCount_;

  Matrix      stiff      ( rank_,     rank_ );

  Matrix      coords     ( qRank_, nodeCount_ );
              coords     = 0.;

  Vector      elemDisp   ( dofCount  );

  Vector      jump       ( rank_     );
  Vector      traction   ( rank_     );
  Vector      normal     ( rank_     );

  Matrix      sfuncs     = shape_->getShapeFunctions ();
  Matrix      negSfuncs  ( sfuncs.size(0), sfuncs.size(1) );
              negSfuncs  = - sfuncs;

  Matrix      N          ( rank_, rank_*nodeCount_  );
  Matrix      Nt         = N.transpose ();

  Matrix      normals    ( qRank_, nodeCount_/2 );
  Matrix      transMat   ( rank_ , rank_ );
  Matrix      transMatT  = transMat.transpose ();

  IntVector   inodes     ( nodeCount_ );
  IntVector   idofs      ( dofCount   );

  MChain2     mc2;

  Vector      disp;

  StateVector::get ( disp, dofs_, globdat );

  idx_t       ipoint = 0;
  idx_t       it;

  globdat.get ( it, Globdat::TIME_STEP );

  *xOut_ << "newXOutput " << it <<  '\n';

  for ( idx_t ie = 0; ie < elemCount_; ie++ )
  {
    // Get the element coordinates and DOFs.

    elems_.getElemNodes  ( inodes, ielems_[ie] );
    nodes_.getSomeCoords ( coords( slice(0,rank_) , ALL ), 
                           inodes );
    dofs_->getDofIndices ( idofs,  inodes, dofTypes_ );

    if ( qRank_ == 2 )
    {
      shape_->getVertexNormals ( normals, coords );
      normal = normals ( ALL , 0 );
    }

    getTransMatrix_ ( transMat, normal );

    // Get the displacements at the element nodes.

    elemDisp = disp[idofs];

    // Evaluate and write jump and traction

    for ( idx_t ip = 0; ip < ipCount_; ip++ )
    {
      *xOut_ << ie << " " << ip << " ";

      // assemble N-matrix

      getShapeFuncs_( N(ALL,slice(0,dofCount/2)), sfuncs(ALL,ip) );

      getShapeFuncs_( N(ALL,slice(dofCount/2,END)), negSfuncs(ALL,ip) );

      // Compute the displacement jump (in local {n,s}-frame)

      jump = mc2.matmul ( transMat, N, elemDisp );

      // Get the tangent stiffness matrix and the traction

      coheMat_->update ( traction, stiff, jump, ipoint );

      for ( idx_t j = 0; j < rank_; j++ )
      {
        *xOut_ << jump[j] << " " << traction[j] << " ";
      }
      *xOut_ << coheMat_->giveHistory( ipoint ) 
                    << " " << norm2( traction ) 
                    << " " << coheMat_->isLoading( ipoint )
                    << '\n';
      ipoint++;
    }
  }
  xOut_->flush();
}


//-----------------------------------------------------------------------
//   getTransMatrix_
//-----------------------------------------------------------------------


void InterfaceModel::getTransMatrix_
   
  ( const Matrix&       transMat,
    const Vector&       normal   ) const

{
  if ( qRank_ == 2 )
  {
    transMat(0,0) =  normal[0];
    transMat(0,1) =  normal[1];
    transMat(1,0) = -normal[1];
    transMat(1,1) =  normal[0];
  }
  else if ( qRank_ == 3 && rank_ == 3 )
  {
    // assuming n = { 0, 0, -1. }
    
    transMat = 0.;
    transMat(0,2) = -1.;
    transMat(1,0) = 1.;
    transMat(2,1) = 1.;
  }
  else if ( qRank_ == 3 && rank_ == 2 )
  {
    // assuming n = { 0, 0, -1. }
    
    transMat = 0.;
    transMat(0,0) = 1.;
    transMat(1,1) = 1.;
  }
  else // qRank_ == 1
  {
    transMat = 1.;
  }
}
  
//-----------------------------------------------------------------------
//   getDissipation_
//-----------------------------------------------------------------------


void InterfaceModel::getDissipation_

  ( const Properties& params ) const

{
  Matrix      coords     ( qRank_, nodeCount_ );
              coords     = 0.;
  Matrix      ipcoords   ( 2, nodeCount_ );
  Vector      ipWeights  (           ipCount_ );
  IntVector   inodes     (         nodeCount_ );

  double dissipation = 0.;
  idx_t       ipoint = 0;
  double thickness   = 0.;

  // loop over integration points

  for ( idx_t ie = 0; ie < elemCount_; ie++ )
  {
    // get the integrationpoint weights

    elems_.getElemNodes  ( inodes, ielems_[ie] );
    nodes_.getSomeCoords ( coords( slice(0,rank_) , ALL ), inodes );
    shape_->getIntegrationWeights ( ipWeights, coords );

    if ( axisym_ )
    {
      shape_->getGlobalIntegrationPoints ( ipcoords, coords );

      for ( idx_t ip = 0; ip < ipCount_; ++ip )
      { 
        ipWeights[ip] *= ipcoords(1,ip);
      }
    }

    // get dissipation

    for ( idx_t ip = 0; ip < ipCount_; ip++ )
    {

      double    G  = coheMat_->giveDissipation ( ipoint++ );

      dissipation += thickness * ipWeights[ ip ] * G;
    }
  }
  params.set ( myTag_, dissipation );
}

//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   newInterfaceModel
//-----------------------------------------------------------------------


static Ref<Model>     newInterfaceModel

  ( const String&       name,
    const Properties&   conf,
    const Properties&   props,
    const Properties&   globdat )

{
  return newInstance<InterfaceModel> ( name, conf, props, globdat );
}


//-----------------------------------------------------------------------
//   declareInterfaceModel
//-----------------------------------------------------------------------


void declareInterfaceModel ()
{
  using jive::model::ModelFactory;

  ModelFactory::declare ( "Interface", & newInterfaceModel );
}


//-----------------------------------------------------------------------
//   makeBTriangleNC
//-----------------------------------------------------------------------


Ref<BShape> makeBTriangleNC ( const Properties& props )
{
  using jive::Matrix;
  using jive::geom::BoundaryTriangle3;
  using jive::geom::BoundaryTriangle6;

  String     type;
  String     intScheme;
  Matrix     ischeme;

  props.find (      type,      "type" );
  props.find ( intScheme, "intScheme" );

  // Set up the Newton-Cotes scheme for the interior of the
  // triangle. The first matrix row contains the weights and the other
  // two rows contain the coordinates. Both the weights and
  // coordinates must be specified in the local coordinate system of a
  // standard triangle. The weights must add up to 0.5 (the area of a
  // standard triangle).

  if ( intScheme == "NewtonCotes3" )
  {
    ischeme.resize ( 3, 3 );

    ischeme(0,ALL) = 1.0 / 6.0;

    ischeme(1,0) = 0.0;
    ischeme(2,0) = 0.0;

    ischeme(1,1) = 1.0;
    ischeme(2,1) = 0.0;

    ischeme(1,2) = 0.0;
    ischeme(2,2) = 1.0;
  }
  else if ( intScheme == "NewtonCotes6" )
  {
    ischeme.resize ( 3, 6 );

    ischeme(0,slice(0,END,2)) = 1.0 / 24.0;
    ischeme(0,slice(1,END,2)) = 3.0 / 24.0;

    ischeme(1,0) = 0.0;
    ischeme(2,0) = 0.0;

    ischeme(1,1) = 0.5;
    ischeme(2,1) = 0.0;

    ischeme(1,2) = 1.0;
    ischeme(2,2) = 0.0;

    ischeme(1,3) = 0.5;
    ischeme(2,3) = 0.5;

    ischeme(1,4) = 0.0;
    ischeme(2,4) = 1.0;

    ischeme(1,5) = 0.0;
    ischeme(2,5) = 0.5;
  }
  else
  {
    return NIL;
  }

  if ( type == "BTriangle3" ) 
  {
    return BoundaryTriangle3::getShape ( "BTriangleNC3", ischeme );
  }
  else if ( type == "BTriangle6" )
  {
    return BoundaryTriangle6::getShape ( "BTriangleNC6", ischeme );
  }

  return NIL;
}
