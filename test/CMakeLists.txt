cmake_minimum_required(VERSION 3.14)

project(cpp-serializerTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(cpp-serializer REQUIRED)
  enable_testing()
endif()

find_package(Catch2 REQUIRED)
include(Catch)

# ---- Tests ----

add_executable(cpp-serializer_test source/cpp-serializer_test.cpp)
target_link_libraries(
    cpp-serializer_test PRIVATE
    cpp-serializer::cpp-serializer
    Catch2::Catch2WithMain
)
target_compile_features(cpp-serializer_test PRIVATE cxx_std_20)

catch_discover_tests(cpp-serializer_test)

# ---- End-of-file commands ----

add_folders(Test)
