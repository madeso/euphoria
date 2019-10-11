#include "core/cmdline.h"
#include "core/stringutils.h"

#include "core/commandlineparser.h"
#include "core/str.h"

namespace euphoria::core
{
    CmdLine::CmdLine()
    {
        Register("help", [this](PrintFunction print, const Args& arg) { this->PrintHelp(print, arg); });
    }

    void
    CmdLine::Register(const std::string& name, Callback callback)
    {
        callbacks[ToLower(name)] = callback;
    }

    void
    CmdLine::Run(PrintFunction print, const std::string& cmd)
    {
        if(cmd.empty())
        {
            return;
        }
        const auto line = ParseCommandLine(cmd);
        if(line.empty())
        {
            return;
        }
        const auto name  = line[0];
        auto       found = callbacks.find(ToLower(name));
        if(found == callbacks.end())
        {
            // unable to find cmd
            print(Str{} << "Unknown command " << name);
            // todo(Gustav): list commands that are the closest match
            return;
        }

        Callback callback = found->second;
        callback(print, std::vector<std::string> {line.begin() + 1, line.end()});
    }

    void
    CmdLine::PrintHelp(CmdLine::PrintFunction print, const Args&)
    {
        print("Available commands:");
        for(const auto& c: callbacks)
        {
            print(Str{}  << "  " << c.first);
        }
        print("");
    }
}  // namespace euphoria::core
