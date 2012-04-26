
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

# Find the jwasm assembler.

SET(CMAKE_ASM_JWASM_COMPILER_INIT jwasm)

IF(NOT CMAKE_ASM_JWASM_COMPILER)
  FIND_PROGRAM(CMAKE_ASM_JWASM_COMPILER jwasm)
ENDIF(NOT CMAKE_ASM_JWASM_COMPILER)

# Load the generic DetermineASM compiler file with the DIALECT set properly:
SET(ASM_DIALECT "_JWASM")
INCLUDE(CMakeDetermineASMCompiler)
SET(ASM_DIALECT)
