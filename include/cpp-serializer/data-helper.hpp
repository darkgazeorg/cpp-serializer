#pragma once

#include "data-concepts.hpp"
#include "location.hpp"

#include <variant>


namespace CPPSerializer::internal {
    template<bool HasMap, class TraitType>
    struct datakeyhelper {
        using Type = TraitType::KeyType;
    };
    
    template<class TraitType>
    struct datakeyhelper<false, TraitType> {
        using Type = void;
    };

    template<bool HasMap, class TraitType>
    struct datamaphelper {
        using Type = TraitType::KeyType;
    };
    
    template<class TraitType>
    struct datamaphelper<false, TraitType> {
        using Type = void;
    };
    
    template<bool HasSequence, class TraitType>
    struct dataindexhelper {
        using Type = TraitType::IndexType;
    };

    template<class TraitType>
    struct dataindexhelper<false, TraitType> {
        using Type = void;

    };
    
    template<bool HasSequence, class TraitType>
    struct datasequencehelper {
        using Type = TraitType::SequenceType;
    };

    template<class TraitType>
    struct datasequencehelper<false, TraitType> {
        using Type = void;
    };
    
    
    template<DataTraitConcept TraitType>
    struct dataoptionalhelper {
        using IndexType = dataindexhelper<TraitType::HasSequence(), typename TraitType::IndexType>;
        using KeyType = datakeyhelper<TraitType::HasMap(), typename TraitType::KeyType>;
        using SequenceType = datasequencehelper<TraitType::HasSequence(), typename TraitType::SequenceType>;
        using MapType = datamaphelper<TraitType::HasMap(), typename TraitType::MapType>;
    };

    
    
    template<
        bool Vector, bool Map, class IndexType, class KeyType, 
        class StorageType, class SequenceType, class MapType
    > 
    requires Vector && Map && 
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

    template<class IndexType, class StorageType, class SequenceType>
    requires 
            (!std::same_as<IndexType, void>) && 
            (!std::same_as<SequenceType, void>)
    class datadatahelper<true, false, IndexType, void, StorageType, SequenceType, void> {
    protected:
        std::variant<std::nullptr_t, StorageType, SequenceType> data;
    };

    template<class KeyType, class StorageType, class MapType>
    requires 
            (!std::same_as<KeyType, void>) && 
            (!std::same_as<MapType, void>)
    class datadatahelper<false, true, void, KeyType, StorageType, void, MapType> {
    protected:
        std::variant<std::nullptr_t, StorageType, MapType> data;
    };
    
    template<class StorageType>
    class datadatahelper<false, false, void, void, StorageType, void, void> {
    protected:
        std::variant<std::nullptr_t, StorageType> data;
    };
    
    template<
        DataTraitConcept DataTraits,
        bool Vector, bool Map, class IndexType, class KeyType, 
        class StorageType, class SequenceType, class MapType
    > 
    class datahelper : public datadatahelper<Vector, Map, IndexType, KeyType, StorageType, SequenceType, MapType> {
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
        bool Vector, class IndexType, 
        class StorageType, class SequenceType
    > 
    class datahelper<DataTraits, Vector, false, IndexType, void, StorageType, SequenceType, void> : 
        public datadatahelper<Vector, false, IndexType, void, StorageType, SequenceType, void> 
    { };


}