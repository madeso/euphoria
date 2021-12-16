#pragma once

#include <cstddef>
#include <vector>

#include "core/assert.h"

namespace euphoria::core
{
    template<typename String, typename OnCodepoint>
    bool utf8_to_codepoints(const String& string, OnCodepoint on_codepoint)
    {
        // reference: https://en.wikipedia.org/wiki/UTF-8
        using CodePointInt = int;
        auto to_byte = [](char c)->std::byte { return static_cast<std::byte>(c); };

        constexpr auto mask0 = std::byte{0b10000000}; constexpr auto bit0 = std::byte{0b00000000};
        constexpr auto mask1 = std::byte{0b11000000}; constexpr auto bit1 = std::byte{0b10000000};
        constexpr auto mask2 = std::byte{0b11100000}; constexpr auto bit2 = std::byte{0b11000000};
        constexpr auto mask3 = std::byte{0b11110000}; constexpr auto bit3 = std::byte{0b11100000};
        constexpr auto mask4 = std::byte{0b11111000}; constexpr auto bit4 = std::byte{0b11110000};

        unsigned int bits = 0;
        std::vector<std::byte> buffer;

        for(auto c: string)
        {
            const auto b = to_byte(c);
            if(bits == 0)
            {
                if( (mask0 & b) == bit0)
                {
                    on_codepoint(std::to_integer<CodePointInt>(b));
                }
                else
                {
                    buffer.push_back(b);
                    if((mask4 & b) == bit4)
                    {
                        bits = 4;
                    }
                    else if((mask3 & b) == bit3)
                    {
                        bits = 3;
                    }
                    else if((mask2 & b) == bit2)
                    {
                        bits = 2;
                    }
                    else if((mask1 & b) == bit1)
                    {
                        // stray continuation byte...
                        return false;
                    }
                    else
                    {
                        DIE("Implementation error?");
                        return false;
                    }
                }
            }
            else
            {
                if((mask1 & b) != bit1)
                {
                    // invalid continuation bit
                    return false;
                }
                buffer.push_back(b);
                if(buffer.size() == bits)
                {
                    switch(bits)
                    {
                    case 2:
                        on_codepoint
                        (
                            ( std::to_integer<CodePointInt>(buffer[0] & ~mask2) << 6) |
                            ( std::to_integer<CodePointInt>(buffer[1] & ~mask1) << 0)
                        );
                        break;
                    case 3:
                        on_codepoint
                        (
                            ( std::to_integer<CodePointInt>(buffer[0] & ~mask3) << 12) |
                            ( std::to_integer<CodePointInt>(buffer[1] & ~mask1) << 6 ) |
                            ( std::to_integer<CodePointInt>(buffer[2] & ~mask1) << 0 )
                        );
                        break;
                    case 4:
                        on_codepoint
                        (
                            ( std::to_integer<CodePointInt>(buffer[0] & ~mask4) << 18) |
                            ( std::to_integer<CodePointInt>(buffer[1] & ~mask1) << 12) |
                            ( std::to_integer<CodePointInt>(buffer[2] & ~mask1) << 6 ) |
                            ( std::to_integer<CodePointInt>(buffer[3] & ~mask1) << 0 )
                        );
                        break;
                    default:
                        // unhandled size
                        return false;
                    }

                    bits = 0;
                    buffer.resize(0);
                }
            }
        }

        // if bits != 0 this means a unfinished codepoint
        return bits == 0;
    }
}
