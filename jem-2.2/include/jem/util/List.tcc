
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

#ifndef JEM_UTIL_LIST_TCC
#define JEM_UTIL_LIST_TCC

#include <jem/base/assert.h>


JEM_BEGIN_PACKAGE( util )


//=======================================================================
//   class ListEntry
//=======================================================================


template <class T>

  inline ListEntry<T>::ListEntry ( const T& val ) :

    value ( val )

{}


//=======================================================================
//   class List
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


template <class T, class A>

  inline List<T,A>::List () :

    allocator_ ( sizeof(Entry) )

{
  size_ = 0;
  end_  = new ListNode ();
}


template <class T, class A>

  List<T,A>::List ( const List& rhs ) :

    allocator_ ( sizeof(Entry) )

{
  size_ = 0;
  end_  = new ListNode ();

  try
  {
    insert ( end(), rhs.begin(), rhs.end() );
  }
  catch ( ... )
  {
    cleanup_ ();
    throw;
  }
}


template <class T, class A>

  template <class InputIterator>

  List<T,A>::List

  ( InputIterator  first,
    InputIterator  last ) :

    allocator_ ( sizeof(Entry) )

{
  size_ = 0;
  end_  = new ListNode();

  try
  {
    insert ( end(), first, last );
  }
  catch ( ... )
  {
    cleanup_ ();
    throw;
  }
}


template <class T, class A>

  inline List<T,A>::~List ()

{
  cleanup_ ();
}


//-----------------------------------------------------------------------
//   begin & end
//-----------------------------------------------------------------------


template <class T, class A>

  inline typename List<T,A>::Iterator
  List<T,A>::begin ()

{
  return Iterator ( end_->next() );
}


template <class T, class A>

  inline typename List<T,A>::ConstIterator
  List<T,A>::begin () const

{
  return ConstIterator ( end_->next() );
}


template <class T, class A>

  inline typename List<T,A>::Iterator
  List<T,A>::end ()

{
  return Iterator ( end_ );
}


template <class T, class A>

  inline typename List<T,A>::ConstIterator
  List<T,A>::end () const

{
  return ConstIterator ( end_ );
}


//-----------------------------------------------------------------------
//   front & back
//-----------------------------------------------------------------------


template <class T, class A>

  inline T& List<T,A>::front ()

{
  JEM_ASSERT2 ( size() > 0, "empty List" );

  return static_cast<Entry*>( end_->next() )->value;
}


template <class T, class A>

  inline const T& List<T,A>::front () const

{
  JEM_ASSERT2 ( size() > 0, "empty List" );

  return static_cast<Entry*>( end_->next() )->value;
}


template <class T, class A>

  inline T& List<T,A>::back ()

{
  JEM_ASSERT2 ( size() > 0, "empty List" );

  return static_cast<Entry*>( end_->prev() )->value;
}


template <class T, class A>

  inline const T& List<T,A>::back () const

{
  JEM_ASSERT2 ( size() > 0, "empty List" );

  return static_cast<Entry*>( end_->prev() )->value;
}


//-----------------------------------------------------------------------
//   assignment operator
//-----------------------------------------------------------------------


template <class T, class A>

  inline List<T,A>& List<T,A>::operator = ( const List& rhs )

{
  if ( this != &rhs )
  {
    clear  ();
    insert ( end(), rhs.begin(), rhs.end() );
  }

  return *this;
}


//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


template <class T, class A>

  inline void List<T,A>::swap ( List& rhs )

{
  allocator_.swap ( rhs.allocator_ );

  jem::swap ( size_, rhs.size_ );
  jem::swap ( end_,  rhs.end_ );
}


//-----------------------------------------------------------------------
//   pushFront
//-----------------------------------------------------------------------


template <class T, class A>

  inline void List<T,A>::pushFront ( const T& item )

{
  end_->pushBack ( newEntry_( item ) );
}


//-----------------------------------------------------------------------
//   pushBack
//-----------------------------------------------------------------------


template <class T, class A>

  inline void List<T,A>::pushBack ( const T& item )

{
  end_->pushFront ( newEntry_( item ) );
}


//-----------------------------------------------------------------------
//   popFront
//-----------------------------------------------------------------------


template <class T, class A>

  inline void List<T,A>::popFront ()

{
  JEM_ASSERT2 ( size() > 0, "empty List" );

  free_ ( static_cast<Entry*>( end_->next() ) );
}


//-----------------------------------------------------------------------
//   popBack
//-----------------------------------------------------------------------


template <class T, class A>

  inline void List<T,A>::popBack ()

{
  JEM_ASSERT2 ( size() > 0, "empty List" );

  free_ ( static_cast<Entry*>( end_->prev() ) );
}


//-----------------------------------------------------------------------
//   insert
//-----------------------------------------------------------------------


