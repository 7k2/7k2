cmake_minimum_required(VERSION 2.8)
project(7K2)
set(CMAKE_MODULE_PATH ${CMAKE_HOME_DIRECTORY}/cmake/modules)

find_package(JPEG REQUIRED)
find_package(OpenAL REQUIRED)
find_package(SDL2 REQUIRED)
add_subdirectory(src/imgfun)
add_subdirectory(src)
