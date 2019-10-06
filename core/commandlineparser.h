#ifndef CORE_COMMANDLINE_H
#define CORE_COMMANDLINE_H

#include <vector>
#include <string>

namespace euphoria::core
{
    std::vector<std::string>
    ParseCommandLine(const std::string& str);

}

#endif  // CORE_COMMANDLINE_H
