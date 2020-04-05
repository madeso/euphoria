#include "core/argparse.h"

#include "core/cint.h"
#include "core/stringutils.h"
#include "core/stringmerger.h"
#include "core/table.h"
#include "core/cint.h"
#include "core/str.h"
#include "core/wordwrap.h"
#include "core/functional.h"

#include <iostream>
#include <iomanip>
#include <algorithm>


/*

   argparse issues & todos:

   * less 'passive agressive' --help suggestion. only suggest -h when it's the "first" argument

   * assert invalid setups and argument name collisions

   * help should optional expect a argument name to print detailed help

   * generic OnComplete unit test

   * instead of .Add("arg").Help("") perhaps change to .Add(Arg{"arg"}.Help("")) so chainging arguments is possible

   * extra validators combinable with | that adds more conditions (files must exists, strings can be empty, numbers must be in range etc)

   * vector input

   * support python @ append arguments from file "arguments"

   * support reading palettes and rgb(a)s from commandline, including common palette files

   * add real-world tests in unit tests, like fake-git and fake-compiler

   * if bored, get ideas from https://www.npmjs.com/package/commander

*/


namespace euphoria::core::argparse
{
    namespace
    {
        bool
        IsOptionalArgument(const std::string& name)
        {
            return name[0] == '-';
        }

        bool
        IsValidArgumentName(const std::string& str)
        {
            if (str.empty())
            {
                return false;
            }

            constexpr auto VALID_ARGUMENT_NAME_CHARACTERS = "abcdefghijklmnopqrstuvwxyz-_0123456789";
            const auto first_invalid_character = ToLower(str).find_first_not_of(VALID_ARGUMENT_NAME_CHARACTERS);
            if (first_invalid_character != std::string::npos)
            {
                return false;
            }

            return true;
        }
    }

    const std::string
    QuoteAndCombineEnglishOr(const std::vector<std::string>& matches)
    {
        const auto quoted_names =
            Map<std::string>(matches, [](const std::string& s)
            {
                return static_cast<std::string>
                (
                    Str() << '\'' << s << '\''
                );
            });

        return StringMerger::EnglishOr().Generate(quoted_names);
    }


    FileOutput::FileOutput(const std::string& o) : file(o), single(!(EndsWith(o, "/") || EndsWith(o, "\\")))
    {}


    std::string
    FileOutput::NextFile(bool print)
    {
        if(single)
        {
            return file;
        }
            
        std::ostringstream ss;
        index += 1;
        if(print)
        {
            std::cout << "Generating " << index << "...\n";
        }

        // todo(Gustav): provide option for file extension
        ss << file << std::setfill('0') << std::setw(5) << index << ".png";
        return ss.str();
    }


    void
    FileOutput::CreateDirIfMissing() const
    {
        if(single) { return; }
        // std::filesystem::create_directories(file);
    }


    namespace
    {
        constexpr
        int
        DefaultReturnValue(ParseResult::Type pr)
        {
            switch (pr)
            {
            case ParseResult::Type::Ok: return 0;
            case ParseResult::Type::Error: return -1;
            case ParseResult::Type::Quit: return 0;
            default: return -1;
            }
        }
    }


    const ParseResult ParseResult::Error = ParseResult { Type::Error };
    const ParseResult ParseResult::Ok = ParseResult { Type::Ok };
    const ParseResult ParseResult::Quit = ParseResult { Type::Quit };


    constexpr
    ParseResult::ParseResult(Type t)
        : type(t)
        , return_value(DefaultReturnValue(t))
    {
    }


    constexpr
    ParseResult::ParseResult(int rv)
        : type(ParseResult::Type::Custom)
        , return_value(rv)
    {
    }


    std::ostream&
    operator<<(std::ostream& o, const ParseResult& pr)
    {
        o << EnumToString(pr.type) << "(" << pr.return_value << ")";
        return o;
    }

    bool
    operator==(const ParseResult& lhs, const ParseResult& rhs)
    {
        return lhs.type == rhs.type && lhs.return_value == rhs.return_value;
    }


