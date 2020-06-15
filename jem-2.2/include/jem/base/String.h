
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

#ifndef JEM_BASE_STRING_H
#define JEM_BASE_STRING_H

#include <jem/base/assert.h>
#include <jem/base/Slice.h>


JEM_BEGIN_PACKAGE_BASE


//-----------------------------------------------------------------------
//   forward declarations
//-----------------------------------------------------------------------


class Nil;
class String;
class StringBuffer;

namespace io
{
  class TextInput;
  class DataInput;
  class DataOutput;
}


//-----------------------------------------------------------------------
//   class TinyString
//-----------------------------------------------------------------------


class TinyString
{
 public:

  static const int      MAXLEN = sizeof(void*);


  explicit inline       TinyString

    ( void*               str );


 public:

  union
  {
    void* const         data;
    char                chars[MAXLEN];
  };

};


//-----------------------------------------------------------------------
//   class String
//-----------------------------------------------------------------------


class String
{
 public:

  friend class          StringBuffer;

  typedef char          ValueType;
  typedef const char*   Iterator;
  typedef const char*   ConstIterator;


                        String      ();

  explicit              String

    ( bool                value );

  explicit              String

    ( char                value );

  explicit              String

    ( byte                value );

  explicit              String

    ( short               value );

  explicit              String

    ( int                 value );

  explicit              String

    ( lint                value );

  explicit              String

    ( float               value );

  explicit              String

    ( double              value );

                        String

    ( const char*         str );

                        String

    ( const char*         str,
      idx_t               len );

                        String

  ( const TinyString&     str );

                        String

    ( char*               first,
      char*               last );

                        String

    ( const char*         first,
      const char*         last );

  template <class InputIterator>

                        String

    ( InputIterator       first,
      InputIterator       last );

                        String

    ( const String&       lhs,
      const String&       rhs );

                        String

    ( const String&       rhs );

                       ~String      ();

  void                  readFrom

    ( io::DataInput&      in );

  void                  writeTo

    ( io::DataOutput&     out )        const;

  void                  readFrom

    ( io::TextInput&      in );

  String&               operator =

    ( const String&       rhs );

  String&               operator =

    ( const char*         rhs );

  void                  swap

    ( String&             rhs );

  inline Iterator       begin       () const;
  inline Iterator       end         () const;

  inline char           front       () const;
  inline char           back        () const;

  template <class I>
    inline char         operator    []

    ( I                   i )          const;

  String                operator    []

    ( const Slice&        s )          const;

  inline String         operator    []

    ( const SliceFromTo&  s )          const;

  inline String         operator    []

    ( const SliceTo&      s )          const;

  inline String         operator    []

    ( const SliceFrom&    s )          const;

  inline String         operator    []

    ( const SliceAll&     s )           const;

  inline idx_t          size         () const;

  inline const char*    addr

    ( idx_t               i = 0 )       const;

  bool                  hasNull      () const;
  lint                  hashValue    () const;
  String                toUpper      () const;
  String                toLower      () const;
  String                stripWhite   () const;

  bool                  equals

    ( const String&       rhs )        const;

  bool                  equals

    ( const char*         rhs )        const;

  bool                  equalsIgnoreCase

    ( const String&       rhs )        const;

  int                   compareTo

    ( const String&       rhs )        const;

  int                   compareToIgnoreCase

    ( const String&       rhs )        const;

  bool                  startsWith

    ( const String&       word )       const;

  inline bool           startsWith

    ( char                ch )         const;

  bool                  endsWith

    ( const String&       word )       const;

  inline bool           endsWith

    ( char                ch )         const;

  idx_t                 find

    ( const String&       needle,
      idx_t               index = 0 )  const;

  idx_t                 find

    ( char                needle,
      idx_t               index = 0 )  const;

  idx_t                 rfind

    ( const String&       needle )     const;

  idx_t                 rfind

    ( const String&       needle,
      idx_t               fromIndex )  const;

  idx_t                 rfind

