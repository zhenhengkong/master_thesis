
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

#ifndef JIVE_UTIL_DATABASESKIN_H
#define JIVE_UTIL_DATABASESKIN_H

#include <jive/util/import.h>
#include <jive/util/Database.h>


JIVE_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class DatabaseSkin
//-----------------------------------------------------------------------


class DatabaseSkin
{
 public:

  typedef DatabaseSkin      Self;
  typedef Database          Data;

  typedef IntDBColumn       IntColumn;
  typedef FloatDBColumn     FloatColumn;


  inline Event<Data&>&      newValuesEvent  () const;
  inline Event<Data&>&      newStructEvent  () const;

  inline void               resetEvents     () const;

  template <class Input>

    inline void             readFrom

    ( Input&                  in );

  template <class Output>

    inline void             writeTo

    ( Output&                 out )            const;

  inline bool               operator ==

    ( const Self&             rhs )            const;

  inline bool               operator ==

    ( const Nil&              rhs )            const;

  inline bool               operator !=

    ( const Self&             rhs )            const;

  inline bool               operator !=

    ( const Nil&              rhs )            const;

  inline Data*              getData         () const;
  inline void               clearData       ();
  inline void               clearAll        ();
  inline void               trimToSize      ();

  inline void               store

    ( const Properties&       globdat )        const;

  inline void               store

    ( const String&           name,
      const Properties&       globdat )        const;

  static void               dataError

    ( const String&           context,
      const String&           itemKind,
      const Ref<Data>&        data );


 protected:

  inline                    DatabaseSkin    ();

  explicit inline           DatabaseSkin

    ( const Ref<Data>&        data );

  inline                    DatabaseSkin

    ( const Self&             rhs );

  inline void               assign_

    ( const Self&             rhs );

  inline void               swap_

    ( Self&                   rhs );


 protected:

  Ref<Data>                 data_;


 private:

  Self&                     operator = ( const Self& );

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


template <class Input>

  inline void               decode

  ( Input&                    in,
    DatabaseSkin&             dbase );

template <class Output>

  inline void               encode

  ( Output&                   out,
    const DatabaseSkin&       dbase );

template <class Output>

  inline void               print

  ( Output&                   out,
    const DatabaseSkin&       dbase );

inline bool                 operator ==

  ( const Nil&                lhs,
    const DatabaseSkin&       rhs );

inline bool                 operator !=

  ( const Nil&                lhs,
    const DatabaseSkin&       rhs );





//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class DatabaseSkin
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


inline DatabaseSkin::DatabaseSkin ()
{}


inline DatabaseSkin::DatabaseSkin ( const Ref<Data>& data ) :

  data_ ( data )

{}


inline DatabaseSkin::DatabaseSkin ( const Self& rhs ) :

  data_ ( rhs.data_ )

{}


//-----------------------------------------------------------------------
//   newValuesEvent
//-----------------------------------------------------------------------


inline Event<Database&>& DatabaseSkin::newValuesEvent () const
{
  return data_->newValuesEvent;
}


//-----------------------------------------------------------------------
//   newStructEvent
//-----------------------------------------------------------------------


inline Event<Database&>& DatabaseSkin::newStructEvent () const
{
  return data_->newStructEvent;
}


//-----------------------------------------------------------------------
//   resetEvents
//-----------------------------------------------------------------------


inline void DatabaseSkin::resetEvents () const
{
  data_->resetEvents ();
}


//-----------------------------------------------------------------------
//   readFrom
//-----------------------------------------------------------------------


template <class Input>

  inline void DatabaseSkin::readFrom ( Input& in )

{
  decode ( in, data_ );
}


//-----------------------------------------------------------------------
//   writeTo
//-----------------------------------------------------------------------


template <class Output>

  inline void DatabaseSkin::writeTo ( Output& out ) const

{
  encode ( out, data_ );
}


//-----------------------------------------------------------------------
//   operator ==
//-----------------------------------------------------------------------


inline bool DatabaseSkin::operator == ( const Self& rhs ) const
{
  return (data_ == rhs.data_);
}


inline bool DatabaseSkin::operator == ( const Nil& rhs ) const
{
  return (data_ == rhs);
}


//-----------------------------------------------------------------------
//   operator !=
//-----------------------------------------------------------------------


inline bool DatabaseSkin::operator != ( const Self& rhs ) const
{
  return (data_ != rhs.data_);
}


inline bool DatabaseSkin::operator != ( const Nil& rhs ) const
{
  return (data_ != rhs);
}


//-----------------------------------------------------------------------
//   getData
//-----------------------------------------------------------------------


inline Database* DatabaseSkin::getData () const
{
  return data_.get ();
}


//-----------------------------------------------------------------------
//   clearData
//-----------------------------------------------------------------------


inline void DatabaseSkin::clearData ()
{
  data_->clearData ();
}


//-----------------------------------------------------------------------
//   clearAll
//-----------------------------------------------------------------------


inline void DatabaseSkin::clearAll ()
{
  data_->clearAll ();
}


//-----------------------------------------------------------------------
//   trimToSize
//-----------------------------------------------------------------------


inline void DatabaseSkin::trimToSize ()
{
  data_->trimToSize ();
}


//-----------------------------------------------------------------------
//   store
//-----------------------------------------------------------------------


inline void DatabaseSkin::store

  ( const Properties&  globdat ) const

{
  data_->store ( globdat );
}


inline void DatabaseSkin::store

  ( const String&      name,
    const Properties&  globdat ) const

{
  data_->store ( name, globdat );
}


//-----------------------------------------------------------------------
//   assign_
//-----------------------------------------------------------------------


inline void DatabaseSkin::assign_ ( const Self& rhs )
{
  data_ = rhs.data_;
}


//-----------------------------------------------------------------------
//   swap_
//-----------------------------------------------------------------------


inline void DatabaseSkin::swap_ ( Self& rhs )
{
  data_.swap ( rhs.data_ );
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   decode
//-----------------------------------------------------------------------


template <class Input>

  inline void             decode

  ( Input&                  in,
    DatabaseSkin&           dbase )

{
  dbase.readFrom ( in );
}


//-----------------------------------------------------------------------
//   encode
//-----------------------------------------------------------------------


template <class Output>

  inline void             encode

  ( Output&                 out,
    const DatabaseSkin&    dbase )

{
  dbase.writeTo ( out );
}


//-----------------------------------------------------------------------
//   print
//-----------------------------------------------------------------------


template <class Output>

  inline void             print

  ( Output&                 out,
    const DatabaseSkin&    dbase )

{
  dbase.getData()->printTo ( out );
}


//-----------------------------------------------------------------------
//   operator ==
//-----------------------------------------------------------------------


inline bool               operator ==

  ( const Nil&              lhs,
    const DatabaseSkin&    rhs )

{
  return rhs.operator == ( lhs );
}


//-----------------------------------------------------------------------
//   operator !=
//-----------------------------------------------------------------------


inline bool               operator !=

  ( const Nil&              lhs,
    const DatabaseSkin&    rhs )

{
  return rhs.operator != ( lhs );
}


JIVE_END_PACKAGE( util )

#endif
