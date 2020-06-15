
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

#ifndef JEM_UTIL_LISTITERATOR_H
#define JEM_UTIL_LISTITERATOR_H

#include <iterator>
#include <jem/util/ListNode.h>


JEM_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class ListIterator
//-----------------------------------------------------------------------


template <class T,
          class R = typename T::ValueType&,
          class P = typename T::ValueType*>

  class ListIterator

{
 public:

  typedef T                       Node;
  typedef ListIterator            Self;

  typedef std::
    bidirectional_iterator_tag    iterator_category;
  typedef idx_t                   difference_type;
  typedef typename T::ValueType   value_type;
  typedef R                       reference;
  typedef P                       pointer;


  inline              ListIterator  ();

  explicit inline     ListIterator

    ( ListNode*         node );

  inline              ListIterator

    ( const Self&       rhs );

  inline Self&        operator ++   ();
  inline Self         operator ++   (int);
  inline Self&        operator --   ();
  inline Self         operator --   (int);

  inline reference    operator *    () const;
  inline pointer      operator ->   () const;

  inline bool         operator ==

    ( const Self&       rhs )          const;

  inline bool         operator !=

    ( const Self&       rhs )          const;

  inline ListNode*    getNode       () const;


 private:

  ListNode*           node_;

};





//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


template <class T, class R, class P>

  inline ListIterator<T,R,P>::ListIterator () : node_ ( 0 )

{}


template <class T, class R, class P>

  inline ListIterator<T,R,P>::ListIterator

  ( ListNode* node ) : node_ ( node )

{}


// Note: without a copy constructor, the Intel compiler may
//       generate invalid code.


template <class T, class R, class P>

  inline ListIterator<T,R,P>::ListIterator ( const Self& rhs ) :

    node_ ( rhs.node_ )

{}


//-----------------------------------------------------------------------
//   incr & decr operators
//-----------------------------------------------------------------------


template <class T, class R, class P>

  inline ListIterator<T,R,P>&
  ListIterator<T,R,P>::operator ++ ()

{
  node_ = node_->next ();

  return *this;
}


template <class T, class R, class P>

  inline ListIterator<T,R,P>
  ListIterator<T,R,P>::operator ++ (int)

{
  Self tmp ( *this );

  operator ++ ();
  return tmp;
}


template <class T, class R, class P>

  inline ListIterator<T,R,P>&
  ListIterator<T,R,P>::operator -- ()

{
  node_ = node_->prev ();
  return *this;
}


template <class T, class R, class P>

  inline ListIterator<T,R,P>
  ListIterator<T,R,P>::operator -- (int)

{
  Self tmp ( *this );

  operator -- ();
  return tmp;
}


//-----------------------------------------------------------------------
//   access operators
//-----------------------------------------------------------------------


template <class T, class R, class P>

  inline typename ListIterator<T,R,P>::reference
  ListIterator<T,R,P>::operator * () const

{
  return static_cast<Node*>( node_ )->value;
}


template <class T, class R, class P>

  inline typename ListIterator<T,R,P>::pointer
  ListIterator<T,R,P>::operator -> () const

{
  return &( static_cast<Node>( node_ )->value );
}


//-----------------------------------------------------------------------
//   (in)equality operators
//-----------------------------------------------------------------------


template <class T, class R, class P>

  inline bool ListIterator<T,R,P>::operator ==

  ( const Self& rhs ) const

{
  return (node_ == rhs.node_);
}


template <class T, class R, class P>

  inline bool ListIterator<T,R,P>::operator !=

  ( const Self& rhs ) const

{
  return (node_ != rhs.node_);
}


//-----------------------------------------------------------------------
//   getNode
//-----------------------------------------------------------------------


template <class T, class R, class P>

  inline ListNode* ListIterator<T,R,P>::getNode () const

{
  return node_;
}


JEM_END_PACKAGE( util )

#endif
