/*
 *
 * Model for interface elements: 
 *   - assembly of stiffness and internal force vector
 *   - output
 * 
 * Frans van der Meer, June 2008
 *
 */

#include <jem/io/PrintWriter.h>
#include <jem/io/FileWriter.h>
#include <jem/numeric/algebra/matmul.h>
#include <jem/numeric/algebra/MatmulChain.h>
#include <jem/util/Properties.h>

#include <jive/util/utilities.h>
#include <jive/util/XDofSpace.h>
#include <jive/util/Assignable.h>
#include <jive/algebra/MatrixBuilder.h>
#include <jive/model/Model.h>
#include <jive/geom/IntegrationSchemes.h>
#include <jive/geom/InterfaceShape.h>
#include <jive/fem/ElementGroup.h>

#include "utilities.h"
#include "CohesiveMat.h"

using namespace jem;

using jem::io::PrintWriter;
using jem::io::FileWriter;
using jem::numeric::matmul;
using jem::numeric::MatmulChain;
using jem::util::Properties;
using jive::Vector;
using jive::IntVector;
using jive::StringVector;
using jive::util::joinNames;
using jive::util::XDofSpace;
using jive::util::Assignable;
using jive::algebra::MatrixBuilder;
using jive::model::Model;
using jive::geom::InterfaceShape;
using jive::geom::BShape;
using jive::fem::NodeSet;
using jive::fem::ElementSet;
using jive::fem::ElementGroup;

// some typedef to avoid typing

typedef ElementSet              ElemSet;
typedef ElementGroup            ElemGroup;
typedef MatmulChain<double,3>   MChain3;
typedef MatmulChain<double,2>   MChain2;
typedef MatmulChain<double,1>   MChain1;

class InterfaceModel : public Model
{
 public:

  typedef InterfaceModel     Self;
  typedef Model              Super;

  static const char*         DOF_NAMES[3];
  static const char*         SHAPE_PROP;
  static const char*         COHEMAT_PROP;
  static const char*         AXI_PROP;

                       InterfaceModel
			 
    ( const String&       name,
      const Properties&   conf,
      const Properties&   props,
      const Properties&   globdat );

  virtual void         configure

    ( const Properties&   props,
      const Properties&   globdat );

  virtual void         getConfig

    ( const Properties&   conf,
      const Properties&   globdat )      const;

  virtual bool         takeAction

    ( const String&       action,
      const Properties&   params,
      const Properties&   globdat );

 protected:

  virtual              ~InterfaceModel ();

 protected:

  void                 initWriter_

    ( const Properties&   params );

  void                 writeGeom_()    const;

  void                 checkGeom_()   const;

  virtual void         getMatrix_

    ( Ref<MatrixBuilder>  mbuilder,
      const Vector&       force,
      const Vector&       disp )       const;

  virtual void         writeOutput_

    ( const Properties&   globdat )    const;

  void                 getTransMatrix_
   
    ( const Matrix&       transMat,
      const Vector&       normal )     const;

  virtual void         getDissipation_ 

    ( const Properties&   params ) const;
    
protected: 

  String                  myTag_;

  // model rank
  
  idx_t                   rank_;

  // interface shape rank

  idx_t                   qRank_;

  bool                    axisym_;
  idx_t                   nodeCount_;
  idx_t                   elemCount_;
  idx_t                   ipCount_;

  IntVector               ielems_;

  Assignable<ElemGroup>   egroup_;
  Assignable<ElemSet>     elems_;
  Assignable<NodeSet>     nodes_;

  Ref<InterfaceShape>     shape_;

  String                  ischeme_;

  Ref<XDofSpace>          dofs_;
  IntVector               dofTypes_;

  Ref<CohesiveMat>        coheMat_;

  ShapeFuncsFunc          getShapeFuncs_;
  
  Ref<PrintWriter>        xOut_;
};

Ref<BShape> makeBTriangleNC  ( const Properties& props );

Ref<BShape> makeBTriangleNC3 ( const String& name = "BTriangleNC3" );

Ref<BShape> makeBTriangleNC6 ( const String& name = "BTriangleNC6" );

