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
        
        using DataParserType = internal::SimpleTextDataConverter<LocationType_>;
        using DataEmitterType = internal::SimpleTextDataConverter<LocationType_>;
        using LocationType = LocationType_;
    };

    struct SimpleTextSettings {
        constexpr static auto SkipList = YesNoRuntime::No;
        constexpr static auto Folding = YesNoRuntime::No;
        constexpr static auto Glue = YesNoRuntime::No;
        constexpr static auto WordWrap = YesNoRuntime::No;

        using DataTraits = TextDataTraits<NoLocation>;
        using DataType   = Data<DataTraits>;
    };

    struct SimpleSkipListTextSettings {
        constexpr static auto SkipList = YesNoRuntime::Runtime;
        constexpr static auto Folding = YesNoRuntime::No;
        constexpr static auto Glue = YesNoRuntime::No;
        constexpr static auto WordWrap = YesNoRuntime::No;

        using DataTraits = TextDataTraits<InnerLocation>;
        using DataType   = Data<DataTraits>;
    };
    
    template<class Location = NoLocation>
    struct RuntimeTextSettings {
        constexpr static auto SkipList = Location::HasSkipList() ? YesNoRuntime::Runtime : YesNoRuntime::No;
        constexpr static auto Folding = YesNoRuntime::Runtime;
        constexpr static auto Glue = YesNoRuntime::Runtime;
        constexpr static auto WordWrap = YesNoRuntime::Runtime;

        using DataTraits = TextDataTraits<Location>;
        using DataType   = Data<DataTraits>;
    };
    
    CPPSER_DEFINE_MIXTIME_STRUCT(TextTransport, SkipList, skiplist, true)
    CPPSER_DEFINE_MIXTIME_STRUCT(TextTransport, Folding, folding, true)
    CPPSER_DEFINE_MIXTIME_STRUCT(TextTransport, Glue, glue, true)
    namespace internal {
        CPPSER_DEFINE_MIXTIME_STRUCT_LEAVEOPEN(TextTransport, WordWrap, wordwrap, true) //{
            void SetWrapWidth(size_t value) { wrapwidth = value; }
            size_t GetWrapWidth() const { return wrapwidth; }
        protected:
            size_t wrapwidth = 80;
        };
        template <> 
        struct TextTransport_wordwrap_helper<YesNoRuntime::Yes> {
            void SetWrapWidth(size_t value) { wrapwidth = value; }
            size_t GetWrapWidth() const { return wrapwidth; }
        protected:
            size_t wrapwidth = 80;
        };
    }

    
    //TODO: Implement line glueing, escape characters as well as wordwrap
    /**
     * @brief Allows parsing emmiting text files.
     *
     * This class can handle text files with different properties, such as wordwrap, whitespace folding.
     * @important Currently this class is used for experimentation, it should not be used.
     *
     * @tparam Settings_ Use this structure to specify text transport settings. Text transport settings
     * should follow TextSettingsConcept
     */
    template<TextSettingsConcept Settings_ = SimpleTextSettings>
    class TextTransport : 
        public internal::TextTransport_skiplist_helper<Settings_::SkipList>,
        public internal::TextTransport_folding_helper<Settings_::Folding>,
        public internal::TextTransport_glue_helper<Settings_::Glue> ,
        public internal::TextTransport_wordwrap_helper<Settings_::WordWrap> 
    {
    public:
        using Settings     = Settings_;
        using DataType     = Settings::DataType;
        using DataTraits   = Settings::DataTraits;
        using StorageType  = DataType::StorageType;
        using LocationType = DataTraits::LocationType;
        
        /**
         * @brief Parses a given source into the given data target.
         * Performs text parsing according to the options of the parser.
         * @tparam Source_ Source type, if AutoTranslateSource is set to false this requires 
         *         SourceConcept, otherwise any type that can be turned into Source is allowed.
         *         String/const char* /string_view are evaluated as in memory data source. std::path
         *         is used to open a file and use the file, a given stream will be used as a stream
         *         source.
         * @tparam AutoTranslateSource if set to false, source is used as is. In that case source
         *         requires SourceConcept. Otherwise, the source is given to make_source function.
         *         This function will not perform any operation if given source is a specialization
         *         of Source. If not it will automatically select a matching Source. 
         *         String/const char* /string_view are evaluated as in memory data source. std::path
         *         is used to open a file and use the file, a given stream will be used as a stream
         *         source.
         * @param source Data source, this will be turned into a reader and used. How it is transformed
         *        to a reader depends on the template arguments.
         * @param data Data target, this variable will be filled with the parsed data.
         */
        template<bool AutoTranslateSource = true, class Source_>
        void Parse(Source_ &source, DataType &data) {
            auto reader = make_source<AutoTranslateSource>(source);
            
            auto settings = std::array<bool, 3>{};
            
            CPPSER_READ_IF_RUNTIME(SkipList, 0);
            CPPSER_READ_IF_RUNTIME(Folding, 1);
            CPPSER_READ_IF_RUNTIME(Glue, 2);
            
            internal::parseText<Settings::SkipList, Settings::Folding, Settings::Glue>(reader, data, settings);
        }


        /**
         * @brief Parses a given source and returns the result.
         * Performs text parsing according to the options of the parser.
         * @tparam Source_ Source type, if AutoTranslateSource is set to false this requires 
         *         SourceConcept, otherwise any type that can be turned into Source is allowed.
         *         String/const char* /string_view are evaluated as in memory data source. std::path
         *         is used to open a file and use the file, a given stream will be used as a stream
         *         source.
         * @tparam AutoTranslateSource if set to false, source is used as is. In that case source
         *         requires SourceConcept. Otherwise, the source is given to make_source function.
         *         This function will not perform any operation if given source is a specialization
         *         of Source. If not it will automatically select a matching Source. 
         *         String/const char* /string_view are evaluated as in memory data source. std::path
         *         is used to open a file and use the file, a given stream will be used as a stream
         *         source.
         * @param source Data source, this will be turned into a reader and used. How it is transformed
         *        to a reader depends on the template arguments.
         */
        template<bool AutoTranslateSource = true, class Source_>
        DataType Parse(Source_ &source) {
            DataType data;
            Parse(source, data);
            return data;
        }
        
        template<class T_>
        void Emit(T_ &target, const DataType &data) {
            auto writer = make_target(target);
            typename DataTraits::DataEmitterType emitter{};
            
            writer.Put(emitter(data.GetData()));
        }
    };
    
    inline TextTransport<> TextTransportSimple;
    using RuntimeTextTransport = TextTransport<RuntimeTextSettings<>>;
    using RuntimeTextTransportSkipList = TextTransport<RuntimeTextSettings<GlobalInnerLocation>>;

}

#include "unmacro.hpp"
