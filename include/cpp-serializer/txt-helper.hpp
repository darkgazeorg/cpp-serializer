#pragma once

#include "config.hpp"
#include "concepts.hpp"
#include "cpp-serializer/source.hpp"
#include "utf.hpp"
#include "types.hpp"
#include "location.hpp"
#include "tmp.hpp"

#include <cctype>
#include <cstddef>
#include <limits>
#include <string>
#include <array>

namespace CPP_SERIALIZER_NAMESPACE::internal {

#include "macros.hpp"
    
    /// Performs unity string conversion.
    template<class LocationType_ = NoLocation>
    struct SimpleTextDataConverter {
        using LocationType = LocationType_;

        auto operator()(const Context<LocationType> &c, const std::string_view &s) {
            return std::pair{c.location, std::string(s)};
        }
        auto operator()(std::string &s) { return s; }
        auto operator()(const std::string &s) { return s; }
    };


    /**
     * @brief Parses the given reader for text to the target
     * Parses all the data from a given reader in to target using options. Folding folds
     * non-newline white space characters to a single one. In case they are different
     * the first one is used, rest is discarded.
     * @tparam skiplist_ Mixed time option to use skiplist, should be no if Location does
     *         not skiplists
     * @tparam folding_ Mixed time option for whitespace folding. Does not control newlines
     * @tparam glue_ Mixed time option for glueing consecutive lines.
     * @tparam SourceType Automatically determined
     * @tparam DataType  Automatically determined
     * @param reader The data source
     * @param target The obtained text will stored here
     * @param settings Mixed time settings in the order of skiplist, folding and glue. They
     *        have no effect unless corresponding template argument is set to Runtime
     */
    template<YesNoRuntime skiplist_, YesNoRuntime folding_, YesNoRuntime glue_, SourceConcept SourceType, DataConcept DataType>
    void ParseText(SourceType &reader, DataType &target, const std::array<bool, 3> &settings) {
        //extract necessary types
        using DataTraits   = DataType::DataTraits;
        using LocationType = DataTraits::LocationType;
        using StorageType = DataTraits::StorageType;
        
        //mixed time options
        const bool skiplist = GetMixedTimeOption<skiplist_, 0>(settings);
        const bool folding = GetMixedTimeOption<folding_, 1>(settings);
        const bool glue = GetMixedTimeOption<glue_, 2>(settings);
        
        auto location = LocationType{0, 1, 1};
        auto str      = std::string{};
        auto char_off = size_t(1);
        auto has_space= false;
        
        if(skiplist || folding || glue) {
            //if size is known, allocate that much space
            if(auto sz = reader.Size(); sz)
                str.reserve(*sz);
            
            size_t line = 1;
            size_t seqline = 0;
            
            while(!reader.IsEof()) {
                auto c = reader.Get();

                //detect special characters
                auto do_newline = c == '\n' || c == '\r';
                auto do_space   = folding && UTF8IsSpace(c, reader);

                //if only one enter was there, convert it to space
                //this can only happen if glue is on
                if(!do_newline) {
                    //only one enter is there and glueing is on
                    if(seqline == 1) {
                        //from the source perspective, next character is on the next line
                        if(!has_space) str.push_back(' ');

                        AddSkipLine(skiplist, location, reader, str.size(), line, char_off);
                        char_off = 1;

                        seqline = 0;
                        has_space = do_space;
                    }
                    else seqline = 0;
                }

                //just a regular character but there was a space before, thus a skip point
                //is necessary
                if(!do_newline && !do_space && has_space) {
                    AddSkip(skiplist, location, reader, str.size(), line, char_off);
                    has_space = false;
                }
                
                //new line
                if(do_newline) {     
                    //If it is \r\n, ignore \n
                    if(c == '\r' && reader.TryPeek() == '\n') {
                        reader.Advance();
                    }

                    if(glue) { //join lines if there is no additional line breaks
                        if(seqline < 1) {
                            seqline++; //ignore first enter, if space is needed, it will be handled later
                        }
                        else {
                            //There is an extra line in source, we need to add this.
                            if(seqline == 1) {
                                line++;
                            }

                            seqline++; //incremented further to signal enter is added

                            str.push_back('\n');
                            AddSkipLine(skiplist, location, reader, str.size(), line, char_off);
                        }
                    }
                    else {
                        str.push_back('\n'); //simply add the new line
                        AddSkipLine(skiplist, location, reader, str.size(), line, char_off);
                    }
                }
                else if(do_space) {
                    if(!has_space) {
                        CPPSER_UTF_COPY(reader, c, str);
                    }
                    else {
                        CPPSER_UTF_IGNORE_REST(reader, c);
                    }


                    char_off++;
                    
                    has_space = true;
                }
                else {
                    CPPSER_UTF_COPY(reader, c, str);

                    char_off++;
                }
            }
        }
        else {
            //read entire buffer
            str = reader.Read(std::numeric_limits<size_t>::max());
        }
        
        //further parse data
        typename DataTraits::DataParserType parser{};
        StorageType data;
        std::tie(location, data) = parser(Context<LocationType>{location, {}}, str);
        target.SetData(data);
        target.SetLocation(location);
    }

    template<YesNoRuntime wordwrap_, TargetConcept TargetType, DataConcept DataType>
    void EmitText(const DataType &source, TargetType &target, std::array<bool, 1> settings, size_t wrapwidth) {
        //extract necessary types
        using DataTraits   = DataType::DataTraits;

        //mixed time options
        const bool wordwrap = GetMixedTimeOption<wordwrap_, 0>(settings);


        typename DataTraits::DataEmitterType emitter{};

        if(wordwrap) {
            auto acc        = std::string{};
            auto data       = emitter(source.GetData());
            auto lastbreak  = size_t{};
            auto reader     = make_source(data);
            auto chars      = size_t{};

            while(!reader.IsEof()) {
                auto c = reader.Get();

                //new line resets all
                if(c == '\n') {
                    CPPSER_UTF_COPY(reader, c, acc);
                    target.Put(acc);
                    acc.clear();
                    lastbreak = 0;
                    chars = 0;
                }
                else if(UTF8IsSpace(c, reader)) {
                    lastbreak = acc.size();
                    CPPSER_UTF_COPY(reader, c, acc);
                    chars++;
                }
                else {
                    CPPSER_UTF_COPY(reader, c, acc);
                    chars++;

                    if(chars > wrapwidth) {
                        //write all if no breaking chars are found
                        if(lastbreak == 0) {
                            target.Put(acc);
                            acc.clear();
                            chars = 0;
                        }
                        else {
                            //write out until the last break
                            target.Put(acc, lastbreak);
                            target.Put('\n');
                            //skip last break
                            lastbreak += UTF8Bytes(acc[lastbreak]);
                            acc = acc.substr(lastbreak);
                            lastbreak = 0;

                            //determine number of characters remaining in the buffer
                            chars = 0;
                            auto size = acc.size();
                            for(size_t i{}; i<size;) {
                                i += UTF8Bytes(acc[i]);
                                chars++;
                            }
                        }
                    }
                }
            }

            //write the remaining in the buffer
            target.Put(acc);
        }
        else {
            target.Put(emitter(source.GetData()));
        }
    }

#include "unmacro.hpp"
}
