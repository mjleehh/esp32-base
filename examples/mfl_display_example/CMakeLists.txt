cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(EXTRA_COMPONENT_DIRS ../..)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mfl_display_example)
