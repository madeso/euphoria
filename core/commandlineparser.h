#ifndef CORE_COMMANDLINE_H
#define CORE_COMMANDLINE_H

#include <vector>
#include <string>

namespace euphoria::core
{
    // todo(Gustav): merge with cmdline.h
    std::vector<std::string>
    ParseCommandLine(const std::string& str);
}  // namespace euphoria::core

#endif  // CORE_COMMANDLINE_H
