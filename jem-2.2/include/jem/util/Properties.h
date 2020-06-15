
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

#ifndef JEM_UTIL_PROPERTIES_H
#define JEM_UTIL_PROPERTIES_H

#include <jem/base/Object.h>
#include <jem/base/array/Array.h>
#include <jem/util/typedefs.h>


namespace jem
{
  namespace io
  {
    class Writer;
    class PrintWriter;
    class TextOutput;
    class ObjectInput;
    class ObjectOutput;
  }
}


JEM_BEGIN_PACKAGE( util )


class Tokenizer;
class Properties;


//-----------------------------------------------------------------------
//   class Property
//-----------------------------------------------------------------------


class Property
{
 public:

  inline                    Property

    ( const String&           name,
      const Properties&       props );

  inline                    Property

    ( const Property&         rhs );

  inline const Property&    operator =

    ( const Property&         rhs )           const;

  inline const Property&    operator =

    ( bool                    value )         const;

  inline const Property&    operator =

    ( char                    value )         const;

  inline const Property&    operator =

    ( byte                    value )         const;

  inline const Property&    operator =

    ( short                   value )         const;

  inline const Property&    operator =

    ( int                     value )         const;

  inline const Property&    operator =

    ( lint                    value )         const;

  inline const Property&    operator =

    ( float                   value )         const;

  inline const Property&    operator =

    ( double                  value )         const;

  inline const Property&    operator =

    ( const char*             value )         const;

  inline const Property&    operator =

    ( const String&           value )         const;

  inline const Property&    operator =

    ( const Properties&       values )        const;

  template <class T>
    inline const Property&  operator =

    ( const Array<T>&         values )        const;

  template <class T>
    inline const Property&  operator =

    ( const Ref<T>&           value  )        const;

  template <class T>
    inline const Property&  operator =

    ( T*                      value )         const;

  inline                    operator

    bool                      ()              const;

  inline                    operator

    char                      ()              const;

  inline                    operator

    byte                      ()              const;

  inline                    operator

    short                     ()              const;

  inline                    operator

    int                       ()              const;

  inline                    operator

    lint                      ()              const;

  inline                    operator

    float                     ()              const;

  inline                    operator

    double                    ()              const;

  inline                    operator

    String                    ()              const;

  inline                    operator

    Properties                ()              const;

  template <class T>
    inline                  operator

    Array<T>                  ()              const;

  template <class T>
    inline                  operator

    Ref<T>                    ()              const;


 private:

  const String&             name_;
  const Properties&         props_;

};


//-----------------------------------------------------------------------
//   class Properties
//-----------------------------------------------------------------------


class Properties
{
 public:

  typedef Properties        Self;
  typedef DictEnum          Enumerator;

  static const int          MAX_DEPTH;
  static const int          PARSE_STRICT;
  static const int          PARSE_TO_UPPER;
  static const int          PARSE_INCLUDE;
  static const int          CLONE_STRICT;
  static const int          MERGE_REPLACE;


  explicit                  Properties

    ( const String&           name = "" );

  explicit                  Properties

    ( const Ref<Dict>&        dict,
      const String&           name = "" );

                            Properties

    ( const Ref<Dict>&        dict,
      Ref<ObjConverter>       conv,
      const String&           name = "" );

                            Properties

    ( const Properties&       rhs );

  Properties&               operator =

    ( const Properties&       rhs );

  inline Property           operator []

    ( const String&           name )          const;

  void                      swap

    ( Properties&             rhs );

  Properties                clone

    ( int                     options = 0 )   const;

  idx_t                     size           () const;
  bool                      isShared       () const;
  inline String             getName        () const;
  Dict*                     getContents    () const;

  void                      setContents

    ( const Ref<Dict>&        dict );

  inline ObjConverter*      getConverter   () const;

  inline void               setConverter

    ( Ref<ObjConverter>       conv );

  Ref<Enumerator>           enumerate      () const;

  void                      parseFile

    ( const String&           fname,
      int                     options = 0 )   const;

  void                      parseFrom

    ( Tokenizer&              input,
      const String&           source,
      int                     options = 0 )   const;

  void                      mergeWith

    ( const Properties&       rhs,
      int                     options = 0 )   const;

  void                      printTo

    ( io::PrintWriter&        out )           const;

  static bool               printValue

    ( io::PrintWriter&        out,
      const String&           prefix,
      const Ref<Object>&      value );

