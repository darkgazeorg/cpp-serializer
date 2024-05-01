#include <cpp-serializer/utf.hpp>
#include <cpp-serializer/location.hpp>
#include <cpp-serializer/data.hpp>
#include <cpp-serializer/txt.hpp>

#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include <string>

using namespace std::literals;
using namespace CPP_SERIALIZER_NAMESPACE;

TEST_CASE("UTF8Bytes", "[helpers][utf]") {
    auto str = "aÂᴬ𝐴"s;
    REQUIRE(UTF8Bytes(str[0]) == 1);
    REQUIRE(UTF8Bytes(str[1]) == 2);
    REQUIRE(UTF8Bytes(str[3]) == 3);
    REQUIRE(UTF8Bytes(str[6]) == 4);
}

TEST_CASE("Test text reader string", "[Parse][Text][Source<string_view>]") {
    TextTransport<>::DataType data;
    TextTransportSimple.Parse("Hello", data);
    REQUIRE(data.GetData() == "Hello");
}

TEST_CASE("Stream source", "[Stream][Source]") {
    std::stringstream ss("Hello world");

    //simple
    //REQUIRE(TextTransportSimple.Parse(ss).GetData() == "Hello world");

    
    //complex
    RuntimeTextTransportSkipList transport;
    RuntimeTextTransportSkipList::DataType data;
    ss = std::stringstream("a \xc2\xa0lâd\t c\xc2\xa0 g\n x \nZ"); //c2a0 is non-breaking space
    transport.Parse(ss, data);
    auto parsed = data.GetData();
    REQUIRE(parsed == "a lâd\tc\xc2\xa0g x Z");

    auto loc = data.GetLocation(0);
    REQUIRE(loc.LineOffset == 1); REQUIRE(loc.CharOffset == 1);

    loc = data.GetLocation(1);
    REQUIRE(loc.LineOffset == 1); REQUIRE(loc.CharOffset == 2);

    loc = data.GetLocation(2);
    REQUIRE(loc.LineOffset == 1); REQUIRE(loc.CharOffset == 4);

    loc = data.GetLocation(5);
    REQUIRE(loc.LineOffset == 1); REQUIRE(loc.CharOffset == 6);

    loc = data.GetLocation(7);
    REQUIRE(loc.LineOffset == 1); REQUIRE(loc.CharOffset == 9);

    loc = data.GetLocation(10);
    REQUIRE(loc.LineOffset == 1); REQUIRE(loc.CharOffset == 12);

    loc = data.GetLocation(12);
    REQUIRE(loc.LineOffset == 2); REQUIRE(loc.CharOffset == 2);

    loc = data.GetLocation(14);
    REQUIRE(loc.LineOffset == 3); REQUIRE(loc.CharOffset == 1);
}

TEST_CASE("Test text reader string", "[Parse][Text][RuntimeSettings]") {
    RuntimeTextTransport transport;
    RuntimeTextTransport::DataType data;

    transport.SetGlue(false);
    transport.SetFolding(false);

    transport.Parse("Hello\nWorld", data);
    REQUIRE(data.GetData() == "Hello\nWorld");


    transport.SetGlue(true);

    transport.Parse("Hello\nWorld", data);
    REQUIRE(data.GetData() == "Hello World");

    transport.Parse("Hello\n\nWorld", data);
    REQUIRE(data.GetData() == "Hello\nWorld");


    transport.SetFolding(true);
    transport.Parse("Hello  world", data);
    REQUIRE(data.GetData() == "Hello world");
}

