#pragma once

#include <type_traits>
namespace CPPSerializer {
    
    template<class T_, template<class...> class O_>
    struct IsInstantiation : public std::false_type {};
    
    template<template<class...> class T_, class ...Args_>
    struct IsInstantiation<T_<Args_...>, T_> : public std::true_type {};

    template<class T_, template<class...> class O_>
    inline constexpr bool IsInstantiationV = IsInstantiation<T_, O_>{};

    template <class T, std::size_t = sizeof(T)>
    std::true_type hasimp(T *);
    
    std::false_type hasimp(...);
    
    template <class T>
    constexpr inline bool HasImp = decltype(hasimp(std::declval<T*>())){};
}