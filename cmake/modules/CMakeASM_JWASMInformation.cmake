
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

# support for the jwasm assembler

set(CMAKE_ASM_JWASM_SOURCE_FILE_EXTENSIONS asm)

if(NOT CMAKE_ASM_JWASM_OBJECT_FORMAT)
  set(CMAKE_ASM_JWASM_OBJECT_FORMAT elf)
endif()

set(CMAKE_ASM_JWASM_COMPILE_OBJECT "<CMAKE_ASM_JWASM_COMPILER> <FLAGS> -${CMAKE_ASM_JWASM_OBJECT_FORMAT} -zt1 -Fo <OBJECT> <SOURCE>")

# Load the generic ASMInformation file:
set(ASM_DIALECT "_JWASM")
include(CMakeASMInformation)
set(ASM_DIALECT)
