#include "cpp-serializer/utf.hpp"
#include <cpp-serializer/location.hpp>
#include <cpp-serializer/data.hpp>

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <cpp-serializer/txt.hpp>

namespace cpps = CPPSerializer;
using namespace std::literals;

TEST_CASE("UTF8Bytes", "[helpers][utf]") {
    auto str = "aÂᴬ𝐴"s;
    REQUIRE(cpps::UTF8Bytes(str[0]) == 1);
    REQUIRE(cpps::UTF8Bytes(str[1]) == 2);
    REQUIRE(cpps::UTF8Bytes(str[3]) == 3);
    REQUIRE(cpps::UTF8Bytes(str[6]) == 4);
}

TEST_CASE("File offset", "[Basics]") {
    /*CPPSerializer::Offset f;
    auto [line, off] = f.Offset(5);
    REQUIRE(line == 0);
    REQUIRE(off == 5);*/
}

TEST_CASE("Test text reader string", "[Parse][Text]") {
    cpps::TextTransport<>::DataType data;
    cpps::TextTransportSimple.Parse("Hello", data);
    REQUIRE(data.GetData() == "Hello");
}
