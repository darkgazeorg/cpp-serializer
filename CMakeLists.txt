cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    cpp-serializer
    VERSION 0.1.0
    DESCRIPTION "This is a modern C++ library for YAML based serialization."
    HOMEPAGE_URL "https://github.com/darkgazeorg/cpp-serializer/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)
include(cmake/source-tree.cmake)

# ---- Declare library ----

SET(All)
SET(Public)

StartSource(include/cpp-serializer)

add_library(
    cpp-serializer_cpp-serializer INTERFACE
    ${All}
)
add_library(cpp-serializer::cpp-serializer ALIAS cpp-serializer_cpp-serializer)

set_target_properties(
    cpp-serializer_cpp-serializer PROPERTIES
    EXPORT_NAME cpp-serializer
)

target_include_directories(
    cpp-serializer_cpp-serializer ${warning_guard}
    INTERFACE
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(cpp-serializer_cpp-serializer INTERFACE cxx_std_20)

find_package(fmt REQUIRED)
target_link_libraries(cpp-serializer_cpp-serializer INTERFACE fmt::fmt)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${cpp-serializer_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT cpp-serializer_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of cpp-serializer"
  )
endif()

include(cmake/dev-mode.cmake)