    ( char                needle )     const;

  idx_t                 rfind

    ( char                needle,
      idx_t               fromIndex )  const;

  static idx_t          sizeOf

    ( const char*         str );

  static inline String  format

    ( const char*         fmt );

  template <class A>
    static String       format

    ( const char*         fmt,
      const A&            arg0 );

  template <class A,
            class B>
    static String       format

    ( const char*         fmt,
      const A&            arg0,
      const B&            arg1 );

  template <class A,
            class B,
            class C>
    static String       format

    ( const char*         fmt,
      const A&            arg0,
      const B&            arg1,
      const C&            arg2 );

  template <class A,
            class B,
            class C,
            class D>
    static String       format

    ( const char*         fmt,
      const A&            arg0,
      const B&            arg1,
      const C&            arg2,
      const D&            arg3 );

  template <class A,
            class B,
            class C,
            class D,
            class E>
    static String       format

    ( const char*         fmt,
      const A&            arg0,
      const B&            arg1,
      const C&            arg2,
      const D&            arg3,
      const E&            arg4 );

  template <class A,
            class B,
            class C,
            class D,
            class E,
            class F>
    static String       format

    ( const char*         fmt,
      const A&            arg0,
      const B&            arg1,
      const C&            arg2,
      const D&            arg3,
      const E&            arg4,
      const F&            arg5 );

  template <class A,
            class B,
            class C,
            class D,
            class E,
            class F,
            class G>
    static String       format

    ( const char*         fmt,
      const A&            arg0,
      const B&            arg1,
      const C&            arg2,
      const D&            arg3,
      const E&            arg4,
      const F&            arg5,
      const G&            arg6 );


 private:

  class                 Argument_;


                        String

    ( const Nil&          nil,
      idx_t               len );

                        String

    ( const String&       rhs,
      idx_t               first,
      idx_t               last );

                        String

    ( const String&       rhs,
      idx_t               first,
      idx_t               last,
      idx_t               stride );

  char*                 init_

    ( idx_t               size );

  void                  free_       ();

  void                  realloc_

    ( idx_t               len );

  static String         format_

    ( const char*         fmt,
      const Argument_*    args,
      int                 argc );


 private:

  class                 Buffer_;
  class                 Chunk_;
  class                 Utils_;

  friend class          Chunk_;
  friend class          Utils_;

  static const int      BUFSIZE_ = 128;

  idx_t                 size_;

  union
  {
    char*               str_;
    const char*         cstr_;
  };

  union
  {
    Chunk_*             chunk_;
    char                buffer_[TinyString::MAXLEN];
  };

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


String                  clone

  ( const char*           str );

template <class Input>

  inline void           decode

  ( Input&                in,
    String&               s );

template <class Output>

  inline void           encode

  ( Output&               out,
    const String&         s );

template <class Input>

  inline void           read

  ( Input&                in,
    String&               s );

template <class Output>

  inline void           print

  ( Output&               out,
    const String&         s );




//#######################################################################
//   Implementation
//#######################################################################

//=======================================================================
//   class TinyString
//=======================================================================

//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


inline TinyString::TinyString ( void* str ) :

  data ( str )

{}


//=======================================================================
//   class String::Argument_
//=======================================================================


class String::Argument_
{
 public:

  enum                  Type
  {
                          INT,
                          FLOAT,
                          CHARS,
                          STRING,
                          POINTER
  };

  inline                Argument_

    ( bool                arg );

  inline                Argument_

    ( char                arg );

  inline                Argument_

    ( byte                arg );

  inline                Argument_

    ( short               arg );

  inline                Argument_

    ( int                 arg );

  inline                Argument_

    ( lint                arg );

  inline                Argument_

    ( float               arg );

  inline                Argument_

    ( double              arg );

  inline                Argument_

    ( const char*         arg );

  inline                Argument_

    ( const String&       arg );

  inline                Argument_

    ( const void*         arg );


 public:

  Type                  type;

