#pragma once

#include "config.hpp"
#include "cpp-serializer/concepts.hpp"
#include "cpp-serializer/tmp.hpp"
#include "cpp-serializer/types.h"

#include <limits>
#include <string>

namespace CPP_SERIALIZER_NAMESPACE::internal {
    
#include "macros.hpp"
    
    template<YesNoRuntime skiplist_, YesNoRuntime folding_, YesNoRuntime glue_, SourceConcept SourceType, DataConcept DataType, class TupleType>
    void parseText(SourceType &reader, DataType &target, TupleType settings) {
        using DataTraits   = DataType::DataTraits;
        using LocationType = DataTraits::LocationType;
        using StorageType = DataTraits::StorageType;
        
        const bool skiplist = GetMixedTime<skiplist_>::template Obtain<>(settings);
        const bool folding = GetMixedTime<folding_>::template Obtain<skiplist_>(settings);
        const bool glue = GetMixedTime<glue_>::template Obtain<skiplist_, folding_>(settings);
        
        auto location = LocationType{0, 1, 1};
        auto str      = std::string{};
        
        if(skiplist || folding) {
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
                    if(c == '\r' && reader.TryPeek() == '\n') {
                        reader.Advance();
                    }
                    
                    CPPSER_IF_MIXED(LocationType::HasSkipList(), skiplist) {
                        line++;
                        auto curlocation = LocationType{line, 1};
                        
                        if constexpr(LocationType::HasResourceName) {
                            curlocation.ResourceName = reader.GetResourceName();
                        }
                        
                        location.SkipList[reader.Tell()] = curlocation;
                    }
                    
                    if(glue) {
                        if(seqline < 1) {
                            seqline++;
                            str.push_back(' ');
                        }
                        else {
                            str.push_back('\n');
                        }
                    }
                    else str.push_back('\n');
                }
                else {
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

#include "unmacro.hpp"

}
