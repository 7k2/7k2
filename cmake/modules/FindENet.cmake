# - Find ENet
# Find the ENet includes and library
# This module defines
#  ENET_INCLUDE_DIR, where to find enet.h, etc.
#  ENET_LIBRARIES, the libraries needed to use ENet.
#  ENET_FOUND, If false, do not try to use ENet.
# also defined, but not for general use are
#  ENET_LIBRARY, where to find the ENET library.

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

find_path(ENET_INCLUDE_DIR enet.h
          PATH_SUFFIXES enet
          DOC "Path to enet.h")

find_library(ENET_LIBRARY enet
             DOC "Path to the enet library")

# handle the QUIETLY and REQUIRED arguments and set ENET_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ENet DEFAULT_MSG ENET_LIBRARY ENET_INCLUDE_DIR)

if(ENET_FOUND)
  set(ENET_LIBRARIES ${ENET_LIBRARY})
endif()

mark_as_advanced(ENET_LIBRARY ENET_INCLUDE_DIR )
