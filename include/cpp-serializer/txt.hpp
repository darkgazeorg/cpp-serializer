#pragma once

#include "cpp-serializer/concepts.hpp"
#include "cpp-serializer/location.hpp"
#include "data.hpp"
#include "source.hpp"

#include <limits>
#include <string>
#include <string_view>
#include <utility>

namespace CPPSerializer {
    
    namespace internal {
        
        struct SimpleTextDataConverter {
            using LocationType = NoLocation;
            auto operator()(Context<LocationType> &c, const std::string_view &s) {
                return std::pair{c.location, std::string(s)};
            }
            auto operator()(std::string &s) { return s; }
            auto operator()(const std::string &s) { return s; }
        };
        
        template<LocationConcept LocationType_ = NoLocation>
        struct TextData_SimpleTraits {            
            using StorageType = std::string;
            
            using NumberType = void;
            using IntegerType = void;
            using RealType = void;
            using StringType = std::string;
            using NullType = void;
            using BoolType = void;
            using IndexType = void;
            using SequenceType = void;
            using KeyType = void;
            using MapType = void;
            
            using DataParserType = SimpleTextDataConverter;
            using DataEmitterType = SimpleTextDataConverter;
            using LocationType = LocationType_;
        };
        
        template<bool skiplist>
        class texttransportlocationhelper {
        
        };
        
        template<>
        class texttransportlocationhelper<true> {
        public:
            bool IsParsingSkipList() const {
                return parseskiplist;
            }
            
            void SetSkipListParsing(bool state) {
                parseskiplist = state;
            }
            
        
        protected:
            bool parseskiplist = true;
        };
    }
    
    using TextData_Simple = Data<internal::TextData_SimpleTraits<>>;
    using TextData_SkipList = Data<internal::TextData_SimpleTraits<GlobalInnerLocation>>;
        
    template<DataConcept DataType_ = TextData_Simple>
    class TextTransport : public internal::texttransportlocationhelper<DataType_::DataTraits::LocationType::HasSkipList()> {
    public:
        using DataType     = DataType_;
        using DataTraits   = DataType_::DataTraits;
        using StorageType  = DataType_::StorageType;
        using LocationType = DataTraits::LocationType;
        
        template<class T_, bool AutoTranslateSource = true>
        void Parse(T_ &source, DataType &data) {
            auto reader = make_source<AutoTranslateSource>(source);
            bool done = false;
            
            LocationType location;
            
            if constexpr(LocationType::HasSkipList()) {
                if(this->IsParsingSkipList()) {
                    std::string str;
                    
                    //if size is known, allocate that much space
                    if(auto sz = reader.GetSize(); sz)
                        str.reserve(*sz);
                    
                    size_t line = 1;
                    
                    while(!reader.IsEof()) {
                        auto c = reader.Get();
                        
                        if(c == '\n') {
                            if(reader.TryPeek() == '\r') {
                                reader.Advance();
                            }
                            
                            line++;
                            auto curlocation = LocationType{line, 1};
                            
                            if constexpr(LocationType::HasResourceName) {
                                curlocation.ResourceName = reader.GetResourceName();
                            }

                            location.SkipList[reader.Tell()] = curlocation;
                        }
                        
                        str.push_back(c);
                    }
                    
                    done = true;
                }
            }
            
            //if we are not doing any fancy processing, we will read all data here.
            if(!done) data.SetData(std::string(reader.Read(std::numeric_limits<size_t>::max())));

            data.SetLocation(location);
        
            //DataTraits::
        }
        
        template<class T_>
        void Emit(T_ &target, const DataType &data);
    };
    
    inline TextTransport<> TextTransportSimple;

}
