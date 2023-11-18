Include(FetchContent)
FetchContent_Declare(
  gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(gtest)

set(SOURCE
  libtest.cpp
)

set(TEST_NAME libtest)
add_executable(${TEST_NAME} ${SOURCE})
target_link_libraries(${TEST_NAME} PRIVATE gtest_main ${LIB_NAME})
