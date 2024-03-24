#pragma once

#include <map>


namespace CPPSerializer::internal {


template<bool HasMap, class TraitType>
struct maptypehelper {
    using Type = TraitType::KeyType;
};

template<class TraitType>
struct maptypehelper<false, TraitType> {
    using Type = void;
};

template<class KeyType, class DataType>
class datamaphelper {
public:

    //Accessors

protected:
    std::map<KeyType, DataType> map;
};

template<class DataType>
class datamaphelper<void, DataType> {

};


}