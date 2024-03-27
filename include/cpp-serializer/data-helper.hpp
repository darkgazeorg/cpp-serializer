#pragma once

#include "data-concepts.hpp"
#include "location.hpp"

#include <variant>


namespace CPPSerializer::internal {
    //These structs turn optional types into solid types
    
    template<DataTraitConcept DataTraits>
    struct datatraithelper : public DataTraits {
        static constexpr bool HasNumber() {
            return !std::is_same_v<typename DataTraits::NumberType, void>;
        }
        static constexpr bool HasInteger() {
            return !std::is_same_v<typename DataTraits::IntegerType, void>;
        }
        static constexpr bool HasRead() {
            return !std::is_same_v<typename DataTraits::RealType, void>;
        }
        static constexpr bool HasNull() {
            return !std::is_same_v<typename DataTraits::NullType, void>;
        }
        static constexpr bool HasBool() {
            return !std::is_same_v<typename DataTraits::BoolType, void>;
        }
        static constexpr bool HasString() {
            return !std::is_same_v<typename DataTraits::StringType, void>;
        }
        static constexpr bool HasSequence() {
            return !std::is_same_v<typename DataTraits::SequenceType, void>;
        }
        static constexpr bool HasMap() {
            return !std::is_same_v<typename DataTraits::MapType, void>;
        }
    };
    
    template<
        class StorageType, 
        class IndexType, class KeyType, 
        class SequenceType, class MapType
    > 
    requires 
            (!std::same_as<IndexType, void>) && 
            (!std::same_as<KeyType, void>) && 
            (!std::same_as<SequenceType, void>) && 
            (!std::same_as<MapType, void>)
    class datadatahelper {
    protected:
        /// The data stored in this data object. nullptr here denotes data is not set
        /// at all, not that it contains a null value.
        std::variant<std::nullptr_t, StorageType, SequenceType, MapType> data;
    };
    
    template<class StorageType, class IndexType, class SequenceType>
    requires 
            (!std::same_as<IndexType, void>) && 
            (!std::same_as<SequenceType, void>)
    class datadatahelper<StorageType, IndexType, void, SequenceType, void> {
    protected:
        std::variant<std::nullptr_t, StorageType, SequenceType> data;
    };
    
    template<class KeyType, class StorageType, class MapType>
    requires 
            (!std::same_as<KeyType, void>) && 
            (!std::same_as<MapType, void>)
    class datadatahelper<StorageType, void, KeyType, void, MapType> {
    protected:
        std::variant<std::nullptr_t, StorageType, MapType> data;
    };
    
    template<class StorageType>
    class datadatahelper<StorageType, void, void, void, void> {
    protected:
        std::variant<std::nullptr_t, StorageType> data;
    };
    
    template<
        DataTraitConcept DataTraits,
        class StorageType, 
        class IndexType, class KeyType, 
        class SequenceType, class MapType
    > 
    class datahelper : public datadatahelper<StorageType, IndexType, KeyType, SequenceType, MapType> {
    public:
        
        bool KeyExists(const KeyType &key) const {
            if(std::holds_alternative<SequenceType>(this->data)) {
                const auto &map = std::get<SequenceType>(this->data);
                if(auto it = map.find(key); it != end(map))
                    return true;
            }
            
            return false;
        }
    
    protected:
        std::map<KeyType, Location<DataTraits::HasOffset(), DataTraits::HasSkipList()>> key_locations;
    };
    
    
    template<
        DataTraitConcept DataTraits,
        class IndexType, 
        class StorageType, class SequenceType
    > 
    class datahelper<DataTraits, StorageType, IndexType, void, SequenceType, void> : 
        public datadatahelper<StorageType, IndexType, void, SequenceType, void> 
    { };


}
