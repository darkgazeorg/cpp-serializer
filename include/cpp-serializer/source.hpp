#pragma once

#include "cpp-serializer/concepts.hpp"
#include "tmp.hpp"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstdio>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <iostream>

namespace CPP_SERIALIZER_NAMESPACE {
    
    /**
     * @brief Source allows parsers to read data.
     * Source class wraps supported parsers to obtain data from them
     * in a common way. Source class can work with forward only data.
     * 
     * @tparam T_ 
     */
    template<class T_>
    class Source;
    
    template<>
    class Source<std::string_view> {
    public:
        Source(const std::string_view &source_) : source(source_) { }
        
        ~Source() {}
    
        /// Returns a single character from this source, advances one step. Does not
        /// perform bounds checking
        char Get() {
            assert(location < source.size());
            
            return source[location++];
        }
        
        /// Returns a single character from this source. Does not perform bounds checking
        char Peek() const {
            assert(location < source.size());
        
            return source[location];
        }
        
        /// Returns a single character from this source from a forward position. Does not
        /// perform bounds checking
        char PeekNext(size_t forward = 1) const {
            assert(location + forward < source.size());
        
            return source[location + forward];
        }
        
        /// Reads a string data from the source upto the given size. 
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
        
        /// Returns true if no more data is available
        bool IsEof() const {
            return !(location < source.size());
        }
        
        /// Tries to obtain a single character. If at the end of the stream returns nullopt.
        std::optional<char> TryGet() {
            if(IsEof()) return std::nullopt;
            
            return Get();
        }
        
        /// Tries to obtain a single character without advancing read pointer. If at the end 
        /// of the stream returns nullopt.
        std::optional<char> TryPeek() const {
            if(IsEof()) return std::nullopt;
            
            return Peek();
        }
        
        /// Tries to obtain a single character without advancing read pointer. If at the end 
        /// of the stream returns nullopt.
        std::optional<char> TryPeekNext(size_t forward = 1) const {
            if(location + forward >= source.size()) return std::nullopt;
            
            return PeekNext(forward);
        }
        
        /// Returns the current location of the read pointer
        size_t Tell() const {
            return location;
        }
        
        /// Returns the remaining number of bytes. Since not every source can state its end,
        /// std::optional is returned. For string_view source, remaining size is always available
        std::optional<size_t> Remaining() const {
            return source.size() - location;
        }
    
        /// Returns the total number of bytes in stream. Since not every source can state its end,
        /// std::optional is returned. For string_view source, size is always available
        std::optional<size_t> Size() const {
            return source.size();
        }
        
        /// Advances the read pointer forwards. If EOF is encountered, no error will be produced and
        /// the pointer is left at the EOF position.
        void Advance(size_t forward = 1) {
            location += forward;
            location = location > source.size() ? source.size() : location;
        }
        
        /// Returns the name of this resource. Unless set using non-standard SetResourceName, it will be
        /// empty
        std::optional<std::string> GetResourceName() const {
            return resource_name;
        }
        
        /// Sets the resource name to the given name.
        void SetResourceName(const std::string_view &name) {
            resource_name = name;
        }
        
        /// Removes the resource name so it will be empty.
        void RemoveResourceName() {
            resource_name = std::nullopt;
        }
    
    private:
        std::optional<std::string> resource_name = std::nullopt;
        const  std::string_view source;
        size_t location = 0;
    };

    //TODO: Use std::streambuf instead
    template<>
    class Source<std::istream> {
    public:
        Source(std::istream &source_) : source(source_) { }
        
        ~Source() {}
    
        /// Returns a single character from this source, advances one step.
        char Get() {
            char c;
            assert(source.get(c));

            return c;
        }
        
        /// Returns a single character from this source. Does not perform bounds checking
        char Peek() const {
            auto ret = source.peek();
            assert(ret != EOF);

            return static_cast<char>(ret);
        }
        
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wsign-conversion"
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wsign-conversion"
        /// Returns a single character from this source from a forward position. Does not
        /// perform bounds checking
        char PeekNext(size_t forward = 1) const {
            assert(forward < static_cast<size_t>(std::numeric_limits<std::ios::pos_type>::max()));
            
            source.seekg(static_cast<std::ios::pos_type>(forward), std::ios::cur);

            auto ret = source.peek();
            assert(ret != EOF);

            auto c = static_cast<char>(ret);
            source.seekg(-static_cast<std::ios::pos_type>(forward), std::ios::cur);
        
            return c;
        }
        
        /// Tries to obtain a single character without advancing read pointer. If at the end 
        /// of the stream returns nullopt.
        std::optional<char> TryPeekNext(size_t forward = 1) const {
            assert(forward < static_cast<size_t>(std::numeric_limits<std::ios::pos_type>::max()));
            
            source.seekg(static_cast<std::ios::pos_type>(forward), std::ios::cur);

            auto ret = source.peek();
            if(ret == EOF) {
                source.seekg(-static_cast<std::ios::pos_type>(forward), std::ios::cur);
                return std::nullopt;
            }

            auto c = static_cast<char>(ret);
            source.seekg(-static_cast<std::ios::pos_type>(forward), std::ios::cur);
        
            return c;
        }

