#pragma once

#include "data-helper.hpp"

namespace CPPSerializer {
    
    /**
     * @brief Data object to store generic data nodes.
     * Data object is the generic object that is used to store data.
     * This object can be used as intermediate data storage during
     * parse or emit; but also it can be used as the result of a parse
     * or source of emit operations. Depending on the traits, Data
     * can store scalars, sequence or maps. Additionally, it can 
     * contain Location information about where the data is stored.
     * @tparam DataTraits_ Defines what and how this data will store
     *         the data.
     */
    template<DataTraitConcept DataTraits_>
    class Data : internal::datahelper<
        DataTraits_,
        typename DataTraits_::IndexType,
        typename DataTraits_::KeyType,
        typename DataTraits_::StorageType,
        typename DataTraits_::SequenceTypeType,
        typename DataTraits_::MapType
    > {
    public:
        /// Data traits definition.
        using DataTraits = internal::datatraithelper<DataTraits_>;
        
        /// Storage data type definition
        using StorageType = DataTraits_::StorageType;
    
    private:
        StorageType data;
        
        [[no_unique_address]]
        Location<DataTraits::HasOffset(), DataTraits::HasSkipList()> location;
    };

}
