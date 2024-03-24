#pragma once

#include "data-helper.hpp"

#include <optional>
#include <string_view>
#include <variant>
#include <vector>
#include <map>
#include <string>


namespace CPPSerializer {
    
    template<bool Offset, bool SkipList>
    struct Location {
    
    };
    
    template<>
    struct Location<true, false> {
        size_t line, char_offset;
    };
    
    template<>
    struct Location<false, true> {
        std::map<size_t, size_t> skiplist;
    };
    
    template<>
    struct Location<true, true> {
        size_t line, char_offset;
        std::map<size_t, size_t> skiplist;
    };
    
    using FileOffset = Location<true, false>;
    
    struct TypeTag {
        enum Type {
            None,
            Bool,
            Int,
            Float,
            Double,
            Real,
            Number,
            Date,
            Null,
            String,
            Binary,
            Sequence,
            Map,
            Custom
        } type;
        
        std::string name;
    };
    
    struct Path {    
        enum Type {
            Sequence,
            Map
        };
        
        struct Entry {
            Type type;
            std::variant<std::string_view, int> entry;
            std::optional<TypeTag> tag;
        };
        
        std::vector<Entry> entries;
    };
    
    struct Context {
        FileOffset offset;
        Path path;
    };
    
    template<DataTraitConcept DataTraits_>
    class Data : internal::datahelper<
        DataTraits_::HasSequence(), DataTraits_::HasMap(),
        typename internal::dataoptionalhelper<DataTraits_>::IndexType,
        typename internal::dataoptionalhelper<DataTraits_>::KeyType,
        typename DataTraits_::StorageType,
        typename internal::dataoptionalhelper<DataTraits_>::SequenceTypeType,
        typename internal::dataoptionalhelper<DataTraits_>::MapType
    > {
    public:
        using DataTraits = DataTraits_;
        using StorageType = DataTraits_::StorageType;
    
    private:
        StorageType data;
    };

}
