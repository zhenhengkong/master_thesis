
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

#ifndef JEM_BASE_OBJECT_H
#define JEM_BASE_OBJECT_H

#include <jem/base/Ref.h>
#include <jem/base/String.h>


JEM_BEGIN_PACKAGE_BASE


class Class;


//-----------------------------------------------------------------------
//   class Object
//-----------------------------------------------------------------------


class Object : public Collectable
{
 public:

  typedef Object          Self;
  typedef Object          DeclaringClass;

  static const char*      CLASS_NAME;


  static  Class*          makeClass     ();
  static  Class*          getType       ();
  virtual Class*          getClass      () const;

  virtual String          toString      () const;
  virtual lint            hashValue     () const;

  virtual bool            equals

    ( const Ref<Object>&    rhs )          const;

  Ref<Object>             clone         () const;


 protected:

  inline                  Object        ();
  virtual inline         ~Object        ();

};


//-----------------------------------------------------------------------
//   related macros
//-----------------------------------------------------------------------


#undef  JEM_DECLARE_CLASS
#define JEM_DECLARE_CLASS( self, super )        \
                                                \
  typedef self            Self;                 \
  typedef super           Super;                \
  typedef self            DeclaringClass;       \
                                                \
  static const char*      CLASS_NAME;           \
                                                \
  static  jem::Class*     getType     ();       \
  static  jem::Class*     makeClass   ();       \
  virtual jem::Class*     getClass    () const


//-----------------------------------------------------------------------
//   class Ref<Object>
//-----------------------------------------------------------------------


template <>

  class Ref<Object>

{
 public:

  enum                    TypeID
  {
                            NIL_TYPE,
                            BOOL_TYPE,
                            CHAR_TYPE,
                            STRING_TYPE,
                            INT_TYPE,
                            FLOAT_TYPE,
                            OBJECT_TYPE
  };


  inline                  Ref         ();

  inline                  Ref

    ( const Nil&            rhs );

  inline                  Ref

    ( Object*               rhs );

                          Ref

    ( const Ref&            rhs );

  template <class T>
    inline                Ref

    ( const Ref<T>&         rhs );

  inline                  Ref

    ( bool                  rhs,
      const RawRef&         raw );

  inline                  Ref

    ( char                  rhs,
      const RawRef&         raw );

  inline                  Ref

    ( lint                  rhs,
      const RawRef&         raw );

  inline                  Ref

    ( double                rhs,
      const RawRef&         raw );

                          Ref

    ( const String&         rhs,
      const RawRef&         raw );

  inline                  Ref

    ( Object&               rhs,
      Collectable&          base );

  inline                  Ref

    ( Object&               rhs,
      Collectable&          base,
      const RawRef&         raw );

  inline                 ~Ref         ();

  Ref&                    operator =

    ( const Ref&            rhs );

  template <class T>
    inline Ref&           operator =

    ( const Ref<T>&         rhs );

  Ref&                    operator =

    ( Object*               rhs );

  Ref&                    operator =

    ( const Nil&            rhs );

  inline bool             operator ==

    ( const Ref&            rhs )        const;

  template <class T>
    inline bool           operator ==

    ( const Ref<T>&         rhs )        const;

  template <class T>
    inline bool           operator ==

    ( const T*              rhs )        const;

  inline bool             operator ==

    ( const Nil&            rhs )        const;

  inline Object*          operator -> () const;
  inline Object&          operator  * () const;

  void                    swap

    ( Ref&                  rhs );

  inline Object*          get         () const;
  inline Collectable*     getBase     () const;

  inline TypeID           getType     () const;
  Class*                  getClass    () const;

  String                  toString    () const;
  lint                    hashValue   () const;

  bool                    equals

    ( const Ref&            rhs )        const;

  Ref                     clone       () const;

  inline bool             getBool     () const;
  inline char             getChar     () const;
  inline lint             getInt      () const;
  inline double           getFloat    () const;
  inline String           getString   () const;
  inline Object&          getObject   () const;

  static Object*          dynamicCast

    ( Object&               rhs,
      Class*                target );


 private:

  void                    toObject_   () const;

  inline bool             checkAddr_  () const;
  static void             nullError_  ();


 private:

  TypeID                  type_;

  union
  {
    lint                  ivalue_;
    double                xvalue_;
    char                  string_ [TinyString::MAXLEN];
    Object*               object_;
  };

};




//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class Object
//=======================================================================


inline Object::Object  ()
{}


inline Object::~Object ()
{}


