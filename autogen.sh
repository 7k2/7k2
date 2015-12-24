#!/bin/bash

set -x
rm -rf autom4te.cache
rm -f aclocal.m4 compile config.guess config.rpath config.sub configure \
      depcomp install-sh missing
rm -f m4/pkg.m4 m4/sdl2.m4
rm -f include/config.h.in
rm -f Makefile.in include/Makefile.in src/Makefile.in src/imgfun/Makefile.in
test "$1" == clean && exit 0

autoreconf -vif
