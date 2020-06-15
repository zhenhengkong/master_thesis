
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


#include <jem/base/rtti.h>
#include <jem/base/System.h>
#include <jem/base/ClassTemplate.h>
#include <jem/base/IllegalInputException.h>
#include <jem/gl/XYPlot.h>
#include <jem/gl/GeomNode.h>
#include <jem/gl/Compound.h>
#include <jem/util/Properties.h>
#include <jem/util/Dictionary.h>
#include <jive/util/utilities.h>
#include <jive/gl/Names.h>
#include <jive/gl/Viewdat.h>
#include <jive/gl/PluginFactory.h>
#include <jive/gl/GraphPlugin.h>


JEM_DEFINE_CLASS( jive::gl::GraphPlugin );


JIVE_BEGIN_PACKAGE( gl )


using jem::newInstance;
using jem::Array;
using jem::System;
using jem::gl::XYPlot;


//=======================================================================
//   class GraphPlugin::RunData_
//=======================================================================


class GraphPlugin::RunData_ : public jem::Collectable
{
 public:

  typedef RunData_          Self;


  explicit inline           RunData_

    ( const String&           ctx );

  void                      init

    ( const String&           myName,
      const Properties&       myProps,
      const Properties&       myConf,
      const Properties&       viewdat );

  void                      update

    ( const Properties&       viewdat,
      const AppContext&       apx );

  inline void               shutdown      ();

  inline void               configure

    ( const Properties&       myProps );

  void                      getConfig

    ( const Properties&       myProps )      const;


 public:

  const String              context;


 private:

  void                      plotVector_

    ( const String&           name,
      const String&           label,
      int                     style,
      const Vector&           data );

  void                      plotVectors_

    ( const String&           label,
      int                     style,
      const Properties&       props );

  void                      setDataSpecs_

    ( const StringVector&     specs );


 private:

  Ref<XYPlot>               graph_;

  StringVector              dataNames_;
  StringVector              dataLabels_;
  IntVector                 dataStyles_;

};


//-----------------------------------------------------------------------
//   constructor
//-----------------------------------------------------------------------


inline GraphPlugin::RunData_::RunData_ ( const String& ctx ) :

  context ( ctx )

{}


//-----------------------------------------------------------------------
//   init
//-----------------------------------------------------------------------


void GraphPlugin::RunData_::init

  ( const String&      myName,
    const Properties&  conf,
    const Properties&  props,
    const Properties&  viewdat )

{
  using jem::gl::toCompound;
  using jem::gl::GfxNode;
  using jem::gl::GeomNode;

  Properties      myConf  = conf .makeProps   ( myName );
  Properties      myProps = props.findProps   ( myName );

  Properties      config0 = viewdat.makeProps ( Viewdat::CONFIG0 );

  Ref<GfxNode>    world;
  Ref<GeomNode>   gnode;

  StringVector    specs;


  viewdat.get ( world, Viewdat::WORLD_NODE );

  graph_ = newInstance<XYPlot>   ( myName );
  gnode  = newInstance<GeomNode> ( myName, graph_ );

  gnode->configure ( props   );
  gnode->getConfig ( conf    );
  gnode->getConfig ( config0 );

  configure ( myProps );
  getConfig ( myConf  );

  toCompound(*world).addChild ( gnode );
}


//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------


void GraphPlugin::RunData_::update

  ( const Properties&  viewdat,
    const AppContext&  apx )

{
  using jem::toValue;
  using jem::isInstance;
  using jem::dynamicCast;
  using jem::util::Dict;
  using jive::util::joinNames;

  Properties   globdat = viewdat.getProps ( Viewdat::GLOBDAT );

  const idx_t  n       = dataNames_.size  ();

  Ref<Object>  obj;


  graph_->clear ();

  for ( idx_t i = 0; i < n; i++ )
  {
    const String&  name = dataNames_[i];
    String         path = joinNames ( "var", name );

    if ( ! globdat.find( obj, path ) )
    {
      print ( System::warn(), context, " : undefined data set `",
              name, "\'; skipping\n" );

      continue;
    }

    if ( isInstance<Vector>( obj ) )
    {
      plotVector_ ( name,
                    dataLabels_[i],
                    dataStyles_[i],
                    toValue<const Vector&> ( obj ) );
      continue;
    }

    Ref<Dict>  dict = dynamicCast<Dict> ( obj );

    if ( dict != NIL )
    {
      plotVectors_ ( dataLabels_[i],
                     dataStyles_[i],
                     Properties ( dict, name ) );
      continue;
    }

    print ( System::warn(), context, " : data set `",
            name, "\' has an invalid type; should be a vector or "
            "a properties set\n" );
  }
}


