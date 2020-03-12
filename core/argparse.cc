#include "core/argparse.h"

#include "core/cint.h"
#include "core/stringutils.h"
#include "core/stringmerger.h"

#include <iostream>
#include <iomanip>


/*

   argparse issues & todos:

   * output looks horrible

   * unhelpful error messages, hint misspelled argument via StringToEnum code

   * assert invalid setups and arguments

   * group arguments (?) and subparser (!), python has add_subparsers() function
     that create a subparser group and takes name and description/help for that subparser group

   * non-greedy sub commands to operate lite ImageMagic "scripting": https://imagemagick.org/script/magick-script.php

   * Narg chainging functions renaming on return value of Add

   * ParseResult should be a struct/handle custom arguments

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

            constexpr auto VALID_ARGUMENT_NAME_CHARACTERS = "abcdefghijklmnopqrstuwxyz-_";
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


    SubParserContainer::SubParserContainer(SubParserCallback cb)
        : callback(cb)
    {
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


    void
    ParserBase::AddSubParser
    (
        const std::string& name,
        const std::string& desc,
        SubParserCallback sub
    )
    {
        auto container = std::make_shared<SubParserContainer>(sub);
        subparsers.Add(name, container);
    }


    void
    ParserBase::AddSubParser
    (
        const std::string& name,
        SubParserCallback sub
    )
    {
        AddSubParser(name, "", sub);
    }


    std::shared_ptr<Argument>
    ParserBase::FindArgument(const std::string& name)
    {
        return optional_arguments[name];
    }


    ParseResult
    ParserBase::ParseArgs(Runner* runner)
    {
        int positional_index = 0;

        auto has_more_positionals = [&]()
        {
            return positional_index < Csizet_to_int(positional_argument_list.size());
        };

        while (runner->arguments->HasMore())
        {
            if (has_more_positionals())
            {
                // first, peek at the next commandline argument
                // and check for optionals that are allowed before positionals
                if
                (
                    const auto arg = FindArgument(runner->arguments->Peek());
                    arg != nullptr && arg->allow_before_positionals
                )
                {
                    // we have matched a optional, read and parse it!
                    runner->arguments->Read();
                    if
                    (
                        auto arg_parse_result = arg->Parse(runner);
                        arg_parse_result != ParseResult::Ok
                    )
                    {
                        return arg_parse_result;
                    }
                }
                else
                {
                    // the peeked argument isn't a important optional
                    // handle positional
                    auto match = positional_argument_list[positional_index];
                    auto arg_parse_result = match.argument->Parse(runner);
                    if (arg_parse_result != ParseResult::Ok)
                    {
                        return arg_parse_result;
                    }
                    positional_index += 1;
                }
            }
            else
            {
                const auto arg = runner->arguments->Read();
                if(IsOptionalArgument(arg))
                {
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
                else
                {
                    // arg doesn't look like a optional argument
                    // hence it must be a sub command
                    // or it's a error
                    if(subparsers.size == 0)
                    {
                        runner->printer->PrintError("no subcomands for " + arg);
                        return ParseResult::Error;
                    }
                    else
                    {
                        auto match = subparsers.Match(arg, 3);
                        if(match.single_match == false)
                        {
                            runner->printer->PrintError("too many matches for " + arg);
                            return ParseResult::Error;
                        }
                        else
                        {
                            auto sub = SubParser{runner};
                            return match.values[0]->callback(&sub);
                        }
                    }
                }
            }
        }

        if (has_more_positionals())
        {
            runner->printer->PrintError("positionals left");
            return ParseResult::Error;
        }

        if(subparsers.size != 0)
        {
            runner->printer->PrintError("no subparser specified");
            return ParseResult::Error;
        }

        if(on_complete.has_value())
        {
            on_complete.value()();
        }

        return ParseResult::Ok;
    }


    SubParser::SubParser(Runner* r)
        : runner(r)
    {
    }


    ParseResult
    SubParser::OnComplete(CompleteFunction com)
    {
        ParserBase::OnComplete(com);
        return ParseArgs(runner);
    }


    Parser::Parser(const std::string& d)
        : description(d)
        , printer(std::make_shared<ConsolePrinter>())
    {
        AddArgument
        (
            "-h, --help",
            std::make_shared<ArgumentNoValue>
            (
                [this](Runner* runner)
                {
                    PrintHelp(runner->arguments->arguments);
                    return ParseResult::Quit;
                }
            )
        )
        .AllowBeforePositionals()
        .Help("show this help message and exit")
        ;
    }


    std::string
    Parser::GenerateUsageString(const Arguments& args)
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
        auto ret = std::vector<std::string>{args.name};

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

        return StringMerger::Space().Generate(ret);
    }


    void
    Parser::PrintHelp(const Arguments& args)
    {
        printer->PrintInfo("usage: " + GenerateUsageString(args));
        printer->PrintInfo("");
        bool dirty = false;
        auto print_newline = [&]()
        {
            if(dirty)
            {
                printer->PrintInfo("");
                dirty = false;
            }
        };
        if (description.empty() == false)
        {
            printer->PrintInfo(description);
            dirty = true;
        }
        print_newline();

        // todo(Gustav): use a string table here, add wordwrap to table
        if (positional_argument_list.empty() == false)
        {
            dirty = true;
            printer->PrintInfo("positional arguments:");
            for (auto& a : positional_argument_list)
            {
                printer->PrintInfo(a.name.names[0] + " " + a.argument->help);
            }
        }
        print_newline();

        if(optional_argument_list.empty() == false)
        {
            dirty = true;
            printer->PrintInfo("positional arguments:");
            for (auto& a : optional_argument_list)
            {
                const auto names = StringMerger::Comma().Generate(a.name.names);
                std::ostringstream default_text;
                if(a.argument->default_value.empty() == false)
                {
                    default_text << " (default: " << a.argument->default_value << ")";
                }
                printer->PrintInfo(names + " " + a.argument->help + default_text.str());
            }
        }
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
            return ReturnValue(res);
        }
    }


    int
    ParseFromMain(Parser* parser, int argc, char* argv[])
    {
        const auto args = Arguments::Extract(argc, argv);
        const auto res = parser->Parse(args);
        return ReturnValue(res);
    }
}
