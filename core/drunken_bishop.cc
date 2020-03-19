#include "core/drunken_bishop.h"

#include "core/assert.h"


namespace euphoria::core
{
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
    ToCodes(u8 byte)
    {
        auto codes = std::vector<int>{};
        for(int s=0; s<8; s+=2)
        {
            codes.emplace_back
            (
                (byte >> (6-s)) & 3
            );
        }
        return codes;
    }


    template<typename T>
    std::vector<int>
    ToCodesFromHash(T hash)
    {
        const auto bytes = ToBytes(hash);
        auto codes = std::vector<int>{};
        for(auto byte: bytes)
        {
            const auto cc = ToCodes(byte);
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
        int startx,
        int starty
    )
    {
        return DrunkenBishop
        (
            ToCodesFromHash(hash),
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
        int startx,
        int starty
    )
    {
        return DrunkenBishop
        (
            ToCodesFromHash(hash),
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
        auto table = Table<int>::FromWidthHeight(width, height);
        auto x = startx != -1? startx : width/2;
        auto y = starty != -1? starty : height/2;

        for(auto code: codes)
        {
            // vertical
            switch(code)
            {
            case 0: case 1: // up
                y += 1;
                break;
            case 2: case 3: // down
                y -= 1;
                break;
            default:
                DIE("invalid case");
            }
            // horizontal
            switch(code)
            {
            case 0: case 2: // left
                x -= 1;
                break;
            case 1: case 3: // right
                x += 1;
                break;
            default:
                DIE("invalid case");
            }

            x = KeepWithin(table.Indices().GetXRange(), x);
            y = KeepWithin(table.Indices().GetYRange(), y);

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


    std::string
    Collapse
    (
        const Table<int>& table,
        const std::vector<std::string> characters
    )
    {
        std::string r = "";

        for(int y=0; y<table.GetHeight(); y+=1)
        {
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
            r += "\n";
        }

        return r;
    }

    // todo: add image collapse with palette
}

