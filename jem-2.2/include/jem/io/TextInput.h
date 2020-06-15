
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

#ifndef JEM_IO_TEXTINPUT_H
#define JEM_IO_TEXTINPUT_H

#include <jem/base/Interface.h>


JEM_BEGIN_PACKAGE( io )


//-----------------------------------------------------------------------
//   class TextInput
//-----------------------------------------------------------------------


class TextInput : public Interface
{
 public:

  typedef TextInput       Self;


  virtual int             read          () = 0;

  virtual idx_t           read

    ( char*                 buf,
      idx_t                 n )            = 0;

  virtual void            pushBack

    ( int                   c )            = 0;

  virtual void            skipWhite     () = 0;
  virtual bool            parseBool     () = 0;
  virtual char            parseChar     () = 0;
  virtual byte            parseByte     () = 0;
  virtual int             parseInt      () = 0;
  virtual lint            parseLong     () = 0;
  virtual float           parseFloat    () = 0;
  virtual double          parseDouble   () = 0;

  template <class T>
    inline Self&          operator >>

    ( T&                    obj );


 protected:

  virtual                ~TextInput     ();

};


//-----------------------------------------------------------------------
//   related classes
//-----------------------------------------------------------------------


struct                    SkipWhite     {};

extern const SkipWhite    skipWhite;


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


template <class Input>

  inline void             read

  ( Input&                  in,
    bool&                   b );

template <class Input>

  inline void             read

  ( Input&                  in,
    char&                   c );

template <class Input>

  inline void             read

  ( Input&                  in,
    byte&                   b );

template <class Input>

  inline void             read

  ( Input&                  in,
    short&                  i );

template <class Input>

  inline void             read

  ( Input&                  in,
    int&                    i );

template <class Input>

  inline void             read

  ( Input&                  in,
    lint&                   i );

template <class Input>

  inline void             read

  ( Input&                  in,
    float&                  x );

template <class Input>

  inline void             read

  ( Input&                  in,
    double&                 x );

template <class Input>

  inline void             read

  ( Input&                  in,
    const SkipWhite&        sw );

template <class Input,
          class A, class B>

  inline void             read

  ( Input&                  in,
    A&                      a,
    B&                      b );

template <class Input,
          class A, class B, class C>

  inline void             read

  ( Input&                  in,
    A&                      a,
    B&                      b,
    C&                      c );

template <class Input,
          class A, class B, class C,
          class D>

  inline void             read

  ( Input&                  in,
    A&                      a,
    B&                      b,
    C&                      c,
    D&                      d );

template <class Input,
          class A, class B, class C,
          class D, class E>

  inline void             read

  ( Input&                  in,
    A&                      a,
    B&                      b,
    C&                      c,
    D&                      d,
    E&                      e );

template <class Input,
          class A, class B, class C,
          class D, class E, class F>

  inline void             read

  ( Input&                  in,
    A&                      a,
    B&                      b,
    C&                      c,
    D&                      d,
    E&                      e,
    F&                      f );




//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   operator >>
//-----------------------------------------------------------------------


template <class T>

  inline void           readFromTextInput

  ( TextInput&            in,
    T&                    obj )

{
  read ( in, obj );
}


template <class T>

  inline TextInput&   TextInput::operator >>

    ( T&                obj )

{
  // The non-member function read() cannot be called directly because it
  // is hidden by the member function read().

  jem::io::readFromTextInput ( *this, obj );
  return *this;
}


//-----------------------------------------------------------------------
//   read
//-----------------------------------------------------------------------


template <class Input>

  inline void         read

  ( Input&              in,
    bool&               b )

{
  b = in.parseBool ();
}


template <class Input>

  inline void         read

  ( Input&              in,
    char&               c )

{
  c = in.parseChar ();
}


template <class Input>

  inline void         read

  ( Input&              in,
    byte&               b )

{
  b = in.parseByte ();
}


template <class Input>

  inline void         read

  ( Input&              in,
    short&              i )

{
  i = in.parseShort ();
}


template <class Input>

  inline void         read

  ( Input&              in,
    int&                i )

{
  i = in.parseInt ();
}


template <class Input>

  inline void         read

  ( Input&              in,
    lint&               i )

{
  i = in.parseLong ();
}


template <class Input>

  inline void         read

  ( Input&              in,
    float&              x )

{
  x = in.parseFloat ();
}


template <class Input>

  inline void         read

  ( Input&              in,
    double&             x )

{
  x = in.parseDouble ();
}


template <class Input>

  inline void         read

  ( Input&              in,
    const SkipWhite&    ws )

{
  in.skipWhite ();
}


template <class Input,
          class A, class B>

  inline void         read

  ( Input&              in,
    A&                  a,
    B&                  b )

{
  read ( in, a );
  read ( in, b );
}


template <class Input,
          class A, class B, class C>

  inline void         read

  ( Input&              in,
    A&                  a,
    B&                  b,
    C&                  c )

{
  read ( in, a );
  read ( in, b );
  read ( in, c );
}


template <class Input,
          class A, class B, class C,
          class D>

  inline void         read

  ( Input&              in,
    A&                  a,
    B&                  b,
    C&                  c,
    D&                  d )

{
  read ( in, a );
  read ( in, b );
  read ( in, c );
  read ( in, d );
}


template <class Input,
          class A, class B, class C,
          class D, class E>

  inline void         read

  ( Input&              in,
    A&                  a,
    B&                  b,
    C&                  c,
    D&                  d,
    E&                  e )

{
  read ( in, a );
  read ( in, b );
  read ( in, c );
  read ( in, d );
  read ( in, e );
}


template <class Input,
          class A, class B, class C,
          class D, class E, class F>

  inline void         read

  ( Input&              in,
    A&                  a,
    B&                  b,
    C&                  c,
    D&                  d,
    E&                  e,
    F&                  f )

{
  read ( in, a );
  read ( in, b );
  read ( in, c );
  read ( in, d );
  read ( in, e );
  read ( in, f );
}


JEM_END_PACKAGE( io )

#endif
