#ifndef CORE_ARGPARSE_H
#define CORE_ARGPARSE_H

#include "core/enumtostring.h"

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <memory>
#include <optional>
#include <functional>
#include <type_traits>


namespace euphoria::core::argparse
{

    // option where the output can either be a single file or "infinite"
    struct FileOutput
    {
        explicit FileOutput(const std::string& o);

        std::string
        NextFile(bool print = true);

        void
        CreateDirIfMissing() const;

        std::string file;
        bool        single;
        int         index = 0;
    };


    struct ParseResult
    {
        enum class Type { Error, Ok, Quit, Custom };

        Type type;
        int return_value;

        // no error occured
        static const ParseResult Error;

        // all ok
        static const ParseResult Ok;
        
        // all ok, but quit requested
        static const ParseResult Quit;

        constexpr explicit ParseResult(Type t);
        constexpr explicit ParseResult(int rv);
    };

    std::ostream&
    operator<<(std::ostream& o, const ParseResult& pr);

    bool
    operator==(const ParseResult& lhs, const ParseResult& rhs);


    bool
    operator!=(const ParseResult& lhs, const ParseResult& rhs);


    // container for arguments passed to main
    struct Arguments
    {
        std::string name;
        std::vector<std::string> arguments;

        Arguments(const std::string& n, const std::vector<std::string>& a);

        static
        Arguments
        Extract(int argc, char* argv[]);
    };

    std::ostream&
    operator<<(std::ostream& stream, const Arguments& arguments);


    // "file" like api for reading arguments
    struct ArgumentReader
    {
        Arguments arguments;
        int next_position;

        explicit
        ArgumentReader(const Arguments& a);

        bool
        HasMore() const;

        std::string
        Peek() const;

        std::string
        Read();

        void
        UndoRead();
    };

    // generic output class
    struct Printer
    {
        virtual ~Printer();

        virtual void
        PrintError(const std::string& line) = 0;

        virtual void
        PrintInfo(const std::string& line) = 0;
    };

    // console output
    struct ConsolePrinter : public Printer
    {
        void
        PrintError(const std::string& line) override;

        void
        PrintInfo(const std::string& line) override;
    };

    // shared data between parsing functions
    struct Runner
    {
        ArgumentReader* arguments;
        std::shared_ptr<Printer> printer;
    };


    // represents a command line argument
    // optional: start with either - or --
    //    can have many names
    // positional: doesn't start with -
    //    can only have a single name
    struct Name
    {
        std::vector<std::string> names;

        Name(const char* n);

        // return empty string if valid, return error message if not
        [[nodiscard]]
        std::string
        Validate() const;

        bool
        IsOptional() const;
    };


    // base class for argument
    struct Argument
    {
        virtual ~Argument();

        std::string help;
        bool allow_before_positionals = false;

        // automatically filled
        std::string default_value;

        Argument&
        Help(const std::string& h);

        Argument&
        AllowBeforePositionals();

        virtual
        ParseResult
        Parse(Runner* reader) = 0;
    };

    // named tuple class for argument and name
    struct ArgumentAndName
    {
        Name name;
        std::shared_ptr<Argument> argument;

        ArgumentAndName(const Name& n, std::shared_ptr<Argument> a);
    };

    // a argument with no value, probably either a --set-true
    // or a void function like --help
    struct ArgumentNoValue : public Argument
    {
        using Callback = std::function<ParseResult (Runner*)>;
        Callback callback;

        explicit ArgumentNoValue(Callback cb);

        ParseResult
        Parse(Runner* runner) override;
    };

    // a single argument, probably either a --count 3 or positional input
    struct SingleArgument : public Argument
    {
        using Callback = std::function<ParseResult (Runner*, const std::string&)>;
        Callback callback;

        explicit SingleArgument(Callback cb);

        ParseResult
        Parse(Runner* runner) override;
    };

    // generic parse function
    template<typename T>
    using ParseFunction = std::function<std::optional<T> (std::shared_ptr<Printer> printer, const std::string&)>;

    // default parse function for non-enums
    template
    <
        typename T,
        std::enable_if_t<std::is_enum<T>::value == false, int> = 0
    >
    std::optional<T>
    DefaultParseFunction(std::shared_ptr<Printer> printer, const std::string& value)
    {
        auto stream = std::istringstream{value.c_str()};
        T t;
        stream >> t;
        if(stream.fail() || !stream.eof())
        {
            // todo(include argument name here
            printer->PrintError(value + " is not accepted");
            return std::nullopt;
        }
        else
        {
            return t;
        }
    }

    // default parse function for enums
    template
    <
        typename T,
        std::enable_if_t<std::is_enum<T>::value == true, int> = 0
    >
    std::optional<T>
    DefaultParseFunction(std::shared_ptr<Printer> printer, const std::string& value)
    {
        auto matches = core::StringToEnum<T>(value);
        if (matches.single_match)
        {
            return matches.values[0];
        }
        else
        {
            // todo(Gustav): include argument name and matches here
            printer->PrintError(value + " is not accepted");
            return std::nullopt;
        }
    }

    // default value for non enums
    template
    <
        typename T,
        std::enable_if_t<std::is_enum<T>::value == false, int> = 0
    >
    std::string
    DefaultValueToString(const T& t)
    {
        std::ostringstream ss;
        ss << t;
        return ss.str();
    }

