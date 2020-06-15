
#------------------------------------------------------------------------
#   TestMSC ( %cxx [%flag1 %flag2 ...] )
#------------------------------------------------------------------------


TestMSC()
{
  local ret=0
  local src="test-cxx.cpp"
  local exe="test-cxx.exe"
  local cmd="$1"

  shift
  cmd="$cmd $* -Fe $exe $src"

  cat<<EOF > $src

int main ()
{
  return 0;
}

EOF

  eval echo '"Executing \`"'"${cmd}"\"\'' ..." >> "$JEM_LOGFILE"'

  eval "$cmd" '>> "$JEM_LOGFILE" 2>&1'

  if [ -x "$exe" ]; then
    ret=0
    rm -f -- "$exe"
  else
    ret=1
  fi

  rm -f -- "$src"

  return $ret
}


#------------------------------------------------------------------------
#   TestCompiler ( %cxx [%flag1 %flag2 ...] )
#------------------------------------------------------------------------


TestCompiler()
{
  TestMSC "$@"
}
