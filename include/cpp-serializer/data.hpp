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
        DataTraits_::HasSequence(), DataTraits_::HasMap(),
        typename internal::dataoptionalhelper<DataTraits_>::IndexType,
        typename internal::dataoptionalhelper<DataTraits_>::KeyType,
        typename DataTraits_::StorageType,
        typename internal::dataoptionalhelper<DataTraits_>::SequenceTypeType,
        typename internal::dataoptionalhelper<DataTraits_>::MapType
    > {
    public:
        /// Data traits definition.
        using DataTraits = DataTraits_;
        
        /// Storage data type definition
        using StorageType = DataTraits_::StorageType;
    
    private:
        StorageType data;
        
        [[no_unique_address]]
        Location<DataTraits::HasOffset(), DataTraits::HasSkipList()> location;
    };

}
