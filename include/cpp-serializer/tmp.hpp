#pragma once

#include "types.hpp"
#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

namespace CPP_SERIALIZER_NAMESPACE {

    void ConstEvalError(auto);
    
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

    
    template<class T_> concept StringLike = std::is_convertible_v<T_, const std::string_view>;
    
    template<YesNoRuntime ...Opts>
    struct GetLastOpt;

    template<YesNoRuntime Opt, YesNoRuntime Opt2, YesNoRuntime ...Rest>
    struct GetLastOpt<Opt, Opt2, Rest...> {
        static constexpr YesNoRuntime value = GetLastOpt<Opt2, Rest...>::value;
    };

    template<YesNoRuntime Opt>
    struct GetLastOpt<Opt> {
        static constexpr YesNoRuntime value = Opt;
    };
    
    template<YesNoRuntime state>
    struct GetMixedTimeHelper {
        template<size_t ind, class Tuple_>
        static auto Obtain(const Tuple_ &data) {
            return data[ind];
        }
    };
    
    template<>
    struct GetMixedTimeHelper<YesNoRuntime::No> {
        template<size_t ind, class Tuple_>
        static auto Obtain(const Tuple_&) {
            return false;
        }
    };
    
    
    template<>
    struct GetMixedTimeHelper<YesNoRuntime::Yes> {
        template<size_t ind, class Tuple_>
        static auto Obtain(const Tuple_&) {
            return true;
        }
    };

    /**
     * Get the Mixed Time option, supply options up to and including
     * the item to be obtained. If you want to get the second option:
     * GetMixedTimeOption<opt1, opt2>(data);
     */
    template<YesNoRuntime arg, int ind, class Tuple_>
    auto GetMixedTimeOption(const Tuple_& data) {
        return GetMixedTimeHelper<arg>::template Obtain<ind>(data);
    }
    
    template<class Type_, YesNoRuntime Opt>
    struct MaskedTupleImp {
        using Type = std::tuple<>;
    };
    
    template<class Type_>
    struct MaskedTupleImp<Type_, YesNoRuntime::Runtime> {
        using Type = std::tuple<Type_>;
    };
    
    template<YesNoRuntime ...Opts>
    consteval auto MaskedMixedTimeDataHelper() {
        return std::tuple_cat(typename MaskedTupleImp<bool, Opts>::Type{}...);
    }
    
    template<YesNoRuntime ...Opts>
    consteval auto MaskedMixedTimeData() {
        return MaskedMixedTimeDataHelper<Opts...>();
    }

}