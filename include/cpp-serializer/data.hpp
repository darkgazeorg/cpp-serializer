#pragma once

#include "data-helper.hpp"

#include <concepts>
#include <optional>
#include <string_view>
#include <variant>
#include <vector>
#include <string>


namespace CPPSerializer {
    
    #define CONCEPT_ASSERT(check, message) []{ static_assert(check, message); return check; }()
    
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
    
    template <class T_, class StorageType, class LocationType>
    concept TypeConverterConcept = requires(T_ t, Context c, std::string_view s) {
        {t(c, s)} -> std::same_as<std::pair<LocationType, StorageType>>;
    };

    
    template<class T_>
    concept DataTraitConcept = requires {
        typename T_::StorageType;
        
        {T_::HasNumber} -> std::same_as<bool>;
        !T_::HasNumber() || requires {typename T_::NumberType;};
        {T_::HasReal} -> std::same_as<bool>;
        !T_::HasReal() || requires {typename T_::RealType;};
        {T_::HasInteger} -> std::same_as<bool>;
        !T_::HasInteger() || requires {typename T_::IntegerType;};
        CONCEPT_ASSERT(
            !T_::HasNumber() || !(T_::HasReal() || T_::HasInteger()), 
            "Type cannot have combined number type a long with separated integer/real types"
        );
        CONCEPT_ASSERT(!T_::HasInteger() || T_::HasReal(), "If type has integer, it should have real numbers as well");
        
        typename T_::StringType;
        
        {T_::HasNull} -> std::same_as<bool>;
        
        {T_::HasBool} -> std::same_as<bool>;
        {!T_::HasBool() || requires {typename T_::BoolType;} };
        
        {T_::HasMap} -> std::same_as<bool>;
        !T_::HasMap() || requires{typename T_::KeyType;};
        
        {T_::HasFileOffset} -> std::same_as<bool>;
        {T_::HasSkipList} -> std::same_as<bool>;
        
        typename T_::ConverterType;
        
        requires TypeConverterConcept<typename T_::ConverterType, typename T_::StorageType, Location<T_::HasFileOffset(), T_::HasSkipList()>>;
    };
    
    #undef CONCEPT_ASSERT
        
    template<DataTraitConcept DataTraits_>
    class Data : internal::datamaphelper<typename internal::maptypehelper<DataTraits_::HasMap(), DataTraits_>::Type, Data<DataTraits_>> {
    public:
        using DataTraits = DataTraits_;
        using StorageType = DataTraits_::StorageType;
    
        
    };

}
