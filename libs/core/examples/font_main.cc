#include <array>

#include "core/argparse.h"
#include "core/loadedfont.h"
#include "core/textbox.h"
#include "core/imageops.h"
#include "core/utf8.h"
#include "core/io.h"
#include "core/log.h"

using namespace euphoria::core;


bool PrintChar
(
    TextBox* dst,
    const LoadedFont& font,
    int* sx,
    int sy,
    unsigned int codepoint
)
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

    const auto ppx = *sx + glyph.bearing_x;
    const auto ppy = sy + glyph.bearing_y;

    const auto px = std::max(ppx, 0);
    const auto py = std::max(ppy, 0);

    if (ppx < 0 || ppy < 0)
    {
        LOG_INFO("warning: xy out of bounds {0} {1}", ppx, ppy);
    }

    // todo: ImageToStringTable might not convert corectly,
    // consider extending :)
    const auto img = ImageToStringTable(glyph.image, true, Grayscale::A);

    for(auto y=0; y<img.GetHeight(); y+=1)
    {
        for(auto x=0; x<img.GetWidth(); x+=1)
        {
            const auto c = img(x,y);
            if(c == ' ') { continue; }
            dst->PutChar(px+x, img.GetHeight()+py-y, c);
        }
    }

    *sx = *sx + glyph.advance;

    return true;
}


bool PrintString
(
    TextBox* dst,
    const LoadedFont& font,
    int sx,
    int sy,
    const std::string& str
)
{
    int x = sx;
    int y = sy;
    bool status = true;

    const auto r = Utf8ToCodepoints(str, [&](int codepoint)
    {
        status = PrintChar(dst, font, &x, y, codepoint) && status;
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


enum class FontName
{
    Builtin8, Builtin13, FontFile
};


euphoria::core::LoadedFont
GetFont
(
    FontName font_name,
    const std::string& font_file,
    int font_size,
    const std::string& chars
)
{
    if(font_name == FontName::FontFile && font_file.empty())
    {
        std::cerr << "warning: Font file requested, but no file specified!";
        return LoadCharactersFromBuiltin8();
    }

    // todo(gustav): check file name
    if(font_file.empty() == false)
    {
        auto file = io::FileToChunk(font_file);
        if(file == nullptr)
        {
            std::cerr << "Failed to open file: '" << font_file << "'\n";
        }
        else
        {
            return GetCharactersFromFont(file, font_size, chars);
        }
    }

    return font_name == FontName::Builtin13
        ? LoadCharactersFromBuiltin13()
        : LoadCharactersFromBuiltin8()
        ;
}


int
main(int argc, char* argv[])
{
    auto font_name = FontName::Builtin8;
    std::string font_file;
    int size = 10;
    std::string chars = "ABCDEFGHIJKLMNOPQRSTUWXYZ!@#$%^&*()_+abcdefghijklmnopqrstuwxyz0123456789-=<>,./\\[]{};:";
    std::string text;

    auto parser = argparse::Parser {"font test"};
    parser.Add("text", &text).Help("The text to print");
    parser.Add("--font", &font_name).Help("The font to use");
    parser.Add("--file", &font_file).Help("If not empty, use this font file");
    parser.Add("--size", &size).Help("the font file size");
    parser.Add("--chars", &chars).Help("The font file chars");

    if(auto r = parser.Parse(argc, argv))
    {
        return *r;
    }

    auto font = GetFont(font_name, font_file, size, chars);

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

