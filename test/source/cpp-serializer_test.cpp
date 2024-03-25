#include <cpp-serializer/location.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("File offset", "[Basics]") {
    CPPSerializer::FileOffset f;
    auto [line, off] = f.Offset(5);
    REQUIRE(line == 0);
    REQUIRE(off == 5);
}