  union
  {
    lint                intValue;
    double              fltValue;
    const void*         ptrValue;
  };

};


//-----------------------------------------------------------------------
//   constructors
//-----------------------------------------------------------------------


inline String::Argument_::Argument_  ( bool arg )
{
  type     = INT;
  intValue = (idx_t) arg;
}


inline String::Argument_::Argument_ ( char arg )
{
  type     = INT;
  intValue = arg;
}


inline String::Argument_::Argument_ ( byte arg )
{
  type     = INT;
  intValue = arg;
}


inline String::Argument_::Argument_ ( short arg )
{
  type     = INT;
  intValue = arg;
}


inline String::Argument_::Argument_ ( int arg )
{
  type     = INT;
  intValue = arg;
}


inline String::Argument_::Argument_ ( lint arg )
{
  type     = INT;
  intValue = arg;
}


inline String::Argument_::Argument_ ( float arg )
{
  type     = FLOAT;
  fltValue = arg;
}


inline String::Argument_::Argument_ ( double arg )
{
  type     = FLOAT;
  fltValue = arg;
}


inline String::Argument_::Argument_ ( const char* arg )
{
  type     = CHARS;
  ptrValue = arg;
}


inline String::Argument_::Argument_ ( const String& arg )
{
  type     = STRING;
  ptrValue = &arg;
}


inline String::Argument_::Argument_ ( const void* arg )
{
  type     = POINTER;
  ptrValue = arg;
}


//=======================================================================
//   class String
//=======================================================================

//-----------------------------------------------------------------------
//   iterator constructor
//-----------------------------------------------------------------------


template <class InputIterator>

  String::String

  ( InputIterator  first,
    InputIterator  last )

{
  InputIterator  i = first;
  char*          dest;
  idx_t          n;

  for ( n = 0; i != last; ++i )
  {
    n++;
  }

  dest = init_ ( n );

  try
  {
    for ( ; first != last; ++first, dest++ )
    {
      *dest = *first;
    }
  }
  catch ( ... )
  {
    free_ ();
    throw;
  }
}


//-----------------------------------------------------------------------
//   begin & end
//-----------------------------------------------------------------------


inline String::Iterator String::begin () const
{
  return cstr_;
}


inline String::Iterator String::end () const
{
  return (cstr_ + size_);
}


//-----------------------------------------------------------------------
//   front & back
//-----------------------------------------------------------------------


inline char String::front () const
{
  JEM_ASSERT_NOTHROW ( size() > 0 );

  return str_[0];
}


inline char String::back () const
{
  JEM_ASSERT_NOTHROW ( size() > 0 );

  return str_[size_ - 1];
}


//-----------------------------------------------------------------------
//   subscript operator
//-----------------------------------------------------------------------


template <class I>

  inline char String::operator [] ( I i ) const

{
  JEM_ASSERT_NOTHROW ( checkIndex( i, size() ) );

  return cstr_[i];
}


//-----------------------------------------------------------------------
//   slice operators
//-----------------------------------------------------------------------


inline String String::operator [] ( const SliceFromTo& s ) const
{
  JEM_ASSERT_NOTHROW ( s.first() <= s.last() && s.last() <= size() );

  return String ( *this, s.first(), s.last() );
}


inline String String::operator [] ( const SliceTo& s ) const
{
  return String ( *this, 0, s.last() );
}


inline String String::operator [] ( const SliceFrom& s ) const
{
  JEM_ASSERT_NOTHROW ( s.first() <= size() );

  return String ( *this, s.first(), size_ );
}


inline String String::operator [] ( const SliceAll& ) const
{
  return *this;
}


//-----------------------------------------------------------------------
//   startsWith
//-----------------------------------------------------------------------


inline bool String::startsWith ( char ch ) const
{
  return (size_ > 0 && cstr_[0] == ch);
}


//-----------------------------------------------------------------------
//   endsWith
//-----------------------------------------------------------------------


inline bool String::endsWith ( char ch ) const
{
  return (size_ > 0 && cstr_[size_ - 1] == ch);
}


//-----------------------------------------------------------------------
//   size
//-----------------------------------------------------------------------


inline idx_t String::size () const
{
  return size_;
}


//-----------------------------------------------------------------------
//   addr
//-----------------------------------------------------------------------


inline const char* String::addr ( idx_t i ) const
{
  return (cstr_ + i);
}


//-----------------------------------------------------------------------
//   format
//-----------------------------------------------------------------------


inline String String::format ( const char* fmt )
{
  return format_ ( fmt, 0, 0 );
}


template <class A>

