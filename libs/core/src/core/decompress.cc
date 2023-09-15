// clang-tidy: ignore
#include "core/decompress.h"

#include <cstring>

#include "assert/assert.h"

namespace eu::core
{

unsigned int Decompressor::stb_decompress_length(const unsigned char *input)
{
    return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
}

void Decompressor::stb_match(const unsigned char *data, unsigned int length)
{
    // INVERSE of memmove... write each byte before copying the next...
    ASSERT(stb_dout + length <= stb_barrier_out_e);
    if (stb_dout + length > stb_barrier_out_e) { stb_dout += length; return; }
    if (data < stb_barrier_out_b) { stb_dout = stb_barrier_out_e+1; return; }
    while (length--) *stb_dout++ = *data++;
}

void Decompressor::stb_lit(const unsigned char *data, unsigned int length)
{
    ASSERT(stb_dout + length <= stb_barrier_out_e);
    if (stb_dout + length > stb_barrier_out_e) { stb_dout += length; return; }
    if (data < stb_barrier_in_b) { stb_dout = stb_barrier_out_e+1; return; }
    memcpy(stb_dout, data, length);
    stb_dout += length;
}

#define STB_IN2(x)   ((i[x] << 8) + i[(x)+1])
#define STB_IN3(x)   ((i[x] << 16) + STB_IN2((x)+1))
#define STB_IN4(x)   ((i[x] << 24) + STB_IN3((x)+1))

const unsigned char * Decompressor::stb_decompress_token(const unsigned char *i)
{
    if (*i >= 0x20) { // use fewer if's for cases that expand small
        if (*i >= 0x80)       stb_match(stb_dout-i[1]-1, i[0] - 0x80 + 1), i += 2;
        else if (*i >= 0x40)  stb_match(stb_dout-(STB_IN2(0) - 0x4000 + 1), i[2]+1), i += 3;
        else /* *i >= 0x20 */ stb_lit(i+1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
    } else { // more ifs for cases that expand large, since overhead is amortized
        if (*i >= 0x18)       stb_match(stb_dout-(STB_IN3(0) - 0x180000 + 1), i[3]+1), i += 4;
        else if (*i >= 0x10)  stb_match(stb_dout-(STB_IN3(0) - 0x100000 + 1), STB_IN2(3)+1), i += 5;
        else if (*i >= 0x08)  stb_lit(i+2, STB_IN2(0) - 0x0800 + 1), i += 2 + (STB_IN2(0) - 0x0800 + 1);
        else if (*i == 0x07)  stb_lit(i+3, STB_IN2(1) + 1), i += 3 + (STB_IN2(1) + 1);
        else if (*i == 0x06)  stb_match(stb_dout-(STB_IN3(1)+1), i[4]+1), i += 5;
        else if (*i == 0x04)  stb_match(stb_dout-(STB_IN3(1)+1), STB_IN2(4)+1), i += 6;
    }
    return i;
}

unsigned int Decompressor::stb_adler32(unsigned int adler32, unsigned char *buffer, unsigned int buflen)
{
    const unsigned long ADLER_MOD = 65521;
    unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
    unsigned long blocklen, i;

    blocklen = buflen % 5552;
    while (buflen) {
        for (i=0; i + 7 < blocklen; i += 8) {
            s1 += buffer[0], s2 += s1;
            s1 += buffer[1], s2 += s1;
            s1 += buffer[2], s2 += s1;
            s1 += buffer[3], s2 += s1;
            s1 += buffer[4], s2 += s1;
            s1 += buffer[5], s2 += s1;
            s1 += buffer[6], s2 += s1;
            s1 += buffer[7], s2 += s1;

            buffer += 8;
        }

        for (; i < blocklen; ++i)
            s1 += *buffer++, s2 += s1;

        s1 %= ADLER_MOD, s2 %= ADLER_MOD;
        buflen -= blocklen;
        blocklen = 5552;
    }
    return (unsigned int)(s2 << 16) + (unsigned int)s1;
}

unsigned int Decompressor::stb_decompress(unsigned char *output, const unsigned char *i, unsigned int /*length*/)
{
    unsigned int olen;
    if (STB_IN4(0) != 0x57bC0000) return 0;
    if (STB_IN4(4) != 0)          return 0; // error! stream is > 4GB
    olen = stb_decompress_length(i);
    stb_barrier_in_b = i;
    stb_barrier_out_e = output + olen;
    stb_barrier_out_b = output;
    i += 16;

    stb_dout = output;
    for (;;) {
        const unsigned char *old_i = i;
        i = stb_decompress_token(i);
        if (i == old_i) {
            if (*i == 0x05 && i[1] == 0xfa) {
                ASSERT(stb_dout == output + olen);
                if (stb_dout != output + olen) return 0;
                if (stb_adler32(1, output, olen) != (unsigned int) STB_IN4(2))
                    return 0;
                return olen;
            } else {
                ASSERT(0); /* NOTREACHED */
                return 0;
            }
        }
        ASSERT(stb_dout <= output + olen);
        if (stb_dout > output + olen)
            return 0;
    }
}


}
