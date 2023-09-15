#include <array>

#include "core/argparse.h"
#include "core/loadedfont.h"
#include "core/textbox.h"
#include "core/image_to_text.h"
#include "core/utf8.h"
#include "core/io.h"
#include "log/log.h"

using namespace eu::core;


bool print_char
(
    TextBox* dst,
    const LoadedFont& font,
    int* sx,
    int sy,
    int codepoint
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

    // todo(Gustav): ImageToStringTable might not convert corectly,
    // consider extending :)
    const auto img = from_image_to_string_table(glyph.image, true, Grayscale::a);

    for(auto y=0; y<img.get_height(); y+=1)
    {
        for(auto x=0; x<img.get_width(); x+=1)
        {
            const auto c = img[{x, y}];
            if(c == ' ') { continue; }
            dst->put_char(px+x, img.get_height()+py-y, c);
        }
    }

    *sx = *sx + glyph.advance;

    return true;
}


bool print_string
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

    const auto r = calc_utf8_to_codepoints(str, [&](int codepoint)
    {
        status = print_char(dst, font, &x, y, codepoint) && status;
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
    builtin8, builtin13, font_file
};


eu::core::LoadedFont
get_font
(
    FontName font_name,
    const std::string& font_file,
    int font_size,
    const std::string& chars
)
{
    if(font_name == FontName::font_file && font_file.empty())
    {
        std::cerr << "warning: Font file requested, but no file specified!";
        return load_characters_from_builtin8();
    }

    // todo(gustav): check file name
    if(font_file.empty() == false)
    {
        auto file = io::read_file_to_chunk(font_file);
        if(file == nullptr)
        {
            std::cerr << "Failed to open file: '" << font_file << "'\n";
        }
        else
        {
            return get_characters_from_font(file, font_size, chars);
        }
    }

    return font_name == FontName::builtin13
        ? load_characters_from_builtin13()
        : load_characters_from_builtin8()
        ;
}


int
main(int argc, char* argv[])
{
    auto font_name = FontName::builtin8;
    std::string font_file;
    int size = 10;
    std::string chars = "ABCDEFGHIJKLMNOPQRSTUWXYZ!@#$%^&*()_+abcdefghijklmnopqrstuwxyz0123456789-=<>,./\\[]{};:";
    std::string text;

    auto parser = argparse::Parser {"font test"};
    parser.add("text", &text).set_help("The text to print");
    parser.add("--font", &font_name).set_help("The font to use");
    parser.add("--file", &font_file).set_help("If not empty, use this font file");
    parser.add("--size", &size).set_help("the font file size");
    parser.add("--chars", &chars).set_help("The font file chars");

    if(auto r = parser.parse(argc, argv))
    {
        return *r;
    }

    auto font = get_font(font_name, font_file, size, chars);

    auto box = TextBox::create_empty();
    const bool printed = print_string(&box, font, 0, 0, text);
    // print textbox
    const auto strings = box.to_string();
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

