#pragma once

#include "tmp.hpp"

#include <cassert>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace CPPSerializer {
    
    template<class T_>
    class Source;
    
    template<>
    class Source<std::string_view> {
    public:
        Source(const std::string_view &source_) : source(source_) { }
        
        ~Source() {}
    
        char Get() {
            assert(location < source.size());
            
            return source[location++];
        }
        
        char Peek() const {
            assert(location < source.size());
        
            return source[location];
        }
        
        char PeekNext(size_t forward = 1) {
            assert(location + forward < source.size());
        
            return source[location + forward];
        }
        
        std::string_view Read(size_t size) {            
            if(location < source.size()) {
                auto ret = source.substr(location, size);
                location += size;
                location = location > source.size() ? source.size() : location;
                
                return ret;
            }
            else {
                return {};
            }
        }
        
        bool IsEof() const {
            return !(location < source.size());
        }
        
        std::optional<char> TryGet() {
            if(IsEof()) return std::nullopt;
            
            return Get();
        }
        
        std::optional<char> TryPeek() {
            if(IsEof()) return std::nullopt;
            
            return Peek();
        }
        
        std::optional<char> TryPeekNext(size_t forward = 1) {
            if(location + forward >= source.size()) return std::nullopt;
            
            return PeekNext(forward);
        }
        
        size_t Tell() const {
            return location;
        }
        
        size_t Remaining() const {
            return source.size() - location;
        }
    
        size_t Size() const {
            return source.size();
        }
        
        void Advance(size_t forward = 1) {
            location += forward;
            location = location > source.size() ? source.size() : location;
        }
        
        std::optional<std::string> GetResourceName() const {
            return std::nullopt;
        }
    
    private:
        const  std::string_view source;
        size_t location = 0;
    };
    
    //TODO: specialize for std::path, ifstream
    
    template<class T_>
    auto make_source(T_ &obj) {
        if constexpr(IsInstantiationV<T_, Source>) {
            return obj;
        }
        
        if constexpr(HasImp<Source<std::decay<T_>>>) {
            return Source{obj};
        }
        else if constexpr(std::is_convertible_v<T_, const std::string_view>) {
            return Source<std::string_view>{obj};
        }
        else return Source{obj};
    }


}