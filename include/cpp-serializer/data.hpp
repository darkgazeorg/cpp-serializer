#pragma once

#include "data-helper.hpp"

namespace CPPSerializer {
    
    
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
        using DataTraits = DataTraits_;
        using StorageType = DataTraits_::StorageType;
    
    private:
        StorageType data;
    };

}
