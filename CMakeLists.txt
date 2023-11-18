cmake_minimum_required(VERSION 3.18)
project(darkgaze-yaml-serializer)

set(INCLUDE_PATH include)
set(LIB_HEADERS
  ${INCLUDE_PATH}/darkgaze/yaml/lib.h
)

set(LIB_SOURCE
  ${LIB_HEADERS}
  src/lib.cpp
)

set(LIB_NAME darkgaze-yaml-serializer)
add_library(${LIB_NAME} STATIC ${LIB_SOURCE})
target_include_directories(${LIB_NAME} PUBLIC ${INCLUDE_PATH})

set(COMPILER_OPTIONS
  -Wall
  -Wextra
  -Werror
  -Wshadow
  -Wpedantic
  -pedantic-errors
)
target_compile_options(${LIB_NAME} PUBLIC ${COMPILER_OPTIONS})
target_compile_features(${LIB_NAME} PUBLIC cxx_std_17)

option(COMPILE_TESTS "Compile tests." ON)

if(COMPILE_TESTS)
  add_subdirectory(test)
endif()
