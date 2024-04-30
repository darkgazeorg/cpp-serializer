#pragma once

#include "config.hpp"

#include "cpp-serializer/tmp.hpp"
#include "data-helper.hpp"
#include <any>
#include <string_view>
#include <variant>


namespace CPP_SERIALIZER_NAMESPACE {
    
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
        typename DataTraits_::StorageType,
        typename DataTraits_::IndexType,
        typename DataTraits_::KeyType,
        typename DataTraits_::SequenceType,
        typename DataTraits_::MapType
    > {
    public:
        /// Data traits definition.
        using DataTraits = internal::datatraithelper<DataTraits_>;
        
        /// Storage data type definition
        using StorageType = DataTraits::StorageType;
    
        template<class T_>
        void SetData(const T_ &val) {
            this->data = StorageType{val};
        }
        
        StorageType GetData() const {
            return std::get<StorageType>(this->data);
        }

        DataTraits::LocationType GetLocation() const {
            return location;
        }

        void SetLocation(const DataTraits::LocationType &value) {
            location = value;
        }

        auto GetLocation(size_t offset) {
            if constexpr(!std::is_same_v<typename DataTraits::StringType, void>) {
                if constexpr(IsInstantiationV<StorageType, std::variant>) {
                    auto data = GetData();

                    if(std::holds_alternative<typename DataTraits::StringType>(data))
                        return location.Obtain(offset, std::get<typename DataTraits::StringType>(data));
                }
                else if constexpr(std::is_same_v<StorageType, std::any>) {
                    auto data = GetData();

                    if(data.type() == typeid(StorageType)) {
                        return location.Obtain(offset, std::any_cast<typename DataTraits::StringType>(data));
                    }
                }
                else if constexpr(std::is_convertible_v<StorageType, std::string_view>) {
                    return location.Obtain(offset, std::any_cast<typename DataTraits::StringType>(GetData()));
                }
            }
                
            return location.Obtain(offset, "");;
        }
    
    private:
        [[no_unique_address]]
        DataTraits::LocationType location;
    };

}
