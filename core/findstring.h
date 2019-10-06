#ifndef CORE_FINDSTRING_H
#define CORE_FINDSTRING_H

#include <vector>
#include <string>

namespace euphoria::core
{
    // find part of a string in another
    // todo: replace with a regex instead?
    bool
    Find(const std::string& target, const std::string& search);

    // find all of the strings in the target
    bool
    Find(const std::string& target, const std::vector<std::string>& searches);

    // find the string in any target
    bool
    Find(const std::vector<std::string>& targets, const std::string& search);

    // find all of the strings in any target
    bool
    Find(const std::vector<std::string>& targets,
         const std::vector<std::string>& searches);

}  // namespace euphoria::core

#endif  // CORE_FINDSTRING_H
