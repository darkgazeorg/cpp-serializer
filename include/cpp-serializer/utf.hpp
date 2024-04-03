#pragma once

#include <stddef.h>

namespace CPP_SERIALIZER_NAMESPACE {
    
    /// Returns the number of bytes in a UTF8 code point.
    constexpr inline size_t UTF8Bytes(char first_byte) noexcept {
        unsigned char c = static_cast<unsigned char>(first_byte);
        return 1 + size_t(c >= 0b11000000) + size_t(c >= 0b11100000) + size_t(c >= 0b11110000);
    }

}
