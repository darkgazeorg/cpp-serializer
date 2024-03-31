#pragma once

#include <map>
#include <optional>
#include <variant>
#include <vector>
#include <string>
#include <string_view>

namespace CPPSerializer {
    
    struct NoLocation {
        static constexpr bool HasByteOffset() { return false; }
        static constexpr bool HasCharOffset() { return false; }
        static constexpr bool HasLineOffset() { return false; }
        static constexpr bool HasSkipList() { return false; }
        static constexpr bool HasResourceName() { return false; }
    };
    
    struct ByteOffset {
        static constexpr bool HasByteOffset() { return true; }
        static constexpr bool HasCharOffset() { return false; }
        static constexpr bool HasLineOffset() { return false; }
        static constexpr bool HasSkipList() { return false; }
        static constexpr bool HasResourceName() { return false; }
        
        size_t ByteOffset = 0;
    };
    
    struct Offset {
        static constexpr bool HasByteOffset() { return true; }
        static constexpr bool HasCharOffset() { return true; }
        static constexpr bool HasLineOffset() { return false; }
        static constexpr bool HasSkipList() { return false; }
        static constexpr bool HasResourceName() { return false; }
        
        size_t CharOffset = 0;
        size_t ByteOffset = 0;
    };
    
    struct LineLocation {
        static constexpr bool HasByteOffset() { return true; }
        static constexpr bool HasCharOffset() { return true; }
        static constexpr bool HasLineOffset() { return true; }
        static constexpr bool HasSkipList() { return false; }
        static constexpr bool HasResourceName() { return false; }
    
        size_t ByteOffset = 0;
        size_t LineOffset = 0;
        size_t CharOffset = 0;
    };
    
    struct GlobalLocation {
        static constexpr bool HasByteOffset() { return true; }
        static constexpr bool HasCharOffset() { return true; }
        static constexpr bool HasLineOffset() { return true; }
        static constexpr bool HasSkipList() { return false; }
        static constexpr bool HasResourceName() { return true; }
        
        size_t ByteOffset = 0;
        size_t LineOffset = 0;
        size_t CharOffset = 0;
        std::optional<std::string> ResourceName = "";
    };
    
    struct InnerLocation {
        static constexpr bool HasByteOffset() { return true; }
        static constexpr bool HasCharOffset() { return true; }
        static constexpr bool HasLineOffset() { return true; }
        static constexpr bool HasSkipList() { return false; }
        static constexpr bool HasResourceName() { return true; }
    
        size_t ByteOffset = 0;
        size_t LineOffset = 0;
        size_t CharOffset = 0;
        std::map<size_t, LineLocation> skiplist;
        
    };
    
    struct GlobalInnerLocation {
        static constexpr bool HasByteOffset() { return true; }
        static constexpr bool HasCharOffset() { return true; }
        static constexpr bool HasLineOffset() { return true; }
        static constexpr bool HasSkipList() { return true; }
        static constexpr bool HasResourceName() { return true; }
    
        size_t ByteOffset = 0;
        size_t LineOffset = 0;
        size_t CharOffset = 0;
        std::optional<std::string> ResourceName = "";
        std::map<size_t, GlobalLocation> skiplist;
    };
    
    //TODO: Redo location to add support for filename
    //      Difference between byte and char offset
    
    
    struct Path {    
        enum Type {
            Sequence,
            Map
        };
        
        struct Entry {
            Type type;
            std::variant<std::string_view, int> entry;
            std::map<std::string, std::string> attributes;
        };
        
        std::vector<Entry> entries;
    };
    
    
    template<class LocationType>
    struct Context {
        LocationType location;
        Path path;
    };

}