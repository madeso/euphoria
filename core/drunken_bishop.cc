#include "core/drunken_bishop.h"


namespace euphoria::core
{
    Table<int>
    DrunkenBishop
    (
        int hash,
        int width,
        int height,
        int startx,
        int starty
    )
    {
        auto codes = std::vector<int>{};
        for(int byte_index=0; byte_index<4; byte_index +=1)
        {
            const unsigned char byte = hash >> (byte_index * 8);
            for(int s=0; s<8; s+=2)
            {
                codes.emplace_back
                (
                    (byte >> s) & 3
                );
            }
        }
        return DrunkenBishop
        (
            codes,
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