  void                      set

    ( const String&           name,
      const Ref<Object>&      value  )        const;

  template <class T>
    inline void             set

    ( const String&           name,
      T*                      value )         const;

  void                      set

    ( const String&           name,
      bool                    value  )        const;

  void                      set

    ( const String&           name,
      char                    value  )        const;

  void                      set

    ( const String&           name,
      byte                    value  )        const;

  void                      set

    ( const String&           name,
      short                   value  )        const;

  void                      set

    ( const String&           name,
      int                     value  )        const;

  void                      set

    ( const String&           name,
      lint                    value  )        const;

  void                      set

    ( const String&           name,
      float                   value  )        const;

  void                      set

    ( const String&           name,
      double                  value  )        const;

  void                      set

    ( const String&           name,
      const char*             value  )        const;

  void                      set

    ( const String&           name,
      const String&           value  )        const;

  void                      set

    ( const String&           name,
      const Array<bool>&      values )        const;

  void                      set

    ( const String&           name,
      const Array<int>&       values )        const;

  void                      set

    ( const String&           name,
      const Array<double>&    values )        const;

  void                      set

    ( const String&           name,
      const Array<String>&    values )        const;

  void                      set

    ( const String&           name,
      const Properties&       values )        const;

  template <class T>
    bool                    find

    ( Ref<T>&                 value,
      const String&           name )          const;

  bool                      find

    ( Ref<Object>&            value,
      const String&           name )          const;

  bool                      find

    ( bool&                   value,
      const String&           name )          const;

  bool                      find

    ( char&                   value,
      const String&           name )          const;

  bool                      find

    ( byte&                   value,
      const String&           name )          const;

  bool                      find

    ( short&                  value,
      const String&           name )          const;

  bool                      find

    ( int&                    value,
      const String&           name )          const;

  bool                      find

    ( lint&                   value,
      const String&           name )          const;

  bool                      find

    ( float&                  value,
      const String&           name )          const;

  bool                      find

    ( double&                 value,
      const String&           name )          const;

  bool                      find

    ( String&                 value,
      const String&           name )          const;

  bool                      find

    ( Array<bool>&            values,
      const String&           name )          const;

  bool                      find

    ( Array<int>&             values,
      const String&           name )          const;

  bool                      find

    ( Array<double>&          values,
      const String&           name )          const;

  bool                      find

    ( Array<String>&          values,
      const String&           name )          const;

  bool                      find

    ( Properties&             values,
      const String&           name )          const;

  bool                      find

    ( int&                    value,
      const String&           name,
      int                     lbound,
      int                     ubound )        const;

  bool                      find

    ( lint&                   value,
      const String&           name,
      lint                    lbound,
      lint                    ubound )        const;

  bool                      find

    ( double&                 value,
      const String&           name,
      double                  lbound,
      double                  ubound )        const;

  bool                      find

    ( Array<int>&             values,
      const String&           name,
      int                     lbound,
      int                     ubound )        const;

  bool                      find

    ( Array<double>&          values,
      const String&           name,
      double                  lbound,
      double                  ubound )        const;

  Ref<Object>               get

    ( const String&           name )          const;

  template <class T>
    void                    get

    ( T&                      value,
      const String&           name )          const;

  template <class T>
    void                    get

    ( T&                      value,
      const String&           name,
      T                       lbound,
      T                       ubound )        const;

  template <class T>
    void                    get

    ( Array<T>&               values,
      const String&           name,
      T                       lbound,
      T                       ubound )        const;

  Properties                makeProps

    ( const String&           name )          const;

  Properties                findProps

    ( const String&           name )          const;

  Properties                getProps

    ( const String&           name )          const;

  Array<String>             listProps

    ( const String&           prefix = "" )   const;

  bool                      contains

    ( const String&           name )          const;

  void                      erase

    ( const String&           name )          const;

  void                      clear          () const;

  String                    getContext     () const;

  String                    getContext

    ( const String&           name )          const;

  void                      propertyError

    ( const String&           name,
      const String&           msg )           const;

  void                      depthError     () const;

  Ref<Dict>                 newDictionary  () const;


 private:

  inline void               init_          () const;

  void                      setRecursive_

    ( const String&           name,
      idx_t                   last,
      const Ref<Object>&      value )         const;

  bool                      findProps_

    ( Ref<Dict>&              props,
      const String&           name )          const;

  bool                      findRecursive_

