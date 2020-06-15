
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


#include <jem/base/ClassTemplate.h>
#include <jem/util/Properties.h>
#include <jem/gl/input.h>
#include <jem/gl/utilities.h>
#include <jem/gl/Menu.h>
#include <jem/gl/Color.h>
#include <jem/gl/Actions.h>
#include <jem/gl/GfxContext.h>
#include <jem/gl/RedrawRoot.h>
#include <jem/gl/TextPainter.h>
#include <jem/gl/MenuNode.h>


JEM_DEFINE_CLASS( jem::gl::MenuNode );


JEM_BEGIN_PACKAGE( gl )


//=======================================================================
//   class MenuNode
//=======================================================================

//-----------------------------------------------------------------------
//   static data
//-----------------------------------------------------------------------


const char*  MenuNode::HIDE_       =  "_HIDE_MENU";
const idx_t  MenuNode::FOCUS_NONE_ = -1;


//-----------------------------------------------------------------------
//   constructors & destructor
//-----------------------------------------------------------------------


MenuNode::MenuNode

  ( const String&  name,
    const Menu&    menu ) :

  Super ( name )

{
  using jem::util::List;

  List<String>::ConstIterator  first = menu.entries_.begin ();
  List<String>::ConstIterator  last  = menu.entries_.end   ();

  items_.resize ( menu.entries_.size() );

  for ( idx_t i = 0; first != last; ++first, i++ )
  {
    items_[i].label = *first;
  }

  space_ = 0;
  focus_ = FOCUS_NONE_;
}


MenuNode::MenuNode

  ( const String&         name,
    const Array<String>&  list ) :

  Super ( name )

{
  const idx_t  n = list.size ();

  items_.resize ( n );

  for ( idx_t i = 0; i < n; i++ )
  {
    items_[i].label = list[i];
  }

  space_ = 0;
  focus_ = FOCUS_NONE_;
}


MenuNode::~MenuNode ()
{}


//-----------------------------------------------------------------------
//   takeAction
//-----------------------------------------------------------------------


bool MenuNode::takeAction

  ( const String&      action,
    const Properties&  params )

{
  if ( isActive() )
  {
    if ( action == HIDE_ || action == Actions::GIVE_FOCUS )
    {
      hide ();

      return true;
    }
    else
    {
      passAction ( child_, action, params );
    }
  }
  else
  {
    bool  retval = passAction ( child_, action, params );

    if ( action == Actions::SET_FONT )
    {
      font_  = Font::getDefault ();
      retval = true;
    }

    return retval;
  }

  return false;
}


//-----------------------------------------------------------------------
//   takeGLAction
//-----------------------------------------------------------------------


bool MenuNode::takeGLAction

  ( GfxContext&        gtx,
    int                action,
    const Properties&  params )

{
  typedef GLActionParams  Params;

  bool  result = true;

  switch ( action )
  {
  case GLActions::KEY_EVENT:

    {
      int  key;

      params.get ( key, Params::KEY_CODE );

      if      ( key == Keys::UP )
      {
        moveFocus_ ( gtx, 1 );
      }
      else if ( key == Keys::DOWN )
      {
        moveFocus_ ( gtx, -1 );
      }
      else if ( key == '\n' )
      {
        selectFocus_ ( gtx );
      }
      else
      {
        hide ( gtx );
      }
    }

    break;

  case GLActions::MOUSE_EVENT:

    {
      bool  down;

      params.get ( down, Params::MOUSE_DOWN );

      if ( down )
      {
        int  x, y;

        params.get ( x, Params::MOUSE_XPOS );
        params.get ( y, Params::MOUSE_YPOS );

        if ( ! rect_.contains( (GLint) x, (GLint) y ) )
        {
          hide ( gtx );
        }
      }
      else if ( focus_ != FOCUS_NONE_ )
      {
        selectFocus_ ( gtx );
      }
    }

    break;

  case GLActions::MOTION_EVENT:

    {
      int  x, y;

      params.get ( x, Params::MOUSE_XPOS );
      params.get ( y, Params::MOUSE_YPOS );

      setFocus_  ( gtx, (GLint) x, (GLint) y );
    }

    break;

  case GLActions::REDRAW:
  case GLActions::REDRAW_FAST:

    passGLAction ( child_, gtx, action, params );
    redraw_      ( gtx );

    break;

  case GLActions::INIT_VIEW:

    passGLAction ( child_, gtx, action, params );
    hide         ();

    break;

  default:

    result = passGLAction ( child_, gtx, action, params );
  }

  return result;
}


//-----------------------------------------------------------------------
//   setChild
//-----------------------------------------------------------------------


void MenuNode::setChild ( const Ref<GfxNode>& child )
{
  Super::setChild_ ( child );
}


//-----------------------------------------------------------------------
//   getChild
//-----------------------------------------------------------------------


