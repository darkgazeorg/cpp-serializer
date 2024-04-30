#pragma once

#include "config.hpp"
#include "concepts.hpp"
#include "types.hpp"
#include "location.hpp"
#include "tmp.hpp"

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
        
        if(skiplist || folding || glue) {
            //if size is known, allocate that much space
            if(auto sz = reader.Size(); sz)
                str.reserve(*sz);
            
            size_t line = 1;
            size_t seqline = 0;
            
            while(!reader.IsEof()) {
                auto c = reader.Get();
                
                //new line
                if(c == '\n' || c == '\r') {     
                    //If it is \r\n, ignore \n
                    if(c == '\r' && reader.TryPeek() == '\n') reader.Advance();

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
                        }
                    }
                    else {
                        str.push_back('\n'); //simply add the new line
                        AddNewLine(skiplist, location, reader, str.size(), line);
                    }
                }
                else {
                    //if only one enter was there, convert it to space
                    //this can only happen if glue is on
                    if(seqline == 1) {
                        str.push_back(' ');
                        //from the source perspective, next character is on the next line
                        AddNewLine(skiplist, location, reader, str.size(), line);
                    }

                    str.push_back(c);
                    seqline = 0;
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
