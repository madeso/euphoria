#include "core/console.h"

#include "core/stringutils.h"
#include "core/stringbuilder.h"


namespace euphoria::core
{


VirtualConsole::VirtualConsole()
{
    register_command("help", [this](PrintFunction print, const Args& arg) {
        this->print_help(print, arg);
    });
}


void
VirtualConsole::register_command(const std::string& name, ActionFunction callback)
{
    callbacks[to_lower(name)] = callback;
}


void
VirtualConsole::run(PrintFunction print, const std::string& cmd)
{
    if(cmd.empty())
    {
        return;
    }
    const auto line = parse_commandline(cmd);
    if(line.empty())
    {
        return;
    }
    const auto name = line[0];
    auto found = callbacks.find(to_lower(name));
    if(found == callbacks.end())
    {
        // unable to find cmd
        print("Unknown command {}"_format(name));
        // todo(Gustav): list commands that are the closest match
        return;
    }

    ActionFunction callback = found->second;
    callback(print, std::vector<std::string> {line.begin() + 1, line.end()});
}


void
VirtualConsole::print_help(VirtualConsole::PrintFunction print, const Args&)
{
    print("Available commands:");
    for(const auto& c: callbacks)
    {
        print("  {}"_format(c.first));
    }
    print("");
}


namespace
{
    bool
    is_space(char c)
    {
        switch (c)
        {
        case '\t':
        case ' ': return true;
        default: return false;
        }
    }

    bool
    is_quote(char c)
    {
        switch (c)
        {
        case '\"':
        case '\'': return true;
        default: return false;
        }
    }

    char
    handle_escape_character(char c)
    {
        switch (c)
        {
        case 'n': return '\n';
        case 't': return '\t';
        default: return c;
        }
    }
}


std::vector<std::string>
parse_commandline(const std::string& str)
{
    std::vector<std::string> ret;

    bool escape = false;
    char current_string_character = 0;
    auto buffer = StringBuilder2{};

    for (char c : str)
    {
        if (escape)
        {
            buffer.add_char(handle_escape_character(c));
            escape = false;
        }
        else
        {
            if (c == '\\')
            {
                escape = true;
            }
            else
            {
                if (current_string_character != 0)
                {
                    if (c == current_string_character)
                    {
                        current_string_character = 0;
                        ret.emplace_back(buffer.to_string());
                        buffer.clear();
                    }
                    else
                    {
                        buffer.add_char(c);
                    }
                }
                else
                {
                    // not within string
                    if (is_quote(c) || is_space(c))
                    {
                        if (is_quote(c))
                        {
                            current_string_character = c;
                        }
                        const auto b = buffer.to_string();
                        buffer.clear();
                        if (!b.empty())
                        {
                            ret.emplace_back(b);
                        }
                    }
                    else
                    {
                        // neither quote nor space
                        buffer.add_char(c);
                    }
                }
            }
        }
    }

    const auto rest = buffer.to_string();
    if (!rest.empty() || current_string_character != 0)
    {
        ret.emplace_back(rest);
    }

    return ret;
}



}
