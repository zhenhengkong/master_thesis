/*
 * 
 *  Copyright (C) 2008 TU Delft. All rights reserved.
 *  
 *  F.P. van der Meer, June 2008
 *
 *  Module that interacts with XFEMModel and InterfaceModel
 *  to write output to file
 *
 */

#include <jem/base/array/intrinsics.h>
#include <jem/base/array/operators.h>
#include <jem/base/array/utilities.h>
#include <jem/base/limits.h>
#include <jem/base/System.h>
#include <jem/io/FileWriter.h>
#include <jem/util/Properties.h>
#include <jem/util/Dictionary.h>
#include <jive/app/ModuleFactory.h>
#include <jive/util/DenseTable.h>
#include <jive/util/Globdat.h>
#include <jive/util/ItemSet.h>
#include <jive/util/utilities.h>
#include <jive/model/Actions.h>
#include <jive/model/Model.h>
#include <jive/Array.h>

#include "XOutputModule.h"

using jem::NIL;
using jem::Limits;
using jem::dynamicCast;
using jem::newInstance;
using jem::System;
using jem::io::endl;
using jem::io::FileWriter;
using jem::util::Dict;
using jem::util::DictEnum;
using jive::model::Actions;
using jive::model::ActionParams;
using jive::util::Globdat;
using jive::util::ItemSet;
using jive::util::DenseTable;
using jive::util::XTable;
using jive::StringVector;
using jive::Vector;
using jive::IdxVector;

//=======================================================================
//   class XOutputModule
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------

const char* XOutputModule::TYPE_NAME       = "XOutput";
const char* XOutputModule::TABLE_CONTENTS  = "contentString";
const char* XOutputModule::TABLE_NAME      = "xoutTable";

//-----------------------------------------------------------------------
//   constructor & destructor
//-----------------------------------------------------------------------


XOutputModule::XOutputModule ( const String&  myName ) :
  Super ( myName )

{ 
  // default values
  
  write_       = true;
  writeCr_     = false;
  dissipation_ = true;
  tableData_   = "";

  dissOut_     = NIL;
  tableOut_    = NIL;
}


XOutputModule::~XOutputModule ()

{ }


//-----------------------------------------------------------------------
//   init
//-----------------------------------------------------------------------


Module::Status XOutputModule::init

  ( const Properties&  conf,
    const Properties&  props,
    const Properties&  globdat )

{
  using jive::model::Model;
  Ref<Model>  model = Model::get ( globdat, getContext() );

  out_      = &System::out();
  StringVector interfaces;

  Properties  myProps = props.findProps ( myName_ );
  Properties  myConf  = conf.makeProps ( myName_ );

  myProps.find( write_, "write" );
  myConf.set  ( "write", write_ );

  myProps.find( tableData_, "table" );
  myConf.set  ( "table", tableData_ );

  interval_ = 1;
  myProps.find( interval_, "interval", 1, 100 );
  myConf.set  ( "interval", interval_ );

  myProps.find( dissipation_, "dissipation" );
  myConf.set  ( "dissipation", dissipation_ );

  if ( write_ )
  {
    String dummy;

    if ( myProps.find ( dummy, "tractions" ) )
    {
      // don't write tractions by default

      tracOut_  = initWriter_(   "tractions", myProps );
    }

    writeCr_  = true;
    myProps.find( writeCr_, "writeCracks" );
    myConf.set  ( "writeCracks", writeCr_ );

    writeCohInfo_ = false;
    myProps.find( writeCohInfo_, "writeCohInfo" );
    myConf.set  ( "writeCohInfo", writeCohInfo_ );
  }
  else
  {
    tracOut_ = NIL;
  }

  return OK;
}


//-----------------------------------------------------------------------
//   run
//-----------------------------------------------------------------------


Module::Status XOutputModule::run ( const Properties& globdat )
{
  using jive::model::Model;

  idx_t       it;

  Ref<Model>  model = Model::get ( globdat, getContext() );

  globdat.get ( it, Globdat::TIME_STEP );

  Properties  params;

  // find info from boolean from FlexArclenModule
  
  bool accepted = true;

  globdat.find ( accepted, "var.accepted" );

  if ( accepted )
  {
    // ask models for dissipation data and write to file

    if ( dissipation_ ) 
    {
      params.clear();

      model->takeAction ( "GET_DISSIPATION", params, globdat );

      writeDissipation_ ( params );
    }

    // for bigger write tasks: possibly use interval

    if ( write_ && (it-1) % interval_ == 0 )
    {
      // fill params for WRITE_XOUTPUT

      if ( tracOut_ != NIL )
      {
        params.set( "tracOut", tracOut_ );
      }

      params.set( "writeCrack", writeCr_ );

      params.set( "writeCohInfo", writeCohInfo_ );

      // ask models to write data

      model->takeAction ( "WRITE_XOUTPUT", params, globdat );

      // System::setOutStream ( out_ );

      if ( tableData_.size() > 0 )
      {
        Ref<ItemSet>   items;
        Ref<Table>     table;
        Vector         weights;

        String         NODES   = "nodes";
        String         context = "xoutput.writeTable";

        params.clear();

        items = ItemSet::get ( NODES, globdat, context );
        table = Table::find ( TABLE_NAME, items, globdat );

        if ( table == NIL )
        {
          Ref<XTable>  xtable = newInstance<DenseTable> ( "xoutput", items );

          weights . resize ( items->size() );
          weights = 0.0;

          xtable->clearAll();

          params.set ( ActionParams::TABLE,         xtable     );
          params.set ( ActionParams::TABLE_NAME,    TABLE_NAME );
          params.set ( ActionParams::TABLE_WEIGHTS, weights    );
          params.set ( TABLE_CONTENTS,              tableData_ );

          bool updated = model->takeAction ( Actions::GET_TABLE, 
              params, globdat );

          if ( ! updated )
          {
            System::warn() << context << " TABLE has not been updated \n";
          }

          weights = where ( abs(weights) < Limits<double>::TINY_VALUE,
              1.0, 1.0 / weights );

          table = xtable;

          xtable->scaleRows ( weights );
        }
        writeTable_ ( table, globdat );
      }
    }
  }
  return OK;
}


