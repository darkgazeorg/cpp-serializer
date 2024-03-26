#pragma once

#include <concepts>
#include <string_view>

#define CONCEPT_ASSERT(check, message) []{ static_assert(check, message); return check; }()
    

namespace CPPSerializer {
    
    template<class OffsetType>
    struct Context;

    template<bool Offset, bool SkipList>
    struct Location;
    
    /**
     * Type converter concepts will take a context and a string and should convert that string
     * to a type that is compatible with the selected storage type. It also should convert the 
     * context to the location type that is selected.
     */
    template <class T_, class StorageType, class LocationType>
    concept TypeConverterConcept = requires(T_ t, Context<LocationType> c, std::string_view s) {
        {t(c, s)} -> std::same_as<std::pair<LocationType, StorageType>>;
    };
    
    /**
     * This concept defines an std::map like Map type. The map type should at least have
     * begin/end iterator, insert, erase, at and find functions as well as member access 
     * operator.
     */
    template<class T_, class Key, class Data>
    concept MapConcept = requires (T_ t, Key k, Data d) {
        requires std::forward_iterator<decltype(begin(t))>;
        requires std::forward_iterator<decltype(end(t))>;
        requires std::forward_iterator<decltype(t.find())>;
        
        {t[k]}            -> std::convertible_to<Data>;
        {t.at[k]}         -> std::convertible_to<Data>;
        
        {std::get<0>(*begin(t))} -> std::convertible_to<Key>;
        {std::get<1>(*begin(t))} -> std::convertible_to<Data>;
        
        t.insert(k, d);
    };
    
    /**
     * This concept defines an std::vector like sequence that at least support begin/end
     * iterator, size, push_back, insert, erase and clear functions as well as member
     * access operator
     */
    template<class T_, class Key, class Data>
    concept SequenceConcept = requires (T_ t, Key k, Data d) {
        requires std::forward_iterator<decltype(begin(t))>;
        requires std::forward_iterator<decltype(end(t))>;
        
        {t[k]}            -> std::convertible_to<Data>;
        
        {*begin(t)}       -> std::convertible_to<Data>;
        
        {t.size()}        -> std::same_as<Key>;
        
        t.push_back(d);
        t.insert(*begin(t), d);
        t.erase(*begin(t));
        t.clear();
    };
    
    /**
     * @brief Concept for data traits.
     * This concept checks traits type for data storage. At the basic level, DataTraits should supply
     * data type for storage and converter. Rest of the Has... functions could simply return false
     * to disable all other features. However, at least one type should be set for system to work.
     * Storage type could be derived from std::variant, std::any or if only a single type is set to
     * true, it could simply be that type.
     */
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
        CONCEPT_ASSERT(
            !T_::HasInteger() || T_::HasReal(), 
            "If type has integer, it should have real numbers as well"
        );
        
        {T_::HasString} -> std::same_as<bool>;
        !T_::HasString() || requires {typename T_::StringType;};
        
        {T_::HasNull} -> std::same_as<bool>;
        
        {T_::HasBool} -> std::same_as<bool>;
        !T_::HasBool() || requires {typename T_::BoolType;};
        
        CONCEPT_ASSERT(
            !T_::HasString() && !T_::HasNumber() &&
            !T_::HasBool() &&
            !T_::HasInteger() && !T_::HasReal(), "Data should have at least one basic type set.");
        
        {T_::HasMap} -> std::same_as<bool>;
        !T_::HasMap() || requires{typename T_::KeyType;};
        !T_::HasMap() || requires{typename T_::MapType;};
        !T_::HasMap() || MapConcept<typename T_::MapType, typename T_::KeyType, typename T_::StorageType>;
        
        {T_::HasSequence} -> std::same_as<bool>;
        !T_::HasSequence() || requires{typename T_::IndexType;};
        !T_::HasSequence() || requires{typename T_::SequenceType;};
        !T_::HasMap() || SequenceConcept<typename T_::SequenceType, typename T_::IndexType, typename T_::StorageType>;
        
        {T_::HasOffset} -> std::same_as<bool>;
        {T_::HasSkipList} -> std::same_as<bool>;
        
        typename T_::ConverterType;
        requires TypeConverterConcept<typename T_::ConverterType, typename T_::StorageType, Location<T_::HasOffset(), T_::HasSkipList()>>;
    };
    
    

}
    
#undef CONCEPT_ASSERT