//=======================================================================
//   class Ref<Object>
//=======================================================================

//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


inline Ref<Object>::Ref ()
{
  type_   = NIL_TYPE;
  object_ = NULL;
}


inline Ref<Object>::Ref ( const Nil& )
{
  type_   = NIL_TYPE;
  object_ = NULL;
}


inline Ref<Object>::Ref ( Object* rhs )
{
  object_ = rhs;

  if ( rhs )
  {
    type_ = OBJECT_TYPE;

    Collectable::addRef ( *rhs );
  }
  else
  {
    type_ = NIL_TYPE;
  }
}


template <class T>

  inline Ref<Object>::Ref ( const Ref<T>& rhs )

{
  object_ = rhs.get ();

  if ( object_ )
  {
    type_ = OBJECT_TYPE;

    Collectable::addRef ( *object_ );
  }
  else
  {
    type_ = NIL_TYPE;
  }
}


inline Ref<Object>::Ref

  ( bool           rhs,
    const RawRef&  raw )

{
  type_   = BOOL_TYPE;
  ivalue_ = rhs ? JEM_LINT_C(1) : JEM_LINT_C(0);
}


inline Ref<Object>::Ref

  ( char           rhs,
    const RawRef&  raw )

{
  type_   = CHAR_TYPE;
  ivalue_ = rhs;
}


inline Ref<Object>::Ref

  ( lint           rhs,
    const RawRef&  raw )

{
  type_   = INT_TYPE;
  ivalue_ = rhs;
}


inline Ref<Object>::Ref

  ( double         rhs,
    const RawRef&  raw )

{
  type_   = FLOAT_TYPE;
  xvalue_ = rhs;
}


inline Ref<Object>::Ref

  ( Object&       rhs,
    Collectable&  base )

{
  type_   = OBJECT_TYPE;
  object_ = & rhs;

  Collectable::addRef ( rhs );
}


inline Ref<Object>::Ref

  ( Object&        rhs,
    Collectable&   base,
    const RawRef&  raw )

{
  type_   = OBJECT_TYPE;
  object_ = & rhs;
}


inline Ref<Object>::~Ref ()
{
  if ( type_ == OBJECT_TYPE )
  {
    Collectable::delRef ( *object_ );
  }
}


//-----------------------------------------------------------------------
//   assignment operators
//-----------------------------------------------------------------------


template <class T>

  inline Ref<Object>&
  Ref<Object>::operator = ( const Ref<T>& rhs )

{
  return operator = ( rhs.get() );
}


//-----------------------------------------------------------------------
//   equality operators
//-----------------------------------------------------------------------


inline bool Ref<Object>::operator == ( const Ref& rhs ) const
{
  if ( type_ == OBJECT_TYPE && rhs.type_ == OBJECT_TYPE )
  {
    return (object_ == rhs.object_);
  }
  else
  {
    return (type_ == NIL_TYPE && rhs.type_ == NIL_TYPE);
  }
}


template <class T>

  inline bool Ref<Object>::operator == ( const Ref<T>& rhs ) const

{
  return operator == ( rhs.get() );
}


template <class T>

  inline bool Ref<Object>::operator == ( const T* rhs ) const

{
  if ( type_ == NIL_TYPE || type_ == OBJECT_TYPE )
  {
    return (object_ == rhs);
  }
  else
  {
    return false;
  }
}


inline bool Ref<Object>::operator == ( const Nil& rhs ) const
{
  return (type_ == NIL_TYPE);
}


//-----------------------------------------------------------------------
//   access operators
//-----------------------------------------------------------------------


inline Object* Ref<Object>::operator -> () const
{
  JEM_ASSERT ( checkAddr_() );

  if ( type_ != OBJECT_TYPE )
  {
    toObject_ ();
  }

  return object_;
}


inline Object& Ref<Object>::operator  * () const
{
  JEM_ASSERT ( checkAddr_() );

  if ( type_ != OBJECT_TYPE )
  {
    toObject_ ();
  }

  return *object_;
}


//-----------------------------------------------------------------------
//   get & getBase
//-----------------------------------------------------------------------


inline Object* Ref<Object>::get () const
{
  if ( type_ != OBJECT_TYPE && type_ != NIL_TYPE )
  {
    toObject_ ();
  }

  return object_;
}


inline Collectable* Ref<Object>::getBase () const
{
  return get ();
}


//-----------------------------------------------------------------------
//   getType
//-----------------------------------------------------------------------


inline Ref<Object>::TypeID Ref<Object>::getType () const
{
  return type_;
}


