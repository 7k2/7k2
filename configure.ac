#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([7k2], [1.20], [skfans-devel@lists.sourceforge.net], [],
	[https://7kfans.com/])
AC_CONFIG_SRCDIR([src/am.cpp])
AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign])
ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

dnl Utility definitions

dnl SEARCH_LIB_FLAGS(FUNCTION, LIB-FLAGS,
dnl		     [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
AC_DEFUN([SEARCH_LIB_FLAGS], [
  ac_save_LIBS="$LIBS"
  ac_check_libs_found=no
  AS_FOR([], [ac_lib], [$2], [
    LIBS="$ac_lib $ac_save_LIBS"
    AC_MSG_CHECKING([for $1 in $ac_lib])
    AC_LINK_IFELSE([AC_LANG_CALL([], [$1])], [
      AC_MSG_RESULT([yes])
      ac_check_libs_found=yes
      $3
      break
    ], [
      AC_MSG_RESULT([no])
    ])
  ])
  AS_IF([test "$ac_check_libs_found" = no], [
    $4
  ])
])

# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB
PKG_PROG_PKG_CONFIG

# Checks for typedefs, structures, and compiler characteristics.
AC_CANONICAL_HOST

AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Configuration variables

use_windows=no
use_mingw=no

# Set in argument handlers:
# enable_debug=no
# enable_dynamic_support_libs=no
# enable_mouse_accel=no

# Configuration options

AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug], [enable debugging features]),
  [], [enable_debug=no]
)

AC_ARG_ENABLE([dynamic-support-libs],
  AS_HELP_STRING([--enable-dynamic-suppport-libs],
		 [do not link statically with libgcc and libstdc++ on Windows]),
  [], [enable_dynamic_support_libs=no]
)

AC_ARG_ENABLE([mouse-accel],
  AS_HELP_STRING([--enable-mouse-accel], [enable mouse acceleration]),
  [], [enable_mouse_accel=no]
)


# Check if cross-compiling for Windows.

AS_CASE(["$host"],
  [*-mingw*], [
    use_windows=yes
    use_mingw=yes
])

AM_CONDITIONAL([USE_WINDOWS], [test "$use_windows" = yes])


# Library dependencies

AM_PATH_SDL2([2.0.2], [], [
  AC_MSG_ERROR([SDL not found])
])

CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
LIBS="$SDL_LIBS $LIBS"

AM_PATH_OPENAL([], [
  AC_MSG_ERROR([OpenAL not found])
])

CXXFLAGS="$CXXFLAGS $OPENAL_CFLAGS"
LIBS="$LIBS $OPENAL_LIBS"

PKG_CHECK_MODULES([ENET], [libenet >= 1.3.10],, [
  PKG_CHECK_MODULES([ENET], [enet >= 1.3.10],, [
    SEARCH_LIB_FLAGS([enet_initialize], ["-lenet -lws2_32 -lwinmm" -lenet],, [
      AC_MSG_ERROR([enet not found])
    ])
  ])
])

AS_IF([test "$ENET_CFLAGS" != "" || test "$ENET_LIBS" != ""], [
  CXXFLAGS="$CXXFLAGS $ENET_CFLAGS"
  LIBS="$LIBS $ENET_LIBS"
])

PKG_CHECK_MODULES([LIBJPEG], [libjpeg],, [
  PKG_CHECK_MODULES([LIBJPEG], [jpeg],, [
    SEARCH_LIB_FLAGS([jpeg_CreateDecompress], [-ljpeg],, [
      AC_MSG_ERROR([libjpeg not found])
    ])
  ])
])

AS_IF([test "$LIBJPEG_CFLAGS" != "" || test "$LIBJPEG_LIBS" != ""], [
  CXXFLAGS="$CXXFLAGS $LIBJPEG_CFLAGS"
  LIBS="$LIBS $LIBJPEG_LIBS"
])

AS_IF([test "$use_windows" = yes], [
  LIBS="-lole32 -lmsvcrt -lwinmm $LIBS"

  AS_IF([test "$enable_debug" = no], [
    LDFLAGS="$LDFLAGS -mwindows"
  ])
])

AS_IF([test "$use_mingw" = yes], [
  AS_IF([test "$enable_dynamic_support_libs" = no], [
    LIBS="$LIBS -static-libstdc++ -static-libgcc"
  ])
])


# Defines

# Ensure the prefix is set
test "x$prefix" = xNONE && prefix=$ac_default_prefix

AS_IF([test "$use_windows" != yes], [
  AC_DEFINE([NO_WINDOWS], [1], [Define to 1 if not using Windows headers])
])

AC_DEFINE([USE_SDL], [1], [
  Define to 1 if using SDL as the video/input backend
])

AC_DEFINE([USE_OPENAL], [1], [
  Define to 1 if using OpenAL as the audio backend
])

AS_IF(
  [test "$enable_debug" = yes], [
    AC_DEFINE([DEBUG], [1], [Define to 1 if building with debugging features])
  ],
  [test "$enable_debug" = no], [
    AC_DEFINE([NO_MEM_CLASS], [1], [Define to 1 if not monitoring memory alloc])
])

AS_IF([test "$enable_mouse_accel" = yes], [
  AC_DEFINE([MOUSE_ACCEL], [1], [Define to 1 if using mouse acceleration])
])

AC_DEFINE([USE_FLIP], [1], [
  Define to 1 if using triple buffering
])

CXXFLAGS="$CXXFLAGS -I\"\$(top_srcdir)/include\""
CXXFLAGS="$CXXFLAGS -include \"\$(top_builddir)/include/config.h\""

AC_CONFIG_FILES([
  Makefile
  include/Makefile
  src/imgfun/Makefile
  src/Makefile
])

AC_OUTPUT
AS_ECHO([])
AC_MSG_NOTICE([debugging features: $enable_debug])
AC_MSG_NOTICE([mouse acceleration: $enable_mouse_accel])
