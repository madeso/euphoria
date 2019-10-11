#ifndef CORE_CMDLINE_H
#define CORE_CMDLINE_H

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
    struct CmdLine
    {
        typedef std::vector<std::string>         Args;
        typedef std::function<void(const std::string&)> PrintFunction;
        typedef std::function<void(PrintFunction, const Args&)> Callback;

        CmdLine();

        void
        Register(const std::string& name, Callback callback);

        void
        Run(PrintFunction print, const std::string& cmd);

        void PrintHelp(CmdLine::PrintFunction print, const Args&);

    private:
        // private to make sure the case of the name is consistent
        std::map<std::string, Callback> callbacks;
    };
}  // namespace euphoria::core

#endif  // CORE_CMDLINE_H
