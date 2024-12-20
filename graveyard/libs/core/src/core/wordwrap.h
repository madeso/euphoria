#pragma once




#include <functional>

namespace eu::core
{
    std::vector<std::string>
    get_word_wrapped
    (
        const std::string& str,
        std::function<bool (const std::string&)> measure
    );
}