        /// Reads a string data from the source upto the given size. 
        std::string Read(size_t size) {
            if(size == 0) return "";

            auto maxsize = *Size();
            if(size > maxsize) size = maxsize;

            std::string str;
            str.reserve(size);

            while(size) {
                char buf[1024];
                source.read(buf, std::min<size_t>(size, 1024));
                if(source.bad() || source.fail()) return "";
                
                size_t obtained = static_cast<size_t>(source.gcount());

                if(size > obtained) size -= obtained;
                else size = 0;

                str.append(buf, static_cast<size_t>(obtained));
            }

            return str;
        }

        
        /// Returns true if no more data is available
        bool IsEof() const {
            auto ret = source.peek();
            return ret == EOF;
        }
        
        /// Tries to obtain a single character. If at the end of the stream returns nullopt.
        std::optional<char> TryGet() {
            if(IsEof()) return std::nullopt;
            
            return Get();
        }
        
        /// Tries to obtain a single character without advancing read pointer. If at the end 
        /// of the stream returns nullopt.
        std::optional<char> TryPeek() const {
            if(IsEof()) return std::nullopt;
            
            return Peek();
        }
        
        /// Returns the current location of the read pointer
        size_t Tell() const {
            return static_cast<size_t>(source.tellg());
        }
        
        /// Returns the remaining number of bytes. Since not every source can state its end,
        /// std::optional is returned. For string_view source, remaining size is always available
        std::optional<size_t> Remaining() const {
            auto cur = source.tellg();
            source.seekg(0, std::ios::end);
            auto size = source.tellg();
            source.seekg(cur);
            return size - cur;
        }
    
        /// Returns the total number of bytes in stream. Since not every source can state its end,
        /// std::optional is returned. For string_view source, size is always available
        std::optional<size_t> Size() const {
            auto cur = source.tellg();
            source.seekg(0, std::ios::end);
            auto size = source.tellg();
            source.seekg(cur);

            return size;
        }
        
        /// Advances the read pointer forwards. If EOF is encountered, no error will be produced and
        /// the pointer is left at the EOF position.
        void Advance(size_t forward = 1) {
            assert(forward < static_cast<size_t>(std::numeric_limits<std::ios::pos_type>::max()));
            
            source.seekg(static_cast<std::ios::pos_type>(forward), std::ios::cur);
        }
        
        /// Returns the name of this resource. Unless set using non-standard SetResourceName, it will be
        /// empty
        std::optional<std::string> GetResourceName() const {
            return resource_name;
        }
        
        /// Sets the resource name to the given name.
        void SetResourceName(const std::string_view &name) {
            resource_name = name;
        }
        
        /// Removes the resource name so it will be empty.
        void RemoveResourceName() {
            resource_name = std::nullopt;
        }
        #pragma clang diagnostic pop
        #pragma GCC diagnostic pop
    
    private:
        std::optional<std::string> resource_name = std::nullopt;
        std::istream &source;
    };
    
    //TODO: specialize for std::path, ifstream
    
    template<class T_> concept SourceInstatiation = IsInstantiationV<T_, Source>;
    template<class T_> concept IStreamInstatiation = std::derived_from<T_, std::istream>;
    template<class T_> concept SourceSpecializedFor = HasImp<Source<std::decay<T_>>>;
    
    template<bool Translate, class T_>
    struct make_source_type {
        using Type = T_;
    };
    
    template<SourceInstatiation T_>
    struct make_source_type<true, T_> {
        using Type = T_;
    };
    
    template<SourceSpecializedFor T_>
    struct make_source_type<true, T_> {
        using Type = Source<T_>;
    };
    
    template<IStreamInstatiation T_>
    struct make_source_type<true, T_> {
        using Type = Source<std::istream>;
    };
    
    template<StringLike T_>
    struct make_source_type<true, T_> {
        using Type = Source<std::string_view>;
    };
    
    /**
     * @brief Converts given object to source.
     * Converts given object to source by checking its type. If translate is false or object
     * is derived from Source<> directly returns the object. If Source is specialized for the
     * given object, that is returned. Then the function checks is the object is a type of 
     * string, if so, a Source<string_view> is returned.
     * @tparam Translate If set to false, the given object is returned
     * @return auto An object guaranteed to satisfy SourceConcept.
     */
    template<bool Translate = true, class T_>
    auto make_source(T_ &obj) requires SourceConcept<typename make_source_type<Translate, T_>::Type> {
        if constexpr(IsInstantiationV<T_, Source> || !Translate) {
            return obj;
        }
        else {
            return typename make_source_type<Translate, T_>::Type{obj};
        }
    }


}