//----------------------------------------------------------------------
//   shutdown
//----------------------------------------------------------------------


void XOutputModule::shutdown ( const Properties& globdat )
{
  shutdownWriter_( tracOut_ );
  shutdownWriter_( dissOut_ );
  shutdownWriter_( tableOut_ );
}


//-----------------------------------------------------------------------
//   configure
//-----------------------------------------------------------------------


void XOutputModule::configure

  ( const Properties&  props,
    const Properties&  globdat )

{ }


//-----------------------------------------------------------------------
//   getConfig
//-----------------------------------------------------------------------


void XOutputModule::getConfig

  ( const Properties&  conf,
    const Properties&  globdat ) const

{ }

//-----------------------------------------------------------------------
//   initWriter
//-----------------------------------------------------------------------

Ref<PrintWriter> XOutputModule::initWriter_

  ( const String      name,
    const Properties& myProps ) const

{
  using jive::util::joinNames;

  String fileName;
  String prepend;

  if ( ! myProps.find( fileName , name ) )
  {
    // default name

    fileName = joinNames( name, "dat" );
  }

  if ( myProps.find( prepend, "prepend" ) )
  {
    fileName = joinNames( prepend, fileName );
  }

  if ( fileName == "" || fileName == "none" )
  {
    return  NIL;
  }
  else
  {
    return newInstance<PrintWriter>(
           newInstance<FileWriter> ( fileName ) );
  }
}

//-----------------------------------------------------------------------
//   shutdownWriter
//-----------------------------------------------------------------------

void XOutputModule::shutdownWriter_

  ( const Ref<PrintWriter> outName ) const

{
  if ( outName != NIL )
  {
    outName->close();
  }
}

//-----------------------------------------------------------------------
//   writeDissipation_
//-----------------------------------------------------------------------

void XOutputModule::writeDissipation_

  ( const Properties&      params )

{
  Ref<Dict>       models;
  Ref<DictEnum>   e;
  double          value;

  models = params.getContents();

  // initialize file

  if ( dissOut_ == NIL )
  {
    dissOut_ = initWriter_ ( "dissipation", Properties() );

    for ( e = models->enumerate(); ! e->atEnd(); e->toNext() )
    {
      // write headers 

      *dissOut_ << e->getKey() << " ";
    }
    *dissOut_ << '\n';
  }

  // write values 

  for ( e = models->enumerate(); ! e->atEnd(); e->toNext() )
  {
    params.find ( value, e->getKey() );
    *dissOut_ << value ;
    *dissOut_ << ' ' ;
  }
  *dissOut_ << '\n';
  dissOut_->flush();
}

//-----------------------------------------------------------------------
//   writeTable_
//-----------------------------------------------------------------------

void XOutputModule::writeTable_

  ( const Ref<Table>   table,
    const Properties&  globdat )

{
  // initialize file

  if ( tableOut_ == NIL )
  {
    tableOut_ = initWriter_ ( "xotable", Properties() );
  }

  // get time step number

  idx_t it;
  globdat.get ( it, Globdat::TIME_STEP );

  // fill zeroes in unspecified nodes

  // write table 
  
  *tableOut_ << "newXOutput " << it << endl;
  table->printTo ( *tableOut_ );
  tableOut_->flush();
}

//-----------------------------------------------------------------------
//   makeNew
//-----------------------------------------------------------------------

Ref<Module>  XOutputModule::makeNew

  ( const String&           name,
    const Properties&       conf,
    const Properties&       props,
    const Properties&       globdat )

{
  return newInstance<Self> ( name );
}

//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   declareXOutputModule
//-----------------------------------------------------------------------

void declareXOutputModule ()
{
  using jive::app::ModuleFactory;

  ModuleFactory::declare ( XOutputModule::TYPE_NAME,
                          &XOutputModule::makeNew );
}
