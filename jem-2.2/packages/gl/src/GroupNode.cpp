
/*
 *  Copyright (C) 2016 DRG. All rights reserved.
 *
 *  This file is part of Jem, a general purpose programming toolkit.
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
 *
 *  Jem version: 2.2
 *  Date:        Thu 28 Jan 10:36:48 CET 2016
 */


#include <jem/base/ClassTemplate.h>
#include <jem/util/Event.h>
#include <jem/util/Properties.h>
#include <jem/gl/Box.h>
#include <jem/gl/Actions.h>
#include <jem/gl/Attributes.h>
#include <jem/gl/GroupNode.h>


JEM_DEFINE_CLASS( jem::gl::GroupNode );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class GroupNode
//=======================================================================

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


GroupNode::GroupNode

  ( const String&    name,
    Ref<Attributes>  attribs,
    Rank             rank ) :

    Super    ( name ),
    rank_    ( rank ),
    attribs_ ( attribs )

{
  using jem::util::connect;

  transfm_.setIdentity ();

  if ( attribs_ != NIL )
  {
    attribs0_ = checkedCast<Attributes> ( attribs_->clone() );

    connect ( attribs_->newValuesEvent, this, &Self::newAttribs_ );
  }

  hidden     = false;
  newFields_ = 0;
}


GroupNode::~GroupNode ()
{}


//-----------------------------------------------------------------------
//   takeAction
//-----------------------------------------------------------------------


bool GroupNode::takeAction

  ( const String&      action,
    const Properties&  params )

{
  if ( action == Actions::SET_ATTRIBS && attribs_ != NIL )
  {
    attribs_->copyFrom ( params );
    attribs_->copyTo   ( params, newFields_ );

    newFields_ = 0;

    Super::takeAction ( action, params );

    return true;
  }

  if ( action == Actions::HIDE )
  {
    hidden = true;
    return   true;
  }

  if ( action == Actions::SHOW )
  {
    hidden = false;
    return   true;
  }

  return Super::takeAction ( action, params );
}


//-----------------------------------------------------------------------
//   takeNRAction
//-----------------------------------------------------------------------


bool GroupNode::takeNRAction

  ( const String&      action,
    const Properties&  params )

{
  if ( action == NRActions::GET_ATTRIBS && attribs_ != NIL )
  {
    attribs_->copyTo ( params, Attributes::ALL_FIELDS );

    return true;
  }
  else
  {
    return Super::takeNRAction ( action, params );
  }
}


//-----------------------------------------------------------------------
//   takeGLAction
//-----------------------------------------------------------------------


bool GroupNode::takeGLAction

  ( GfxContext&        gtx,
    int                action,
    const Properties&  params )

{
  if ( hidden && (action == GLActions::REDRAW ||
                  action == GLActions::REDRAW_FAST) )
  {
    return false;
  }

  getCurrent  ( transfm0_ );
  multCurrent ( transfm_  );

  if ( attribs_ != NIL )
  {
    if ( newFields_ )
    {
      Properties  props ( "attributes" );

      attribs_->copyTo  ( props, newFields_ );
      Super::takeAction ( Actions::SET_ATTRIBS, props );

      newFields_ = 0;
    }

    getCurrent ( *attribs0_ );
    setCurrent ( *attribs_  );
  }

  bool  result = Super::takeGLAction ( gtx, action, params );

  if ( attribs_ != NIL )
  {
    setCurrent ( *attribs0_ );
  }

  setCurrent ( transfm0_ );

  return result;
}


//-----------------------------------------------------------------------
//   getBBox
//-----------------------------------------------------------------------


void GroupNode::getBBox ( Box& box ) const
{
  Super::getBBox ( box );

  if ( rank_ == RANK2D )
  {
    box.transform2D  ( transfm_ );
  }
  else
  {
    box.transform    ( transfm_ );
  }
}


//-----------------------------------------------------------------------
//   getBareBBox
//-----------------------------------------------------------------------


void GroupNode::getBareBBox ( Box& box ) const
{
  Super::getBBox ( box );
}


//-----------------------------------------------------------------------
//   getVisibility
//-----------------------------------------------------------------------


void GroupNode::getVisibility

  ( bool&             viz,
    Box&              vbox,
    const Transform&  proj ) const

{
  if ( hidden )
  {
    viz = false;
    vbox.makeEmpty ();
  }
  else
  {
    Transform  t;

    matmul ( t, proj, transfm_ );

    Super::getVisibility ( viz, vbox, t );

    if ( rank_ == RANK2D )
    {
      vbox.transform2D ( transfm_ );
    }
    else
    {
      vbox.transform   ( transfm_ );
    }
  }
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void GroupNode::configure ( const Properties& props )
{
  if ( props.contains( myName_ ) )
  {
    Properties  myProps = props.findProps ( myName_ );

    if ( rank_ == RANK2D )
    {
      transfm_.configure2D ( myProps );
    }
    else
    {
      transfm_.configure3D ( myProps );
    }

    if ( attribs_ != NIL )
    {
      attribs_->copyFrom ( myProps );

      if ( newFields_ )
      {
        Properties  props ( "attributes" );

        attribs_->copyTo  ( props, newFields_ );
        Super::takeAction ( Actions::SET_ATTRIBS, props );

        newFields_ = 0;
      }
    }
  }

  Super::configure ( props );
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void GroupNode::getConfig ( const Properties& props ) const
{
  Properties  myProps = props.makeProps ( myName_ );

  if ( rank_ == RANK2D )
  {
    transfm_.getConfig2D ( myProps );
  }
  else
  {
    transfm_.getConfig3D ( myProps );
  }

  if ( attribs_ != NIL )
  {
    attribs_->copyTo ( myProps, Attributes::ALL_FIELDS );
  }

  Super::getConfig ( props );
}


//-----------------------------------------------------------------------
//   listProps
//-----------------------------------------------------------------------


void GroupNode::listProps ( const Properties& props ) const
{
  Properties  myProps = props.makeProps ( myName_ );

  if ( rank_ == RANK2D )
  {
    Transform::listProps2D ( myProps );
  }
  else
  {
    Transform::listProps3D ( myProps );
  }

  if ( attribs_ != NIL )
  {
    attribs_->listProps ( myProps );
  }

  Super::listProps ( props );
}


//-----------------------------------------------------------------------
//   getTransform
//-----------------------------------------------------------------------


Transform GroupNode::getTransform () const
{
  return transfm_;
}


//-----------------------------------------------------------------------
//   setTransform
//-----------------------------------------------------------------------


void GroupNode::setTransform ( const Transform& tr )
{
  transfm_ = tr;
}


//-----------------------------------------------------------------------
//   getAttribs
//-----------------------------------------------------------------------


Attributes* GroupNode::getAttribs () const
{
  return attribs_.get ();
}


//-----------------------------------------------------------------------
//   initAttribs
//-----------------------------------------------------------------------


void GroupNode::initAttribs ()
{
  if ( attribs_ != NIL )
  {
    Properties  attribs;

    getAttribsOf ( getParent(), attribs );

    attribs_->copyFrom ( attribs );
  }
}


//-----------------------------------------------------------------------
//   newAttribs_
//-----------------------------------------------------------------------


void GroupNode::newAttribs_ ( int fields )
{
  newFields_ |= fields;
}


JEM_END_PACKAGE( gl )
