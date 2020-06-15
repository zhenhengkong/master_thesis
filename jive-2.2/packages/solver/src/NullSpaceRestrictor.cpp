
/*
 *  Copyright (C) 2016 DRG. All rights reserved.
 *
 *  This file is part of Jive, an object oriented toolkit for solving
 *  partial differential equations.
 *
 *  Commercial License Usage
 *
 *  This file may be used under the terms of a commercial license
 *  provided with the software, or under the terms contained in a written
 *  agreement between you and DRG. For more information contact DRG at
 *  http://www.dynaflow.com.
 *
 *  GNU Lesser General Public License Usage
 *
 *  Alternatively, this file may be used under the terms of the GNU
 *  Lesser General Public License version 2.1 or version 3 as published
 *  by the Free Software Foundation and appearing in the file
 *  LICENSE.LGPLv21 and LICENSE.LGPLv3 included in the packaging of this
 *  file. Please review the following information to ensure the GNU
 *  Lesser General Public License requirements will be met:
 *  https://www.gnu.org/licenses/lgpl.html and
 *  http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 *  This file is part of Jive, an object oriented toolkit for
 *  solving partial differential equations.
 *
 *  Jive version: 2.2
 *  Date:         Thu 28 Jan 10:31:15 CET 2016
 */


#include <jem/base/Array.h>
#include <jem/base/System.h>
#include <jem/base/ClassTemplate.h>
#include <jem/util/Event.h>
#include <jive/util/utilities.h>
#include <jive/util/Constraints.h>
#include <jive/solver/Names.h>
#include <jive/solver/SolverParams.h>
#include <jive/solver/DirectSolver.h>
#include <jive/solver/SparseIFactor.h>
#include <jive/solver/RestrictorFactory.h>
#include <jive/solver/NullSpaceRestrictor.h>


JEM_DEFINE_CLASS( jive::solver::NullSpaceRestrictor );


JIVE_BEGIN_PACKAGE( solver )


//=======================================================================
//   class NullSpaceRestrictor
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  NullSpaceRestrictor::TYPE_NAME   = "NullSpace";

const int    NullSpaceRestrictor::NEW_VALUES_ = 1 << 0;
const int    NullSpaceRestrictor::NEW_STRUCT_ = 1 << 1;


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


NullSpaceRestrictor::NullSpaceRestrictor

  ( const String&      name,
    Ref<DirectSolver>  solver ) :

    Super   ( name ),
    solver_ ( solver )

{
  JEM_PRECHECK ( solver != NIL );

  evMask_ = NEW_STRUCT_;

  connect_ ();
}


NullSpaceRestrictor::~NullSpaceRestrictor ()
{}


//-----------------------------------------------------------------------
//   resetEvents
//-----------------------------------------------------------------------


