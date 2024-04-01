#pragma once

#include "cpp-serializer/concepts.hpp"
#include "tmp.hpp"

#include <cassert>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

namespace CPPSerializer {
    
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
    
    //TODO: specialize for std::path, ifstream
    
    template<class T_> concept SourceInstatiation = IsInstantiationV<T_, Source>;
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
        else if constexpr(HasImp<Source<std::decay<T_>>>) {
            return Source{obj};
        }
        else if constexpr(std::is_convertible_v<T_, const std::string_view>) {
            return Source<std::string_view>{obj};
        }
        else return obj;
    }


}