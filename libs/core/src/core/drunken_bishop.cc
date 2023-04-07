#include "core/drunken_bishop.h"

#include "assert/assert.h"

#include <algorithm>


namespace euphoria::core
{
    // drunken bishop algorithm inspired by
    // https://blog.benjojo.co.uk/post/ssh-randomart-how-does-it-work-art
    // todo(Gustav): possible incorperate http://www.ece.cmu.edu/~adrian/projects/validation/validation.pdf


    template<typename T, int total_bytes>
    std::vector<U8>
    to_bytes_generic(T hash)
    {
        auto bytes = std::vector<U8>{};
        for(int byte_index=0; byte_index<total_bytes; byte_index +=1)
        {
            const U8 byte = static_cast<U8>
            (
                0xFF & (hash >> (((total_bytes-1)-byte_index) * 8))
            );
            bytes.emplace_back(byte);
        }
        return bytes;
    }


    std::vector<U8>
    to_bytes(U32 hash)
    {
        return to_bytes_generic<U32, 4>(hash);
    }


    std::vector<U8>
    to_bytes(U64 hash)
    {
        return to_bytes_generic<U64, 8>(hash);
    }


    std::vector<int>
    to_codes(U8 byte, bool msb_first)
    {
        auto codes = std::vector<int>{};
        for(int step=0; step<8; step += 2)
        {
            const auto code_entry = msb_first
                ? (byte >> (6-step)) & 3
                : (byte >> step) & 3
                ;
            codes.emplace_back(code_entry);
        }
        return codes;
    }


    std::vector<int>
    to_codes(const std::vector<U8>& bytes, bool msb_first)
    {
        auto codes = std::vector<int>{};
        for(auto byte: bytes)
        {
            const auto cc = to_codes(byte, msb_first);
            for(auto c: cc)
            {
                codes.emplace_back(c);
            }
        }
        return codes;
    }


    Table<int>
    get_drunken_bishop_result
    (
        U32 hash,
        int width,
        int height,
        bool msb_first,
        int startx,
        int starty
    )
    {
        return get_drunken_bishop_result
        (
            to_codes(to_bytes(hash), msb_first),
            width,
            height,
            startx,
            starty
        );
    }

    Table<int>
    get_drunken_bishop_result
    (
        U64 hash,
        int width,
        int height,
        bool msb_first,
        int startx,
        int starty
    )
    {
        return get_drunken_bishop_result
        (
            to_codes(to_bytes(hash), msb_first),
            width,
            height,
            startx,
            starty
        );
    }

    Table<int>
    get_drunken_bishop_result
    (
        const std::vector<int>& codes,
        int width,
        int height,
        int startx,
        int starty
    )
    {
        auto table = core::Table<int>::from_width_height(width, height, 0);
        auto x = startx != -1? startx : width/2;
        auto y = starty != -1? starty : height/2;

        for(auto code: codes)
        {
            constexpr int vertical_mask = 0b10;
            constexpr int horizontal_mask = 0b01;

            const auto dy = (vertical_mask & code) != 0 ? 1 : -1;
            const auto dx = (horizontal_mask & code) != 0 ? 1 : -1;

            x = keep_within(table.get_indices().get_range_x(), x + dx);
            y = keep_within(table.get_indices().get_range_y(), y + dy);

            table(x, y) += 1;
        }

        return table;
    }


    std::vector<std::string>
    get_ssh_characters()
    {
        return
        {
            " ", ".", "o", "+", "=",
            "*", "B", "O", "X", "@",
            "%", "&", "#", "/", "?"
        };
    }


    std::vector<std::string>
    render_table
    (
        const Table<int>& table,
        const std::vector<std::string>& characters
    )
    {
        auto rr = std::vector<std::string>{};

        for(int y=0; y<table.get_height(); y+=1)
        {
            std::string r;
            for(int x=0; x<table.get_width(); x+=1)
            {
                const auto v = std::max
                (
                    0,
                    std::min
                    (
                        table(x,y),
                        c_sizet_to_int(characters.size())
                    )
                );
                r += characters[v];
            }
            rr.emplace_back(r);
        }

        return rr;
    }

    // todo(Gustav): add image collapse with palette
}