//-----------------------------------------------------------------------
//   shutdown
//-----------------------------------------------------------------------


inline void GraphPlugin::RunData_::shutdown ()
{
  using jem::gl::toCompound;
  using jem::gl::GfxNode;

  GfxNode*  gnode = graph_->getParent ();

  toCompound( *gnode->getParent() ).eraseChild ( gnode );
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void GraphPlugin::RunData_::configure

  ( const Properties&  myProps )

{
  StringVector  specs;

  if ( myProps.find( specs, PropNames::DATA_SETS ) )
  {
    try
    {
      setDataSpecs_ ( specs );
    }
    catch ( jem::IllegalInputException& ex )
    {
      ex.setContext ( myProps.getContext( PropNames::DATA_SETS ) );
      throw;
    }
  }
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void GraphPlugin::RunData_::getConfig

  ( const Properties&  myProps ) const

{
  const idx_t   n = dataNames_.size ();

  StringVector  specs ( n );

  String        styleName;


  for ( idx_t i = 0; i < n; i++ )
  {
    int  istyle = dataStyles_[i];

    if      ( istyle & XYPlot::LINES_POINTS )
    {
      styleName = "linespoints";
    }
    else if ( istyle & XYPlot::LINES )
    {
      styleName = "lines";
    }
    else
    {
      styleName = "points";
    }

    specs[i] = String::format ( "%s = %s : %s",
                                dataNames_ [i],
                                dataLabels_[i],
                                styleName );
  }

  myProps.set ( PropNames::DATA_SETS, specs );
}


//-----------------------------------------------------------------------
//   plotVector_
//-----------------------------------------------------------------------


void GraphPlugin::RunData_::plotVector_

  ( const String&  name,
    const String&  label,
    int            style,
    const Vector&  data )

{
  if ( data.size() % 2 )
  {
    print ( System::warn(), context, " : data set `", name,
            "\' has an odd length (",
            data.size(), "); skipping\n" );

    return;
  }

  graph_->newDataSet ( label,
                       data[slice(0,END,2)].clone (),
                       data[slice(1,END,2)].clone (),
                       style );
}


//-----------------------------------------------------------------------
//   plotVectors_
//-----------------------------------------------------------------------


void GraphPlugin::RunData_::plotVectors_

  ( const String&      label,
    int                style,
    const Properties&  props )

{
  using jem::toValue;
  using jem::isInstance;
  using jive::util::joinNames;

  StringVector  list = props.listProps ();

  String        labeli;

  for ( idx_t i = 0; i < list.size(); i++ )
  {
    String       name = joinNames ( props.getName(), list[i] );
    Ref<Object>  obj  = props.get ( list[i] );

    if ( ! isInstance<Vector>( obj ) )
    {
      print ( System::warn(), context, " : data set `",
              name, "\' has an invalid type; expected a vector\n" );
      continue;
    }

    if ( label.size() )
    {
      labeli = String::format ( "%s [%s]", label, list[i] );
    }
    else
    {
      labeli = list[i];
    }

    plotVector_ ( name, labeli, style,
                  toValue<const Vector&> ( obj ) );
  }
}


//-----------------------------------------------------------------------
//   setDataSpecs_
//-----------------------------------------------------------------------


void GraphPlugin::RunData_::setDataSpecs_

  ( const StringVector&  specs )

{
  const idx_t   n = specs.size ();

  StringVector  newNames  ( n );
  StringVector  newLabels ( n );
  IntVector     newStyles ( n );

  String        styleName;
  int           istyle;


  for ( idx_t i = 0; i < n; i++ )
  {
    String  s = specs[i];
    idx_t   j = s.rfind ( ':' );

    if ( j < 0 )
    {
      s         = s.stripWhite ();
      istyle    = XYPlot::LINES_POINTS;
    }
    else
    {
      s         = s[slice(BEGIN,j)]  .stripWhite ();
      styleName = s[slice(j + 1,END)].stripWhite ();


      if      ( styleName.equalsIgnoreCase( "linespoints" ) )
      {
        istyle = XYPlot::LINES_POINTS;
      }
      else if ( styleName.equalsIgnoreCase( "lines" ) )
      {
        istyle = XYPlot::LINES;
      }
      else if ( styleName.equalsIgnoreCase( "points" ) )
      {
        istyle = XYPlot::POINTS;
      }
      else
      {
        throw jem::IllegalInputException (
          CLASS_NAME,
          String::format (
            "invalid data style: `%s\'; must be `lines\', "
            "`points\' or `linespoints\'",
            styleName
          )
        );
      }
    }

    newStyles[i] = istyle;

    j = s.find ( '=' );

    if ( j < 0 )
    {
      newNames[i] = s;
    }
    else
    {
      newNames [i] = s[slice(BEGIN,j)]  .stripWhite ();
      newLabels[i] = s[slice(j + 1,END)].stripWhite ();
    }

    if ( ! newNames[i].size() )
    {
      throw jem::IllegalInputException (
        CLASS_NAME,
        "empty data set name"
      );
    }
  }

  dataNames_ .swap ( newNames  );
  dataLabels_.swap ( newLabels );
  dataStyles_.swap ( newStyles );
}



//=======================================================================
//   class GraphPlugin
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  GraphPlugin::TYPE_NAME = "Graph";


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


GraphPlugin::GraphPlugin ( const String& name ) :

  Super ( name )

{}


GraphPlugin::~GraphPlugin ()
{}


//-----------------------------------------------------------------------
//   init
//-----------------------------------------------------------------------


void GraphPlugin::init

  ( const Properties&  conf,
    const Properties&  props,
    const Properties&  viewdat,
    const AppContext&  apx )

{
  rundat_ = NIL;

  Ref<RunData_>  newdat = newInstance<RunData_> ( getContext() );

  newdat->init ( myName_, conf, props, viewdat );
  rundat_.swap ( newdat );
}


//-----------------------------------------------------------------------
//   update
//-----------------------------------------------------------------------


void GraphPlugin::update

  ( const Properties&  viewdat,
    const AppContext&  apx )

{
  if ( rundat_ != NIL )
  {
    rundat_->update ( viewdat, apx );
  }
}


//-----------------------------------------------------------------------
//   shutdown
//-----------------------------------------------------------------------


void GraphPlugin::shutdown

  ( const Properties&  viewdat,
    const AppContext&  apx )

{
  if ( rundat_ != NIL )
  {
    rundat_->shutdown ();
    rundat_ = NIL;
  }
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void GraphPlugin::configure

  ( const Properties&  props,
    const Properties&  viewdat,
    const AppContext&  apx )

{
  if ( rundat_ != NIL && props.contains( myName_ ) )
  {
    rundat_->configure ( props.findProps( myName_ ) );
  }
}


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void GraphPlugin::getConfig ( const Properties& props ) const
{
  if ( rundat_ != NIL )
  {
    rundat_->getConfig ( props.makeProps( myName_ ) );
  }
}


//-----------------------------------------------------------------------
//   declare
//-----------------------------------------------------------------------


void GraphPlugin::declare ()
{
  PluginFactory::declare ( TYPE_NAME,  & makeNew );
  PluginFactory::declare ( CLASS_NAME, & makeNew );
}


//-----------------------------------------------------------------------
//   makeNew
//-----------------------------------------------------------------------


Ref<Plugin> GraphPlugin::makeNew

  ( const String&      name,
    const Properties&  conf,
    const Properties&  props )

{
  return newInstance<Self> ( name );
}


JIVE_END_PACKAGE( gl )
