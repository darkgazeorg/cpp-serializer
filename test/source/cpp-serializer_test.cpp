#include <string>

#include "cpp-serializer/cpp-serializer.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Name is cpp-serializer", "[library]")
{
  auto const exported = exported_class {};
  REQUIRE(std::string("cpp-serializer") == exported.name());
}
