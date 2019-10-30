#include "core/textbox.h"

#include <cstdlib>

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
}


namespace euphoria::core
{

TextBoxStyle TerminalStyle()
{
    if(TerminalSupportUtf8())
    {
        return Utf8Style();
    }
    else
    {
        return AsciiStyle();
    }
}
    
TextBoxStyle Utf8Style()
{
    TextBoxStyle style;
    style.enable_vt100 = true;
    return style;
}

TextBoxStyle AsciiStyle()
{
    TextBoxStyle style;
    style.enable_vt100 = false;
    return style;
}

// bitmasks
constexpr unsigned char u=1;
constexpr unsigned char d=2;
constexpr unsigned char l=4;
constexpr unsigned char r=8;
constexpr unsigned char nonline = ~(u+d+l+r);


TextBox::TextBox()
{}


TextBox::TextBox(const std::vector<std::string>& a_data) : data(a_data)
{}


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


void TextBox::putline(const std::string& s, std::size_t x_start, std::size_t y)
{
    auto xx = s.empty() ? 0 : s.size()-1;
    extend(x_start+xx, y);
    
    for(std::size_t begin = 0; begin < s.size(); ++begin)
    {
        const auto x = x_start + begin;
        const unsigned char c = s[begin];

        ASSERT(x < data[y].size());

        if(c==' ' || !c)
        {
            continue;
        }

        char &tgt = data[y][x];
        if(tgt==' ' || !tgt || (c&nonline))
        {
            tgt = c;
        }
        else
        {
            if(tgt&nonline)
            {
                tgt=0; tgt|=c;
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
        while(end > 0 && (s[end-1]==' ' || s[end-1]=='\0'))
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


void TextBox::hline(std::size_t x, std::size_t y, std::size_t width, bool bef, bool aft)
{
    for(std::size_t p=0; p<width; ++p)
    {
        modchar(x+p, y, [&](char& c)
        {
            if(c&nonline)
            {
                c=0;
            }

            if(p>0||bef)
            {
                c |= l;
            }

            if(aft||(p+1)<width)
            {
                c |= r;
            }
        });
    }
}
    

void TextBox::vline(std::size_t x, std::size_t y, std::size_t height, bool bef, bool aft)
{
    for(std::size_t p=0; p<height; ++p)
    {
        modchar(x, y+p, [&](char& c)
        {
            if(c&nonline)
            {
                c=0;
            }

            if(p>0||bef)
            {
                c |= u;
            }

            if(aft||(p+1)<height)
            {
                c |= d;
            }
        });
    }
}


std::size_t TextBox::horiz_append_position(std::size_t y, const TextBox& b) const
{
    std::size_t mywidth = width(), theirheight = b.height();
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
    std::size_t myheight = height(), theirwidth = b.width();
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

    bool drawing = false, quo = false, space = true, unstr = false;
    std::string cur_attr;
    const std::string_view linedraw = style.enable_vt100 ? "xxxqjkuqmltqvwn" : "|||-'.+-`,+-+++";
    
    auto attr = [&](const char* s)
    {
        if (style.enable_vt100)
        {
            // if(cur_attr!=s) { last_line() += "\33["; last_line() += s; last_line() += 'm'; cur_attr = s; }
        }
    };

    auto append = [&](bool v, char c)
    {
        if (style.enable_vt100)
        {
            const char* a = nullptr;
            bool num = false;
            if(v&&!drawing)
            {
              a = "0;32";
              last_line() += "\33)0\16";
              drawing = v;
            }
            else if(!v&&drawing)
            {
              a = "";
              last_line() += "\33)B\17";
              drawing = v;
            }
            if(!v && c=='"')
            {
              quo = !quo;
              if(quo)
              {
                a = "1;34";
              }
            }
            if(!v && !quo && ((c>='0' && c<='9') || c=='-'))
            {
              a = space ? "1;38;5;165" : "0;38;5;246";
              num=true;
            }
            if(!v && !quo && ((c>='a' && c<='z') || c=='_'))
            {
              a = "1;37";
            }
            if(!v && !quo && c>='A' && c<='Z')
            {
              a = "0;38;5;246";
            }
            if(!v && !quo && c=='`')
            {
              unstr = true;
              c = ' ';
            }
            if(c == '\n')
            {
              unstr = false;
            }
            if(unstr)
            {
              a = nullptr;
            }
            if(a)
            {
              attr(a);
            }
            if(!num)
            {
              space = (c==' ');
            }
        }

        if(c == '\n')
        {
            want_newline = true;
        }
        else
        {
            last_line() += c;
        }
    };

    for(std::size_t h = height(), y = 0; y < h; ++y)
    {
        const std::string& s = data[y];
        for(std::size_t x = 0; x < s.size(); ++x)
        {
            unsigned char c = s[x];
            if(c > 0 && c < 16)
            {
              append(true, linedraw[c-1]);
            }
            else
            {
              append(false, c);
            }
        }
        attr("");
        append(false, '\n');
    }
    return ret;
}

std::size_t TextBox::FindLeftPadding(std::size_t y) const
{
    std::size_t max = width(), result = 0;
    if(y >= data.size())
    {
        return max;
    }

    const std::string& line = data[y];
    while(result < line.size() && (line[result] == ' ' || line[result] == '\0'))
    {
        ++result;
    }

    return result;
}


std::size_t TextBox::FindRightPadding(std::size_t y) const
{
    std::size_t max = width(), position = max, result = 0;
    if(y >= data.size())
    {
        return max;
    }

    const std::string& line = data[y];
    while(position-- > 0 && (position >= line.size() || line[position]==' ' || line[position]=='\0'))
    {
        ++result;
    }

    return result;
}


std::size_t TextBox::FindTopPadding(std::size_t x) const
{
    std::size_t result = 0, max = data.size();
    
    while(result < max && (x >= data[result].size() || data[result][x] == ' ' || data[result][x] == '\0'))
    {
        ++result;
    }

    return result;
}


std::size_t TextBox::FindBottomPadding(std::size_t x) const
{
    std::size_t result = 0, max = data.size(), position = max;
    
    while(position-- > 0 && (x >= data[position].size() || data[position][x] == ' ' || data[position][x] == '\0'))
    {
        ++result;
    }

    return result;
}

namespace detail
{
    void CreateTreeGraph(TextBox& result, size_t maxwidth, const std::vector<TextBox>& boxes, bool oneliner_test, bool simple_test, bool separate1st_test, std::string atom)
    {
        constexpr std::size_t margin = 4, firstx = 2;

        std::size_t sum_width = 0;
        for(const auto& b: boxes) sum_width += b.width()+margin;

        bool oneliner = false;
        if(oneliner_test && !separate1st_test)
        {
            std::size_t totalwidth = 0;
            for(auto i = boxes.begin(); ; )
            {
                const auto& cur = *i;
                if(++i == boxes.end()) { totalwidth += cur.width(); break; }
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
            if(x==usemargin) x = oneliner ? atom.size()+usemargin : firstx;
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
                    if(!oneliner) ++y;
                }
            }
            if(!horizontal)
                y = std::max(result.vert_append_position(x, cur), std::size_t(1));
            if(horizontal && !simple && !oneliner)
                for(;;)
                {
                    // Check if there is room for a horizontal connector. If not, increase y
                    TextBox conn;
                    conn.putline(std::string(1+(x-0), '-'), 0, 0);
                    if(result.horiz_append_position(y-1, conn) > x) ++y; else break;
                    y = std::max(result.vert_append_position(x, cur), y);
                }

            if(simple)
            {
                if(x > atom.size())
                    result.hline(atom.size(), 0, 1+x-atom.size(), false,false);
            }
            else if(oneliner)
            {
                unsigned cx = x, cy = y-1;
                if(x > atom.size())
                    result.hline(atom.size(), 0, 1+x-atom.size(), false,false);
                result.vline(cx, cy, 1,          false,true);
            }
            else if(horizontal)
            {
                unsigned cx = x, cy = y-1;
                result.vline(0,  1,  1 + (cy-1), true,false);
                result.hline(0,  cy, 1 + (cx-0), false,false);
                result.vline(cx, cy, 1,          false,true);
            }
            else
            {
                unsigned cx = x-1, cy = y;
                result.vline(0,1,  1 + (cy-1), true,false);
                result.hline(0,cy, 1 + (cx-0), false,true);
            }

            result.putbox(x, y, cur);
        }
    }
}

}
