
#========================================================================
#   Global variables
#========================================================================


USE_GL=1
USE_OSMESA=""

GL_LIBS="glut GLU GL"
GL_LIBDIRS=""
GL_INCDIRS=""

GL_H="gl.h"
GLU_H="glu.h"
GLUT_H="glut.h"

OSMESA_LIB=""
OS_DISPLAY="OSDummyDisplay"


#========================================================================
#   Functions
#========================================================================

#------------------------------------------------------------------------
#   SetDefaults ( [%arg1 %arg2 ...] )
#------------------------------------------------------------------------


SetDefaults()
{
  local f="$(FindSysconfFile defaults.sh)"

  if [ -n "$f" ]; then
    Log "Executing \`$f' ..."
    . "$f"
  fi
}


#------------------------------------------------------------------------
#   ParseArgs ( [%arg1 %arg2 ...] )
#------------------------------------------------------------------------


ParseArgs()
{
  local arg=""

  for arg in "$@" ; do

    case "$arg" in

      --gl-libs=?*)

        GL_LIBS="${arg#--gl-libs=}" ;;

      --gl-libdirs=?*)

        GL_LIBDIRS="${arg#--gl-libdirs=}" ;;

      --gl-incdirs=?*)

        GL_INCDIRS="${arg#--gl-incdirs=}" ;;

      --with-osmesa)

        USE_OSMESA=1 ;;

      --without-osmesa)

        USE_OSMESA="" ;;

      --osmesa=?*)

        OSMESA_LIB="${arg#--osmesa=}" ;;

    esac

  done
}


#------------------------------------------------------------------------
#   TestGL ( [-s|--silent] )
#------------------------------------------------------------------------


TestGL()
{
  local silent=""
  local f="$(FindSysconfFile test-gl.cpp)"

  if [ -z "$f" ]; then
    return 1
  fi

  case "$1" in
    -s|--silent) silent=1
  esac

  MY_LIBS="$GL_LIBS"
  MY_LIBDIRS="$GL_LIBDIRS"
  MY_INCDIRS="$GL_INCDIRS"

  Log "Testing GL setup ..."

  if [ -z "$silent" ]; then
    printf "Testing if OpenGL and GLUT work ... "
  fi

  if TestSyslibs "$f" ; then
    if [ -z "$silent" ]; then
      echo "yes."
    fi
    return 0
  else
    if [ -z "$silent" ]; then
      echo "no."
    fi
    return 1
  fi
}


#------------------------------------------------------------------------
#   SelectOSMesa
#------------------------------------------------------------------------


SelectOSMesa()
{
  local f="$(FindSysconfFile test-osmesa.cpp)"
  local lib=""

  if [ -z "$f" ]; then
    return 1
  fi

  MY_LIBDIRS="$GL_LIBDIRS"
  MY_INCDIRS="$GL_INCDIRS"

  for lib in "$OSMESA_LIB" OSMesa OSMesa16 OSMesa32; do

    if [ -z "$lib" ]; then
      continue
    fi

    MY_LIBS="$lib $GL_LIBS"

    if TestSyslibs "$f" ; then

      OSMESA_LIB="$lib"
      OS_DISPLAY="OSMesaDisplay"
      GL_LIBS="$MY_LIBS"

      return 0

    fi

  done

  OSMESA_LIB=""

  return 1
}


#------------------------------------------------------------------------
#   SelectOSDisplay
#------------------------------------------------------------------------


SelectOSDisplay()
{
  local silent=""

  case "$1" in
    -s|--silent) silent=1
  esac

  Log "Testing off-screen rendering support ..."

  if [ -z "$silent" ]; then
    printf "Testing off-screen rendering support ... "
  fi

  if SelectOSMesa ; then

    if [ -z "$silent" ]; then
      echo "yes ($OSMESA_LIB)."
    fi

    return 0

  fi

  if [ -z "$silent" ]; then
    echo "no."
  fi

  USE_OSMESA=""

  return 1
}


#------------------------------------------------------------------------
#   ConfigureSource ()
#------------------------------------------------------------------------


ConfigureSource()
{
  local f=""

  PushDir "$PKG_DIR/src" || Die

  cp -f "native/$GL_H"   "$PKG_INC_DIR/gl.h"   || Die
  cp -f "native/$GLU_H"  "$PKG_INC_DIR/glu.h"  || Die
  cp -f "native/$GLUT_H" "$PKG_INC_DIR/glut.h" || Die

  f="${OS_DISPLAY}.cpp"

  if [ ! -e "native/$f" ]; then
    f="OSDummyDisplay.cpp"
  fi

  cp -f "native/$f" "OSDisplay.cpp" || Die

  PopDir || Die
}


#========================================================================
#   Main script
#========================================================================


SetDefaults "$@"
ParseArgs "$@"

if [ -n "$USE_GL" ] && TestGL ; then

  if [ -n "$USE_OSMESA" ]; then
    SelectOSDisplay
  fi

  if [ -n "$VERBOSE" ]; then

    echo ""
    echo "  --gl-libs=$(quote "$GL_LIBS")"
    echo "  --gl-libdirs=$(quote "$GL_LIBDIRS")"
    echo "  --gl-incdirs=$(quote "$GL_INCDIRS")"

    if [ -n "$USE_OSMESA" ]; then
      echo "  --with-osmesa"
      echo "  --osmesa=$(quote "$OSMESA_LIB")"
    else
      echo "  --without-osmesa"
    fi

    echo ""

  fi

  ConfigureSource

  MY_LIBS="$GL_LIBS"
  MY_LIBDIRS="$GL_LIBDIRS"
  MY_INCDIRS="$GL_INCDIRS"

else

  LogEcho "Disabled the gl package."

  exit 0

fi
