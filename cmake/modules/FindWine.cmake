# - Find Wine
# Find the native Wine C++ compiler and includes
# This module defines
#  WINE_MSVCRT_INCLUDE_DIR, where to find dos.h, etc.
#  WINE_WINDOWS_INCLUDE_DIR, where to find windows.h, etc.
#  WINEG++_EXECUTABLE, the full path to the Wine C++ compiler.
#  WINE_FOUND, If false, do not try to use Wine.

#=============================================================================
# Copyright 2001-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

FIND_PATH(WINE_MSVCRT_INCLUDE_DIR
  dos.h "/usr/include/wine/msvcrt" "/usr/local/include/wine/msvcrt"
  DOC "Wine msvcrt include path")
FIND_PATH(WINE_WINDOWS_INCLUDE_DIR
  windows.h "/usr/include/wine/windows" "/usr/local/include/wine/windows"
  DOC "Wine windows include path")

SET(WINEG++_NAMES ${WINEG++_NAMES} wineg++)
FIND_PROGRAM(WINEG++_EXECUTABLE NAMES ${WINEG++_NAMES} DOC "Path to wineg++")

# handle the QUIETLY and REQUIRED arguments and set WINE_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Wine DEFAULT_MSG
  WINEG++_EXECUTABLE WINE_MSVCRT_INCLUDE_DIR WINE_WINDOWS_INCLUDE_DIR)

MARK_AS_ADVANCED(WINEG++_EXECUTABLE WINE_MSVCRT_INCLUDE_DIR
  WINE_WINDOWS_INCLUDE_DIR)
