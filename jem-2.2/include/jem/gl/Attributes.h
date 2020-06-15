
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

#ifndef JEM_GL_ATTRIBUTES_H
#define JEM_GL_ATTRIBUTES_H

#include <jem/base/Object.h>
#include <jem/util/event/Event.h>
#include <jem/gl/import.h>


JEM_BEGIN_PACKAGE( gl )


class GfxNode;


//-----------------------------------------------------------------------
//   class Attributes
//-----------------------------------------------------------------------


class Attributes : public Object
{
 public:

  JEM_DECLARE_CLASS       ( Attributes, Object );

  static const int          ALL_FIELDS;

  util::Event
    < int, Self& >          newValuesEvent;


  virtual int               copyFrom

    ( const Properties&       props )              = 0;

  virtual void              copyTo

    ( const Properties&       props,
      int                     fields )       const = 0;

  virtual void              listProps

    ( const Properties&       props )        const = 0;

  virtual void              setDefaults   ()       = 0;

  virtual void              setCurrent

    ( int                     fields )       const = 0;

  virtual void              getCurrent

    ( int                     fields )             = 0;


 protected:

  virtual                  ~Attributes    ();

};


//-----------------------------------------------------------------------
//   related functions
//-----------------------------------------------------------------------


inline void                 setCurrent

  ( const Attributes&         attribs );

inline void                 setCurrent

  ( const Attributes&         attribs,
    int                       fields );

inline void                 getCurrent

  ( Attributes&               attribs );

inline void                 getCurrent

  ( Attributes&               attribs,
    int                       fields );

void                        getAttribsOf

  ( GfxNode*                  node,
    const Properties&         attribs );





//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   setCurrent
//-----------------------------------------------------------------------


inline void           setCurrent

  ( const Attributes&   attribs )

{
  attribs.setCurrent ( Attributes::ALL_FIELDS );
}


inline void           setCurrent

  ( const Attributes&   attribs,
    int                 fields )

{
  attribs.setCurrent ( fields );
}


//-----------------------------------------------------------------------
//   getCurrent
//-----------------------------------------------------------------------


inline void           getCurrent

  ( Attributes&         attribs )

{
  attribs.getCurrent ( Attributes::ALL_FIELDS );
}


inline void           getCurrent

  ( Attributes&         attribs,
    int                 fields )

{
  attribs.getCurrent ( fields );
}


JEM_END_PACKAGE( gl )

#endif
