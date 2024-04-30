#pragma once

#include "config.hpp"

#include "concepts.hpp"
#include "location.hpp"
#include "txt-helper.hpp"
#include "types.hpp"
#include "data.hpp"
#include "source.hpp"

#include <array>
#include <string>

#include "macros.hpp"

namespace CPP_SERIALIZER_NAMESPACE {
    
    template<LocationConcept LocationType_>
    struct TextDataTraits {            
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
        
        using DataParserType = internal::SimpleTextDataConverter;
        using DataEmitterType = internal::SimpleTextDataConverter;
        using LocationType = LocationType_;
    };

    struct SimpleTextSettings {
        constexpr static auto SkipList = YesNoRuntime::No;
        constexpr static auto Folding = YesNoRuntime::No;
        constexpr static auto Glue = YesNoRuntime::No;

        using DataTraits = TextDataTraits<NoLocation>;
        using DataType   = Data<DataTraits>;
    };

    struct SimpleSkipListTextSettings {
        constexpr static auto SkipList = YesNoRuntime::Runtime;
        constexpr static auto Folding = YesNoRuntime::No;
        constexpr static auto Glue = YesNoRuntime::No;

        using DataTraits = TextDataTraits<InnerLocation>;
        using DataType   = Data<DataTraits>;
    };
    
    struct RuntimeTextSettings {
        constexpr static auto SkipList = YesNoRuntime::No;
        constexpr static auto Folding = YesNoRuntime::Runtime;
        constexpr static auto Glue = YesNoRuntime::Runtime;

        using DataTraits = TextDataTraits<NoLocation>;
        using DataType   = Data<DataTraits>;
    };
    
    CPPSER_DEFINE_MIXTIME_STRUCT(TextTransport, SkipList, skiplist, true)
    CPPSER_DEFINE_MIXTIME_STRUCT(TextTransport, Folding, folding, true)
    CPPSER_DEFINE_MIXTIME_STRUCT(TextTransport, Glue, glue, true)

    
    //TODO: Implement line glueing, escape characters as well as wordwrap
    /**
     * @brief Allows parsing emmiting text files.
     *
     * This class can handle text files with different properties, such as wordwrap, whitespace folding.
     * @important Currently this class is used for experimentation, it should not be used.
     *
     * @tparam Settings_ Use this structure to specify text transport settings 
     */
    template<TextSettingsConcept Settings_ = SimpleTextSettings>
    class TextTransport : 
        public internal::TextTransport_skiplist_helper<Settings_::SkipList>,
        public internal::TextTransport_folding_helper<Settings_::Folding>,
        public internal::TextTransport_glue_helper<Settings_::Glue> 
    {
    public:
        using Settings     = Settings_;
        using DataType     = Settings::DataType;
        using DataTraits   = Settings::DataTraits;
        using StorageType  = DataType::StorageType;
        using LocationType = DataTraits::LocationType;
        
        template<class T_, bool AutoTranslateSource = true>
        void Parse(T_ &source, DataType &data) {
            auto reader = make_source<AutoTranslateSource>(source);
            
            auto settings = std::array<bool, 3>{};
            
            CPPSER_READ_IF_RUNTIME(SkipList, 0);
            CPPSER_READ_IF_RUNTIME(Folding, 1);
            CPPSER_READ_IF_RUNTIME(Glue, 2);
            
            internal::parseText<Settings::SkipList, Settings::Folding, Settings::Glue>(reader, data, settings);
        }
        
        template<class T_>
        void Emit(T_ &target, const DataType &data) {
            auto writer = make_target(target);
            typename DataTraits::DataEmitterType emitter{};
            
            writer.Put(emitter(data.GetData()));
        }
    };
    
    inline TextTransport<> TextTransportSimple;
    using RuntimeTextTransport = TextTransport<RuntimeTextSettings>;

}

#include "unmacro.hpp"
