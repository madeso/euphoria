#include "core/textbox.h"

#include <cstdlib>

#include "core/cint.h"
#include "core/assert.h"
#include "core/stringutils.h"

namespace
{
    bool TerminalSupportUtf8()
    {
        const auto clang = std::getenv("LANG");
        
        if(clang)
        {
            const auto lang = std::string(clang);
            const auto lower = euphoria::core::ToLower(lang);
            const auto ends = euphoria::core::EndsWith(lower, "utf-8");
            return ends;
        }

        return false;
    }

    bool HasChar(char c)
    {
        return c & euphoria::core::BIT_NO_LINE;
    }

    bool IsEmpty(char c)
    {
        return c ==' ' || !c;
    };
}


namespace euphoria::core
{

    std::string TextBoxStyle::GetString(char c) const
    {
        ASSERTX(c>0, c);
        ASSERTX(c-1 < Csizet_to_int(connections.size()), CharToString(c, CharToStringStyle::IncludeHex), connections.size());
        return connections[c-1];
    }


    TextBoxStyle TextBoxStyle::Create(std::function<std::string(char)> connections_func)
    {
        euphoria::core::TextBoxStyle style;
        for(char c=1; c<16; c+=1)
        {
            ASSERTX(c-1 < Csizet_to_int(style.connections.size()), CharToString(c, CharToStringStyle::IncludeHex), style.connections.size());
            style.connections[c-1] = connections_func(c);
        }
        return style;
    }

TextBoxStyle TerminalStyle()
{
    if(TerminalSupportUtf8())
    {
        // return Utf8StraightStyle();
        return Utf8RoundedStyle();
    }
    else
    {
        return AsciiStyle();
    }
}

TextBoxStyle::TextBoxStyle()
{
    connections.resize(15);
}

TextBoxStyle Utf8StraightStyle()
{
    return TextBoxStyle::Create([](char c) {
        switch(c) {
        case BIT_LEFT:                                 return u8"─";
        case BIT_RIGHT:                                return u8"─";
        case BIT_LEFT | BIT_RIGHT:                     return u8"─";
        case BIT_UP:                                   return u8"│";
        case BIT_DOWN:                                 return u8"│";
        case BIT_UP | BIT_DOWN:                        return u8"│";
        case BIT_LEFT| BIT_UP:                         return u8"┘";
        case BIT_LEFT | BIT_DOWN:                      return u8"┐";
        case BIT_RIGHT | BIT_UP:                       return u8"└";
        case BIT_RIGHT | BIT_DOWN:                     return u8"┌";
        case BIT_LEFT | BIT_RIGHT | BIT_UP:            return u8"┴";
        case BIT_LEFT | BIT_RIGHT | BIT_DOWN:          return u8"┬";
        case BIT_LEFT | BIT_UP | BIT_DOWN:             return u8"┤";
        case BIT_RIGHT | BIT_UP | BIT_DOWN:            return u8"├";
        case BIT_LEFT | BIT_RIGHT | BIT_UP | BIT_DOWN: return u8"┼";
        default:
            DIE("Invalid combination");
            return "X";
        }
    });
}

TextBoxStyle Utf8RoundedStyle()
{
    return TextBoxStyle::Create([](char c) {
        switch(c) {
        case BIT_LEFT:                                 return u8"─";
        case BIT_RIGHT:                                return u8"─";
        case BIT_LEFT | BIT_RIGHT:                     return u8"─";
        case BIT_UP:                                   return u8"│";
        case BIT_DOWN:                                 return u8"│";
        case BIT_UP | BIT_DOWN:                        return u8"│";
        case BIT_LEFT| BIT_UP:                         return u8"╯";
        case BIT_LEFT | BIT_DOWN:                      return u8"╮";
        case BIT_RIGHT | BIT_UP:                       return u8"╰";
        case BIT_RIGHT | BIT_DOWN:                     return u8"╭";
        case BIT_LEFT | BIT_RIGHT | BIT_UP:            return u8"┴";
        case BIT_LEFT | BIT_RIGHT | BIT_DOWN:          return u8"┬";
        case BIT_LEFT | BIT_UP | BIT_DOWN:             return u8"┤";
        case BIT_RIGHT | BIT_UP | BIT_DOWN:            return u8"├";
        case BIT_LEFT | BIT_RIGHT | BIT_UP | BIT_DOWN: return u8"┼";
        default:
            DIE("Invalid combination");
            return "X";
        }
    });
}

TextBoxStyle Utf8DoubleLineStyle()
{
    return TextBoxStyle::Create([](char c) {
        switch(c) {
        case BIT_LEFT:                                 return u8"═";
        case BIT_RIGHT:                                return u8"═";
        case BIT_LEFT | BIT_RIGHT:                     return u8"═";
        case BIT_UP:                                   return u8"║";
        case BIT_DOWN:                                 return u8"║";
        case BIT_UP | BIT_DOWN:                        return u8"║";
        case BIT_LEFT| BIT_UP:                         return u8"╝";
        case BIT_LEFT | BIT_DOWN:                      return u8"╗";
        case BIT_RIGHT | BIT_UP:                       return u8"╚";
        case BIT_RIGHT | BIT_DOWN:                     return u8"╔";
        case BIT_LEFT | BIT_RIGHT | BIT_UP:            return u8"╩";
        case BIT_LEFT | BIT_RIGHT | BIT_DOWN:          return u8"╦";
        case BIT_LEFT | BIT_UP | BIT_DOWN:             return u8"╣";
        case BIT_RIGHT | BIT_UP | BIT_DOWN:            return u8"╠";
        case BIT_LEFT | BIT_RIGHT | BIT_UP | BIT_DOWN: return u8"╬";
        default:
            DIE("Invalid combination");
            return "X";
        }
    });
}

TextBoxStyle AsciiStyle()
{
    return TextBoxStyle::Create([](char c) {
        switch(c) {
        case BIT_LEFT:                                 return "-";
        case BIT_RIGHT:                                return "-";
        case BIT_LEFT | BIT_RIGHT:                     return "-";
        case BIT_UP:                                   return "|";
        case BIT_DOWN:                                 return "|";
        case BIT_UP | BIT_DOWN:                        return "|";
        case BIT_LEFT| BIT_UP:                         return "'";
        case BIT_LEFT | BIT_DOWN:                      return ".";
        case BIT_RIGHT | BIT_UP:                       return "`";
        case BIT_RIGHT | BIT_DOWN:                     return ",";
        case BIT_LEFT | BIT_RIGHT | BIT_UP:            return "+";
        case BIT_LEFT | BIT_RIGHT | BIT_DOWN:          return "+";
        case BIT_LEFT | BIT_UP | BIT_DOWN:             return "+";
        case BIT_RIGHT | BIT_UP | BIT_DOWN:            return "+";
        case BIT_LEFT | BIT_RIGHT | BIT_UP | BIT_DOWN: return "+";
        default:
            DIE("Invalid combination");
            return "X";
        }
    });
}


TextBox::TextBox()
{}

TextBox TextBox::Empty()
{
    return {};
}

TextBox TextBox::FromString(const std::vector<std::string>& str)
{
    TextBox ret;
    ret.data = str;
    return ret;
}


void TextBox::putchar(char c, std::size_t x, std::size_t y)
{
    extend(x,y);
    data[y][x] = c;
}


void TextBox::extend(std::size_t x, std::size_t y)
{
    if(y >= data.size())
    {
        data.resize(y+1);
    }
    if(data[y].size() <= x)
    {
        data[y].resize(x+1, ' ');
    }
}


void TextBox::putline(const std::string& line, std::size_t x_start, std::size_t y)
{
    auto size_minus_1 = line.empty() ? 0 : line.size()-1;
    extend(x_start+size_minus_1, y);
    
    for(std::size_t line_index = 0; line_index < line.size(); ++line_index)
    {
        const auto x = x_start + line_index;
        const auto source_texel = line[line_index];

        ASSERT(x < data[y].size());

        if(IsEmpty(source_texel))
        {
            continue;
        }

        char& target = data[y][x];
        if( IsEmpty(target) || HasChar(source_texel))
        {
            target = source_texel;
        }
        else
        {
            if( HasChar(target) )
            {
                target = 0;
                target |= source_texel;
            }
        }
    }
}


void TextBox::putbox(std::size_t x, std::size_t y, const TextBox& b)
{
    for(std::size_t p = 0; p < b.data.size(); ++p)
    {
        putline(b.data[p], x, y+p);
    }
}


TextBox TextBox::PutBoxCopy(std::size_t x, std::size_t y, const TextBox& b) const
{
    TextBox self = *this;
    self.putbox(x, y, b);
    return self;
}
    

void TextBox::trim()
{
    for(auto& s: data)
    {
        std::size_t end = s.size();
        while(end > 0 && IsEmpty(s[end-1]))
        {
            --end;
        }
        s.erase(end);
    }

    while(!data.empty() && data.back().empty())
    {
        data.pop_back();
    }
}


std::size_t TextBox::height() const
{
    return data.size();
}


std::size_t TextBox::width()  const
{
    std::size_t result = 0;

    for(const auto& s: data)
    {
        result = std::max(result, s.size());
    }

    return result;
}

std::pair<std::size_t, std::size_t> TextBox::Size() const
{
    return {width(), height()};
}


void TextBox::hline(std::size_t x, std::size_t y, std::size_t line_width, bool bef, bool aft)
{
    for(std::size_t line_index=0; line_index<line_width; ++line_index)
    {
        modchar(x+line_index, y, [&](char& c)
        {
            if( HasChar(c) )
            {
                c = 0;
            }

            if(line_index>0 || bef)
            {
                c |= BIT_LEFT;
            }

            if(aft || (line_index+1)<line_width)
            {
                c |= BIT_RIGHT;
            }
        });
    }
}
    

void TextBox::vline(std::size_t x, std::size_t y, std::size_t line_height, bool bef, bool aft)
{
    for(std::size_t line_index=0; line_index<line_height; ++line_index)
    {
        modchar(x, y+line_index, [&](char& c)
        {
            if( HasChar(c) )
            {
                c = 0;
            }

            if(line_index>0 || bef)
            {
                c |= BIT_UP;
            }

            if(aft || (line_index+1)<line_height)
            {
                c |= BIT_DOWN;
            }
        });
    }
}


std::size_t TextBox::horiz_append_position(std::size_t y, const TextBox& b) const
{
    std::size_t mywidth = width();
    std::size_t theirheight = b.height();
    std::size_t reduce = mywidth;

    for(std::size_t p=0; p<theirheight; ++p)
    {
        std::size_t theirpadding = b.FindLeftPadding(p);
        std::size_t mypadding    = FindRightPadding(y+p);
        reduce = std::min(reduce, mypadding + theirpadding);
    }

    return mywidth - reduce;
}


std::size_t TextBox::vert_append_position(std::size_t x, const TextBox& b) const
{
    std::size_t myheight = height();
    std::size_t theirwidth = b.width();
    std::size_t reduce = myheight;
    
    for(std::size_t p=0; p<theirwidth; ++p)
    {
        std::size_t theirpadding = b.FindTopPadding(p);
        std::size_t mypadding    = FindBottomPadding(x+p);
        reduce = std::min(reduce, mypadding + theirpadding);
    }
    return myheight - reduce;
}


std::vector<std::string> TextBox::to_string(const TextBoxStyle& style) const
{
    std::vector<std::string> ret;
    bool want_newline = true;
    auto last_line = [&ret, &want_newline]() -> std::string&
    {
        if(want_newline)
        {
          ret.emplace_back("");
          want_newline = false;
        }

        return ret[ret.size()-1];
    };

    auto append = [&](char c)
    {
        if(c == '\n')
        {
            if(want_newline)
            {
                // if we want another newline, add the one before first
                last_line();
            }
            want_newline = true;
        }
        else
        {
            last_line() += c;
        }
    };

    const std::size_t h = height();
    for(std::size_t y = 0; y < h; ++y)
    {
        const std::string& s = data[y];
        for(std::size_t x = 0; x < s.size(); ++x)
        {
            unsigned char c = s[x];
            if(c > 0 && c < 16)
            {
              const auto str = style.GetString(c);
              for(auto line_char: str)
              {
                  append(line_char);
              }
            }
            else
            {
              append(c);
            }
        }
        append('\n');
    }
    return ret;
}

std::size_t TextBox::FindLeftPadding(std::size_t y) const
{
    std::size_t max = width();
    std::size_t result = 0;
    if(y >= data.size())
    {
        return max;
    }

    const std::string& line = data[y];
    while(result < line.size() && IsEmpty(line[result]))
    {
        ++result;
    }

    return result;
}


std::size_t TextBox::FindRightPadding(std::size_t y) const
{
    std::size_t max = width();
    std::size_t position = max;
    std::size_t result = 0;
    if(y >= data.size())
    {
        return max;
    }

    const std::string& line = data[y];
    while(position-- > 0 && (position >= line.size() || IsEmpty(line[position])))
    {
        ++result;
    }

    return result;
}


std::size_t TextBox::FindTopPadding(std::size_t x) const
{
    std::size_t result = 0;
    std::size_t max = data.size();
    
    while(result < max && (x >= data[result].size() || IsEmpty(data[result][x])))
    {
        ++result;
    }

    return result;
}


std::size_t TextBox::FindBottomPadding(std::size_t x) const
{
    std::size_t result = 0;
    std::size_t max = data.size();
    std::size_t position = max;
    
    while(position-- > 0 && (x >= data[position].size() || IsEmpty(data[position][x])))
    {
        ++result;
    }

    return result;
}

namespace detail
{
    void CreateTreeGraph(
        TextBox& result,
        size_t maxwidth,
        const std::vector<TextBox>& boxes,
        bool oneliner_test,
        bool simple_test,
        bool separate1st_test,
        std::string atom,
        std::size_t margin,
        std::size_t firstx
        )
    {
        std::size_t sum_width = 0;
        for(const auto& b: boxes)
        {
          sum_width += b.width()+margin;
        }

        bool oneliner = false;
        if(oneliner_test && !separate1st_test)
        {
            std::size_t totalwidth = 0;
            for(auto i = boxes.begin(); ; )
            {
                const auto& cur = *i;
                if(++i == boxes.end())
                {
                  totalwidth += cur.width();
                  break;
                }

                totalwidth += cur.width() + margin;
            }
            oneliner = (atom.size() + margin + totalwidth) < maxwidth;
        }

        bool simple = oneliner && boxes.size() == 1 && simple_test;

        std::size_t y = simple ? 0 : 1;

        for(auto i = boxes.begin(); i != boxes.end(); ++i)
        {
            auto next = std::next(i);
            const TextBox& cur = *i;
            unsigned width = cur.width();

            std::size_t usemargin = (simple || oneliner) ? (margin/2) : margin;
            std::size_t x = result.horiz_append_position(y, cur) + usemargin;
            if(x==usemargin)
            {
              x = oneliner ? atom.size()+usemargin : firstx;
            }

            if(!oneliner && (x + width > maxwidth || (separate1st_test && i == ++boxes.begin())))
            {
                // Start a new line if this item won't fit in the end of the current line
                x        = firstx;
                simple   = false;
                oneliner = false;
            }

            // At the beginning of line, judge whether to add room for horizontal placement
            bool horizontal = x > firstx;
            if(!oneliner && !horizontal && next != boxes.end() && !(separate1st_test && i == boxes.begin()))
            {
                std::size_t nextwidth      = next->width();
                std::size_t combined_width = cur.horiz_append_position(0, *next) + margin + nextwidth;
                if(combined_width <= maxwidth)
                {
                    // Enact horizontal placement by giving 1 row of room for the connector
                    horizontal = true;
                    TextBox combined = cur;
                    combined.putbox(cur.horiz_append_position(0, *next) + margin, 0, *next);
                    y = std::max(result.vert_append_position(x, combined), std::size_t(1));
                    if(!oneliner)
                    {
                      ++y;
                    }
                }
            }
            if(!horizontal)
            {
                y = std::max(result.vert_append_position(x, cur), std::size_t(1));
            }
            if(horizontal && !simple && !oneliner)
            {
                for(;;)
                {
                    // Check if there is room for a horizontal connector. If not, increase y
                    auto connector = TextBox::Empty();
                    connector.putline(std::string(1+(x-0), '-'), 0, 0);
                    if(result.horiz_append_position(y-1, connector) > x)
                    {
                      ++y;
                    }
                    else
                    {
                      break;
                    }
                    y = std::max(result.vert_append_position(x, cur), y);
                }
            }

            if(simple)
            {
                if(x > atom.size())
                {
                    result.hline(atom.size(), 0, 1+x-atom.size(), false,false);
                }
            }
            else if(oneliner)
            {
                unsigned cx = x;
                unsigned cy = y-1;
                if(x > atom.size())
                {
                    result.hline(atom.size(), 0, 1+x-atom.size(), false,false);
                }
                result.vline(cx, cy, 1,          false,true);
            }
            else if(horizontal)
            {
                unsigned cx = x;
                unsigned cy = y-1;
                result.vline(0,  1,  1 + (cy-1), true,false);
                result.hline(0,  cy, 1 + (cx-0), false,false);
                result.vline(cx, cy, 1,          false,true);
            }
            else
            {
                unsigned cx = x-1;
                unsigned cy = y;
                result.vline(0,1,  1 + (cy-1), true,false);
                result.hline(0,cy, 1 + (cx-0), false,true);
            }

            result.putbox(x, y, cur);
        }
    }
}

}
