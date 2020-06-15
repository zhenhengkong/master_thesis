
GL_H="gl-windows.h"
GLU_H="glu-windows.h"

GL_LIBS="glut"

if [ "$CXX_NAME" != "msc" ]; then
  GL_LIBS="$GL_LIBS glu32 opengl32 gdi32 winmm"
fi
