#pragma once

#include "config.hpp"
#include "concepts.hpp"
#include "cpp-serializer/utf.hpp"
#include "types.hpp"
#include "location.hpp"
#include "tmp.hpp"

#include <cctype>
#include <limits>
#include <string>

namespace CPP_SERIALIZER_NAMESPACE::internal {
    
    template<class LocationType_ = NoLocation>
    struct SimpleTextDataConverter {
        using LocationType = LocationType_;

        auto operator()(const Context<LocationType> &c, const std::string_view &s) {
            return std::pair{c.location, std::string(s)};
        }
        auto operator()(std::string &s) { return s; }
        auto operator()(const std::string &s) { return s; }
    };


    template<YesNoRuntime skiplist_, YesNoRuntime folding_, YesNoRuntime glue_, SourceConcept SourceType, DataConcept DataType>
    void parseText(SourceType &reader, DataType &target, const std::array<bool, 3> &settings) {
        using DataTraits   = DataType::DataTraits;
        using LocationType = DataTraits::LocationType;
        using StorageType = DataTraits::StorageType;
        
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

                auto do_newline = c == '\n' || c == '\r';
                auto do_space   = folding && UTF8IsSpace(c, reader);

                //if only one enter was there, convert it to space
                //this can only happen if glue is on
                if(!do_newline) {
                    if(seqline == 1) {
                        //from the source perspective, next character is on the next line
                        if(!has_space) str.push_back(' ');

                        AddNewLine(skiplist, location, reader, str.size(), line);
                        char_off = 1;

                        seqline = 0;
                        has_space = do_space;
                    }
                    else seqline = 0;
                }

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
                            AddNewLine(skiplist, location, reader, str.size(), line);
                            char_off = 1;
                        }
                    }
                    else {
                        str.push_back('\n'); //simply add the new line
                        AddNewLine(skiplist, location, reader, str.size(), line);
                        char_off = 1;
                    }
                }
                else if(do_space) {
                    if(!has_space) {
                        str.push_back(c);
                        //copy additional utf8 bytes
                        for(size_t i=1; i<UTF8Bytes(c) && !reader.IsEof(); i++) 
                            str.push_back(reader.Get());
                    }
                    else {
                        //eat additional utf8 bytes
                        for(size_t i=1; i<UTF8Bytes(c); i++) 
                            reader.TryGet();
                    }


                    char_off++;
                    
                    has_space = true;
                }
                else {
                    str.push_back(c);

                    //copy additional utf8 bytes
                    for(size_t i=1; i<UTF8Bytes(c) && !reader.IsEof(); i++) 
                        str.push_back(reader.Get());

                    char_off++;
                }
            }
        }
        else str = reader.Read(std::numeric_limits<size_t>::max());
        
        typename DataTraits::DataParserType parser{};
        
        StorageType data;
        std::tie(location, data) = parser(Context<LocationType>{location, {}}, str);
        target.SetData(data);
        target.SetLocation(location);
    }


}
