#include "cpp-serializer/source.hpp"
#include "cpp-serializer/utf.hpp"
#include <cpp-serializer/location.hpp>
#include <cpp-serializer/data.hpp>

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <cpp-serializer/txt.hpp>

using namespace std::literals;
using namespace CPP_SERIALIZER_NAMESPACE;

TEST_CASE("UTF8Bytes", "[helpers][utf]") {
    auto str = "aÂᴬ𝐴"s;
    REQUIRE(UTF8Bytes(str[0]) == 1);
    REQUIRE(UTF8Bytes(str[1]) == 2);
    REQUIRE(UTF8Bytes(str[3]) == 3);
    REQUIRE(UTF8Bytes(str[6]) == 4);
}

TEST_CASE("File offset", "[Basics]") {
    /*CPPSerializer::Offset f;
    auto [line, off] = f.Offset(5);
    REQUIRE(line == 0);
    REQUIRE(off == 5);*/
}

TEST_CASE("Test text reader string", "[Parse][Text][Source<string_view>]") {
    TextTransport<>::DataType data;
    TextTransportSimple.Parse("Hello", data);
    REQUIRE(data.GetData() == "Hello");
}

TEST_CASE("Test text reader string", "[Parse][Text][Source<path>]") {
    TextTransport<>::DataType data;
    TextTransportSimple.Parse("Hello", data);
    REQUIRE(data.GetData() == "Hello");

    RuntimeTextTransport transport;
    RuntimeTextTransport::DataType data2;
    transport.SetGlue(true);

    transport.Parse("Hello\nWorld", data2);
    REQUIRE(data2.GetData() == "Hello World");

    transport.Parse("Hello\n\nWorld", data2);
    REQUIRE(data2.GetData() == "Hello\nWorld");
}
