#pragma once

#include <functional>
#include <iostream>
#include <vector>
#include <string>
#include <map>

namespace euphoria::core
{
    /** Provides a terminal like experience.
     * Case of the name is not important
     */
    struct VirtualConsole
    {
        using Args = std::vector<std::string>;
        using PrintFunction = std::function<void(const std::string&)>;
        using ActionFunction = std::function<void(PrintFunction, const Args&)>;

        VirtualConsole();

        void
        register_command(const std::string& name, ActionFunction callback);

        void
        run(PrintFunction print, const std::string& cmd);

        void
        print_help(VirtualConsole::PrintFunction print, const Args&);

    private:
        // private to make sure the case of the name is consistent
        std::map<std::string, ActionFunction> callbacks;
    };

    std::vector<std::string>
    parse_commandline(const std::string& str);
}
