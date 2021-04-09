// clang-tidy: ignore

#ifndef EUPHORIA_CORE_DECOMPRESS_H
#define EUPHORIA_CORE_DECOMPRESS_H

namespace euphoria::core
{
    // Decompress is slighly modified code from the imgui repo, only to remove the use of global variables
    // this is manly here so we can use the handly compression tool from the imgui distro for files other than fonts
    //
    // The following note is from imgui repo comment:
    //
    //-----------------------------------------------------------------------------
    // Compressed with stb_compress() then converted to a C array and encoded as base85.
    // Use the program in misc/fonts/binary_to_compressed_c.cpp to create the array from a TTF file.
    // The purpose of encoding as base85 instead of "0x00,0x01,..." style is only save on _source code_ size.
    // Decompression from stb.h (public domain) by Sean Barrett https://github.com/nothings/stb/blob/master/stb.h
    //-----------------------------------------------------------------------------

    struct Decompress
    {
        static unsigned int stb_decompress_length(const unsigned char *input);

        unsigned int stb_decompress(unsigned char *output, const unsigned char *i, unsigned int /*length*/);

    private:
        unsigned char *stb__barrier_out_e, *stb__barrier_out_b;
        const unsigned char *stb__barrier_in_b;
        unsigned char *stb__dout;

        void stb__match(const unsigned char *data, unsigned int length);
        void stb__lit(const unsigned char *data, unsigned int length);

        const unsigned char *stb_decompress_token(const unsigned char *i);

        unsigned int stb_adler32(unsigned int adler32, unsigned char *buffer, unsigned int buflen);
    };

}

#endif // EUPHORIA_CORE_DECOMPRESS_H