  String String::format

  ( const char*  fmt,
    const A&     arg0 )

{
  Argument_  args[1] = { arg0 };

  return format_ ( fmt, args, 1 );
}


template <class A,
          class B>

  String String::format

  ( const char*  fmt,
    const A&     arg0,
    const B&     arg1 )

{
  Argument_  args[2] = { arg0, arg1 };

  return format_ ( fmt, args, 2 );
}


template <class A,
          class B,
          class C>

  String String::format

  ( const char*  fmt,
    const A&     arg0,
    const B&     arg1,
    const C&     arg2 )

{
  Argument_  args[3] = { arg0, arg1, arg2 };

  return format_ ( fmt, args, 3 );
}


template <class A,
          class B,
          class C,
          class D>

  String String::format

  ( const char*  fmt,
    const A&     arg0,
    const B&     arg1,
    const C&     arg2,
    const D&     arg3 )

{
  Argument_  args[4] = { arg0, arg1, arg2, arg3 };

  return format_ ( fmt, args, 4 );
}


template <class A,
          class B,
          class C,
          class D,
          class E>

  String String::format

  ( const char*  fmt,
    const A&     arg0,
    const B&     arg1,
    const C&     arg2,
    const D&     arg3,
    const E&     arg4 )

{
  Argument_  args[5] = { arg0, arg1, arg2, arg3, arg4 };

  return format_ ( fmt, args, 5 );
}


template <class A,
          class B,
          class C,
          class D,
          class E,
          class F>

  String String::format

  ( const char*  fmt,
    const A&     arg0,
    const B&     arg1,
    const C&     arg2,
    const D&     arg3,
    const E&     arg4,
    const F&     arg5 )

{
  Argument_  args[6] = { arg0, arg1, arg2, arg3, arg4, arg5 };

  return format_ ( fmt, args, 6 );
}


template <class A,
          class B,
          class C,
          class D,
          class E,
          class F,
          class G>

  String String::format

  ( const char*  fmt,
    const A&     arg0,
    const B&     arg1,
    const C&     arg2,
    const D&     arg3,
    const E&     arg4,
    const F&     arg5,
    const G&     arg6 )

{
  Argument_  args[7] = { arg0, arg1, arg2, arg3, arg4, arg5, arg6 };

  return format_ ( fmt, args, 7 );
}

//=======================================================================
//   Related functions
//=======================================================================

//-----------------------------------------------------------------------
//   swap
//-----------------------------------------------------------------------


inline void       swap

  ( String&         lhs,
    String&         rhs )

{
  lhs.swap ( rhs );
}


//-----------------------------------------------------------------------
//   operator +
//-----------------------------------------------------------------------


inline String     operator +

  ( const String&   lhs,
    const String&   rhs )

{
  return String ( lhs, rhs );
}


inline String     operator +

  ( const String&   lhs,
    const char*     rhs )

{
  return String ( lhs, String( rhs ) );
}


inline String     operator +

  ( const char*     lhs,
    const String&   rhs )

{
  return String ( String( lhs ), rhs );
}


inline String     operator +

  ( const String&   lhs,
    char            rhs )

{
  return String ( lhs, String( rhs ) );
}


inline String     operator +

  ( char            lhs,
    const String&   rhs )

{
  return String ( String( lhs ), rhs );
}


//-----------------------------------------------------------------------
//   operator ==
//-----------------------------------------------------------------------


inline bool       operator ==

