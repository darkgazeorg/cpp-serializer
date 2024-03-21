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

# ---- Declare library ----

add_library(
    cpp-serializer_cpp-serializer
    source/cpp-serializer.cpp
)
add_library(cpp-serializer::cpp-serializer ALIAS cpp-serializer_cpp-serializer)

include(GenerateExportHeader)
generate_export_header(
    cpp-serializer_cpp-serializer
    BASE_NAME cpp-serializer
    EXPORT_FILE_NAME export/cpp-serializer/cpp-serializer_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(cpp-serializer_cpp-serializer PUBLIC CPP_SERIALIZER_STATIC_DEFINE)
endif()

set_target_properties(
    cpp-serializer_cpp-serializer PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME cpp-serializer
    OUTPUT_NAME cpp-serializer
)

target_include_directories(
    cpp-serializer_cpp-serializer ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    cpp-serializer_cpp-serializer SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(cpp-serializer_cpp-serializer PUBLIC cxx_std_20)

find_package(fmt REQUIRED)
target_link_libraries(cpp-serializer_cpp-serializer PRIVATE fmt::fmt)

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
