#include "core/wordwrap.h"

#include <optional>

#include "assert/assert.h"
#include "core/stringutils.h"


namespace euphoria::core
{

// get_word_wrapped is currently based on https://stackoverflow.com/a/17635

namespace
{

    bool
    is_whitespace(char c)
    {
        return space_characters.find(c) != std::string::npos;
    }

    const std::string split_chars = space_characters;

    std::vector<std::string>
    explode(const std::string& str)
    {
        std::vector<std::string> parts;
        std::size_t start_index = 0;
        while (true)
        {
            const auto index = str.find_first_of(split_chars, start_index);

            if (index == std::string::npos)
            {
                parts.emplace_back(str.substr(start_index));
                return parts;
            }

            ASSERTX(index > start_index, index, start_index);
            auto word = str.substr(start_index, index - start_index);
            char next_char = str.substr(index, 1)[0];
            if (is_whitespace(next_char))
            {
                parts.emplace_back(word);
                parts.emplace_back(std::string(1, next_char));
            }
            else
            {
                parts.emplace_back(word + next_char);
            }

            start_index = index + 1;
        }
    }

}

// todo(Gustav): rename to get_word_wrapped
std::vector<std::string>
get_word_wrapped
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

