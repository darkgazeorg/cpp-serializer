#pragma once

#include "cpp-serializer/location.hpp"
#include <concepts>
#include <string_view>
#include <type_traits>

#define CONCEPT_ASSERT(check, message) []{ static_assert(check, message); return check; }()


namespace CPPSerializer {
    
    template<class LocationType>
    struct Context;
    
    /**
     * Type parser concepts will take a context and a string and should convert that string
     * to a type that is compatible with the selected storage type. It also should convert the 
     * context to the location type that is selected.
     */
    template <class T_, class StorageType, class LocationType>
    concept DataParserConcept = requires(T_ t, Context<LocationType> c, std::string_view s) {
        {t(c, s)} -> std::same_as<std::pair<LocationType, StorageType>>;
    };
    
    /**
     * Type emitter concepts will take a context and storage type and should convert it to a
     * type compatible with string_view
     */
    template <class T_, class StorageType>
    concept DataEmitterConcept = requires(T_ t, StorageType s) {
        {t(s)} -> std::convertible_to<std::string_view>;
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
     * Location should supply functions that allows discovery of its abilities. Additionally,
     * any ability that is set should have the necessary data member.
     */
    template<class T_>
    concept LocationConcept = requires (T_ t, ByteOffset bo) {
        {T_::HasByteOffset()} -> std::same_as<bool>;
        {T_::HasCharOffset()} -> std::same_as<bool>;
        {T_::HasLineOffset()} -> std::same_as<bool>;
        {T_::HasResourceName()} -> std::same_as<bool>;
        {T_::HasSkipList()} -> std::same_as<bool>;
        
        !T_::HasByteOffset() || requires { requires std::same_as<decltype(t.ByteOffset), size_t>; };
        !T_::HasCharOffset() || requires { requires std::same_as<decltype(t.CharOffset), size_t>; };
        !T_::HasLineOffset() || requires { requires std::same_as<decltype(t.LineOffset), size_t>; };
        !T_::HasResourceName() || requires { t.ResourceName; };
        !T_::HasSkipList() || requires {
            t.SkipList;
            t.Obtain(bo) -> LocationConcept;
        };
    };
    
    /**
     * @brief Concept for data traits.
     * This concept checks traits type for data storage. At the basic level, DataTraits should supply
     * data type for storage and converter. You may supply void to disable any type. However, at 
     * least one type should be set for system to work. Storage type could be derived from 
     * std::variant, std::any or if only a single type is set, it could simply be that type.
     * TODO: Explain type parser, emitter
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
        
        typename T_::DataParserType;
        typename T_::DataEmitterType;
        typename T_::LocationType;
        
        
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
        
        std::is_same_v<typename T_::DataParserType,  void> || DataParserConcept <typename T_::DataParserType , typename T_::StorageType, typename T_::LocationType>;
        std::is_same_v<typename T_::DataEmitterType, void> || DataEmitterConcept<typename T_::DataEmitterType, typename T_::StorageType>;
    
        requires LocationConcept<typename T_::LocationType>;
    };
    
    template<class T_>
    concept DataConcept = requires { 
        typename T_::StorageType;
        requires DataTraitConcept<typename T_::DataTraits>;
    };

}
    
#undef CONCEPT_ASSERT