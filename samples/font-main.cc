#include <array>

#include "core/argparse.h"
#include "core/fonts.h"
#include "core/textbox.h"
#include "core/imageops.h"
#include "core/utf8.h"

using namespace euphoria::core;


bool PrintChar(TextBox* dst, LoadedFont& font, int* sx, int* sy, unsigned int codepoint)
{
    const auto glyph_index = font.codepoint_to_glyph.find(codepoint);
    if(glyph_index == font.codepoint_to_glyph.end())
    {
        std::cerr << "Glyph not found in font " << codepoint << "\n";
        return false;
    }

    const auto& glyph = glyph_index->second;
    if(glyph.valid == false)
    {
        std::cerr << "Invalid glyph for " << codepoint << "\n";
        return false;
    }

    const auto px = *sx + glyph.bearing_x;
    const auto py = *sy + glyph.bearing_y;

    // todo: ImageToStringTable might not convert corectly, consider extending :)
    const auto img = ImageToStringTable(glyph.image, true, Grayscale::A);

    for(auto y=0; y<img.GetHeight(); y+=1)
    for(auto x=0; x<img.GetWidth(); x+=1)
    {
        const auto c = img(x,y);
        if(c == ' ') continue;
        dst->PutChar(px+x, img.GetHeight()+py-y, c);
    }

    *sx = *sx + glyph.advance;

    return true;
}


bool PrintString(TextBox* dst, LoadedFont& font, int sx, int sy, const std::string& str)
{
    int x = sx;
    int y = sy;
    bool status = true;

    const auto r = Utf8ToCodepoints(str, [&](int codepoint)
    {
        status = PrintChar(dst, font, &x, &y, codepoint) && status;
    });

    if(r == false)
    {
        std::cerr << "failed to convert string to codepoints\n";
    }

    if(status == false)
    {
        std::cerr << "failed to find some chars in font\n";
    }

    return status && r;
}


int
main(int argc, char* argv[])
{
    bool use_alternate_builtin = false;
    std::string text;

    auto parser = argparse::Parser {"font test"};
    parser.Add("text", &text).Help("The text to print");
    parser.SetTrue("-alt", &use_alternate_builtin).Help("Use another builtin font");

    if(auto r = parser.Parse(argc, argv))
    {
        return *r;
    }

    auto font = use_alternate_builtin
        ? LoadCharactersFromBuiltin13()
        : LoadCharactersFromBuiltin8();

    auto box = TextBox::Empty();
    const bool printed = PrintString(&box, font, 0, 0, text);
    // print textbox
    const auto strings = box.ToString();
    for(const auto& s: strings)
    {
        std::cout << s << "\n";
    }

    if(!printed)
    {
        std::cerr << "Failed to print\n";
    }

    return printed ? 0 : -1;
}

