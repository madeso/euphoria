#pragma once





namespace euphoria::core
{
    // find part of a string in another
    // todo(Gustav): replace with a regex instead?
    bool
    find(const std::string& target, const std::string& search);

    // find all of the strings in the target
    bool
    find(const std::string& target, const std::vector<std::string>& searches);

    // find the string in any target
    bool
    find(const std::vector<std::string>& targets, const std::string& search);

    // find all of the strings in any target
    bool
    find
    (
        const std::vector<std::string>& targets,
        const std::vector<std::string>& searches
    );
}
