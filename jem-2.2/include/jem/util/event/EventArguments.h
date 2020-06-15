
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

#ifndef JEM_UTIL_EVENT_EVENTARGUMENTS_H
#define JEM_UTIL_EVENT_EVENTARGUMENTS_H

#include <jem/base/utilities.h>


JEM_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class EventArguments (primary template)
//-----------------------------------------------------------------------


template <class A = Nil, class B = Nil, class C = Nil>

  class EventArguments

{
 public:

  static const int    ARG_COUNT = 3;

  A                   first;
  B                   second;
  C                   third;


                      EventArguments ( A a, B b, C c ) :

    first  ( a ),
    second ( b ),
    third  ( c )

  {}

};


//-----------------------------------------------------------------------
//   class EventArguments<A,B,Nil>
//-----------------------------------------------------------------------


template <class A, class B>

  class EventArguments<A,B,Nil>

{
 public:

  static const int    ARG_COUNT = 2;

  A                   first;
  B                   second;


                      EventArguments ( A a, B b ) :

    first  ( a ),
    second ( b )

  {}

};


//-----------------------------------------------------------------------
//   class EventArguments<A,Nil,Nil>
//-----------------------------------------------------------------------


template <class A>

  class EventArguments<A,Nil,Nil>

{
 public:

  static const int    ARG_COUNT = 1;

  A                   first;


  explicit            EventArguments ( A a ) :

    first ( a )

  {}

};


//-----------------------------------------------------------------------
//   class EventArguments<Nil,Nil,Nil>
//-----------------------------------------------------------------------


template <>

  class EventArguments<Nil,Nil,Nil>

{
 public:

  static const int    ARG_COUNT = 0;


                      EventArguments ()
  {}

};


JEM_END_PACKAGE( util )

#endif