//-----------------------------------------------------------------------
//   getBool
//-----------------------------------------------------------------------


inline bool Ref<Object>::getBool () const
{
  JEM_ASSERT2 ( getType() == BOOL_TYPE,
                "invalid type (expected a boolean)" );

  return (ivalue_ != 0);
}


//-----------------------------------------------------------------------
//   getChar
//-----------------------------------------------------------------------


inline char Ref<Object>::getChar () const
{
  JEM_ASSERT2 ( getType() == CHAR_TYPE,
                "invalid type (expected a character)" );

  return (char) ivalue_;
}


//-----------------------------------------------------------------------
//   getInt
//-----------------------------------------------------------------------


inline lint Ref<Object>::getInt () const
{
  JEM_ASSERT2 ( getType() == INT_TYPE,
                "invalid type (expected an integer)" );

  return ivalue_;
}


//-----------------------------------------------------------------------
//   getFloat
//-----------------------------------------------------------------------


inline double Ref<Object>::getFloat () const
{
  JEM_ASSERT2 ( getType() == FLOAT_TYPE,
                "invalid type (expected a floating point number)" );

  return xvalue_;
}


//-----------------------------------------------------------------------
//   getString
//-----------------------------------------------------------------------


inline String Ref<Object>::getString () const
{
  JEM_ASSERT2 ( getType() == STRING_TYPE,
                "invalid type (expected a string)" );

  return TinyString ( object_ );
}



//-----------------------------------------------------------------------
//   getObject
//-----------------------------------------------------------------------


inline Object& Ref<Object>::getObject () const
{
  JEM_ASSERT2 ( getType() == OBJECT_TYPE,
                "invalid type (expected an Object)" );

  return *object_;
}


//-----------------------------------------------------------------------
//   checkAddr_
//-----------------------------------------------------------------------


inline bool Ref<Object>::checkAddr_ () const
{
  if ( type_ == NIL_TYPE )
  {
    nullError_ ();
  }

  return true;
}


//=======================================================================
//   class DynRefCaster<T,Object>
//=======================================================================


template <class T>

  class DynRefCaster<T,Object>

{
 public:

  static inline T*        dynamicCast

    ( const Ref<Object>&    rhs );


 private:

  static inline T*        dynamicCast_

    ( const Ref<Object>&    rhs,
      void*                 target );

  static inline T*        dynamicCast_

    ( const Ref<Object>&    rhs,
      Object*               target );

  static inline T*        dynamicCast2_

    ( Object&               rhs,
      void*                 target );

  static inline T*        dynamicCast2_

    ( Object&               rhs,
      T*                    target );

};


//-----------------------------------------------------------------------
//   dynamicCast
//-----------------------------------------------------------------------


template <class T>

  inline T* DynRefCaster<T,Object>::dynamicCast

  ( const Ref<Object>& rhs )

{
  return dynamicCast_ ( rhs, (T*) NULL );
}


//-----------------------------------------------------------------------
//   dynamicCast_
//-----------------------------------------------------------------------


template <class T>

  inline T* DynRefCaster<T,Object>::dynamicCast_

  ( const Ref<Object>&  rhs,
    void*               target )

{
  return dynamic_cast<T*> ( rhs.get() );
}


template <class T>

  inline T* DynRefCaster<T,Object>::dynamicCast_

  ( const Ref<Object>&  rhs,
    Object*             target )

{
  if ( rhs.getType() == Ref<Object>::OBJECT_TYPE )
  {
    return dynamicCast2_ ( rhs.getObject(),
                           (typename T::DeclaringClass*) NULL );
  }
  else
  {
    return (T*) NULL;
  }
}


//-----------------------------------------------------------------------
//   dynamicCast2_
//-----------------------------------------------------------------------


template <class T>

  inline T* DynRefCaster<T,Object>::dynamicCast2_

  ( Object&  rhs,
    void*    target )

{
  return dynamic_cast<T*> ( &rhs );
}


template <class T>

  inline T* DynRefCaster<T,Object>::dynamicCast2_

  ( Object&  rhs,
    T*       target )

{
  return static_cast<T*>

    ( Ref<Object>::dynamicCast( rhs, T::getType() ) );
}


//=======================================================================
//   class DynRefCaster<Object,Object>
//=======================================================================


template <>

  class DynRefCaster<Object,Object>

{
 public:

  static Object*          dynamicCast

    ( const Ref<Object>&    rhs )

  {
    return rhs.get ();
  }

};


JEM_END_PACKAGE_BASE

#endif
