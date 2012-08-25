cmake_minimum_required(VERSION 2.8)
project(7K2)
set(CMAKE_MODULE_PATH ${CMAKE_HOME_DIRECTORY}/cmake/modules)

if(UNIX)
  set(CMAKE_BUILD_TYPE Debug)
endif(UNIX)

find_package(JPEG REQUIRED)
find_package(OpenAL REQUIRED)
include(FindPkgConfig)
pkg_check_modules(SDL2 REQUIRED sdl2)
add_subdirectory(src/imgfun)
add_subdirectory(src)
