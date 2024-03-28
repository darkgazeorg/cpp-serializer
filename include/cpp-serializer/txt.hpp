#pragma once

#include "data.hpp"

#include <string>
#include <string_view>

namespace CPPSerializer {
    
    namespace internal {
        
        struct SimpleTextDataConverter {
            using LocationType = Location<false, false>;
            auto operator()(Context<LocationType> &c, const std::string_view &s) {
                return std::pair{c.offset, std::string(s)};
            }
            auto operator()(std::string &s) { return s; }
            auto operator()(const std::string &s) { return s; }
        };
        
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
            
            static constexpr bool HasOffset() { return false; }
            static constexpr bool HasSkipList() { return false; }
            
            using DataParserType = SimpleTextDataConverter;
            using DataEmitterType = SimpleTextDataConverter;
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
    
    using TextData_Simple = Data<internal::TextData_SimpleTraits>;
        
    template<DataConcept DataType_>
    class TextTransport : public internal::texttransportlocationhelper<DataType_::DataTraits::HasSkipList()> {
    public:
        using DataType    = DataType_;
        using DataTraits  = DataType_::DataTraits;
        using StorageType = DataType_::StorageType;
        
    
        
        template<class T_>
        void Parse(T_ &source, DataType &data) {
            auto reader = make_source(source);
            bool done = false;
            
            if constexpr(DataTraits::HasSkipList()) {
                if(this->IsParsingSkipList()) {
                    std::string str;
                    str.reserve(reader.GetSize());
                    
                    Location<true, true> location;
                    
                    while(!reader.IsEof()) {
                        auto c = reader.Get();
                        //if()
                        str.push_back(c);
                    }
                    
                    done = true;
                }
            }
            
            if(!done) data.SetData(reader.Read(reader.GetSize()));
        
            //DataTraits::
        }
        
        template<class T_>
        void Emit(T_ &target, const DataType &data);
    };

}
