#							-*- shell-script -*-
#
# This file is part of the HDF5 build script.  It is processed shortly
# after configure starts and defines, among other things, flags for
# the various compile modes.
#
# See BlankForm in this directory for details

# The default compiler is `sunpro cc'
if test -z "$CXX"; then
    CXX=CC
    CXX_BASENAME=CC
fi

# Try gcc compiler flags
#. $srcdir/config/gnu-flags

cxx_version="`$CXX -V 2>&1 |grep 'WorkShop' |\
                sed 's/.*WorkShop.*C\+\+ \([0-9\.]*\).*/\1/'`"

cxx_vers_major=`echo $cxx_version | cut -f1 -d.`
cxx_vers_minor=`echo $cxx_version | cut -f2 -d.`
cxx_vers_patch=`echo $cxx_version | cut -f3 -d.`

# Specify the "-features=tmplife" if the compiler can handle this...
if test -n "$cxx_version"; then
  if test $cxx_vers_major -ge 5 -a $cxx_vers_minor -ge 3 -o $cxx_vers_major -gt 5; then
    CXXFLAGS="-features=tmplife"
  fi
fi

# Try solaris native compiler flags
if test -z "$cxx_flags_set"; then
    CXXFLAGS="$CXXFLAGS -instances=global"
    CPPFLAGS="-LANG:std"
    LIBS="$LIBS -lsocket"
    DEBUG_CXXFLAGS=-g
    DEBUG_CPPFLAGS=
    PROD_CXXFLAGS="-O -s"
    PROD_CPPFLAGS=
    PROFILE_CXXFLAGS=-xpg
    PROFILE_CPPFLAGS=
    cxx_flags_set=yes
fi
