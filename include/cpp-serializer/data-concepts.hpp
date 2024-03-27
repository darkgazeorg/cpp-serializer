#pragma once

#include <concepts>
#include <string_view>
#include <type_traits>

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
     * data type for storage and converter. You may supply void to disable any type. However, at 
     * least one type should be set for system to work. Storage type could be derived from 
     * std::variant, std::any or if only a single type is set, it could simply be that type.
     */
    template<class T_>
    concept DataTraitConcept = requires {
        typename T_::StorageType;
        
        //Data types, use void to disable
        typename T_::NumberType;
        typename T_::RealType;
        typename T_::IntegerType;        
        typename T_::StringType;
        typename T_::NullType; //either void to disable or nullptr_t        
        typename T_::BoolType;
        
        //compound types
        //either set both to void or none
        typename T_::KeyType;
        typename T_::MapType;
        
        //either set both to void or none
        typename T_::IndexType;
        typename T_::SequenceType;
        
        {T_::HasOffset()} -> std::same_as<bool>;
        {T_::HasSkipList()} -> std::same_as<bool>;
        
        typename T_::ConverterType;
        
        //checks
        
        CONCEPT_ASSERT(
            (std::is_same_v<typename T_::NumberType, void> || 
            (std::is_same_v<typename T_::RealType, void> && std::is_same_v<typename T_::IntegerType, void>)),
            "Type cannot have combined number type a long with separated integer/real types"
        );
        CONCEPT_ASSERT(
            (std::is_same_v<typename T_::IntegerType, void> || !std::is_same_v<typename T_::RealType, void>),
            "If type has integer, it should have real numbers as well"
        );
        CONCEPT_ASSERT(
            !(
                std::is_same_v<typename T_::NumberType, void> &&
                std::is_same_v<typename T_::IntegerType, void> && 
                std::is_same_v<typename T_::RealType, void> &&
                std::is_same_v<typename T_::StringType, void> && 
                std::is_same_v<typename T_::NullType, void> &&
                std::is_same_v<typename T_::BoolType, void>
            ),
            "Data should have at least one basic type set."
        );
        
        std::is_same_v<typename T_::MapType, void> || MapConcept<typename T_::MapType, typename T_::KeyType, typename T_::StorageType>;
        std::is_same_v<typename T_::KeyType, void> == std::is_same_v<typename T_::MapType, void>;
        std::is_same_v<typename T_::SequenceType, void> || SequenceConcept<typename T_::SequenceType, typename T_::IndexType, typename T_::StorageType>;
        std::is_same_v<typename T_::IndexType, void> == std::is_same_v<typename T_::SequenceType, void>;
        requires TypeConverterConcept<typename T_::ConverterType, typename T_::StorageType, Location<T_::HasOffset(), T_::HasSkipList()>>;
    
    };
    
    

}
    
#undef CONCEPT_ASSERT