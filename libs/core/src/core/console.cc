#include "core/console.h"
#include "core/stringutils.h"

#include "core/str.h"

namespace euphoria::core
{
    Console::Console()
    {
        Register("help", [this](PrintFunction print, const Args& arg) {
            this->PrintHelp(print, arg);
        });
    }


    void
    Console::Register(const std::string& name, Callback callback)
    {
        callbacks[ToLower(name)] = callback;
    }


    void
    Console::Run(PrintFunction print, const std::string& cmd)
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
            print(Str {} << "Unknown command " << name);
            // todo(Gustav): list commands that are the closest match
            return;
        }

        Callback callback = found->second;
        callback(
                print, std::vector<std::string> {line.begin() + 1, line.end()});
    }


    void
    Console::PrintHelp(Console::PrintFunction print, const Args&)
    {
        print("Available commands:");
        for(const auto& c: callbacks)
        {
            print(Str {} << "  " << c.first);
        }
        print("");
    }


    namespace
    {
        bool
            IsSpace(char c)
        {
            switch (c)
            {
            case '\t':
            case ' ': return true;
            default: return false;
            }
        }

        bool
            IsQuote(char c)
        {
            switch (c)
            {
            case '\"':
            case '\'': return true;
            default: return false;
            }
        }

        char
            HandleEscapeCharacter(char c)
        {
            switch (c)
            {
            case 'n': return '\n';
            case 't': return '\t';
            default: return c;
            }
        }
    }  // namespace


    std::vector<std::string>
    ParseCommandLine(const std::string& str)
    {
        std::vector<std::string> ret;

        bool               escape = false;
        char               current_string_character = 0;
        std::ostringstream buffer;

        for (char c : str)
        {
            if (escape)
            {
                buffer << HandleEscapeCharacter(c);
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
                            ret.emplace_back(buffer.str());
                            buffer.str("");
                        }
                        else
                        {
                            buffer << c;
                        }
                    }
                    else
                    {
                        // not within string
                        if (IsQuote(c) || IsSpace(c))
                        {
                            if (IsQuote(c))
                            {
                                current_string_character = c;
                            }
                            const auto b = buffer.str();
                            if (!b.empty())
                            {
                                ret.emplace_back(b);
                                buffer.str("");
                            }
                        }
                        else
                        {
                            // neither quote nor space
                            buffer << c;
                        }
                    }
                }
            }
        }

        const auto rest = buffer.str();
        if (!rest.empty() || current_string_character != 0)
        {
            ret.emplace_back(rest);
        }

        return ret;
    }
}  // namespace euphoria::core
