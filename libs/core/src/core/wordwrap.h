#ifndef EUPHORIA_CORE_WORDWRAP_H
#define EUPHORIA_CORE_WORDWRAP_H

#include <string>
#include <vector>
#include <functional>

namespace euphoria::core
{
    std::vector<std::string>
    WordWrap
    (
        const std::string& str,
        std::function<bool (const std::string&)> measure
    );
}

#endif  // EUPHORIA_CORE_WORDWRAP_H

