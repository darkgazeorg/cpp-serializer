#pragma once

#include "cpp-serializer/types.h"
#include <string_view>
#include <tuple>
#include <type_traits>

namespace CPP_SERIALIZER_NAMESPACE {
    
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
    
    template<class IC_>
    struct DecrementToZero {
        using Type = std::integral_constant<int, IC_::value - 1>;
    };
    
    template<>
    struct DecrementToZero<std::integral_constant<int, 0>> {
        using Type = std::integral_constant<int, 0>;
    };
    
    template<int ...args>
    struct Sum;

    template<>
    struct Sum<> {
        static constexpr int value = 0;
    };

    template<int first, int ...args>
    struct Sum<first, args...> {
        static constexpr int value = first + Sum<args...>::value;
    };
    
    template<bool ...args>
    struct SumTrue;
    
    template<>
    struct SumTrue<> {
        static constexpr int value = 0;
    };
    
    template<bool first, bool ...args>
    struct SumTrue<first, args...> {
        static constexpr int value = first + Sum<args...>::value;
    };
    
    template<class IC_, bool ...args>
    struct SumTrueUntil;
    
    template<class IC_>
    struct SumTrueUntil<IC_> {
        static constexpr int value = 0;
    };
    
    template<bool first, bool ...args>
    struct SumTrueUntil<std::integral_constant<int, 0>, first, args...> {
        static constexpr int value = 0;
    };
    
    template<>
    struct SumTrueUntil<std::integral_constant<int, 0>> {
        static constexpr int value = 0;
    };
    
    template<class IC_, bool first, bool ...args>
    struct SumTrueUntil<IC_, first, args...> {
        static constexpr int value = first + SumTrueUntil<typename DecrementToZero<IC_>::Type, args...>::value;
    };
    
    template<bool state>
    struct GetIf {
        template<bool ...args, class Tuple_>
        static bool Obtain(Tuple_ data) {
            return std::get<SumTrue<args...>>(data);
        }
    };
    
    template<>
    struct GetIf<false> {
        template<class ...Args>
        static bool Obtain(Args ...) {
            return false;
        }
    };
    
    template<YesNoRuntime state>
    struct GetMixedTime {
        template<YesNoRuntime ...args, class Tuple_>
        static auto Obtain(Tuple_ data) {
            return std::get<SumTrue<args == YesNoRuntime::Runtime...>::value>(data);
        }
    };
    
    template<>
    struct GetMixedTime<YesNoRuntime::No> {
        template<YesNoRuntime ...args, class Tuple_>
        static auto Obtain(Tuple_) {
            return false;
        }
    };
    
    
    template<>
    struct GetMixedTime<YesNoRuntime::Yes> {
        template<YesNoRuntime ...args, class Tuple_>
        static auto Obtain(Tuple_) {
            return true;
        }
    };
    
    template<bool state>
    struct GetIfNot {
        template<bool ...args, class Tuple_>
        static bool Obtain(Tuple_ data) {
            return std::get<SumTrue<args...>>(data);
        }
    };
    
    template<>
    struct GetIfNot<true> {
        template<class ...Args>
        static bool Obtain(Args ...) {
            return true;
        }
    };
    
    template<class Type_, bool Opt>
    struct MaskTupleImp;
    
    template<class Type_>
    struct MaskTupleImp<Type_, false> {
        using Type = std::tuple<>;
    };
    
    template<class Type_>
    struct MaskTupleImp<Type_, true> {
        using Type = std::tuple<Type_>;
    };
    
    template<bool ...Opts>
    auto MaskedMixedType() {
        return std::tuple_cat(typename MaskTupleImp<bool, Opts>::Type{}...);
    }
    
    //this will not work?
    template<int index, bool ...Opts, class TupleType_>
    auto &MaskedGet(TupleType_ &data) {
        return std::get<SumTrueUntil<std::integral_constant<int, index>, Opts...>::value>(data);
    }
    
    //this will not work?
    template<int index, bool ...Opts, class TupleType_>
    const auto &MaskedGet(const TupleType_ &data) {
        return std::get<SumTrueUntil<std::integral_constant<int, index>, Opts...>::value>(data);
    }
    
}