
. "${PKG_DIR:?}/conf/configure.sh"

if [ ! -e "$PKG_INC_DIR/native" ]; then
  MakeDir "$PKG_INC_DIR/native" || Die
fi

PushDir "$PKG_DIR/src" || Die

cp -f native/WinFile.h \
      "$PKG_INC_DIR/native/File.h"                   || Die
cp -f native/WinFile.cpp        NativeFile.cpp       || Die
cp -f native/WinFileStream.h \
      "$PKG_INC_DIR/native/FileStream.h"             || Die
cp -f native/WinFileStream.cpp  NativeFileStream.cpp || Die
cp -f native/WinDirListing.h \
      "$PKG_INC_DIR/native/DirListing.h"             || Die
cp -f native/WinDirListing.cpp  NativeDirListing.cpp || Die

PopDir || Die
