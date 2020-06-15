#========================================================================
#   Functions
#========================================================================

#------------------------------------------------------------------------
#   SetDarwinGLDefaults ( [%arg1 %arg2 ...] )
#------------------------------------------------------------------------


SetDarwinGLDefaults()
{
  local withMesa=""
  local withOSMesa=""
  local arg=""

  for arg in "$@" ; do

    case "$arg" in

      --with-mesa)       withMesa=1 ;;

      --without-mesa)    withMesa="" ;;

      --with-osmesa)     withOSMesa=1 ;;

      --without-osmesa)  withOSMesa="" ;;

    esac

  done

  # OSMesa requires the Mesa OpenGL libraries.

  if [ -z "$withMesa" ] && [ -n "$withOSMesa" ]; then
    LogEcho "Note: \`--with-osmesa' implies \`--with-mesa'."
    withMesa=1
  fi

  if [ -z "$withMesa" ]; then

    if [ "xx$CXX_NAME" = "xxgcc" ]; then
      GL_LIBS="-NOLIB:-framework -NOLIB:OpenGL\
        -NOLIB:-framework -NOLIB:GLUT\
        -NOLIB:-framework -NOLIB:Foundation"
    fi

    GL_LIBDIRS=""
    GL_INCDIRS=""

    GL_H="gl-darwin.h"
    GLU_H="glu-darwin.h"
    GLUT_H="glut-darwin.h"

  else

    GL_H="gl.h"
    GLU_H="glu.h"
    GLUT_H="glut.h"

    GL_LIBS="glut GLU GL"

    if [ -d /sw/lib/mesa ] && [ -d /sw/include/mesa ]; then
      GL_LIBDIRS="/sw/lib/mesa"
      GL_INCDIRS="/sw/include/mesa"
    else
      GL_LIBDIRS="/usr/X11R6/lib"
      GL_INCDIRS="/usr/X11R6/include"
    fi

  fi
}


#========================================================================
#   Main script
#========================================================================


SetDarwinGLDefaults "$@"