void NullSpaceRestrictor::resetEvents ()
{
  solver_->getMatrix()->resetEvents ();
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void NullSpaceRestrictor::configure ( const Properties& props )
{
  if ( props.contains( myName_ ) )
  {
    Properties  myProps = props.findProps ( myName_ );

    String      when;

    if ( myProps.find( when, PropNames::UPDATE_WHEN ) )
    {
      if      ( when.equalsIgnoreCase( "NewValues" ) )
      {
        evMask_ = NEW_VALUES_;
      }
      else if ( when.equalsIgnoreCase( "NewStruct" ) )
      {
        evMask_ = NEW_STRUCT_;
      }
      else
      {
        myProps.propertyError (
          PropNames::UPDATE_WHEN,
          String::format (
            "invalid update event: %s; "
            "should be `NewValues\' or `NewStruct\'",
            when
          )
        );
      }

      if ( events_ & evMask_ )
      {
        invalidate_ ();
      }
    }
  }

  solver_->configure ( props );
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void NullSpaceRestrictor::getConfig ( const Properties& conf ) const
{
  Properties  myConf = conf.makeProps ( myName_ );

  String      when;

  if ( evMask_ == NEW_VALUES_ )
  {
    when = "NewValues";
  }
  else
  {
    when = "NewStruct";
  }

  myConf.set ( PropNames::UPDATE_WHEN, when );

  solver_->getConfig ( conf );
}


//-----------------------------------------------------------------------
//   makeNew
//-----------------------------------------------------------------------


Ref<Restrictor> NullSpaceRestrictor::makeNew

  ( const String&      name,
    const Properties&  conf,
    const Properties&  props,
    const Properties&  params,
    const Properties&  globdat )

{
  using jem::dynamicCast;
  using jem::newInstance;
  using jem::System;
  using jive::util::joinNames;

  String      solverName  =

    joinNames ( name, PropNames::SOLVER );

  Properties  solverProps = props.makeProps ( solverName );

  // Set the default properties for the solver.

  if ( ! solverProps.contains( PropNames::TYPE ) )
  {
    solverProps.set ( PropNames::TYPE, "SparseLU" );
  }

  if ( ! solverProps.contains( PropNames::MAX_ZERO_PIVOTS ) )
  {
    solverProps.set ( PropNames::MAX_ZERO_PIVOTS, -1 );
  }

  if ( ! solverProps.contains( PropNames::ZERO_THRESHOLD ) )
  {
    solverProps.set ( PropNames::ZERO_THRESHOLD,
                      SparseIFactor::ZERO_THRESHOLD );
  }

  // Create the solver.

  Ref<Solver>  solver =

    newSolver ( solverName, conf, props, params, globdat );

  if ( solver == NIL )
  {
    return NIL;
  }

  // Make sure that the solver is of the correct type.

  Ref<DirectSolver>  dsolver =

    dynamicCast<DirectSolver> ( solver );

  if ( dsolver == NIL )
  {
    print ( System::warn(), "solver `", solverName,
            "\' is not a direct solver\n" );

    return NIL;
  }

  return newInstance<Self> ( name, dsolver );
}


//-----------------------------------------------------------------------
//   declare
//-----------------------------------------------------------------------


void NullSpaceRestrictor::declare ()
{
  RestrictorFactory::declare ( TYPE_NAME,  & Self::makeNew );
  RestrictorFactory::declare ( CLASS_NAME, & Self::makeNew );
}


//-----------------------------------------------------------------------
//   connect_
//-----------------------------------------------------------------------


void NullSpaceRestrictor::connect_ ()
{
  using jem::util::connect;

  AbstractMatrix*  mx = solver_->getMatrix ();

  events_ = ~0x0;

  mx->resetEvents ();

  connect ( mx->newValuesEvent, this, & Self::valuesChanged_ );
  connect ( mx->newStructEvent, this, & Self::structChanged_ );
}


//-----------------------------------------------------------------------
//   update_
//-----------------------------------------------------------------------


void NullSpaceRestrictor::update_ ()
{
  using jem::select;
  using jem::System;

  Constraints*  cons = solver_->getConstraints ();


  print ( System::debug( myName_ ), myName_,
          " : computing the matrix null space ...\n" );

  solver_->getNullSpace ( basis_ );

  // Free memory if requested.

  if ( evMask_ == NEW_STRUCT_ )
  {
    solver_->clear ();
  }

  if ( cons )
  {
    // Zero the slave DOFs.

    IdxVector  slaveDofs = cons->getSlaveDofs ();

    select ( basis_, slaveDofs, ALL ) = 0.0;
  }

  print ( System::debug( myName_ ), myName_, " : got ",
          basis_.size(1), " vectors spanning the null space.\n" );
}


//-----------------------------------------------------------------------
//   invalidate_
//-----------------------------------------------------------------------


void NullSpaceRestrictor::invalidate_ ()
{
  basis_.resize ( 0, 0 );

  newStructEvent.emit ( *this );
}


//-----------------------------------------------------------------------
//   valuesChanged_
//-----------------------------------------------------------------------


void NullSpaceRestrictor::valuesChanged_ ()
{
  events_ |= NEW_VALUES_;

  if ( events_ & evMask_ )
  {
    invalidate_ ();
  }
}


//-----------------------------------------------------------------------
//   structChanged_
//-----------------------------------------------------------------------


void NullSpaceRestrictor::structChanged_ ()
{
  events_ |= (NEW_VALUES_ | NEW_STRUCT_);

  invalidate_ ();
}



JIVE_END_PACKAGE( solver )
