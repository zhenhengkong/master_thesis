
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

#ifndef JEM_UTIL_LISTNODE_H
#define JEM_UTIL_LISTNODE_H

#include <jem/base/TypeTraits.h>


JEM_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class ListNode
//-----------------------------------------------------------------------


class ListNode
{
 public:

  inline              ListNode    ();
  inline             ~ListNode    ();

  inline void         unlink      ();

  inline void         pushFront

    ( ListNode*         node );

  inline void         pushBack

    ( ListNode*         node );

  inline ListNode*    next        () const;
  inline ListNode*    prev        () const;


 private:

                      ListNode    ( const ListNode& );
  ListNode&           operator =  ( const ListNode& );


 private:

  ListNode*           next_;
  ListNode*           prev_;

};




//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class ListNode
//=======================================================================


inline ListNode::ListNode ()
{
  next_ = prev_ = this;
}


inline ListNode::~ListNode ()
{
  unlink ();
}


inline void ListNode::unlink ()
{
  next_->prev_  = prev_;
  prev_->next_  = next_;
  next_ = prev_ = this;
}


inline void ListNode::pushFront ( ListNode* node )
{
  node->next_  = this;
  node->prev_  = prev_;
  prev_->next_ = node;
  prev_        = node;
}


inline void ListNode::pushBack ( ListNode* node )
{
  node->prev_  = this;
  node->next_  = next_;
  next_->prev_ = node;
  next_        = node;
}


inline ListNode* ListNode::next () const
{
  return next_;
}


inline ListNode* ListNode::prev () const
{
  return prev_;
}


JEM_END_PACKAGE( util )

JEM_BEGIN_PACKAGE_BASE


//=======================================================================
//   class TypeTraits<util::ListNode>
//=======================================================================


template <>

  class TypeTraits < util::ListNode > : public BasicTypeTraits

{
 public:

  static String  whatis   ()
  {
    return String ( "a list node" );
  }

  static String  typeName ()
  {
    return String ( "jem::util::ListNode" );
  }

};


JEM_END_PACKAGE_BASE

#endif
