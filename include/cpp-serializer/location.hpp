#pragma once

#include "config.hpp"

#include "cpp-serializer/concepts.hpp"
#include "cpp-serializer/utf.hpp"
#include <map>
#include <optional>
#include <variant>
#include <vector>
#include <string>
#include <string_view>

namespace CPP_SERIALIZER_NAMESPACE {
    
    namespace internal {
        template<LocationConcept Parent, bool skiplist = Parent::HasSkipList()> 
        Parent::ObtainedType ObtainLocation(const Parent &source, size_t byte_offset, const std::string_view &data) {
            auto loc = static_cast<Parent::ObtainedType>(source);
            auto off = size_t{0};
            
            //use skip list if available
            if constexpr(skiplist) {
                auto it = source.SkipList.upper_bound(byte_offset); 

                if(!source.SkipList.empty() && it != begin(source.SkipList)) 
                    it = std::prev(it);
                else
                    it = end(source.SkipList);

                if(it != end(source.SkipList)) {
                    loc = it->second;
                    off = it->first;
                }
            }
            
            bool prevn = false;
            //go through the data until we hit the byte offset we want
            while(off < byte_offset && off < data.size()) {
                //get a character and move offset by how many bytes are
                //necessary to move that utf8 character
                auto c = data[off];
                off += UTF8Bytes(c);

                if(c == '\n') {
                    prevn = true;
                    if constexpr(Parent::ObtainedType::HasLineOffset()) loc.LineOffset++;
                    loc.CharOffset = 1;
                }
                else if(c == '\r') {
                    if(prevn) prevn = false;
                    else {
                        if constexpr(Parent::ObtainedType::HasLineOffset()) loc.LineOffset++;
                        loc.CharOffset = 1;
                    }
                }
                else {
                    loc.CharOffset++;
                    prevn = false;
                }
            }

            if(off != byte_offset) loc.CharOffset += byte_offset - off;
            
            return loc;
        }
    }
    
    /**
     * This structure stores no location information but 
     * satisfies location concept.
     */
    struct NoLocation {
        using ObtainedType = NoLocation;

        NoLocation(size_t, size_t ,size_t) { }
        NoLocation() { }

        static constexpr bool HasByteOffset() { return false; }
        static constexpr bool HasCharOffset() { return false; }
        static constexpr bool HasLineOffset() { return false; }
        static constexpr bool HasSkipList() { return false; }
        static constexpr bool HasResourceName() { return false; }

        /// Obtains line location from the given offset and data
        auto Obtain(size_t, const std::string_view &) {
            return ObtainedType{};
        }
    };
    
    /**
     * Only stores byte offset from the start of the source.
     */
    struct ByteLocation {
        using ObtainedType = ByteLocation;

        ByteLocation() = default;
        
        ByteLocation(size_t byte_offset, size_t, size_t) :
            ByteLocation(byte_offset) 
        { }

        ByteLocation(size_t byte_offset) :
            ByteOffset(byte_offset) 
        { }
        
        static constexpr bool HasByteOffset() { return true; }
        static constexpr bool HasCharOffset() { return false; }
        static constexpr bool HasLineOffset() { return false; }
        static constexpr bool HasSkipList() { return false; }
        static constexpr bool HasResourceName() { return false; }

        /// Obtains line location from the given offset and data
        auto Obtain(size_t byte_offset, const std::string_view &) {
            return ByteLocation{byte_offset};
        }
        
        size_t ByteOffset = 0;
    };
    
    /*
     * Stores both character and byte offset from the start of the source.
     * Character offset is in unicode code points.
     */
    struct Offset {
        using ObtainedType = Offset;

        Offset() = default;

        Offset(size_t byte_offset, size_t, size_t char_offset) :
            Offset(byte_offset, char_offset)
        { }

        Offset(size_t byte_offset, size_t char_offset) :
            ByteOffset(byte_offset),
            CharOffset(char_offset)
        { }
        
        static constexpr bool HasByteOffset() { return true; }
        static constexpr bool HasCharOffset() { return true; }
        static constexpr bool HasLineOffset() { return false; }
        static constexpr bool HasSkipList() { return false; }
        static constexpr bool HasResourceName() { return false; }

        /// Obtains line location from the given offset and data
        auto Obtain(size_t byte_offset, const std::string_view &data) {
            return internal::ObtainLocation(*this, byte_offset, data);
        }
        
        size_t ByteOffset = 0;
        size_t CharOffset = 0;
    };
    
    /**
     * Stores line and character offset from the current line.
     * Character offset is in unicode code points.
     */
    struct LineLocation {
        using ObtainedType = LineLocation;
        
        LineLocation() = default;

        LineLocation(size_t,  size_t line_offset, size_t char_offset) :
            LineLocation(line_offset, char_offset)
        { }
        
        LineLocation(size_t line_offset, size_t char_offset) :
            LineOffset(line_offset),
            CharOffset(char_offset)
        { }
        
        
        static constexpr bool HasByteOffset() { return false; }
        static constexpr bool HasCharOffset() { return true; }
        static constexpr bool HasLineOffset() { return true; }
        static constexpr bool HasSkipList() { return false; }
        static constexpr bool HasResourceName() { return false; }
    
        size_t LineOffset = 0;
        size_t CharOffset = 0;
        
        /// Obtains line location from the given offset and data
        auto Obtain(size_t byte_offset, const std::string_view &data) {
            return internal::ObtainLocation(*this, byte_offset, data);
        }
    };
    
