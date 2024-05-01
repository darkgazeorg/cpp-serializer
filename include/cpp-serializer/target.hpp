#pragma once

#include "config.hpp"
#include "cpp-serializer/concepts.hpp"
#include "cpp-serializer/tmp.hpp"
#include <cassert>
#include <string>
#include <string_view>

namespace CPP_SERIALIZER_NAMESPACE {

    template<class T_>
    class Target;
    
    template<>
    class Target<std::string> {
    public:
        /// The data emitted will be stored in the given string
        Target(std::string &target_) : target(target_) { }
        
        ~Target() {}
    
        void Put(const std::string_view &data) {
            target += data;
        }
        
        void Put(char &data) {
            target.push_back(data);
        }
        
        /// Returns the current location of the write pointer
        size_t Tell() const {
            return target.size();
        }
        
        
    private:
        std::string &target;
    };
    
    template<>
    class Target<void> {
    public:
        /// The data emitted will be stored in the given string
        Target() { }
        
        ~Target() {}
    
        void Put(const std::string_view &data) {
            target += data;
        }
        
        void Put(char &data) {
            target.push_back(data);
        }
        
        /// Returns the current location of the write pointer
        size_t Tell() const {
            return target.size();
        }
        
        const std::string &Get() const {
            return target;
        }
        
        std::string Get() {
            return target;
        }
        
    private:
        std::string target;
    };
    
    //TODO: specialize for std::path, ifstream
    
    template<class T_> concept TargetInstatiation = IsInstantiationV<T_, Target>;
    template<class T_> concept TargetSpecializedFor = HasImp<Target<std::decay<T_>>>;
    
    template<bool Translate, class T_>
    struct make_target_type {
        using Type = T_;
    };
    
    template<TargetInstatiation T_>
    struct make_target_type<true, T_> {
        using Type = T_;
    };
    
    template<TargetSpecializedFor T_>
    struct make_target_type<true, T_> {
        using Type = Target<T_>;
    };
    
    template<StringLike T_>
    struct make_target_type<true, T_> {
        using Type = Target<std::string>;
    };
    
    /**
     * @brief Converts given object to target.
     * Converts given object to target by checking its type. If translate is false or object
     * is derived from Target<> directly returns the object. If Target is specialized for the
     * given object, that is returned. Then the function checks is the object is a type of 
     * string, if so, a Target<string_view> is returned.
     * @tparam Translate If set to false, the given object is returned
     * @return auto An object guaranteed to satisfy TargetConcept.
     */
    template<bool Translate = true, class T_>
    auto make_target(T_ &obj) requires TargetConcept<typename make_target_type<Translate, T_>::Type> {
        if constexpr(IsInstantiationV<T_, Target> || !Translate) {
            return obj;
        }
        else if constexpr(HasImp<Target<std::decay<T_>>>) {
            return Target{obj};
        }
        else if constexpr(std::is_convertible_v<T_, const std::string>) {
            return Target<std::string>{obj};
        }
        else return obj;
    }



}