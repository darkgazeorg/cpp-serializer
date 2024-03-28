#include <cpp-serializer/location.hpp>
#include <cpp-serializer/data.hpp>

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <string_view>

struct NoConversion {
    std::pair<CPPSerializer::Location<false, false>, std::string> operator()(auto, const std::string_view &s)  { 
        return {{}, std::string(s)}; 
    }
    auto operator()(std::string &s) { return s; }
};

struct SimpleTraits {
    using StorageType = std::string;
    
    using NumberType = void;
    using IntegerType = void;
    using RealType = void;
    using StringType = std::string;
    using NullType = void;
    using BoolType = void;
    using IndexType = void;
    using SequenceType = void;
    using KeyType = void;
    using MapType = void;
    
    static constexpr bool HasOffset() { return false; }
    static constexpr bool HasSkipList() { return false; }

    using DataParserType = NoConversion;
    using DataEmitterType = NoConversion;
};

static_assert(CPPSerializer::DataTraitConcept<SimpleTraits>, "Simple traits do not satisfy data traits.");

TEST_CASE("File offset", "[Basics]") {
    CPPSerializer::FileOffset f;
    auto [line, off] = f.Offset(5);
    REQUIRE(line == 0);
    REQUIRE(off == 5);
}
