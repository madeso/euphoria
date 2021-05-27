#include "core/console.h"
#include "core/stringutils.h"

#include "core/str.h"

namespace euphoria::core
{
    console::console()
    {
        register_command("help", [this](print_function print, const args& arg) {
            this->print_help(print, arg);
        });
    }


    void
    console::register_command(const std::string& name, callback callback)
    {
        callbacks[to_lower(name)] = callback;
    }


    void
    console::run(print_function print, const std::string& cmd)
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
            print(string_builder {} << "Unknown command " << name);
            // todo(Gustav): list commands that are the closest match
            return;
        }

        callback callback = found->second;
        callback(
                print, std::vector<std::string> {line.begin() + 1, line.end()});
    }


    void
    console::print_help(console::print_function print, const args&)
    {
        print("Available commands:");
        for(const auto& c: callbacks)
        {
            print(string_builder {} << "  " << c.first);
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
    } // namespace


    std::vector<std::string>
    parse_commandline(const std::string& str)
    {
        std::vector<std::string> ret;

        bool escape = false;
        char current_string_character = 0;
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
} // namespace euphoria::core
