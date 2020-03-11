#include "core/argparse.h"

namespace euphoria::core::argparse
{
    int
    ReturnValue(ParseResult pr)
    {
        switch (pr)
        {
        case ParseResult::Ok: return 0;
        case ParseResult::Error: return -1;
        case ParseResult::Quit: return 0;
        default: return -1;
        }
    }


    Arguments::Arguments(const std::string& n, const std::vector<std::string>& a)
        : name(n)
        , arguments(a)
    {
    }

    Arguments
    Arguments::Extract(int argc, char* argv[])
    {
        auto ret = Arguments{argv[0] , {}};
        
        for (int i = 1; i < argc; i += 1)
        {
            ret.arguments.emplace_back(argv[i]);
        }

        return ret;
    }


    ArgumentReader::ArgumentReader(const Arguments& a)
        : arguments(a)
        , next_position(0)
    {
    }


    bool
    ArgumentReader::HasMore() const
    {
        // todo(Gustav): assert on size
        return next_position < arguments.arguments.size();
    }


    std::string
    ArgumentReader::Peek() const
    {
        if (HasMore())
        {
            const auto arg = arguments.arguments[next_position];
            return arg;
        }
        else
        {
            return "";
        }
    }


    std::string
    ArgumentReader::Read()
    {
        const auto arg = Peek();
        next_position += 1;
        return arg;
    }


    Printer::~Printer()
    {
    }


    void
    ConsolePrinter::PrintError(const std::string& line)
    {
        std::cerr << line << "\n";
    }


    void
    ConsolePrinter::PrintInfo(const std::string& line)
    {
        std::cout << line << "\n";
    }


    Name::Name(const std::string& n)
        : name(n)
    {
    }


    Name::Name(const char* n)
        : name(n)
    {
    }


    bool
    Name::IsOptional() const
    {
        return name[0] == '-';
    }


    Argument::~Argument()
    {
    }


    ArgumentAndName::ArgumentAndName(const Name& n, std::shared_ptr<Argument> a)
        : name(n)
        , argument(a)
    {
    }


    ArgumentNoValue::ArgumentNoValue(Callback cb)
        : callback(cb)
    {
    }


    ParseResult
    ArgumentNoValue::Parse(Runner* runner)
    {
        return callback(runner);
    }


    SingleArgument::SingleArgument(Callback cb)
        : callback(cb)
    {
    }


    ParseResult
    SingleArgument::Parse(Runner* runner)
    {
        if (runner->arguments->HasMore())
        {
            auto res = callback(runner, runner->arguments->Read());
            return res;
        }
        else
        {
            // todo(Gustav): improve eof error message by including argument name
            runner->printer->PrintError("missing argument");
            return ParseResult::Error;
        }
    }


    Argument&
    Parser::AddArgument(const Name& name, std::shared_ptr<Argument> argument)
    {
        if (name.IsOptional())
        {
            optional_arguments[name.name] = argument;
            optional_argument_list.emplace_back(name, argument);
        }
        else
        {
            positional_argument_list.emplace_back(name, argument);
        }
        return *argument;
    }


    Argument&
    Parser::AddVoidFunction(const Name& name, std::function<void()> void_function)
    {
        return AddArgument(name, std::make_shared<ArgumentNoValue>([void_function](Runner*) {void_function(); return ParseResult::Ok; }));
    }


    void
    Parser::AddSubParser(const std::string& name, SubParserCallback sub)
    {
        // todo(Gustav): implement this
    }


    std::shared_ptr<Argument>
    Parser::FindArgument(const std::string& name)
    {
        return optional_arguments[name];
    }


    ParseResult
    Parser::ParseArgs(Runner* runner)
    {
        int positional_index = 0;

        auto has_more_positionals = [&]()
        {
            return positional_index < positional_argument_list.size();
        };

        while (runner->arguments->HasMore())
        {
            if (has_more_positionals())
            {
                auto match = positional_argument_list[positional_index];
                auto arg_parse_result = match.argument->Parse(runner);
                if (arg_parse_result != ParseResult::Ok)
                {
                    return arg_parse_result;
                }
                positional_index += 1;
            }
            else
            {
                const auto arg = runner->arguments->Read();
                auto match = FindArgument(arg);
                if (match == nullptr)
                {
                    runner->printer->PrintError("invalid argument: " + arg);
                    return ParseResult::Error;
                }
                auto arg_parse_result = match->Parse(runner);
                if (arg_parse_result != ParseResult::Ok)
                {
                    return arg_parse_result;
                }
            }
        }

        if (has_more_positionals())
        {
            runner->printer->PrintError("positionals left");
            return ParseResult::Error;
        }

        return ParseResult::Ok;
    }


    RootParser::RootParser(const std::string& d)
        : description(d)
        , printer(std::make_shared<ConsolePrinter>())
    {
        AddArgument("-h", std::make_shared<ArgumentNoValue>([this](Runner*) {PrintHelp(); return ParseResult::Quit; }))
            .help = "print help";
    }


    void
    RootParser::PrintHelp()
    {
        if (description.empty() == false)
        {
            printer->PrintInfo(description);
        }

        // todo(Gustav): use a string table here, add wordwrap to table
        for (auto& a : positional_argument_list)
        {
            printer->PrintInfo(a.name.name + " " + a.argument->help);
        }
        for (auto& a : optional_argument_list)
        {
            printer->PrintInfo(a.name.name + " " + a.argument->help);
        }
    }


    ParseResult
    RootParser::ParseArgs(const Arguments& args)
    {
        auto reader = ArgumentReader{ args };
        auto runner = Runner{ &reader, printer };
        return Parser::ParseArgs(&runner);
    }
}