    ( Ref<Object>&            value,
      const String&           name,
      idx_t                   last )          const;

  void                      eraseRecursive_

    ( const String&           name,
      idx_t                   last )          const;

  void                      typeError_

    ( const String&           name,
      const Ref<Object>&      obj,
      Class*                  type )          const;

  void                      findError_

    ( const String&           name )          const;

  bool                      mergeWith_

    ( Dict&                   lhs,
      DictEnum&               rhs,
      int                     options,
      int                     depth = 0 )     const;


 private:

  class                     Init_;

  Ref<Dict>                 dict_;
  Ref<ObjConverter>         conv_;
  String                    myName_;

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


inline void                 swap

  ( Properties&               lhs,
    Properties&               rhs );

inline void                 mergeNoReplace

  ( const Properties&         lhs,
    const Properties&         rhs );

inline void                 mergeAndReplace

  ( const Properties&         lhs,
    const Properties&         rhs );

void                        print

  ( io::Writer&               out,
    const Properties&         props );

inline void                 print

  ( io::PrintWriter&          out,
    const Properties&         props );

void                        decode

  ( io::ObjectInput&          in,
    Properties&               props );

void                        encode

  ( io::ObjectOutput&         out,
    const Properties&         props );

void                        getBool

  ( int&                      bits,
    int                       mask,
    const Properties&         props,
    const String&             name );

bool                        findBool

  ( int&                      bits,
    int                       mask,
    const Properties&         props,
    const String&             name );

inline void                 setBool

  ( const Properties&         props,
    const String&             name,
    int                       bits,
    int                       mask );






//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class Property
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


inline Property::Property

  ( const String&      name,
    const Properties&  props ) :

    name_  ( name ),
    props_ ( props )

{}


inline Property::Property ( const Property& rhs ) :

  name_  ( rhs.name_ ),
  props_ ( rhs.props_ )

{}


//-----------------------------------------------------------------------
//   assignment operators
//-----------------------------------------------------------------------


inline const Property&  Property::operator =

  ( const Property&  rhs ) const

{
  props_.set ( name_, rhs.props_.get( rhs.name_ ) );

  return *this;
}


inline const Property&  Property::operator =

  ( bool  value ) const

{
  props_.set ( name_, value );

  return *this;
}


inline const Property&  Property::operator =

  ( char  value ) const

{
  props_.set ( name_, value );

  return *this;
}


inline const Property&  Property::operator =

  ( byte  value ) const

{
  props_.set ( name_, value );

  return *this;
}


inline const Property&  Property::operator =

  ( short  value ) const

{
  props_.set ( name_, value );

  return *this;
}


inline const Property&  Property::operator =

  ( int  value ) const

{
  props_.set ( name_, value );

  return *this;
}


inline const Property&  Property::operator =

  ( lint  value ) const

{
  props_.set ( name_, value );

  return *this;
}


inline const Property&  Property::operator =

  ( float  value ) const

{
  props_.set ( name_, value );

  return *this;
}


inline const Property&  Property::operator =

  ( double  value ) const

{
  props_.set ( name_, value );

  return *this;
}


inline const Property&  Property::operator =

  ( const char*  value ) const

{
  props_.set ( name_, value );

  return *this;
}


inline const Property&  Property::operator =

  ( const String&  value ) const

{
  props_.set ( name_, value );

  return *this;
}


inline const Property&  Property::operator =

  ( const Properties&  values ) const

{
  props_.set ( name_, values );

  return *this;
}


template <class T>

  inline const Property&  Property::operator =

  ( const Array<T>&  values ) const

{
  props_.set ( name_, values );

  return *this;
}


template <class T>

  inline const Property&  Property::operator =

  ( const Ref<T>&  value ) const

{
  props_.set ( name_, value );

  return *this;
}


template <class T>

  inline const Property&  Property::operator =

