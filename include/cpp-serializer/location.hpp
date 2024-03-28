#pragma once

#include <map>
#include <variant>
#include <vector>
#include <string_view>

namespace CPPSerializer {
    
    //TODO: Redo location to add support for filename
    //      Difference between byte and char offset

    template<bool Offset_, bool SkipList_>
    struct Location {
        static_assert(false, "Skip list cannot be used without offset");
    };
    
    using FileOffset = Location<true, false>;
    
    template<>
    struct Location<true, false> {
        size_t Line{}, Char{};
        
        /// This function cannot canonicalize line offsets from character offset
        FileOffset Offset(size_t char_offset) const {
            return {Line, Char + char_offset};
        }
    };
    
    template<>
    struct Location<false, false> {
        /// This function simply returns back the given char offset
        FileOffset Offset(size_t char_offset) const {
            return {0, char_offset};
        }
    };
    
    template<>
    struct Location<true, true> {
        size_t Line{}, Char{};
        std::map<size_t, FileOffset> skiplist;
        
        FileOffset Offset(size_t char_offset) const {
            if(auto it = skiplist.upper_bound(char_offset); it != skiplist.begin())
                return {Line + it->second.Line, Char + char_offset + it->second.Char};
            else
                return {Line, Char + char_offset};
        }
    };

    
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
    
    
    template<class OffsetType>
    struct Context {
        OffsetType offset;
        Path path;
    };

}