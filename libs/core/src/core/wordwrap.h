#pragma once




#include <functional>

namespace euphoria::core
{
    std::vector<std::string>
    word_wrap
    (
        const std::string& str,
        std::function<bool (const std::string&)> measure
    );
}