Ref<GfxNode> MenuNode::getChild () const
{
  return child_;
}


//-----------------------------------------------------------------------
//   show
//-----------------------------------------------------------------------


void MenuNode::show

  ( GfxContext&          gtx,
    const Ref<GfxNode>&  node )

{
  Vertex2i  pos = gtx.getMousePos ();

  show ( gtx, node, pos[0], pos[1] );
}


void MenuNode::show

  ( GfxContext&          gtx,
    const Ref<GfxNode>&  node,
    int                  xPos,
    int                  yPos )

{
  if ( isActive() )
  {
    return;
  }

  // Go up the scene graph to find the highest pivotable node that is
  // located below or at the same level as the redraw root.

  GfxNode*    current = node.get ();
  Pivotable*  pivot   = 0;

  while ( current )
  {
    Pivotable*  tmpPivot = dynamic_cast<Pivotable*> ( current );

    if ( tmpPivot )
    {
      pivot = tmpPivot;
    }

    if ( dynamic_cast<RedrawRoot*>( current ) )
    {
      break;
    }

    current = current->getParent ();
  }

  if ( pivot )
  {
    show_ ( gtx, *pivot, xPos, yPos );
  }
}


//-----------------------------------------------------------------------
//   hide
//-----------------------------------------------------------------------


void MenuNode::hide ()
{
  Pivotable*  pivot = dynamic_cast<Pivotable*> ( getParent() );

  if ( pivot )
  {
    Ref<GfxNode>  child = child_;

    Super::setChild_ ( NIL );
    pivot->setChild  ( child );
  }
}


void MenuNode::hide ( GfxContext& gtx )
{
  if ( isActive() )
  {
    gtx.postRedraw ( this );
    hide           ();
  }
}


//-----------------------------------------------------------------------
//   show_
//-----------------------------------------------------------------------


void MenuNode::show_

  ( GfxContext&  gtx,
    Pivotable&   pivot,
    GLint        xPos,
    GLint        yPos )

{
  Ref<GfxNode>  child = pivot.getChild ();

  Properties    params;

  pivot .setChild    ( this );
  Super::setChild_   ( child );
  child->takeAction  ( Actions::GIVE_INPUT, params );
  child->takeAction  ( HIDE_, params );
  this ->init_       ( (GLint) xPos, (GLint) yPos );
  gtx.postFastRedraw ( this );
}


//-----------------------------------------------------------------------
//   init_
//-----------------------------------------------------------------------


void MenuNode::init_

  ( GLint  xPos,
    GLint  yPos )

{
  using jem::max;

  Rect     vport = getViewport ();

  GLsizei  fontHeight;
  GLsizei  w, h;
  GLint    x, y;


  font_  = PROPORTIONAL_MEDIUM_FONT;
  focus_ = FOCUS_NONE_;

  // Set the font size and determine the menu height.

  do
  {
    fontHeight = font_.getHeight ();
    space_     = fontHeight / 4;

    if ( space_ < 1 )
    {
      space_ = 1;
    }

    w = 0;
    h = space_;

    for ( idx_t i = 0; i < items_.size(); i++ )
    {
      const String&  label = items_[i].label;

      h += space_;

      if ( label.size() == 0 )
      {
        h += 2;
      }
      else
      {
        h += fontHeight;
        w  = max ( w, font_.getWidthOf( label ) );
      }

      h += space_;
    }

    h +=     space_;
    w += 4 * space_;

    if ( w <= vport.width && h <= vport.height )
    {
      break;
    }

    if ( font_.getSize() == SMALL_FONT )
    {
      break;
    }

    font_--;
  }
  while ( true );

  rect_.width  = w;
  rect_.height = h;

  // Determine the menu position.

  x = xPos;
  y = yPos;

  if      ( y - h >= vport.y )
  {
    y = y - h;
  }
  else if ( y + h > vport.y + vport.height )
  {
    y = vport.y;
  }

  if ( x + w > vport.x + vport.width )
  {
    if ( x - w >= vport.x )
    {
      x = x - w;
    }
    else
    {
      x = vport.x;
    }
  }

  rect_.x = x;
  rect_.y = y;

  // Initialize the menu item rectangles.

  x = x + space_;
  y = y + h - space_;
  w = w - 2 * space_;

  for ( idx_t i = 0; i < items_.size(); i++ )
  {
    Item_&  item = items_[i];

    h = 2 * space_;

    if ( item.label.size() == 0 )
    {
      h += 2;
    }
    else
    {
      h += fontHeight;
    }

    y = y - h;

    item.rect.x      = x;
    item.rect.y      = y;
    item.rect.width  = w;
    item.rect.height = h;
  }
}


//-----------------------------------------------------------------------
//   redraw_
//-----------------------------------------------------------------------


