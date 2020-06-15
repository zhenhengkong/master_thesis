
#========================================================================
#   Functions
#========================================================================

#------------------------------------------------------------------------
#   ConfigureAtomicOps ()
#------------------------------------------------------------------------


ConfigureAtomicOps()
{
  local f="native/atomic-${SYS_ARCH}"
  local i=""

  for i in "${f}-${CXX_NAME}.h" "${f}.h"; do
    if [ -e "$i" ]; then
      cp -f "$i" "$PKG_INC_DIR/atomic.h" || Die
      break
    fi
  done

  for i in "${f}-${CXX_NAME}.cpp" "${f}.cpp"; do
    if [ -e "$i" ]; then
      cp -f "$i" atomic.cpp || Die
      break
    fi
  done

  if [ ! -e "$PKG_INC_DIR/atomic.h" ]; then

    if [ -n "$SYS_IS_POSIX" ]; then

      cp -f "native/atomic-posix.h" \
            "$PKG_INC_DIR/atomic.h"              || Die
      cp -f "native/atomic-posix.cpp" atomic.cpp || Die

    else

      Oops "Unable to find a suitable implementation for atomic"
           "operations on this system."

    fi

  fi
}


#========================================================================
#   Main script
#========================================================================


PushDir "$PKG_DIR/src" || Die

if [ -z "$USE_THREADS" ]; then
  cp -f native/atomic-dummy.h "$PKG_INC_DIR/atomic.h" || Die
else
  ConfigureAtomicOps
fi

PopDir || Die
