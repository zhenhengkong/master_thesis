
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

#ifndef JEM_GL_PALETTEPAINTER_H
#define JEM_GL_PALETTEPAINTER_H

#include <jem/gl/Rect.h>
#include <jem/gl/GfxNode.h>
#include <jem/gl/Alignment.h>


JEM_BEGIN_PACKAGE( gl )


class Palette;


//-----------------------------------------------------------------------
//   class PalettePainter
//-----------------------------------------------------------------------


class PalettePainter : public GfxNode
{
 public:

  JEM_DECLARE_CLASS       ( PalettePainter, GfxNode );


                            PalettePainter

    ( const String&           name,
      const Ref<Palette>&     pal,
      const Ref<GfxNode>&     buddy = NIL );

  virtual bool              takeAction

    ( const String&           action,
      const Properties&       params );

  virtual bool              takeGLAction

    ( GfxContext&             gtx,
      int                     action,
      const Properties&       params );

  virtual void              getVisibility

    ( bool&                   viz,
      Box&                    vbox,
      const Transform&        proj )           const;

  void                      setPalette

    ( const Ref<Palette>&     pal );

  inline Palette*           getPalette      () const;

  void                      setScaleAndBias

    ( double                  scale,
      double                  bias );

  inline double             getScale        () const;
  inline double             getBias         () const;

  void                      setAlignments

    ( Alignment               hAlign,
      Alignment               vAlign );

  inline Alignment          getHAlignment   () const;
  inline Alignment          getVAlignment   () const;


 public:

  bool                      hidden;
  String                    title;


 protected:

  virtual                  ~PalettePainter  ();


 private:

  void                      findPeers_      ();
  void                      initMyHash_     ();
  bool                      isVisible_      () const;

  void                      drawNoBuddy_

    ( GfxContext&             gtx );

  void                      drawWithBuddy_

    ( GfxContext&             gtx );

  bool                      initDrawArea_

    ( Rect&                   rect );


 private:

  static const char*        PCOUNT_;
  static const char*        GET_PCOUNT_;
  static const char*        SET_PCOUNT_;

  static const GLsizei      MARGIN_;

  Ref<Palette>              palette_;
  Ref<GfxNode>              buddy_;
  double                    scale_;
  double                    bias_;
  Alignment                 hAlign_;
  Alignment                 vAlign_;
  String                    myHash_;
  Rect                      vport_;
  int                       pcount_;
  bool                      hide_;

};



//#######################################################################
//   Implementation
//#######################################################################

//-----------------------------------------------------------------------
//   getPalette
//-----------------------------------------------------------------------


inline Palette* PalettePainter::getPalette () const
{
  return palette_.get ();
}


//-----------------------------------------------------------------------
//   getScale
//-----------------------------------------------------------------------


inline double PalettePainter::getScale () const
{
  return scale_;
}


//-----------------------------------------------------------------------
//   getBias
//-----------------------------------------------------------------------


inline double PalettePainter::getBias () const
{
  return bias_;
}


//-----------------------------------------------------------------------
//   getHAlignment
//-----------------------------------------------------------------------


inline Alignment PalettePainter::getHAlignment () const
{
  return hAlign_;
}


//-----------------------------------------------------------------------
//   getVAlignment
//-----------------------------------------------------------------------


inline Alignment PalettePainter::getVAlignment () const
{
  return vAlign_;
}


JEM_END_PACKAGE( gl )

#endif