void MenuNode::redraw_ ( GfxContext& gtx )
{
  const GLfloat  MENU_COLOR [3] = { 0.800F, 0.800F, 0.800F };
  const GLfloat  FOCUS_COLOR[3] = { 0.438F, 0.500F, 0.563F };
  const GLfloat  LIGHT_COLOR[3] = { 0.900F, 0.900F, 0.900F };
  const GLfloat  DARK_COLOR [3] = { 0.500F, 0.500F, 0.500F };

  CanvasScope    canvasScope;

  TextPainter    txp ( font_, LEFT_ALIGNED, BOT_ALIGNED );

  GLfloat        lw0;
  Color          col0;
  GLsizei        w, h;
  GLint          x, y;


  getCurrent  ( col0 );
  glGetFloatv ( GL_LINE_WIDTH, &lw0 );
  glLineWidth ( 1.0F );

  // Draw the background.

  glColor3fv  ( MENU_COLOR );
  rect_.draw  ();

  if ( focus_ != FOCUS_NONE_ )
  {
    glColor3fv ( FOCUS_COLOR );
    items_[focus_].rect.draw ();
  }

  // Draw the borders.

  x = rect_.x;
  y = rect_.y;
  w = rect_.width;
  h = rect_.height;

  glBegin ( GL_LINES );
  {
    glColor3fv ( LIGHT_COLOR );
    glVertex2i ( x, y + 1 );
    glVertex2i ( x, y + h );
    glVertex2i ( x, y + h );
    glVertex2i ( x + w, y + h );
    glColor3fv ( DARK_COLOR );
    glVertex2i ( x, y );
    glVertex2i ( x + w, y );
    glVertex2i ( x + w, y );
    glVertex2i ( x + w, y + h - 1 );
  }
  glEnd ();

  // Draw the menu items.

  for ( idx_t i = 0; i < items_.size(); i++ )
  {
    const Item_&  item = items_[i];

    x = item.rect.x + space_;
    y = item.rect.y + space_;

    if ( item.label.size() == 0 )
    {
      w = item.rect.width - 2 * space_;

      glBegin ( GL_LINES );
      {
        glColor3fv ( LIGHT_COLOR );
        glVertex2i ( x,     y );
        glVertex2i ( x + w, y );
        glColor3fv ( DARK_COLOR );
        glVertex2i ( x,     y + 1 );
        glVertex2i ( x + w, y + 1 );
      }
      glEnd ();
    }
    else
    {
      if ( i == focus_ )
      {
        glColor3f ( 1.0F, 1.0F, 1.0F );
      }
      else
      {
        glColor3f ( 0.0F, 0.0F, 0.0F );
      }

      txp.begin     ( gtx );
      txp.setCursor ( x, y );
      txp.writeLine ( item.label );
      txp.end       ();
    }

    y = y - space_;
  }

  setCurrent  ( col0 );
  glLineWidth ( lw0 );
}


//-----------------------------------------------------------------------
//   setFocus_
//-----------------------------------------------------------------------


void MenuNode::setFocus_

  ( GfxContext&  gtx,
    GLint        xPos,
    GLint        yPos )

{
  if ( focus_ != FOCUS_NONE_ )
  {
    if ( items_[focus_].rect.contains( xPos, yPos ) )
    {
      return;
    }
  }

  lint  oldFocus = focus_;

  focus_ = FOCUS_NONE_;

  if ( rect_.contains( xPos, yPos ) )
  {
    for ( idx_t i = 0; i < items_.size(); i++ )
    {
      const Item_&  item = items_[i];

      if ( item.rect.contains( xPos, yPos ) )
      {
        if ( item.label.size() > 0 )
        {
          focus_ = i;
        }

        break;
      }
    }
  }

  if ( focus_ != oldFocus )
  {
    gtx.postFastRedraw ( this );
  }
}


//-----------------------------------------------------------------------
//   moveFocus_
//-----------------------------------------------------------------------


void MenuNode::moveFocus_

  ( GfxContext&  gtx,
    int          dir )

{
  if ( focus_ == FOCUS_NONE_ )
  {
    return;
  }

  idx_t  i = focus_ + dir;

  while ( i >= 0 && i < items_.size() )
  {
    if ( items_[i].label.size() > 0 )
    {
      focus_ = i;

      gtx.postFastRedraw ( this );

      break;
    }

    i += dir;
  }
}


//-----------------------------------------------------------------------
//   selectFocus_
//-----------------------------------------------------------------------


void MenuNode::selectFocus_ ( GfxContext& gtx )
{
  const idx_t  i = focus_;

  hide ( gtx );

  if ( focus_ != FOCUS_NONE_ )
  {
    selectEvent.emit ( gtx, items_[i].label );
  }
}


JEM_END_PACKAGE( gl )
