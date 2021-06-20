#include "core/wordwrap.h"

#include <optional>

#include "core/stringutils.h"


namespace euphoria::core
{

// word_wrap is currently based on https://stackoverflow.com/a/17635

namespace
{

    bool
    is_whitespace(char c)
    {
        return kSpaceCharacters.find(c) != std::string::npos;
    }

    const std::string splitChars = kSpaceCharacters;

    std::vector<std::string>
    explode(const std::string& str)
    {
        std::vector<std::string> parts;
        int startIndex = 0;
        while (true)
        {
            int index = str.find_first_of(splitChars, startIndex);

            if (index == -1)
            {
                parts.emplace_back(str.substr(startIndex));
                return parts;
            }

            auto word = str.substr(startIndex, index - startIndex);
            char nextChar = str.substr(index, 1)[0];
            if (is_whitespace(nextChar))
            {
                parts.emplace_back(word);
                parts.emplace_back(std::string(1, nextChar));
            }
            else
            {
                parts.emplace_back(word + nextChar);
            }

            startIndex = index + 1;
        }
    }

}

std::vector<std::string>
word_wrap
(
    const std::string& str,
    std::function<bool (const std::string&)> fit
)
{
    auto words = explode(str);

    auto lines = std::vector<std::string>{};
    std::optional<std::string> current_line;

    auto add_current_line = [&]()
    {
        if(current_line)
        {
            const auto t = trim(*current_line, " \t");
            if(t.empty() == false)
            {
                lines.emplace_back(trim(t));
            }
            current_line = {};
        }
    };

    for(const auto& word: words)
    {
        if (fit(current_line.value_or("") + word) == false)
        {
            add_current_line();
        }
        current_line = current_line.value_or("") + word;
    }

    add_current_line();
    if(lines.empty())
    {
        lines.emplace_back("");
    }
    return lines;
}

}