TEST_CASE("Test text reader skiplist", "[Parse][Text][SkipList]") {
    RuntimeTextTransportSkipList transport;
    RuntimeTextTransportSkipList::DataType data;

    std::string source;


    source = "abc\nâbc";
    transport.Parse(source, data);
    auto parsed = data.GetData();

    REQUIRE(parsed == "abc âbc");

    auto loc = data.GetLocation(0);
    REQUIRE(loc.LineOffset == 1); REQUIRE(loc.CharOffset == 1);

    loc = data.GetLocation(3);
    REQUIRE(loc.LineOffset == 1); REQUIRE(loc.CharOffset == 4);

    loc = data.GetLocation(4);
    REQUIRE(loc.LineOffset == 2); REQUIRE(loc.CharOffset == 1);

    loc = data.GetLocation(6);
    REQUIRE(loc.LineOffset == 2); REQUIRE(loc.CharOffset == 2);

    loc = data.GetLocation(7);
    REQUIRE(loc.LineOffset == 2); REQUIRE(loc.CharOffset == 3);


    source = "abc\n\nâbc\nabc";
    transport.Parse(source, data);
    parsed = data.GetData();

    REQUIRE(parsed == "abc\nâbc abc");

    loc = data.GetLocation(4);
    REQUIRE(loc.LineOffset == 3); REQUIRE(loc.CharOffset == 1);

    loc = data.GetLocation(7);
    REQUIRE(loc.LineOffset == 3); REQUIRE(loc.CharOffset == 3);

    loc = data.GetLocation(8);
    REQUIRE(loc.LineOffset == 3); REQUIRE(loc.CharOffset == 4);

    loc = data.GetLocation(9);
    REQUIRE(loc.LineOffset == 4); REQUIRE(loc.CharOffset == 1);

    source = "a \xc2\xa0lâd\t c\xc2\xa0 g\n x \nZ"; //c2a0 is non-breaking space
    transport.Parse(source, data);
    parsed = data.GetData();

    REQUIRE(parsed == "a lâd\tc\xc2\xa0g x Z");

    loc = data.GetLocation(0);
    REQUIRE(loc.LineOffset == 1); REQUIRE(loc.CharOffset == 1);

    loc = data.GetLocation(1);
    REQUIRE(loc.LineOffset == 1); REQUIRE(loc.CharOffset == 2);

    loc = data.GetLocation(2);
    REQUIRE(loc.LineOffset == 1); REQUIRE(loc.CharOffset == 4);

    loc = data.GetLocation(5);
    REQUIRE(loc.LineOffset == 1); REQUIRE(loc.CharOffset == 6);

    loc = data.GetLocation(7);
    REQUIRE(loc.LineOffset == 1); REQUIRE(loc.CharOffset == 9);

    loc = data.GetLocation(10);
    REQUIRE(loc.LineOffset == 1); REQUIRE(loc.CharOffset == 12);

    loc = data.GetLocation(12);
    REQUIRE(loc.LineOffset == 2); REQUIRE(loc.CharOffset == 2);

    loc = data.GetLocation(14);
    REQUIRE(loc.LineOffset == 3); REQUIRE(loc.CharOffset == 1);
}

TEST_CASE("Test text emit simple", "[Emit][Text]") {
    std::string target;
    TextTransport<>::DataType data;
    data.SetData("Hello world\n");
    TextTransportSimple.Emit(data, target);

    REQUIRE(target == "Hello world\n");
}

TEST_CASE("Test text emit word wrap", "[Emit][Text][WordWrap]") {
    std::string target;
    RuntimeTextTransport::DataType data;
    RuntimeTextTransport transport;


    transport.SetWrapWidth(10);

    data.SetData("Hello world here I am.");
    transport.Emit(data, target);
    REQUIRE(target == "Hello\nworld here\nI am.");

    target = "";
    data.SetData("Hello world\nhere I am...");
    transport.Emit(data, target);
    REQUIRE(target == "Hello\nworld\n\nhere I\nam...");

    target = "";
    std::stringstream ss{"# I\n\n\n# S\nif\n"};
    data = transport.Parse(ss);
    ss = {};
    transport.Emit(data, ss);
    REQUIRE(ss.str() == "# I\n\n\n# S if");
}

TEST_CASE("Test stream writer", "[Stream][Target]") {
    std::stringstream target;
    RuntimeTextTransport::DataType data;
    RuntimeTextTransport transport;


    transport.SetWrapWidth(10);

    data.SetData("Hello world here I am.");
    transport.Emit(data, target);
    REQUIRE(target.str() == "Hello\nworld here\nI am.");

    target = {};
    data.SetData("Hello world\nhere I am...");
    transport.Emit(data, target);
    REQUIRE(target.str() == "Hello\nworld\n\nhere I\nam...");
}
