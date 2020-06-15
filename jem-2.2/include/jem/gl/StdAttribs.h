
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

#ifndef JEM_GL_STDATTRIBS_H
#define JEM_GL_STDATTRIBS_H

#include <jem/base/Clonable.h>
#include <jem/gl/Color.h>
#include <jem/gl/Attributes.h>


JEM_BEGIN_PACKAGE( gl )


//-----------------------------------------------------------------------
//   class StdAttribs
//-----------------------------------------------------------------------


class StdAttribs : public Attributes, public Clonable
{
 public:

  JEM_DECLARE_CLASS       ( StdAttribs, Attributes );


  static const int          POINT_SIZE    = 1 << 0;
  static const int          LINE_WIDTH    = 1 << 1;
  static const int          SHADE_MODEL   = 1 << 2;
  static const int          COLOR         = 1 << 3;
  static const int          SPECULAR      = 1 << 4;
  static const int          SHININESS     = 1 << 5;

  static const int          LAST_FIELD    = SHININESS;

  static const char*        POINT_SIZE_NAME;
  static const char*        LINE_WIDTH_NAME;
  static const char*        SHADE_MODEL_NAME;
  static const char*        COLOR_NAME;
  static const char*        MATERIAL_NAME;
  static const char*        SPECULAR_NAME;
  static const char*        SHININESS_NAME;


                            StdAttribs    ();

                            StdAttribs

    ( const Self&             rhs );


  virtual Ref<Object>       clone         () const;

  virtual int               copyFrom

    ( const Properties&       props );

  virtual void              copyTo

    ( const Properties&       props,
      int                     fields )       const;

  virtual void              listProps

    ( const Properties&       props )        const;

  virtual void              setDefaults   ();

  virtual void              setCurrent

    ( int                     fields )       const;

  virtual void              getCurrent

    ( int                     fields );

  inline GLfloat            getPointSize  () const;

  void                      setPointSize

    ( GLfloat                 size );

  inline GLfloat            getLineWidth  () const;

  void                      setLineWidth

    ( GLfloat                 width );

  inline GLenum             getShadeModel () const;

  void                      setShadeModel

    ( GLenum                  mode );

  inline Color              getColor      () const;

  void                      setColor

    ( const Color&            color );

  inline Color              getSpecular   () const;

  void                      setSpecular

    ( const Color&            color );

  inline GLfloat            getShininess  () const;

  void                      setShininess

    ( GLfloat                 shine );


 protected:

  virtual                  ~StdAttribs    ();


 protected:

  GLfloat                   pointSize_;
  GLfloat                   lineWidth_;
  GLenum                    shadeModel_;
  Color                     color_;
  Color                     specular_;
  GLfloat                   shininess_;

};





//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getPointSize
//-----------------------------------------------------------------------


inline GLfloat StdAttribs::getPointSize () const
{
  return pointSize_;
}


//-----------------------------------------------------------------------
//   getLineWidth
//-----------------------------------------------------------------------


inline GLfloat StdAttribs::getLineWidth () const
{
  return lineWidth_;
}


//-----------------------------------------------------------------------
//   getShadeModel
//-----------------------------------------------------------------------


inline GLenum StdAttribs::getShadeModel () const
{
  return shadeModel_;
}


//-----------------------------------------------------------------------
//   getColor
//-----------------------------------------------------------------------


inline Color StdAttribs::getColor () const
{
  return color_;
}


//-----------------------------------------------------------------------
//   getSpecular
//-----------------------------------------------------------------------


inline Color StdAttribs::getSpecular () const
{
  return specular_;
}


//-----------------------------------------------------------------------
//   getShininess
//-----------------------------------------------------------------------


inline GLfloat StdAttribs::getShininess () const
{
  return shininess_;
}


JEM_END_PACKAGE( gl )

#endif