  ( const String&   lhs,
    const String&   rhs )

{
  return (lhs.size() == rhs.size() && lhs.equals( rhs ));
}


inline bool       operator ==

  ( const String&   lhs,
    const char*     rhs )

{
  return lhs.equals ( rhs );
}


inline bool       operator ==

  ( const char*     lhs,
    const String&   rhs )

{
  return rhs.equals ( lhs );
}


//-----------------------------------------------------------------------
//   operator !=
//-----------------------------------------------------------------------


inline bool       operator !=

  ( const String&   lhs,
    const String&   rhs )

{
  return ! lhs.equals ( rhs );
}


inline bool       operator !=

  ( const String&   lhs,
    const char*     rhs )

{
  return ! lhs.equals ( rhs );
}


inline bool       operator !=

  ( const char*     lhs,
    const String&   rhs )

{
  return ! rhs.equals ( lhs );
}


//-----------------------------------------------------------------------
//   operator <
//-----------------------------------------------------------------------


inline bool       operator <

  ( const String&   lhs,
    const String&   rhs )

{
  return (lhs.compareTo( rhs ) < 0);
}


inline bool       operator <

  ( const String&   lhs,
    const char*     rhs )

{
  return (lhs.compareTo( rhs ) < 0);
}


inline bool       operator <

  ( const char*     lhs,
    const String&   rhs )

{
  return (rhs.compareTo( lhs ) > 0);
}


//-----------------------------------------------------------------------
//   operator <=
//-----------------------------------------------------------------------


inline bool       operator <=

  ( const String&   lhs,
    const String&   rhs )

{
  return (lhs.compareTo( rhs ) <= 0);
}


inline bool       operator <=

  ( const String&   lhs,
    const char*     rhs )

{
  return (lhs.compareTo( rhs ) <= 0);
}


inline bool       operator <=

  ( const char*     lhs,
    const String&   rhs )

{
  return (rhs.compareTo( lhs ) >= 0);
}


//-----------------------------------------------------------------------
//   operator >
//-----------------------------------------------------------------------


inline bool       operator >

  ( const String&   lhs,
    const String&   rhs )

{
  return (lhs.compareTo( rhs ) > 0);
}


inline bool       operator >

  ( const String&   lhs,
    const char*     rhs )

{
  return (lhs.compareTo( rhs ) > 0);
}


inline bool       operator >

  ( const char*     lhs,
    const String&   rhs )

{
  return (rhs.compareTo( lhs ) < 0);
}


//-----------------------------------------------------------------------
//   operator >=
//-----------------------------------------------------------------------


inline bool       operator >=

  ( const String&   lhs,
    const String&   rhs )

{
  return (lhs.compareTo( rhs ) >= 0);
}


inline bool       operator >=

  ( const String&   lhs,
    const char*     rhs )

{
  return (lhs.compareTo( rhs ) >= 0);
}


inline bool       operator >=

  ( const char*     lhs,
    const String&   rhs )

{
  return (rhs.compareTo( lhs ) <= 0);
}


//-----------------------------------------------------------------------
//   decode
//-----------------------------------------------------------------------


template <class Input>

  inline void     decode

  ( Input&          in,
    String&         s )

{
  s.readFrom ( in );
}


//-----------------------------------------------------------------------
//   encode
//-----------------------------------------------------------------------


template <class Output>

  inline void     encode

  ( Output&         out,
    const String&   s )

{
  s.writeTo ( out );
}


//-----------------------------------------------------------------------
//   read
//-----------------------------------------------------------------------


template <class Input>

  inline void     read

  ( Input&          in,
    String&         s )

{
  s.readFrom ( in );
}


//-----------------------------------------------------------------------
//   print
//-----------------------------------------------------------------------


template <class Output>

  inline void     print

  ( Output&         out,
    const String&   s )

{
  out.write ( s.addr(), s.size() );
}


JEM_END_PACKAGE_BASE

#endif
