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
    struct console
    {
        using args = std::vector<std::string>;
        using print_function = std::function<void(const std::string&)>;
        using callback = std::function<void(print_function, const args&)>;

        console();

        void
        register_command(const std::string& name, callback callback);

        void
        run(print_function print, const std::string& cmd);

        void
        print_help(console::print_function print, const args&);

    private:
        // private to make sure the case of the name is consistent
        std::map<std::string, callback> callbacks;
    };

    std::vector<std::string>
    parse_commandline(const std::string& str);
}