template <class T, class A>

  inline void List<T,A>::insert

  ( Iterator  pos,
    const T&  item )

{
  pos.getNode()->pushFront ( newEntry_( item ) );
}


template <class T, class A>
  template <class InputIterator>

  void List<T,A>::insert

  ( Iterator       pos,
    InputIterator  first,
    InputIterator  last )

{
  ListNode* n = pos.getNode ();

  while ( first != last )
  {
    n->pushFront ( newEntry_( *first ) );
    ++first;
  }
}


//-----------------------------------------------------------------------
//   erase
//-----------------------------------------------------------------------


template <class T, class A>

  inline void List<T,A>::erase ( Iterator pos )

{
  JEM_ASSERT2 ( pos != end(), "invalid List iterator" );

  free_ ( static_cast<Entry*>( pos.getNode() ) );
}


template <class T, class A>

  void List<T,A>::erase ( Iterator first, Iterator last )

{
  while ( first != last )
  {
    erase ( first++ );
  }
}


//-----------------------------------------------------------------------
//   clear
//-----------------------------------------------------------------------


template <class T, class A>

  void List<T,A>::clear ()

{
  Entry* entry;

  size_ = 0;

  while ( end_->next() != end_ )
  {
    entry = static_cast<Entry*> ( end_->next() );
    entry->~Entry ();
    allocator_.dealloc ( entry );
  }
}


//-----------------------------------------------------------------------
//   size
//-----------------------------------------------------------------------


template <class T, class A>

  inline idx_t List<T,A>::size () const

{
  return size_;
}


//-----------------------------------------------------------------------
//   cleanup_
//-----------------------------------------------------------------------


template <class T, class A>

  inline void List<T,A>::cleanup_ ()

{
  clear ();
  delete end_;
}


//-----------------------------------------------------------------------
//   newEntry_
//-----------------------------------------------------------------------


template <class T, class A>

  typename List<T,A>::Entry* List<T,A>::newEntry_

  ( const T& value )

{
  Entry*  entry = (Entry*) allocator_.alloc ();

  try
  {
    new (entry) Entry ( value );
  }
  catch( ... )
  {
    allocator_.dealloc ( entry );
    throw;
  }

  size_++;
  return entry;
}


//-----------------------------------------------------------------------
//   free_
//-----------------------------------------------------------------------


template <class T, class A>

  inline void List<T,A>::free_ ( Entry* entry )

{
  size_--;
  entry->~Entry();
  allocator_.dealloc( entry );
}


//=======================================================================
//   Related functions
//=======================================================================

//-----------------------------------------------------------------------
//   decode
//-----------------------------------------------------------------------


template <class Input,
          class T, class A>

  void              decode

  ( Input&            in,
    List<T,A>&        lst )

{
  uidx_t  sz;

  decode ( in, sz );

  idx_t  n = (idx_t) sz;

  lst.clear ();

  for ( idx_t i = 0; i < n; i++ )
  {
    T  item;

    decode ( in, item );

    lst.pushBack ( item );
  }
}


//-----------------------------------------------------------------------
//   encode
//-----------------------------------------------------------------------


template <class Output,
          class T, class A>

  void              encode

  ( Output&           out,
    const List<T,A>&  lst )

{
  typename List<T,A>::ConstIterator  first = lst.begin ();
  typename List<T,A>::ConstIterator  last  = lst.end   ();

  encode ( out, lst.size() );

  for ( ; first != last; ++first )
  {
    encode ( out, *first );
  }
}


//-----------------------------------------------------------------------
//   print
//-----------------------------------------------------------------------


template <class Output,
          class T, class A>

  void              print

  ( Output&           out,
    const List<T,A>&  lst )

{
  if ( lst.size() > 0 )
  {
    print         ( out, "[ " );
    printSequence ( out, lst.begin(), lst.end(), ", " );
    print         ( out, " ]" );
  }
  else
  {
    print         ( out, "[]" );
  }
}


//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


template <class T, class A>

  inline void   swap

  ( List<T,A>&    lhs,
    List<T,A>&    rhs )

{
  lhs.swap ( rhs );
}


JEM_END_PACKAGE( util )

JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class TypeTraits< util::List<T,A> >
//=======================================================================


template <class T, class A>

  class TypeTraits< util::List<T,A> > : public BasicTypeTraits

{
 public:

  static const bool   IS_SERIALIZABLE = IsSerializable<T>::VALUE;

  static String       whatis   ()
  {
    return TypeTraits<T>::whatis() + String ( " list" );
  }

  static String       typeName ()
  {
    return ( String ( "jem::util::List<" ) +
             TypeTraits<T>::typeName ()    +
             String ( ',' )                +
             TypeTraits<A>::typeName()     +
             String ( '>' ) );
  }

};


JEM_END_PACKAGE_BASE

#endif
