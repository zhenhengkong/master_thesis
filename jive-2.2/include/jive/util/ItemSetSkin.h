
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

#ifndef JIVE_UTIL_ITEMSETSKIN_H
#define JIVE_UTIL_ITEMSETSKIN_H

#include <jive/util/ItemSet.h>


JIVE_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class ItemSetSkin
//-----------------------------------------------------------------------


class ItemSetSkin
{
 public:

  typedef ItemSetSkin       Self;
  typedef ItemSet           Data;

  inline Event<Data&>&      changeEvent   () const;
  inline Event
    < idx_t, Data& >&       newSizeEvent  () const;
  inline Event
    < const Reordering&,
      Data& >&              newOrderEvent () const;


  inline void               resetEvents   () const;

  inline idx_t              size          () const;
  inline String             getName       () const;
  inline String             getTagName    () const;
  inline String             getContext    () const;
  inline Data*              getData       () const;

  template <class Input>

    inline void             readFrom

    ( Input&                  in );

  template <class Output>

    inline void             writeTo

    ( Output&                 out )          const;

  inline void               store

    ( const Properties&       globdat )      const;

  inline void               store

    ( const String&           name,
      const Properties&       globdat )      const;

  inline static void        erase

    ( const String&           name,
      const Properties&       globdat );

  inline bool               operator ==

    ( const Self&             rhs )          const;

  inline bool               operator ==

    ( const Nil&              rhs )          const;

  inline bool               operator !=

    ( const Self&             rhs )          const;

  inline bool               operator !=

    ( const Nil&              rhs )          const;

  static void               dataError

    ( const String&           context,
      const String&           itemKind,
      const Ref<Data>&        data );


 protected:

  inline                    ItemSetSkin   ();

  explicit inline           ItemSetSkin

    ( const Ref<Data>&        data );

  inline                    ItemSetSkin

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
    ItemSetSkin&              items );

template <class Output>

  inline void               encode

  ( Output&                   out,
    const ItemSetSkin&        items );

template <class Output>

  inline void               print

  ( Output&                   out,
    const ItemSetSkin&        items );

inline bool                 operator ==

  ( const Nil&                lhs,
    const ItemSetSkin&        rhs );

inline bool                 operator !=

  ( const Nil&                lhs,
    const ItemSetSkin&        rhs );





//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class ItemSetSkin
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


inline ItemSetSkin::ItemSetSkin ()
{}


inline ItemSetSkin::ItemSetSkin ( const Ref<Data>& data ) :

  data_ ( data )

{}


inline ItemSetSkin::ItemSetSkin ( const Self& rhs ) :

  data_ ( rhs.data_ )

{}


//-----------------------------------------------------------------------
//   changeEvent
//-----------------------------------------------------------------------


inline Event<ItemSet&>& ItemSetSkin::changeEvent () const
{
  return data_->changeEvent;
}


//-----------------------------------------------------------------------
//   newSizeEvent
//-----------------------------------------------------------------------


inline Event<idx_t,ItemSet&>& ItemSetSkin::newSizeEvent () const
{
  return data_->newSizeEvent;
}


//-----------------------------------------------------------------------
//   newOrderEvent
//-----------------------------------------------------------------------


inline Event<const Reordering&,ItemSet&>&

  ItemSetSkin::newOrderEvent () const

{
  return data_->newOrderEvent;
}


//-----------------------------------------------------------------------
//   resetEvents
//-----------------------------------------------------------------------


inline void ItemSetSkin::resetEvents () const
{
  data_->resetEvents ();
}


//-----------------------------------------------------------------------
//   size
//-----------------------------------------------------------------------


inline idx_t ItemSetSkin::size () const
{
  return data_->size ();
}


//-----------------------------------------------------------------------
//   getName
//-----------------------------------------------------------------------


inline String ItemSetSkin::getName () const
{
  return data_->getName ();
}


//-----------------------------------------------------------------------
//   getTagName
//-----------------------------------------------------------------------


inline String ItemSetSkin::getTagName () const
{
  return data_->getTagName ();
}


//-----------------------------------------------------------------------
//   getContext
//-----------------------------------------------------------------------


inline String ItemSetSkin::getContext () const
{
  return data_->getContext ();
}


//-----------------------------------------------------------------------
//   getData
//-----------------------------------------------------------------------


inline ItemSet* ItemSetSkin::getData () const
{
  return data_.get ();
}


//-----------------------------------------------------------------------
//   readFrom
//-----------------------------------------------------------------------


template <class Input>

  inline void ItemSetSkin::readFrom ( Input& in )

{
  decode ( in, data_ );
}


//-----------------------------------------------------------------------
//   writeTo
//-----------------------------------------------------------------------


template <class Output>

  inline void ItemSetSkin::writeTo ( Output& out ) const

{
  encode ( out, data_ );
}


//-----------------------------------------------------------------------
//   store
//-----------------------------------------------------------------------


inline void ItemSetSkin::store

  ( const Properties&  globdat ) const

{
  data_->store ( globdat );
}


inline void ItemSetSkin::store

  ( const String&      name,
    const Properties&  globdat ) const

{
  data_->store ( name, globdat );
}


//-----------------------------------------------------------------------
//   erase
//-----------------------------------------------------------------------


inline void ItemSetSkin::erase

  ( const String&      name,
    const Properties&  globdat )

{
  ItemSet::erase ( name, globdat );
}


//-----------------------------------------------------------------------
//   operator ==
//-----------------------------------------------------------------------


inline bool ItemSetSkin::operator == ( const Self& rhs ) const
{
  return (data_ == rhs.data_);
}


inline bool ItemSetSkin::operator == ( const Nil& rhs ) const
{
  return (data_ == rhs);
}


//-----------------------------------------------------------------------
//   operator !=
//-----------------------------------------------------------------------


inline bool ItemSetSkin::operator != ( const Self& rhs ) const
{
  return ( data_ != rhs.data_ );
}


inline bool ItemSetSkin::operator != ( const Nil& rhs ) const
{
  return (data_ != rhs);
}


//-----------------------------------------------------------------------
//   assign_
//-----------------------------------------------------------------------


inline void ItemSetSkin::assign_ ( const Self& rhs )
{
  data_ = rhs.data_;
}


//-----------------------------------------------------------------------
//   swap_
//-----------------------------------------------------------------------


inline void ItemSetSkin::swap_ ( Self& rhs )
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

  inline void           decode

  ( Input&                in,
    ItemSetSkin&          items )

{
  items.readFrom ( in );
}


//-----------------------------------------------------------------------
//   encode
//-----------------------------------------------------------------------


template <class Output>

  inline void           encode

  ( Output&               out,
    const ItemSetSkin&    items )

{
  items.writeTo ( out );
}


//-----------------------------------------------------------------------
//   print
//-----------------------------------------------------------------------


template <class Output>

  inline void           print

  ( Output&               out,
    const ItemSetSkin&    items )

{
  items.getData()->printTo ( out );
}


//-----------------------------------------------------------------------
//   operator ==
//-----------------------------------------------------------------------


inline bool             operator ==

  ( const Nil&            lhs,
    const ItemSetSkin&    rhs )

{
  return rhs.operator == ( lhs );
}


//-----------------------------------------------------------------------
//   operator !=
//-----------------------------------------------------------------------


inline bool             operator !=

  ( const Nil&            lhs,
    const ItemSetSkin&    rhs )

{
  return rhs.operator != ( lhs );
}


JIVE_END_PACKAGE( util )

#endif
