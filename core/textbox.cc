#include "core/textbox.h"

#include <cstdlib>
#include <numeric>

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
    }
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

TextBox TextBox::FromStrings(const std::vector<std::string>& str)
{
    TextBox ret;
    ret.data = str;
    return ret;
}


void TextBox::PutChar(std::size_t x, std::size_t y, char c)
{
    ExtendTo(x,y);
    data[y][x] = c;
}


void TextBox::ExtendTo(std::size_t x, std::size_t y)
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


void TextBox::PutString(std::size_t x, std::size_t y, const std::string& line)
{
    for(std::size_t index = 0; index < line.length(); index+=1)
    {
        PutChar(x+index, y, line[index]);
    }
}

TextBox TextBox::FromString(const std::string& s, std::size_t x, std::size_t y)
{
    auto tb = Empty();
    tb.PutString(x, y, s);
    return tb;
}


void TextBox::PutBox(std::size_t x_start, std::size_t y_start, const TextBox& b)
{
    for(std::size_t p = 0; p < b.data.size(); p+=1)
    {
        const auto line = b.data[p];
        const auto y = y_start + p;

        const auto size_minus_1 = line.empty() ? 0 : line.size()-1;
        ExtendTo(x_start+size_minus_1, y);
        
        for(std::size_t line_index = 0; line_index < line.size(); line_index+=1)
        {
            const auto x = x_start + line_index;
            const auto source_texel = line[line_index];

            ASSERT(x < data[y].size());

            if(!IsEmpty(source_texel))
            {
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
    }
}


TextBox TextBox::PutBoxCopy(std::size_t x, std::size_t y, const TextBox& b) const
{
    TextBox self = *this;
    self.PutBox(x, y, b);
    return self;
}
    

void TextBox::Trim()
{
    for(auto& s: data)
    {
        std::size_t end = s.size();
        while(end > 0 && IsEmpty(s[end-1]))
        {
            end-=1;
        }
        s.erase(end);
    }

    while(!data.empty() && data.back().empty())
    {
        data.pop_back();
    }
}


std::size_t TextBox::Height() const
{
    return data.size();
}


std::size_t TextBox::Width()  const
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
    return {Width(), Height()};
}


void TextBox::PutHoriLine(std::size_t x, std::size_t y, std::size_t line_width, bool bef, bool aft)
{
    for(std::size_t line_index=0; line_index<line_width; line_index+=1)
    {
        ModChar(x+line_index, y, [&](char& c)
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
    

void TextBox::PutVertLine(std::size_t x, std::size_t y, std::size_t line_height, bool bef, bool aft)
{
    for(std::size_t line_index=0; line_index<line_height; line_index+=1)
    {
        ModChar(x, y+line_index, [&](char& c)
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
    std::size_t mywidth = Width();
    std::size_t theirheight = b.Height();
    std::size_t reduce = mywidth;

    for(std::size_t p=0; p<theirheight; p+=1)
    {
        std::size_t theirpadding = b.FindLeftPadding(p);
        std::size_t mypadding    = FindRightPadding(y+p);
        reduce = std::min(reduce, mypadding + theirpadding);
    }

    return mywidth - reduce;
}


std::size_t TextBox::vert_append_position(std::size_t x, const TextBox& b) const
{
    std::size_t myheight = Height();
    std::size_t theirwidth = b.Width();
    std::size_t reduce = myheight;
    
    for(std::size_t p=0; p<theirwidth; p+=1)
    {
        std::size_t theirpadding = b.FindTopPadding(p);
        std::size_t mypadding    = FindBottomPadding(x+p);
        reduce = std::min(reduce, mypadding + theirpadding);
    }
    return myheight - reduce;
}


std::vector<std::string> TextBox::ToString(const TextBoxStyle& style) const
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

    const std::size_t h = Height();
    for(std::size_t y = 0; y < h; y+=1)
    {
        const std::string& s = data[y];
        for(std::size_t x = 0; x < s.size(); x+=1)
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
    const std::size_t max = Width();
    
    if(y >= data.size())
    {
        return max;
    }

    const std::string& line = data[y];

    std::size_t result = 0;
    while(result < line.size() && IsEmpty(line[result]))
    {
        result+=1;
    }

    return result;
}


std::size_t TextBox::FindRightPadding(std::size_t y) const
{
    const std::size_t max = Width();
    
    if(y >= data.size())
    {
        return max;
    }

    const std::string& line = data[y];
    std::size_t position = max;
    std::size_t result = 0;
    while(position != 0 && (position-1 >= line.size() || IsEmpty(line[position-1])))
    {
        position-=1;
        result+=1;
    }

    return result;
}


std::size_t TextBox::FindBottomPadding(std::size_t x) const
{
    const std::size_t max = data.size();

    std::size_t result = 0;
    std::size_t position = max;
    
    while(position != 0 && (x >= data[position-1].size() || IsEmpty(data[position-1][x])))
    {
        position-=1;
        result+=1;
    }

    return result;
}



std::size_t TextBox::FindTopPadding(std::size_t x) const
{
    const std::size_t max = data.size();
    std::size_t result = 0;
    
    while(result < max && (x >= data[result].size() || IsEmpty(data[result][x])))
    {
        result+=1;
    }

    return result;
}


    void TextBox::SubCreateTreeGraph(
        TextBox& result,
        size_t maxwidth,
        const std::vector<TextBox>& boxes,
        bool consider_oneliner,
        bool consider_simple,
        const std::string& label,
        std::size_t margin,
        std::size_t firstx
        )
    {
        constexpr std::size_t min_y = 1;

        const auto totalwidth = boxes.empty() ? 0 : 
                std::accumulate(boxes.begin(), boxes.end(), 0, [](auto t, const auto& b) {return t + b.Width();})
                + (boxes.size()-1) * margin;

        bool oneliner = consider_oneliner && (label.size() + margin + totalwidth) < maxwidth;
        bool simple = consider_simple && oneliner && boxes.size() == 1;

        std::size_t y = simple ? 0 : 1;

        for(auto box_iterator = boxes.begin(); box_iterator != boxes.end(); box_iterator+=1)
        {
            const TextBox& current_box = *box_iterator;
            const unsigned width = current_box.Width();
            const std::size_t usemargin = (simple || oneliner) ? (margin/2) : margin;
            const auto first_valid_x = result.horiz_append_position(y, current_box);
            std::size_t x = first_valid_x != 0 ? first_valid_x + usemargin
                :(
                    oneliner
                    ? label.size()+usemargin
                    : firstx
                );

            if(!oneliner && (x + width > maxwidth))
            {
                // Start a new line if this item won't fit in the end of the current line
                x        = firstx;
                simple   = false;
                oneliner = false;
            }

            // At the beginning of line, judge whether to add room for horizontal placement
            bool horizontal = x > firstx;
            if( !oneliner &&
                !horizontal &&
                std::next(box_iterator) != boxes.end()
                )
            {
                const auto& next_box = *std::next(box_iterator);
                std::size_t combined_width = current_box.horiz_append_position(0, next_box) + margin + next_box.Width();
                if(combined_width <= maxwidth)
                {
                    // Enact horizontal placement by giving 1 row of room for the connector
                    horizontal = true;
                    const TextBox combined = current_box.PutBoxCopy(current_box.horiz_append_position(0, next_box) + margin, 0, next_box);
                    y = std::max(result.vert_append_position(x, combined), std::size_t(1));
                    if(!oneliner)
                    {
                      y+=1;
                    }
                }
            }
            if(!horizontal)
            {
                y = std::max(result.vert_append_position(x, current_box), std::size_t(1));
            }
            if(horizontal && !simple && !oneliner)
            {
                while(true)
                {
                    // Check if there is room for a horizontal connector. If not, increase y
                    const auto connector = TextBox::FromString(std::string(1+x, '-'));
                    if(result.horiz_append_position(y-1, connector) > x)
                    {
                      y+=1;
                    }
                    else
                    {
                      break;
                    }
                    y = std::max(result.vert_append_position(x, current_box), y);
                }
            }

            if(simple)
            {
            }
            else
            {
                y = std::max(y, min_y);
            }

            if(simple)
            {
                if(x > label.size())
                {
                    result.PutHoriLine(label.size(), 0, 1+x-label.size(), false,false);
                }
            }
            else if(oneliner)
            {
                unsigned cx = x;
                unsigned cy = y > min_y ? y-min_y : 0;
                if(x > label.size())
                {
                    result.PutHoriLine(label.size(), 0, 1+x-label.size(), false,false);
                }
                result.PutVertLine(cx, cy, min_y,          false,true);
            }
            else if(horizontal)
            {
                unsigned cx = x;
                unsigned cy = y-1;
                result.PutVertLine(0,  1,  1 + (cy-1), true,false);
                result.PutHoriLine(0,  cy, 1 + (cx-0), false,false);
                result.PutVertLine(cx, cy, 1,          false,true);
            }
            else
            {
                unsigned cx = x-1;
                unsigned cy = y;
                result.PutVertLine(0,1,  1 + (cy-1), true,false);
                result.PutHoriLine(0,cy, 1 + (cx-0), false,true);
            }

            result.PutBox(x, y, current_box);
        }
    }
}
