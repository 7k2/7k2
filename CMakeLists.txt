cmake_minimum_required(VERSION 2.8)
project(7K2)
set(CMAKE_MODULE_PATH ${CMAKE_HOME_DIRECTORY}/cmake/modules)

if(UNIX)
  set(CMAKE_BUILD_TYPE Debug)
endif(UNIX)

find_package(JPEG REQUIRED)
add_subdirectory(src/asm)
add_subdirectory(src)
