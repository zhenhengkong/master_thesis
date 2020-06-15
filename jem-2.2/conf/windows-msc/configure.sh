
#========================================================================
#   Functions
#========================================================================

#------------------------------------------------------------------------
# DetermineSysBits()
#------------------------------------------------------------------------


DetermineSysBits()
{
  local src="sysbits.cpp"
  local exe="sysbits.exe"
  local cmd="$CXX -Fe$exe $src"

  cat<<EOF > "$src"

#include <iostream>

int main ()
{
  std::cout << (int) sizeof(void*) << std::endl;
  return 0;
}

EOF

  eval echo '"Executing \`"'"${cmd}"\"\'' ..." >> "$JEM_LOGFILE"'

  eval "$cmd" '>> "$JEM_LOGFILE" 2>&1'

  if [ -x "$exe" ]; then
    SYS_BITS="$("./$exe")"
    rm -f -- "$exe"
  fi

  if [ -z "$SYS_BITS" ]; then
    Error "Failed to determine the size of a pointer."
  fi

  rm -f -- "$src"
}


#------------------------------------------------------------------------
#   ConfigureWindowsMSC ()
#------------------------------------------------------------------------


ConfigureWindowsMSC()
{
  local oflags="-DNDEBUG -Oi"

  if [ "$SYS_BITS" = "64" ]; then
    oflags="$oflags -arch:sse2"
  fi

  CXX_LNK_FLAGS="-MT"
  CXX_DBG_FLAGS="-RTCsu -Zi -D_DEBUG -D_ITERATOR_DEBUG_LEVEL=2"
  CXX_PRF_FLAGS=""
  CXX_OPT_LEVEL="3"
  CXX_BLD_FLAGS="-D_CRT_SECURE_NO_WARNINGS -wd4250"
  CXX_OPT_FLAGS1="-O1 $oflags"
  CXX_OPT_FLAGS2="-O2 $oflags"
  CXX_OPT_FLAGS3="-Ox $oflags"

  if [ -z "$USR_CXX_FLAGS" ]; then
    CXX_STD_FLAGS="-W3 -nologo"
  fi

  CXX_STD_FLAGS="$CXX_STD_FLAGS -MT -EHcs"

  if [ "$SYS_BITS" = "64" ]; then
    SYS_LINT="long long"
    SYS_ULINT="unsigned long long"
    SYS_LINT_MAX="LLONG_MAX"
    SYS_LINT_MIN="LLONG_MIN"
    SYS_ULINT_MAX="ULLONG_MAX"
    SYS_LINT_SUFFIX="LL"
    SYS_ULINT_SUFFIX="ULL"
  fi
}


#========================================================================
#   Main script
#========================================================================

echo "Configuring for the Microsoft Visual C++ compiler (cl.exe)."

DetermineSysBits
ConfigureWindowsMSC
