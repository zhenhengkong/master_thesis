
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

#ifndef JIVE_GL_APPREF_H
#define JIVE_GL_APPREF_H

#include <jive/gl/AppContext.h>


JIVE_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class AppRef
//-----------------------------------------------------------------------


template <class T>

  class AppRef

{
 public:

  inline                  AppRef    ();

  template <class U>

    inline                AppRef

    ( const U&              rhs );

  inline                  AppRef

    ( const AppRef&         rhs );

  inline AppRef&          operator =

    ( const AppRef&         rhs );

  inline Ref<T>&          operator  []

    ( const AppContext&     apx );

  inline const Ref<T>&    operator  []

    ( const AppContext&     apx )      const;


 private:

  Ref<T>                  ref_;

};




//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


template <class T>

  inline AppRef<T>::AppRef ()

{}


template <class T>

  template <class U>

  inline AppRef<T>::AppRef ( const U& rhs ) :

    ref_ ( rhs )

{}


template <class T>

  inline AppRef<T>::AppRef ( const AppRef& rhs ) :

    ref_ ( rhs.ref_ )

{}


//-----------------------------------------------------------------------
//   operator =
//-----------------------------------------------------------------------


template <class T>

  inline AppRef<T>& AppRef<T>::operator = ( const AppRef& rhs )

{
  ref_ = rhs.ref_;

  return *this;
}


//-----------------------------------------------------------------------
//   operator []
//-----------------------------------------------------------------------


template <class T>

  inline Ref<T>& AppRef<T>::operator [] ( const AppContext& )

{
  return ref_;
}


template <class T>

  inline const Ref<T>& AppRef<T>::operator []

  ( const AppContext& ) const

{
  return ref_;
}


JIVE_END_PACKAGE( gl )

#endif