  ( T*  value ) const

{
  props_.set ( name_, value );

  return *this;
}


//-----------------------------------------------------------------------
//   cast operators
//-----------------------------------------------------------------------


inline Property::operator bool () const
{
  bool  value;

  props_.get ( value, name_ );

  return value;
}


inline Property::operator char () const
{
  char  value;

  props_.get ( value, name_ );

  return value;
}


inline Property::operator byte () const
{
  byte  value;

  props_.get ( value, name_ );

  return value;
}


inline Property::operator short () const
{
  short  value;

  props_.get ( value, name_ );

  return value;
}


inline Property::operator int () const
{
  int  value;

  props_.get ( value, name_ );

  return value;
}


inline Property::operator lint () const
{
  lint  value;

  props_.get ( value, name_ );

  return value;
}


inline Property::operator float () const
{
  float  value;

  props_.get ( value, name_ );

  return value;
}


inline Property::operator double () const
{
  double  value;

  props_.get ( value, name_ );

  return value;
}


inline Property::operator String () const
{
  String  value;

  props_.get ( value, name_ );

  return value;
}


inline Property::operator Properties () const
{
  return props_.getProps ( name_ );
}


template <class T>

  inline Property::operator Array<T> () const

{
  Array<T>  values;

  props_.get ( values, name_ );

  return values;
}


template <class T>

  inline Property::operator Ref<T> () const

{
  Ref<T>  value;

  props_.get ( value, name_ );

  return value;
}


//=======================================================================
//   class Properties
//=======================================================================

//-----------------------------------------------------------------------
//   subscript operator
//-----------------------------------------------------------------------


inline Property  Properties::operator []

  ( const String&  name ) const

{
  return Property ( name, *this );
}


//-----------------------------------------------------------------------
//   getName
//-----------------------------------------------------------------------


inline String Properties::getName () const
{
  return myName_;
}


//-----------------------------------------------------------------------
//   getConverter
//-----------------------------------------------------------------------


inline ObjConverter* Properties::getConverter () const
{
  return conv_.get ();
}


//-----------------------------------------------------------------------
//   setConverter
//-----------------------------------------------------------------------


inline void Properties::setConverter ( Ref<ObjConverter> conv )
{
  conv_ = conv;
}


//-----------------------------------------------------------------------
//   set
//-----------------------------------------------------------------------


template <class T>

  inline void Properties::set

  ( const String&  name,
    T*             value ) const

{
  set ( name, Ref<Object>( value ) );
}


//-----------------------------------------------------------------------
//   find
//-----------------------------------------------------------------------


template <class T>

  bool Properties::find

  ( Ref<T>&        value,
    const String&  name ) const

{
  Ref<Object>  obj;

  if ( ! find( obj, name ) )
  {
    return false;
  }

  T*  tmp = dynamicCast<T*> ( obj );

  if ( ! tmp )
  {
    typeError_ ( name, obj, T::getType() );
  }

  value = tmp;

  return true;
}


//-----------------------------------------------------------------------
//   get
//-----------------------------------------------------------------------


template <class T>

  void Properties::get

  ( T&             value,
    const String&  name ) const

{
  if ( ! find( value, name ) )
  {
    findError_ ( name );
  }
}


template <class T>

  void Properties::get

  ( T&             value,
    const String&  name,
    T              lbound,
    T              ubound ) const

{
  if ( ! find( value, name, lbound, ubound ) )
  {
    findError_ ( name );
  }
}


template <class T>

  void Properties::get

  ( Array<T>&      values,
    const String&  name,
    T              lbound,
    T              ubound ) const

{
  if ( ! find( values, name, lbound, ubound ) )
  {
    findError_ ( name );
  }
}


//=======================================================================
//   related functions
//=======================================================================

//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


inline void               swap

  ( Properties&             lhs,
    Properties&             rhs )

{
  lhs.swap ( rhs );
}


//-----------------------------------------------------------------------
//   mergeNoReplace
//-----------------------------------------------------------------------


inline void               mergeNoReplace

  ( const Properties&       lhs,
    const Properties&       rhs )

{
  lhs.mergeWith ( rhs );
}


//-----------------------------------------------------------------------
//   mergeAndReplace
//-----------------------------------------------------------------------


inline void               mergeAndReplace

  ( const Properties&       lhs,
    const Properties&       rhs )

{
  lhs.mergeWith ( rhs, Properties::MERGE_REPLACE );
}


//-----------------------------------------------------------------------
//   print
//-----------------------------------------------------------------------


inline void               print

  ( io::PrintWriter&        out,
    const Properties&       props )

{
  props.printTo ( out );
}


//-----------------------------------------------------------------------
//   setBool
//-----------------------------------------------------------------------


inline void               setBool

  ( const Properties&       props,
    const String&           name,
    int                     bits,
    int                     mask )

{
  props.set ( name, ((bits & mask) != 0) );
}



JEM_END_PACKAGE( util )

#endif
