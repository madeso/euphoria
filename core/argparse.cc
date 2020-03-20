#include "core/argparse.h"

#include "core/cint.h"
#include "core/stringutils.h"
#include "core/stringmerger.h"
#include "core/table.h"
#include "core/cint.h"
#include "core/str.h"

#include <iostream>
#include <iomanip>
#include <algorithm>


/*

   argparse issues & todos:

   * unhelpful error messages, hint misspelled argument via StringToEnum code

   * assert invalid setups and argument name collisions

   * Narg chainging functions renaming on return value of Add

   * help should optionall expect a argument name to print detailed help

   * generic OnComplete unit test

   * instead of .Add("arg").Help("") perhaps change to .Add(Arg{"arg"}.Help("")) so chainging arguments is possible

   * extra validators combinable with | that adds more conditions (files must exists, strings can be empty, numbers must be in range etc)

   * support python @ append arguments from file "arguments"

   * support reading palettes and rgb(a)s from commandline, including common palette files

   * add real-world tests in unit tests, like fake-git and fake-compiler

   * if bored, get ideas from https://www.npmjs.com/package/commander

*/

/*
    ---------------------------------------------------------------------------

    Sample output from various systems for inspiration:

    ---------------------------------------------------------------------------

    $ python prog.py -h
    usage: prog.py [-h] [--sum] N [N ...]

    Process some integers.

    positional arguments:
     N           an integer for the accumulator

    optional arguments:
     -h, --help  show this help message and exit
     --sum       sum the integers (default: find the max)

    ---------------------------------------------------------------------------

    $ python prog.py a b c
    usage: prog.py [-h] [--sum] N [N ...]
    prog.py: error: argument N: invalid int value: 'a'

    ---------------------------------------------------------------------------

    usage: PROG [-h] [--foo FOO] [bar [bar ...]]

    positional arguments:
     bar         BAR! (default: [1, 2, 3])

    optional arguments:
     -h, --help  show this help message and exit
     --foo FOO   FOO! (default: 42)
    
    ---------------------------------------------------------------------------

    >>> parser = argparse.ArgumentParser(prog='PROG')
    >>> parser.add_argument('-bacon')
    >>> parser.add_argument('-badger')
    >>> parser.parse_args('-bac MMM'.split())
    Namespace(bacon='MMM', badger=None)
    >>> parser.parse_args('-bad WOOD'.split())
    Namespace(bacon=None, badger='WOOD')
    >>> parser.parse_args('-ba BA'.split())
    usage: PROG [-h] [-bacon BACON] [-badger BADGER]
    PROG: error: ambiguous option: -ba could match -badger, -bacon

    ---------------------------------------------------------------------------

    >>> parser.parse_args(['--help'])
    usage: PROG [-h] [--foo] {a,b} ...

    positional arguments:
      {a,b}   sub-command help
        a     a help
        b     b help

    optional arguments:
      -h, --help  show this help message and exit
      --foo   foo help

    >>> parser.parse_args(['a', '--help'])
    usage: PROG a [-h] bar

    positional arguments:
      bar     bar help

    optional arguments:
      -h, --help  show this help message and exit

    >>> parser.parse_args(['b', '--help'])
    usage: PROG b [-h] [--baz {X,Y,Z}]

    optional arguments:
      -h, --help     show this help message and exit
      --baz {X,Y,Z}  baz help

    ---------------------------------------------------------------------------

    $ git -h
    unknown option: -h
    usage: git [--version] [--help] [-C <path>] [-c <name>=<value>]
               [--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]
               [-p | --paginate | --no-pager] [--no-replace-objects] [--bare]
               [--git-dir=<path>] [--work-tree=<path>] [--namespace=<name>]
               <command> [<args>]

    ---------------------------------------------------------------------------

    $ git h
    git: 'h' is not a git command. See 'git --help'.

    The most similar commands are
            am
            co
            gc
            help
            lg
            ls
            mv
            p4
            push
            rm
            show
            up

    ---------------------------------------------------------------------------

    $ git --help
    usage: git [--version] [--help] [-C <path>] [-c <name>=<value>]
               [--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]
               [-p | --paginate | --no-pager] [--no-replace-objects] [--bare]
               [--git-dir=<path>] [--work-tree=<path>] [--namespace=<name>]
               <command> [<args>]

    These are common Git commands used in various situations:

    start a working area (see also: git help tutorial)
       clone      Clone a repository into a new directory
       init       Create an empty Git repository or reinitialize an existing one

    work on the current change (see also: git help everyday)
       add        Add file contents to the index
       mv         Move or rename a file, a directory, or a symlink
       reset      Reset current HEAD to the specified state
       rm         Remove files from the working tree and from the index

    examine the history and state (see also: git help revisions)
       bisect     Use binary search to find the commit that introduced a bug
       grep       Print lines matching a pattern
       log        Show commit logs
       show       Show various types of objects
       status     Show the working tree status

    grow, mark and tweak your common history
       branch     List, create, or delete branches
       checkout   Switch branches or restore working tree files
       commit     Record changes to the repository
       diff       Show changes between commits, commit and working tree, etc
       merge      Join two or more development histories together
       rebase     Reapply commits on top of another base tip
       tag        Create, list, delete or verify a tag object signed with GPG

    collaborate (see also: git help workflows)
       fetch      Download objects and refs from another repository
       pull       Fetch from and integrate with another repository or a local branch
       push       Update remote refs along with associated objects

    'git help -a' and 'git help -g' list available subcommands and some
    concept guides. See 'git help <command>' or 'git help <concept>'
    to read about a specific subcommand or concept.

    ---------------------------------------------------------------------------

    $ myprog --help
    My Super Program 1.0
    Kevin K. <kbknapp@gmail.com>
    Does awesome things

    ARGS:
        INPUT    The input file to use

    USAGE:
        MyApp [FLAGS] [OPTIONS] <INPUT> [SUBCOMMAND]

    FLAGS:
        -h, --help       Prints help information
        -v               Sets the level of verbosity
        -V, --version    Prints version information

    OPTIONS:
        -c, --config <FILE>    Sets a custom config file

    SUBCOMMANDS:
        help    Prints this message or the help of the given subcommand(s)
        test    Controls testing features

    ---------------------------------------------------------------------------

    $ git pull -h
    usage: git pull [<options>] [<repository> [<refspec>...]]

        -v, --verbose         be more verbose
        -q, --quiet           be more quiet
        --progress            force progress reporting
        --recurse-submodules[=<on-demand>]
                              control for recursive fetching of submodules

    Options related to merging
        -r, --rebase[=<false|true|preserve|interactive>]
                              incorporate changes by rebasing rather than merging
        -n                    do not show a diffstat at the end of the merge
        --stat                show a diffstat at the end of the merge
        --log[=<n>]           add (at most <n>) entries from shortlog to merge commit message
        --signoff[=...]       add Signed-off-by:
        --squash              create a single commit instead of doing a merge
        --commit              perform a commit if the merge succeeds (default)
        --edit                edit message before committing
        --ff                  allow fast-forward
        --ff-only             abort if fast-forward is not possible
        --verify-signatures   verify that the named commit has a valid GPG signature
        --autostash           automatically stash/stash pop before and after rebase
        -s, --strategy <strategy>
                              merge strategy to use
        -X, --strategy-option <option=value>
                              option for selected merge strategy
        -S, --gpg-sign[=<key-id>]
                              GPG sign commit
        --allow-unrelated-histories
                              allow merging unrelated histories

    Options related to fetching
        --all                 fetch from all remotes
        -a, --append          append to .git/FETCH_HEAD instead of overwriting
        --upload-pack <path>  path to upload pack on remote end
        -f, --force           force overwrite of local branch
        -t, --tags            fetch all tags and associated objects
        -p, --prune           prune remote-tracking branches no longer on remote
        -j, --jobs[=<n>]      number of submodules pulled in parallel
        --dry-run             dry run
        -k, --keep            keep downloaded pack
        --depth <depth>       deepen history of shallow clone
        --unshallow           convert to a complete repository
        --update-shallow      accept refs that update .git/shallow
        --refmap <refmap>     specify fetch refmap
        -4, --ipv4            use IPv4 addresses only
        -6, --ipv6            use IPv6 addresses only

    ---------------------------------------------------------------------------

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
        if(single) return;
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

    std::optional<std::string>
    ArgumentNoValue::GetSecondLine()
    {
        return std::nullopt;
    }



    ParseResult
    ArgumentNoValue::Parse(Runner* runner)
    {
        return callback(runner);
    }


    SingleArgument::SingleArgument(Callback cb, Describe d)
        : callback(cb)
        , describe(d)
    {
    }


    std::optional<std::string>
    SingleArgument::GetSecondLine()
    {
        return describe();
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
                ss << "[" << aa.name.names[0] << "]";
                // todo(Gustav): include any attributes
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


    void
    ParserBase::PrintHelp(std::shared_ptr<Printer> printer, const Arguments& args)
    {
        using StringTable = Table<std::string>;

        // table functions
        constexpr int MAX_NAME_LENGTH = 10;
        int max_name_length = 0;

        const auto add = [&max_name_length, MAX_NAME_LENGTH]
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

        // todo(Gustav): wordwrap the arguments and description/help
        const auto print = [printer, &max_name_length](const StringTable& t)
        {
            for(int y=0; y<t.GetHeight(); y+=1)
            {
                constexpr auto indent = " ";
                constexpr auto space = "  ";

                const auto name = t(0, y);
                const auto help = t(1, y);
                const auto name_length = Csizet_to_int(name.length());
                if(name_length > max_name_length)
                {
                    printer->PrintInfo(indent + name);
                    const auto extra_space = std::string(max_name_length, ' ');
                    printer->PrintInfo(indent + extra_space + space + help);
                }
                else
                {
                    const auto extra_space = std::string(max_name_length - name_length, ' ');
                    printer->PrintInfo(indent + name + extra_space + space + help);
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
            std::ostringstream default_text;
            if(a.argument->default_value.empty() == false)
            {
                default_text << " (default: " << a.argument->default_value << ")";
            }
            add
            (
                &optionals,
                names,
                a.argument->help + default_text.str(),
                a.argument->GetSecondLine()
            );
        }

        auto subs = std::vector<StringTable>{};
        for(auto group: subparser_groups)
        {
            auto sub = StringTable{};
            for(auto parser: group->parsers)
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
            printer->PrintInfo("positional arguments:");
            print(optionals);
        }

        if(subs.size() != 0)
        {
            bool is_first_group = true;
            printer->PrintInfo("");
            auto sub = subs.begin();
            for(auto group: subparser_groups)
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


    Argument&
    ParserBase::AddArgument(const Name& name, std::shared_ptr<Argument> argument)
    {
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
        auto group = std::make_shared<SubParserGroup>(name, this);
        subparser_groups.emplace_back(group);
        return group;
    }


    std::shared_ptr<Argument>
    ParserBase::FindArgument(const std::string& name)
    {
        return optional_arguments[name];
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
            has_more_positionals();

            void
            print_error(const std::string& error);

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
        ArgumentParser::has_more_positionals()
        {
            const auto positionals_size = Csizet_to_int
            (
                base->positional_argument_list.size()
            );
            return positional_index < positionals_size;
        }


        void
        ArgumentParser::print_error(const std::string& error)
        {
            runner->printer->PrintError(error);
            runner->printer->PrintError
            (
                base->GenerateUsageString
                (
                    runner->arguments->arguments
                )
            );
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
                runner->arguments->Read();
                auto arg_parse_result = arg->Parse(runner);
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
            auto arg_parse_result = match.argument->Parse(runner);
            if (arg_parse_result != ParseResult::Ok)
            {
                return arg_parse_result;
            }
            positional_index += 1;

            return std::nullopt;
        }


        bool
        HasParserThatCanHandleSubparserCallback(ParserBase* base)
        {
            if(base == nullptr) return false;
            const auto can_handle_self =
                base->subparsers.size > 0
                ;
            return can_handle_self ||
                HasParserThatCanHandleSubparserCallback
                (
                    base->GetParentOrNull()
                );
        }


        std::optional<ParseResult>
        ArgumentParser::ParseSubCommand(const std::string& arg)
        {
            auto match = base->subparsers.Match(arg, 3);
            if(match.single_match == false)
            {
                // todo(Gustav): check if this accepts invalid and
                // calls on_complete() on invalid input
                if
                (
                    HasParserThatCanHandleSubparserCallback(base) == false
                )
                {
                    print_error
                    (
                        Str() << "invalid command '" << arg << "'"
                        ", could be " <<
                        StringMerger::EnglishOr().Generate
                        (
                            match.names
                        )
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
                base->GetParserStyle() == SubParserStyle::Greedy
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
                    print_error("invalid argument: " + arg);
                    return ParseResult::Error;
                }
                auto arg_parse_result = match->Parse(runner);
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
            if (has_more_positionals())
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
        auto parser = ArgumentParser{this, runner};

        while (runner->arguments->HasMore())
        {
            auto r = parser.ParseOneArg();
            if(r)
            {
                return *r;
            }
        }

        if (parser.has_more_positionals())
        {
            parser.print_error("positionals left");
            return ParseResult::Error;
        }

        // not sure if should keep or not
        // should handle required subparser, but doesn't
        if(subparsers.size != 0 && parser.found_subparser == false)
        {
            parser.print_error("no subparser specified");
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


    SubParserStyle
    SubParser::GetParserStyle()
    {
        return parser_style;
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


    SubParserStyle
    Parser::GetParserStyle()
    {
        return SubParserStyle::Greedy;
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