    bool
    operator!=(const ParseResult& lhs, const ParseResult& rhs)
    {
        return !(lhs == rhs);
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


    std::ostream&
    operator<<(std::ostream& o, const euphoria::core::argparse::Arguments& args)
    {
        o << args.name << " ";
        bool first = true;
        o << "[";
        for(const auto& s: args.arguments)
        {
            if(first) {first = false;}
            else { o << ", "; }
            o << s;
        }
        o << "]";
        return o;
    }


    ArgumentReader::ArgumentReader(const Arguments& a)
        : arguments(a)
        , next_position(0)
    {
    }


    bool
    ArgumentReader::HasMore() const
    {
        return next_position < Csizet_to_int(arguments.arguments.size());
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


    void
    ArgumentReader::UndoRead()
    {
        ASSERT(next_position > 0);
        next_position -= 1;
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


    void
    PrintParseError
    (
        Runner* runner,
        ParserBase* base,
        const std::string& error
    )
    {
        runner->printer->PrintInfo
        (
            base->GenerateUsageString
            (
                runner->arguments->arguments
            )
        );
        runner->printer->PrintError
        (
            error
        );
    }


    Name::Name(const char* nn)
        : names(Split(nn, ','))
    {
        for (auto& n : names)
        {
            n = Trim(n);
        }

        ASSERTX(Validate().empty(), Validate());
    }


    std::string
    Name::Validate() const
    {
        if (names.empty()) { return "no names found"; }
        if (IsOptional())
        {
            for (const auto& n : names)
            {
                if (IsOptionalArgument(n) == false)
                {
                    return "arguments are optional but '" + n + "' is not";
                }

                if (IsValidArgumentName(n) == false)
                {
                    return "optional arguments '" + n + "' is not considered valid";
                }
            }
        }
        else
        {
            const auto n = names[0];
            if (IsValidArgumentName(n) == false)
            {
                return "positional arguments '" + n + "' is not considered valid";
            }
        }

        return "";
    }


    bool
    Name::IsOptional() const
    {
        if (names.empty())
        {
            return false;
        }

        if (names.size() > 1)
        {
            return true;
        }

        return IsOptionalArgument(names[0]);
    }


    Argument::~Argument()
    {
    }


    Argument&
    Argument::Nargs(const std::string& na)
    {
        nargs = na;
        return *this;
    }


    Argument&
    Argument::Help(const std::string& h)
    {
        help = h;
        return *this;
    }


    Argument&
    Argument::AllowBeforePositionals()
    {
        allow_before_positionals = true;
        return *this;
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


    bool
    ArgumentNoValue::HaveNargs()
    {
        return false;
    }


    std::optional<std::string>
    ArgumentNoValue::GetSecondLine()
    {
        return std::nullopt;
    }



    ParseResult
    ArgumentNoValue::ParseArguments
    (
        Runner* runner,
        const std::string&,
        ParserBase*
    )
    {
        return callback(runner);
    }


    SingleArgument::SingleArgument(Callback cb, Describe d)
        : callback(cb)
        , describe(d)
    {
    }


    bool
    SingleArgument::HaveNargs()
    {
        return true;
    }


    std::optional<std::string>
    SingleArgument::GetSecondLine()
    {
        return describe();
    }


    ParseResult
    SingleArgument::ParseArguments
    (
        Runner* runner,
        const std::string& name,
        ParserBase* caller
    )
    {
        if (runner->arguments->HasMore())
        {
            auto res = callback
            (
                runner,
                name,
                caller,
                runner->arguments->Read()
            );
            return res;
        }
        else
        {
            PrintParseError
            (
                runner,
                caller,
                Str() << "missing value for '" << name << '\''
            );
            return ParseResult::Error;
        }
    }


    SubParserNames::SubParserNames(const char* str)
    {
        names.emplace_back(str);
    }


    SubParserContainer::SubParserContainer
    (
        const SubParserNames& n,
        const std::string& h,
        SubParserCallback cb
    )
        : names(n)
        , help(h)
        , callback(cb)
    {
    }


    SubParserGroup::SubParserGroup
    (
        const std::string& t,
        ParserBase* o
    )
        : title(t)
        , owner(o)
    {
    }


    void
    SubParserGroup::Add
    (
        const SubParserNames& names,
        const std::string& desc,
        SubParserCallback sub
    )
    {
        auto container = std::make_shared<SubParserContainer>(names, desc, sub);
        parsers.emplace_back(container);
        for(const auto& name : names.names)
        {
            owner->subparsers.Add(name, container);
        }
    }


    void
    SubParserGroup::Add
    (
        const SubParserNames& names,
        SubParserCallback sub
    )
    {
        Add(names, "", sub);
    }


    ParserBase::ParserBase(const std::string& d)
        : description(d)
    {
        AddArgument
        (
            "-h, --help",
            std::make_shared<ArgumentNoValue>
            (
                [this](Runner* runner)
                {
                    PrintHelp(runner->printer, runner->arguments->arguments);
                    return ParseResult::Quit;
                }
            )
        )
        .AllowBeforePositionals()
        .Help("show this help message and exit")
        ;
    }

    
    ParserBase::~ParserBase()
    {
    }

    std::string
    ParserBase::GenerateUsageString(const Arguments& args)
    {
        auto arg_to_string = [](const ArgumentAndName& aa)
        {
            if( aa.name.IsOptional() )
            {
                std::ostringstream ss;
                ss << "[" << aa.name.names[0];
                if(aa.argument->HaveNargs())
                {
                    ss << " " << aa.argument->nargs;
                }
                ss << "]";
                // todo(Gustav): include any attributes (vector input)
                return ss.str();
            }
            else
            {
                return ToUpper(aa.name.names[0]);
            }
        };
        auto ret = std::vector<std::string>{"usage:", GetCallingName(args)};

        for(auto& a: optional_argument_list)
        {
            if(a.argument->allow_before_positionals == true)
            {
                ret.emplace_back(arg_to_string(a));
            }
        }

        for(auto& a: positional_argument_list)
        {
            ret.emplace_back(arg_to_string(a));
        }

        for(auto& a: optional_argument_list)
        {
            if(a.argument->allow_before_positionals == false)
            {
                ret.emplace_back(arg_to_string(a));
            }
        }

        if(subparser_groups.empty() == false)
        {
            ret.emplace_back("<command> [<args>]");
        }

        return StringMerger::Space().Generate(ret);
    }

    namespace
    {
        // msvc and clang disagrees on whetering capturing
        // constexpr in lambas is necessary or not
        constexpr int MAX_NAME_LENGTH = 20;
        constexpr int MAX_HELP_LENGTH = 80;
    }

    void
    ParserBase::PrintHelp(std::shared_ptr<Printer> printer, const Arguments& args)
    {
        using StringTable = Table<std::string>;

        // table functions
        int max_name_length = 0;

        const auto add = [&max_name_length]
        (
            StringTable* table,
            const std::string& name,
            const std::string& desc,
            const std::optional<std::string>& second_line
        )
        {
            const auto values = std::vector<std::string>{name, desc};
            table->NewRow(values);
            if(second_line)
            {
                table->NewRow({"", *second_line});
            }

            max_name_length = std::min
            (
                std::max
                (
                    max_name_length,
                    Csizet_to_int(name.length())
                ),
                MAX_NAME_LENGTH
            );
        };

        const auto print = [printer, &max_name_length](const StringTable& table)
        {
            auto t = StringTable{};
            for(int y=0; y<table.GetHeight(); y+=1)
            {
                const auto names = WordWrap
                (
                    table(0, y),
                    [max_name_length](const std::string& s)
                    {
                        return Csizet_to_int(s.size()) <= max_name_length;
                    }
                );
                const auto helps = WordWrap
                (
                    table(1, y),
                    [](const std::string& s)
                    {
                        return s.size() <= MAX_HELP_LENGTH;
                    }
                );
                const auto rows = ZipLongest(names, helps);
                for(auto [name,help]: rows)
                {
                    t.NewRow({name, help});
                }
            }
            for(int y=0; y<t.GetHeight(); y+=1)
            {
                constexpr auto INDENT = "  ";
                constexpr auto SPACE = "  ";

                const auto name = t(0, y);
                const auto help = t(1, y);

                const auto name_length = Csizet_to_int(name.length());
                if(name_length > max_name_length)
                {
                    printer->PrintInfo(INDENT + name);
                    const auto extra_space = std::string(max_name_length, ' ');
                    printer->PrintInfo(INDENT + extra_space + SPACE + help);
                }
                else
                {
                    const auto extra_space = std::string(max_name_length - name_length, ' ');
                    printer->PrintInfo(INDENT + name + extra_space + SPACE + help);
                }
            }
        };

        // collect data

        auto positionals = StringTable{};
        for (auto& a : positional_argument_list)
        {
            add
            (
                &positionals,
                a.name.names[0],
                a.argument->help,
                a.argument->GetSecondLine()
            );
        }

        auto optionals = StringTable{};
        for (auto& a : optional_argument_list)
        {
            const auto names = StringMerger::Comma().Generate(a.name.names);
            const auto names_with_narg = a.argument->HaveNargs() == false
                ? names
                : Str() << names << " " << a.argument->nargs
                ;
            std::ostringstream default_text;
            if(a.argument->default_value.empty() == false)
            {
                default_text << " (default: " << a.argument->default_value << ")";
            }

            add
            (
                &optionals,
                names_with_narg,
                a.argument->help + default_text.str(),
                a.argument->GetSecondLine()
            );
        }

        auto subs = std::vector<StringTable>{};
        for(const auto& group: subparser_groups)
        {
            auto sub = StringTable{};
            for(const auto& parser: group->parsers)
            {
                const auto names = StringMerger::Comma().Generate
                (
                    parser->names.names
                );
                add(&sub, names, parser->help, std::nullopt);
            }
            subs.emplace_back(sub);
        }

        // print all tables now
        printer->PrintInfo(GenerateUsageString(args));
        if (description.empty() == false)
        {
            printer->PrintInfo("");
            printer->PrintInfo(description);
        }

        if (positionals.GetHeight() != 0)
        {
            printer->PrintInfo("");
            printer->PrintInfo("positional arguments:");
            print(positionals);
        }

        if(optionals.GetHeight() != 0)
        {
            printer->PrintInfo("");
            printer->PrintInfo("optional arguments:");
            print(optionals);
        }

        if(!subs.empty())
        {
            bool is_first_group = true;
            printer->PrintInfo("");
            auto sub = subs.begin();
            for(const auto& group: subparser_groups)
            {
                if(is_first_group == false)
                {
                    printer->PrintInfo("");
                }

                printer->PrintInfo(group->title + ":");

                print(*sub);
                sub +=1 ;

                is_first_group = false;
            }
        }
    }


    std::string
    CreateDefaultNarg(const Name& name)
    {
        const auto n = TrimLeft(name.names[0], "-");
        return ToUpper(n);
    }


    Argument&
    ParserBase::AddArgument(const Name& name, std::shared_ptr<Argument> argument)
    {
        ASSERT
        (
            parser_state == ParserState::Adding &&
            "It looks like you are adding argument during parsing... "
            "are you using the wrong parser in a OnComplete?"
        );
        argument->nargs = CreateDefaultNarg(name);
        if (name.IsOptional())
        {
            for (const auto& n : name.names)
            {
                optional_arguments[n] = argument;
            }
            optional_argument_list.emplace_back(name, argument);
        }
        else
        {
            ASSERT(argument->HaveNargs());
            positional_argument_list.emplace_back(name, argument);
        }
        return *argument;
    }


    Argument&
    ParserBase::AddVoidFunction(const Name& name, std::function<void()> void_function)
    {
        return AddArgument(name, std::make_shared<ArgumentNoValue>([void_function](Runner*) {void_function(); return ParseResult::Ok; }));
    }


    Argument&
    ParserBase::SetTrue(const Name& name, bool* target)
    {
        *target = false;
        return SetConst(name, target, true);
    }


    Argument&
    ParserBase::SetFalse(const Name& name, bool* target)
    {
        *target = true;
        return SetConst(name, target, false);
    }


    void
    ParserBase::OnComplete(CompleteFunction cf)
    {
        on_complete = cf;
    }


    std::shared_ptr<SubParserGroup>
    ParserBase::AddSubParsers(const std::string& name)
    {
        ASSERT(parser_state == ParserState::Adding);
        auto group = std::make_shared<SubParserGroup>(name, this);
        subparser_groups.emplace_back(group);
        return group;
    }


    std::shared_ptr<Argument>
    ParserBase::FindArgument(const std::string& name) const
    {
        const auto found = optional_arguments.find(name);
        if(found == optional_arguments.end())
        {
            return nullptr;
        }
        else
        {
            return found->second;
        }
    }


    namespace
    {
        struct ArgumentParser
        {
            ParserBase* base;
            Runner* runner;
            int positional_index = 0;
            bool found_subparser = false;

            bool
            HasMorePositionals() const;

            std::vector<std::string>
            PositionalsLeft() const;

            void
            PrintError(const std::string& error);

            std::optional<ParseResult>
            TryParseImportantOptional();

            std::optional<ParseResult>
            ParseOnePositional();

            std::optional<ParseResult>
            ParseSubCommand(const std::string& arg);

            std::optional<ParseResult>
            ParseOneOptional();

            std::optional<ParseResult>
            ParseOneArg();
        };


        bool
        ArgumentParser::HasMorePositionals() const
        {
            const auto positionals_size = Csizet_to_int
            (
                base->positional_argument_list.size()
            );
            return positional_index < positionals_size;
        }


        std::vector<std::string>
        ArgumentParser::PositionalsLeft() const
        {
            if(HasMorePositionals() == false) { return {}; }

            std::vector<std::string> positionals;

            const auto& list = base->positional_argument_list;
            for
            (
                auto pos = list.begin()+positional_index;
                pos != list.end();
                pos += 1
            )
            {
                positionals.emplace_back(ToUpper(pos->name.names[0]));
            }

            return positionals;
        }


        void
        ArgumentParser::PrintError(const std::string& error)
        {
            PrintParseError(runner, base, error);
        }


        std::optional<ParseResult>
        ArgumentParser::TryParseImportantOptional()
        {
            // first, peek at the next commandline argument
            // and check for optionals that are allowed before positionals
            const auto arg = base->FindArgument(runner->arguments->Peek());
            if(arg != nullptr && arg->allow_before_positionals)
            {
                // we have matched a optional, read and parse it!
                const auto matched_name = runner->arguments->Read();
                auto arg_parse_result = arg->ParseArguments
                (
                    runner,
                    matched_name,
                    base
                );
                if(arg_parse_result != ParseResult::Ok)
                {
                    return arg_parse_result;
                }
                else
                {
                    return std::nullopt;
                }
            }
            else
            {
                // the peeked argument isn't a important optional
                // handle positional
                return std::nullopt;
            }
        }

        std::optional<ParseResult>
        ArgumentParser::ParseOnePositional()
        {
            auto match = base->positional_argument_list[positional_index];
            auto arg_parse_result = match.argument->ParseArguments
            (
                runner,
                match.name.names[0],
                base
            );
            if (arg_parse_result != ParseResult::Ok)
            {
                return arg_parse_result;
            }
            positional_index += 1;

            return std::nullopt;
        }

        SubParserStyle
        GetParserStyle(ParserBase* base, bool from_self=true)
        {
            if(base == nullptr)
            {
                return SubParserStyle::Greedy;
            }

            ParserBase* parent = base->GetParentOrNull();
            if(from_self && parent==nullptr)
            {
                return SubParserStyle::Greedy;
            }

            if(base->parser_style == SubParserStyle::Inherit)
            {
                return GetParserStyle(parent, false);
            }
            else
            {
                return base->parser_style;
            }
        }


        std::optional<ParseResult>
        ArgumentParser::ParseSubCommand(const std::string& arg)
        {
            auto match = base->subparsers.Match(arg, 3);
            if(match.single_match == false)
            {
                // todo(Gustav): check if this accepts invalid and
                // calls on_complete() on invalid input
                if(GetParserStyle(base) == SubParserStyle::Greedy)
                {
                    if(match.names.empty())
                    {
                        PrintError
                        (
                            Str() << '\'' << arg << "' was unexpected"
                        );
                        return ParseResult::Error;
                    }

                    const std::string invalid_command = Str()
                        << "Invalid command '" << arg << "'";
                    const auto names = QuoteAndCombineEnglishOr(match.names);

                    // todo(Gustav): switch between 'did you mean' and
                    // 'could be either' depending on how big the
                    // edit distance is?
                    PrintError
                    (
                        Str() << invalid_command <<
                        ", did you mean " << names << '?'
                    );
                    return ParseResult::Error;
                }
                else
                {
                    // go back one step
                    runner->arguments->UndoRead();

                    if(base->on_complete.has_value())
                    {
                        return base->on_complete.value()();
                    }

                    return ParseResult::Ok;
                }
            }
            found_subparser = true;

            auto container = match.values[0];
            const auto calling_name = base->GetCallingName
            (
                runner->arguments->arguments
            );
            auto sub = SubParser
            {
                container->help,
                base,
                runner,
                calling_name + " " + arg
            };
            const auto subresult = container->callback(&sub);
            if
            (
                subresult == ParseResult::Ok &&
                GetParserStyle(base) == SubParserStyle::Greedy
            )
            {
                // continue here
                return std::nullopt;
            }
            else
            {
                return subresult;
            }
        }

        std::optional<ParseResult>
        ArgumentParser::ParseOneOptional()
        {
            const auto arg = runner->arguments->Read();
            if(IsOptionalArgument(arg))
            {
                auto match = base->FindArgument(arg);
                if (match == nullptr)
                {
                    const auto closest_match =
                        std::min_element
                        (
                            base->optional_arguments.begin(),
                            base->optional_arguments.end(),
                            [&]
                            (
                                const auto& lhs,
                                const auto& rhs
                            )
                            {
                                // return true if lhs < rhs
                                return EditDistance(arg, lhs.first) <
                                       EditDistance(arg, rhs.first) ;
                            }
                        );

                    // todo(Gustav): if we have already detected it, 
                    // change the text to 'invalid argument' but in this case
                    // it's actually a unknown argument, not a 'invalid' one
                    if(closest_match == base->optional_arguments.end())
                    {
                        PrintError
                        (
                            Str() << "unknown argument: '" << arg << '\''
                        );
                    }
                    else
                    {
                        const auto closest_match_name = closest_match->first;
                        PrintError
                        (
                            Str() << "unknown argument: '" << arg <<
                            "', did you mean '" << closest_match_name << "'?"
                        );
                    }

                    return ParseResult::Error;
                }
                auto arg_parse_result = match->ParseArguments
                (
                    runner,
                    arg,
                    base
                );
                if (arg_parse_result != ParseResult::Ok)
                {
                    return arg_parse_result;
                }

                return std::nullopt;
            }
            else
            {
                // arg doesn't look like a optional argument
                // hence it must be a sub command
                // or it's a error
                return ParseSubCommand(arg);
            }
        }


        std::optional<ParseResult>
        ArgumentParser::ParseOneArg()
        {
            if (HasMorePositionals())
            {
                auto opt = TryParseImportantOptional();
                if(opt.has_value())
                {
                    return opt;
                }
                else
                {
                    return ParseOnePositional();
                }
            }
            else
            {
                return ParseOneOptional();
            }
        }
    }

    ParseResult
    ParserBase::ParseArgs(Runner* runner)
    {
        parser_state = ParserState::Parsing;
        auto parser = ArgumentParser{this, runner};

        while (runner->arguments->HasMore())
        {
            auto r = parser.ParseOneArg();
            if(r)
            {
                return *r;
            }
        }

        if (parser.HasMorePositionals())
        {
            const auto missing = parser.PositionalsLeft();
            const auto text = StringMerger::EnglishAnd().Generate(missing);
            parser.PrintError
            (
                missing.size() == 1 ?
                (
                    Str() << "Positional " << text << " was not specified."
                )
                :
                (
                    Str() << "Positionals " << text << " were not specified."
                )
            );
            return ParseResult::Error;
        }

        // not sure if should keep or not
        // should handle required subparser, but doesn't
        if(subparsers.size != 0 && parser.found_subparser == false)
        {
            parser.PrintError("no subparser specified");
            return ParseResult::Error;
        }

        if(on_complete.has_value())
        {
            return on_complete.value()();
        }

        return ParseResult::Ok;
    }


    SubParser::SubParser
    (
        const std::string& d,
        ParserBase* p,
        Runner* r,
        const std::string& cn
    )
        : ParserBase(d)
        , parent(p)
        , runner(r)
        , calling_name(cn)
    {
    }


    ParserBase*
    SubParser::GetParentOrNull()
    {
        return parent;
    }

    
    std::string
    SubParser::GetCallingName(const Arguments&)
    {
        return calling_name;
    }


    ParseResult
    SubParser::OnComplete(CompleteFunction com)
    {
        ParserBase::OnComplete(com);
        return ParseArgs(runner);
    }


    Parser::Parser(const std::string& d)
        : ParserBase(d)
        , printer(std::make_shared<ConsolePrinter>())
    {
    }


    ParserBase*
    Parser::GetParentOrNull()
    {
        return nullptr;
    }


    ParseResult
    Parser::Parse(const Arguments& args)
    {
        auto reader = ArgumentReader{ args };
        auto runner = Runner{ &reader, printer };
        return ParserBase::ParseArgs(&runner);
    }


    std::optional<int>
    Parser::Parse(int argc, char* argv[])
    {
        const auto args = Arguments::Extract(argc, argv);
        const auto res = Parse(args);
        if (res == ParseResult::Ok)
        {
            return std::nullopt;
        }
        else
        {
            return res.return_value;
        }
    }


    std::string
    Parser::GetCallingName(const Arguments& args)
    {
        return args.name;
    }


    int
    ParseFromMain(Parser* parser, int argc, char* argv[])
    {
        const auto args = Arguments::Extract(argc, argv);
        const auto res = parser->Parse(args);
        return res.return_value;
    }
}
