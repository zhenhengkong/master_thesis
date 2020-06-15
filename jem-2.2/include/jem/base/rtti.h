
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

#ifndef JEM_BASE_RTTI_H
#define JEM_BASE_RTTI_H

#include <jem/base/ObjectTraits.h>


JEM_BEGIN_PACKAGE_BASE


//-----------------------------------------------------------------------
//   utility functions
//-----------------------------------------------------------------------


template <class T>

  inline Class*         typeOf ();

template <class T>

  inline Class*         typeOf

  ( const T&              value );

template <class T>

  inline bool           isInstance

  ( const Ref<Object>&    obj );

template <class T>

  inline Ref<Object>    toObject

  ( const T&              value );

template <class T>

  inline T              toValue

  ( const Ref<Object>&    obj );

template <class T>

  inline bool           toValue

  ( T&                    value,
    const Ref<Object>&    obj );







//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   typeOf
//-----------------------------------------------------------------------


template <class T>

  inline Class* typeOf ()

{
  return ObjectTraits<T>::getType ();
}


template <class T>

  inline Class* typeOf ( const T& )

{
  return ObjectTraits<T>::getType ();
}


//-----------------------------------------------------------------------
//   isInstance
//-----------------------------------------------------------------------


template <class T>

  bool isInstance ( const Ref<Object>& obj )

{
  return ObjectTraits<T>::isInstance ( obj );
}


//-----------------------------------------------------------------------
//   toObject
//-----------------------------------------------------------------------


template <class T>

  inline Ref<Object> toObject ( const T& value )

{
  return ObjectTraits<T>::toObject ( value );
}


//-----------------------------------------------------------------------
//   toValue
//-----------------------------------------------------------------------


template <class T>

  inline T toValue ( const Ref<Object>& obj )

{
  return ObjectTraits<T>::toValue ( obj );
}


template <class T>

  inline bool         toValue

  ( T&                  value,
    const Ref<Object>&  obj )

{
  return ObjectTraits<T>::toValue ( value, obj );
}


JEM_END_PACKAGE_BASE

#endif