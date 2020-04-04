#include "core/drunken_bishop.h"

#include "core/assert.h"

#include <algorithm>


namespace euphoria::core
{
    // drunken bishop algorithm inspired by
    // https://blog.benjojo.co.uk/post/ssh-randomart-how-does-it-work-art
    // todo(Gustav): possible incorperate http://www.ece.cmu.edu/~adrian/projects/validation/validation.pdf


    template<typename T, int total_bytes>
    std::vector<u8>
    ToBytesGeneric(T hash)
    {
        auto bytes = std::vector<u8>{};
        for(int byte_index=0; byte_index<total_bytes; byte_index +=1)
        {
            const u8 byte = hash >> (((total_bytes-1)-byte_index) * 8);
            bytes.emplace_back(byte);
        }
        return bytes;
    }


    std::vector<u8>
    ToBytes(u32 hash)
    {
        return ToBytesGeneric<u32, 4>(hash);
    }


    std::vector<u8>
    ToBytes(u64 hash)
    {
        return ToBytesGeneric<u32, 8>(hash);
    }


    std::vector<int>
    ToCodes(u8 byte, bool msb_first)
    {
        auto codes = std::vector<int>{};
        for(int s=0; s<8; s+=2)
        {
            const auto b = msb_first
                ? (byte >> (6-s)) & 3
                : (byte >> s) & 3
                ;
            codes.emplace_back(b);
        }
        return codes;
    }


    std::vector<int>
    ToCodes(const std::vector<u8>& bytes, bool msb_first)
    {
        auto codes = std::vector<int>{};
        for(auto byte: bytes)
        {
            const auto cc = ToCodes(byte, msb_first);
            for(auto c: cc)
            {
                codes.emplace_back(c);
            }
        }
        return codes;
    }


    Table<int>
    DrunkenBishop
    (
        u32 hash,
        int width,
        int height,
        bool msb_first,
        int startx,
        int starty
    )
    {
        return DrunkenBishop
        (
            ToCodes(ToBytes(hash), msb_first),
            width,
            height,
            startx,
            starty
        );
    }


    Table<int>
    DrunkenBishop
    (
        u64 hash,
        int width,
        int height,
        bool msb_first,
        int startx,
        int starty
    )
    {
        return DrunkenBishop
        (
            ToCodes(ToBytes(hash), msb_first),
            width,
            height,
            startx,
            starty
        );
    }


    Table<int>
    DrunkenBishop
    (
        const std::vector<int>& codes,
        int width,
        int height,
        int startx,
        int starty
    )
    {
        auto table = Table<int>::FromWidthHeight(width, height, 0);
        auto x = startx != -1? startx : width/2;
        auto y = starty != -1? starty : height/2;

        for(auto code: codes)
        {
            constexpr int vertical_mask = 0b10;
            constexpr int horizontal_mask = 0b01;

            const auto dy = (vertical_mask & code) ? 1 : -1;
            const auto dx = (horizontal_mask & code) ? 1 : -1;

            x = KeepWithin(table.Indices().GetXRange(), x + dx);
            y = KeepWithin(table.Indices().GetYRange(), y + dy);

            table(x, y) += 1;
        }

        return table;
    }


    std::vector<std::string>
    GetSshCharacters()
    {
        return
        {
            " ", ".", "o", "+", "=",
            "*", "B", "O", "X", "@",
            "%", "&", "#", "/", "?"
        };
    }


    std::vector<std::string>
    Collapse
    (
        const Table<int>& table,
        const std::vector<std::string> characters
    )
    {
        auto rr = std::vector<std::string>{};

        for(int y=0; y<table.GetHeight(); y+=1)
        {
            std::string r = "";
            for(int x=0; x<table.GetWidth(); x+=1)
            {
                const auto v =
                std::max
                (
                    0,
                    std::min
                    (
                        table(x,y),
                        Csizet_to_int(characters.size())
                    )
                );
                r += characters[v];
            }
            rr.emplace_back(r);
        }

        return rr;
    }

    // todo: add image collapse with palette
}