    // default value for enums
    template
    <
        typename T,
        std::enable_if_t<std::is_enum<T>::value == true, int> = 0
    >
    std::string
    DefaultValueToString(const T& t)
    {
        return core::EnumToString(t);
    }


    struct SubParser;

    using SubParserCallback = std::function
    <
        ParseResult (SubParser*)
    >;

    using CompleteFunction = std::function
    <
        ParseResult ()
    >;

    // valid names for a single subparser
    // example: [checkout, co] or [pull]
    struct SubParserNames
    {
        std::vector<std::string> names;

        SubParserNames(const char* str);
    };
    
    // data about a subparser
    struct SubParserContainer
    {
        SubParserNames names;
        std::string help;
        SubParserCallback callback;

        SubParserContainer
        (
            const SubParserNames& n,
            const std::string& h,
            SubParserCallback cb
        );
    };

    struct ParserBase;

    // subparsers can be grouped and this structs represents that group
    struct SubParserGroup
    {
        std::string title;
        ParserBase* owner;
        std::vector<std::shared_ptr<SubParserContainer>> parsers;

        SubParserGroup
        (
            const std::string& t,
            ParserBase* o
        );

        void
        Add
        (
            const SubParserNames& names,
            const std::string& desc,
            SubParserCallback sub
        );

        void
        Add
        (
            const SubParserNames& names,
            SubParserCallback sub
        );
    };

    // how the subparsing is handled, non-greedy are 
    // useful for 'scripting' with subparsers
    enum class SubParserStyle
    {
        // parse all arguments
        Greedy,

        // if argument is invalid, go back one step and try there
        Fallback
    };

    // base for the parser, start with Parser and add one or more subparsers
    struct ParserBase
    {
        std::string description;

        std::vector<ArgumentAndName> positional_argument_list;

        std::map<std::string, std::shared_ptr<Argument>> optional_arguments;
        std::vector<ArgumentAndName> optional_argument_list;

        EnumToStringImpl<std::shared_ptr<SubParserContainer>> subparsers;
        std::vector<std::shared_ptr<SubParserGroup>> subparser_groups;

        std::optional<CompleteFunction> on_complete;

        explicit ParserBase(const std::string& d);

        virtual
        ~ParserBase();

        virtual
        SubParserStyle
        GetParserStyle() = 0;

        virtual
        ParserBase*
        GetParentOrNull() = 0;

        std::string
        GenerateUsageString(const Arguments& args);

        virtual
        std::string
        GetCallingName(const Arguments& args) = 0;

        void
        PrintHelp(std::shared_ptr<Printer> printer, const Arguments& args);

        Argument&
        AddArgument(const Name& name, std::shared_ptr<Argument> argument);

        Argument&
        AddVoidFunction(const Name& name, std::function<void()> void_function);

        void
        OnComplete(CompleteFunction com);

        template<typename T>
        Argument&
        SetConst(const Name& name, T* target, T t)
        {
            return AddVoidFunction(name, [target, t]()
            {
                *target = t;
            });
        }

        Argument&
        SetTrue(const Name& name, bool* target);

        Argument&
        SetFalse(const Name& name, bool* target);

        template<typename T>
        Argument&
        Add(const Name& name, T* target, ParseFunction<T> parse_function = DefaultParseFunction<T>)
        {
            auto arg = std::make_shared<SingleArgument>([target, parse_function](Runner* runner, const std::string& value)
            {
                auto parsed = parse_function(runner->printer, value);
                if(parsed)
                {
                    *target = *parsed;
                    return ParseResult::Ok;
                }
                else
                {
                    return ParseResult::Error;
                }
            });
            arg->default_value = DefaultValueToString(*target);
            return AddArgument(name, arg);
        }

        std::shared_ptr<SubParserGroup>
        AddSubParsers(const std::string& name="Commands");

        std::shared_ptr<Argument>
        FindArgument(const std::string& name);

        [[nodiscard]]
        ParseResult
        ParseArgs(Runner* runner);
    };

    // subparser, don't create manually but add to a existing parser
    // AddSubParsers()->Add(...);
    struct SubParser : public ParserBase
    {
        ParserBase* parent;
        Runner* runner;
        std::string calling_name;
        SubParserStyle parser_style = SubParserStyle::Greedy;

        SubParser
        (
            const std::string& d,
            ParserBase* p,
            Runner* r,
            const std::string& cn
        );

        SubParserStyle
        GetParserStyle() override;

        ParserBase*
        GetParentOrNull() override;

        std::string
        GetCallingName(const Arguments& args) override;

        [[nodiscard]]
        ParseResult
        OnComplete(CompleteFunction com);
    };

    // root parser. start argumentparsing with this one
    struct Parser : public ParserBase
    {
        explicit Parser(const std::string& d = "");

        SubParserStyle
        GetParserStyle() override;

        ParserBase*
        GetParentOrNull() override;

        ParseResult
        Parse(const Arguments& args);

        // value = return from main
        // nullopt = continue, parsing was ok
        std::optional<int>
        Parse(int argc, char* argv[]);

        std::string
        GetCallingName(const Arguments& args) override;

        std::shared_ptr<Printer> printer;
    };

    // helper function for parsing directly from main
    int
    ParseFromMain(Parser* parser, int argc, char* argv[]);
}

#endif  // CORE_ARGPARSE_H
