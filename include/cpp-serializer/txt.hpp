#pragma once

#include "config.hpp"

#include "cpp-serializer/concepts.hpp"
#include "cpp-serializer/location.hpp"
#include "cpp-serializer/tmp.hpp"
#include "cpp-serializer/txt.helper.hpp"
#include "cpp-serializer/types.h"
#include "data.hpp"
#include "source.hpp"

#include <string>
#include <string_view>
#include <utility>

namespace CPP_SERIALIZER_NAMESPACE {
    
    namespace internal {
        
        struct SimpleTextDataConverter {
            using LocationType = NoLocation;
            auto operator()(const Context<LocationType> &c, const std::string_view &s) {
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
    
    template<class T_>
    concept TextTransportConcept = requires {
        {T_::GlueLines()} -> std::same_as<bool>;
        {T_::WordWrap()} -> std::same_as<bool>;
    };
    
    //TODO: Implement line glueing, escape characters as well as wordwrap
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
            
            auto settings = MaskedMixedType<LocationType::HasSkipList(), false, true>();
            
            if constexpr(LocationType::HasSkipList())
                MaskedGet<0, LocationType::HasSkipList(), false, false>(settings) = this->IsParsingSkipList();

            MaskedGet<2, LocationType::HasSkipList(), false, false>(settings) = true;
            
            internal::parseText<LocationType::HasSkipList() ? YesNoRuntime::Runtime : YesNoRuntime::No, YesNoRuntime::Yes, YesNoRuntime::Runtime>(reader, data, settings);
        }
        
        template<class T_>
        void Emit(T_ &target, const DataType &data) {
            auto writer = make_target(target);
            typename DataTraits::DataEmitterType emitter{};
            
            writer.Put(emitter(data.GetData()));
        }
    };
    
    inline TextTransport<> TextTransportSimple;

}
