
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

#ifndef JIVE_UTIL_OBJECTCONVERTER_H
#define JIVE_UTIL_OBJECTCONVERTER_H

#include <jem/util/Properties.h>
#include <jem/util/ObjectConverter.h>
#include <jive/Array.h>
#include <jive/util/import.h>


JIVE_BEGIN_PACKAGE( util )


//-----------------------------------------------------------------------
//   class ObjectConverter
//-----------------------------------------------------------------------


class ObjectConverter : public jem::util::ObjConverter
{
 public:

  JEM_DECLARE_CLASS     ( ObjectConverter,
                          jem::util::ObjConverter );


  explicit                ObjectConverter

    ( const Properties&     globdat );

  virtual void            convert

    ( bool&                 out,
      const Ref<Object>&    obj )              const;

  virtual void            convert

    ( char&                 out,
      const Ref<Object>&    obj )              const;

  virtual void            convert

    ( byte&                 out,
      const Ref<Object>&    obj )              const;

  virtual void            convert

    ( short&                out,
      const Ref<Object>&    obj )              const;

  virtual void            convert

    ( int&                  out,
      const Ref<Object>&    obj )              const;

  virtual void            convert

    ( lint&                 out,
      const Ref<Object>&    obj )              const;

  virtual void            convert

    ( float&                out,
      const Ref<Object>&    obj )              const;

  virtual void            convert

    ( double&               out,
      const Ref<Object>&    obj )              const;

  virtual void            convert

    ( String&               out,
      const Ref<Object>&    obj )              const;

  virtual void            convert

    ( BoolVector&           out,
      const Ref<Object>&    obj )              const;

  virtual void            convert

    ( IntVector&            out,
      const Ref<Object>&    obj )              const;

  virtual void            convert

    ( Vector&               out,
      const Ref<Object>&    obj )              const;

  virtual void            convert

    ( StringVector&         out,
      const Ref<Object>&    obj )              const;


 protected:

  virtual                ~ObjectConverter   ();


 private:

  bool                    toBool_

    ( const String&         str )              const;

  lint                    toInt_

    ( const String&         str,
      lint                  minval,
      lint                  maxval )           const;

  double                  toFloat_

    ( const String&         expr,
      double                maxval )           const;

  static void             typeError_

    ( const String&         expected,
      const Ref<Object>&    obj );


 private:

  Properties              globdat_;

};


//-----------------------------------------------------------------------
//   related types
//-----------------------------------------------------------------------


typedef ObjectConverter   ObjConverter;


JIVE_END_PACKAGE( util )

#endif
