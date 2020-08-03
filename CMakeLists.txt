cmake_minimum_required(VERSION 2.8)
project(changme)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

include_directories(.)
include_directories(/apps/sylar/include)

find_library(YAMLCPP libyaml-cpp.a)

set(LIB_SRC
    src/base_framework/log4J/log.cpp
    src/base_framework/config/config.cpp
    src/base_framework/utility/utility.cpp
   )

add_library(changme SHARED ${LIB_SRC})

add_executable(test_log tests/log4J/test_log.cpp)
add_dependencies(test_log changme)
target_link_libraries(test_log changme -L/apps/sylar/lib -lyaml-cpp)

add_executable(test_config tests/config/test_config.cpp)
add_dependencies(test_config changme)
target_link_libraries(test_config changme -L/apps/sylar/lib -lyaml-cpp)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)