    /**
     * Stores line and character offset from the start of the 
     * current line in unicode points. Additionally, if available,
     * name of the source is also stored.
     */
    struct GlobalLocation {
        using ObtainedType = GlobalLocation;

        GlobalLocation() = default;
        
        GlobalLocation(size_t,  size_t line_offset, size_t char_offset) :
            GlobalLocation(line_offset, char_offset, std::nullopt)
        { }
        
        GlobalLocation(size_t line_offset, size_t char_offset, const std::optional<std::string> &resource_name) :
            LineOffset(line_offset),
            CharOffset(char_offset),
            ResourceName(resource_name)
        { }
        
        static constexpr bool HasByteOffset() { return false; }
        static constexpr bool HasCharOffset() { return true; }
        static constexpr bool HasLineOffset() { return true; }
        static constexpr bool HasSkipList() { return false; }
        static constexpr bool HasResourceName() { return true; }
        
        size_t LineOffset = 0;
        size_t CharOffset = 0;
        std::optional<std::string> ResourceName = "";
        
        /// Obtains line location from the given offset and data
        auto Obtain(size_t byte_offset, const std::string_view &data) {
            return internal::ObtainLocation(*this, byte_offset, data);
        }
    };
    
    /**
     * Inner location stores LineLocation information, but it also
     * contains skiplist allowing it to transform any additional
     * byte offset to exact line and character offset. However, this
     * transformation requires text to ensure unicode compatibility.
     * If the text is not supplied, encoding is assumed to be
     * ASCII. Skip list is only necessary if there is whitespace
     * folding or escape character. It might however, improve lookup
     * speed for the cost of additional memory.
     */
    struct InnerLocation {
        using ObtainedType = LineLocation;
        
        InnerLocation() = default;

        InnerLocation(size_t byte_offset,  size_t line_offset, size_t char_offset) :
            ByteOffset(byte_offset),
            LineOffset(line_offset),
            CharOffset(char_offset)
        { }

        static constexpr bool HasByteOffset() { return true; }
        static constexpr bool HasCharOffset() { return true; }
        static constexpr bool HasLineOffset() { return true; }
        static constexpr bool HasSkipList() { return true; }
        static constexpr bool HasResourceName() { return true; }
        
        /// Converts to a type without skip list
        operator ObtainedType() const {
            return {LineOffset, CharOffset};
        }
    
        size_t ByteOffset = 0;
        size_t LineOffset = 0;
        size_t CharOffset = 0;
        std::map<size_t, ObtainedType> SkipList;
        
        /// Obtains line location from the given offset and data
        auto Obtain(size_t byte_offset, const std::string_view &data) {
            return internal::ObtainLocation(*this, byte_offset, data);
        }
    };
    
    /**
     * Similar to InnerLocation, but in addition stores resource
     * name.
     */
    struct GlobalInnerLocation {
        using ObtainedType = GlobalLocation;

        GlobalInnerLocation() = default;

        GlobalInnerLocation(size_t byte_offset,  size_t line_offset, size_t char_offset) :
            GlobalInnerLocation(byte_offset, line_offset, char_offset, std::nullopt)
        { }

        GlobalInnerLocation(size_t byte_offset,  size_t line_offset, size_t char_offset, const std::optional<std::string> &resource_name) :
            ByteOffset(byte_offset),
            LineOffset(line_offset),
            CharOffset(char_offset),
            ResourceName(resource_name)
        { }
        
        static constexpr bool HasByteOffset() { return true; }
        static constexpr bool HasCharOffset() { return true; }
        static constexpr bool HasLineOffset() { return true; }
        static constexpr bool HasSkipList() { return true; }
        static constexpr bool HasResourceName() { return true; }
        
        /// Converts to a type without skip list
        operator ObtainedType() const {
            return {LineOffset, CharOffset, ResourceName};
        }
    
        size_t ByteOffset = 0;
        size_t LineOffset = 0;
        size_t CharOffset = 0;
        std::optional<std::string> ResourceName = "";
        std::map<size_t, GlobalLocation> SkipList;
        
        /// Obtains line location from the given offset and data
        auto Obtain(size_t byte_offset, const std::string_view &data) {
            return internal::ObtainLocation(*this, byte_offset, data);
        }
    };

#include "macros.hpp"

    template<LocationConcept LocationType, SourceConcept Source>
    void SetSkip(bool skiplist, LocationType &location, const Source &reader, size_t offset, size_t line_offset, size_t char_offset) {
        CPPSER_IF_MIXED(LocationType::HasSkipList(), skiplist) {
            auto curlocation = typename LocationType::ObtainedType{};

            if constexpr(LocationType::ObtainedType::HasByteOffset()) {
                curlocation.ByteOffset = reader.Tell();
            }
            
            if constexpr(LocationType::ObtainedType::HasLineOffset()) {
                curlocation.LineOffset = line_offset;
            }
            
            if constexpr(LocationType::ObtainedType::HasCharOffset()) {
                curlocation.CharOffset = char_offset;
            }
                                    
            if constexpr(LocationType::ObtainedType::HasResourceName()) {
                curlocation.ResourceName = reader.GetResourceName();
            }
            
            location.SkipList[offset] = curlocation;
        }
    }

#include "unmacro.hpp"


    template<LocationConcept LocationType, SourceConcept Source>
    void AddNewLine(bool skiplist, LocationType &location, const Source &reader, size_t offset, size_t &line_offset) {
        SetSkip(skiplist, location, reader, offset, ++line_offset, 1);
    }
    
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