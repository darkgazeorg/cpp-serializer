#pragma once

#include "config.hpp"
#include "concepts.hpp"

#include <stddef.h>


namespace CPP_SERIALIZER_NAMESPACE {
    
    /// Returns the number of bytes in a UTF8 code point.
    constexpr inline size_t UTF8Bytes(char first_byte) noexcept {
        unsigned char c = static_cast<unsigned char>(first_byte);
        return 1 + size_t(c >= 0b11000000) + size_t(c >= 0b11100000) + size_t(c >= 0b11110000);
    }

    template<SourceConcept Source_>
    constexpr inline bool UTF8IsSpace(char first, Source_ &src) {
        switch(static_cast<unsigned char>(first)) {
        case 0x9:
        case 0xa:
        case 0xb:
        case 0xc:
        case 0xd:
        case 0x20:
            return true;
        case 0xc2:
            switch(static_cast<unsigned char>(src.Peek())) {
            case 0x85:
            case 0xa0:
                return true;
            }
            break;
        case 0xe1:
            switch(static_cast<unsigned char>(src.Peek())) {
            case 0x9a:
                if(static_cast<unsigned char>(src.PeekNext(1)) == 0x80) return true;
                break;
            case 0xa0:
                if(static_cast<unsigned char>(src.PeekNext(1)) == 0x8e) return true;
                break;
            }
            break;
        case 0xe2:
            switch(static_cast<unsigned char>(src.Peek())) {
                case 0x80: {
                    auto c = static_cast<unsigned char>(src.PeekNext(1));
                    if(c >= 0x80 && c <= 0x8d) return true;
                    if(c == 0xa8 || c == 0xa9 || c == 0xaf) return true;
                    break;
                }
                case 0x81:
                    switch(static_cast<unsigned char>(src.PeekNext(1))) {
                    case 0x9f:
                    case 0xa0:
                        return true;
                    }

                    break;
            }
            break;
        case 0xe3:
            if(static_cast<unsigned char>(src.Peek()) == 0x80 && static_cast<unsigned char>(src.PeekNext(1)) == 0x80) return true;
            break;
        case 0xef:
            if(static_cast<unsigned char>(src.Peek()) == 0xbb && static_cast<unsigned char>(src.PeekNext(1)) == 0xbf) return true;
            break;
        }

        return false;
    }

}
