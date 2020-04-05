#ifndef CORE_ARGPARSE_H
#define CORE_ARGPARSE_H

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <memory>
#include <optional>
#include <functional>
#include <type_traits>

#include "core/enumtostring.h"
#include "core/stringmerger.h"
#include "core/custom_parser.h"
#include "core/result.h"


namespace euphoria::core::argparse
{
    struct ParserBase;

    const std::string
    QuoteAndCombineEnglishOr(const std::vector<std::string>& matches);

    // enum to catch adding arguments during parsing in a callback
    enum class ParserState
    {
        Adding, Parsing
    };

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

    void
    PrintParseError
    (
        Runner* runner,
        ParserBase* base,
        const std::string& error
    );


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
        std::string nargs;
        bool allow_before_positionals = false;

        // automatically filled
        std::string default_value;

        Argument&
        Nargs(const std::string& na);

        Argument&
        Help(const std::string& h);

        Argument&
        AllowBeforePositionals();

        virtual
        std::optional<std::string>
        GetSecondLine() = 0;

        virtual
        bool
        HaveNargs() = 0;

        virtual
        ParseResult
        ParseArguments
        (
            Runner* runner,
            const std::string& name,
            ParserBase* caller
        ) = 0;
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

        bool
        HaveNargs() override;

        std::optional<std::string>
        GetSecondLine() override;

        ParseResult
        ParseArguments
        (
            Runner* runner,
            const std::string& name,
            ParserBase* caller
        ) override;
    };

    // a single argument, probably either a --count 3 or positional input
    struct SingleArgument : public Argument
    {
        using Callback = std::function
        <
            ParseResult
            (
                Runner* runner,
                const std::string& name,
                ParserBase* caller,
                const std::string& value
            )
        >;
        using Describe = std::function<std::optional<std::string> ()>;
        Callback callback;
        Describe describe;

        SingleArgument(Callback cb, Describe d);

        bool
        HaveNargs() override;

        std::optional<std::string>
        GetSecondLine() override;

        ParseResult
        ParseArguments
        (
            Runner* runner,
            const std::string& name,
            ParserBase* caller
        ) override;
    };

    // generic parse function
    template<typename T>
    using ParseFunction = std::function
    <
        Result<T>
        (
            const std::string& value
        )
    >;

    template
    <
        typename T,
        std::enable_if_t<CustomArgparser<T>::value != 0, int> = 0
    >
    Result<T>
    DefaultParseFunction
    (
        const std::string& value
    )
    {
        auto r = CustomArgparser_Parse<T>(value);
        return r;
    }

    // default parse function for non-enums
    template
    <
        typename T,
        std::enable_if_t
        <
            std::is_enum<T>::value == false &&
            CustomArgparser<T>::value == 0
        , int> = 0
    >
    Result<T>
    DefaultParseFunction
    (
        const std::string& value
    )
    {
        auto stream = std::istringstream{value};
        T t;
        stream >> t;
        if(stream.fail() || !stream.eof())
        {
            return Result<T>::False();
        }
        else
        {
            return Result<T>::True(t);
        }
    }

    // default parse function for enums
    template
    <
        typename T,
        std::enable_if_t
        <
            std::is_enum<T>::value == true &&
            CustomArgparser<T>::value == 0
        , int> = 0
    >
    Result<T>
    DefaultParseFunction
    (
        const std::string& value
    )
    {
        auto matches = core::StringToEnum<T>(value);
        if (matches.single_match)
        {
            return Result<T>::True(matches.values[0]);
        }
        else
        {
            return Result<T>::False(Str() << "did you mean " <<
                QuoteAndCombineEnglishOr(matches.names)
                << '?'
            );
        }
    }

    // default describe function for non-enums
    template
    <
        typename T,
        std::enable_if_t<std::is_enum<T>::value == false, int> = 0
    >
    std::optional<std::string>
    DefaultDescribe()
    {
        return std::nullopt;
    }

    // default describe function for enums
    template
    <
        typename T,
        std::enable_if_t<std::is_enum<T>::value == true, int> = 0
    >
    std::optional<std::string>
    DefaultDescribe()
    {
        const std::string r = Str() << "can be either " <<
            QuoteAndCombineEnglishOr
            (
                EnumToString<T>()
            );

        return r;
    }

    template
    <
        typename T,
        std::enable_if_t<CustomArgparser<T>::value != 0, int> = 0
    >
    std::string
    DefaultValueToString(const T& t)
    {
        return CustomArgparser_ToString(t);
    }


    // default value for non enums
    template
    <
        typename T,
        std::enable_if_t
        <
            std::is_enum<T>::value == false &&
            CustomArgparser<T>::value == 0
        , int> = 0
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
        std::enable_if_t
        <
            std::is_enum<T>::value == true &&
            CustomArgparser<T>::value == 0
        , int> = 0
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
        Fallback,

        // inherit from parent
        Inherit
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
        ParserState parser_state = ParserState::Adding;
        SubParserStyle parser_style = SubParserStyle::Inherit;

        explicit ParserBase(const std::string& d);

        virtual
        ~ParserBase();

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
        Add
        (
            const Name& name,
            T* target,
            ParseFunction<T> parse_function = DefaultParseFunction<T>
        )
        {
            auto arg = std::make_shared<SingleArgument>(
            [target, parse_function]
            (
                Runner* runner,
                const std::string& argument_name,
                ParserBase* caller,
                const std::string& value
            )
            {
                auto parsed = parse_function(value);
                if(parsed)
                {
                    *target = *parsed;
                    return ParseResult::Ok;
                }
                else
                {
                    const std::string base = Str()
                        << '\'' << value << "' is not accepted for '"
                        << argument_name << '\'';
                    const auto error = parsed.Error();
                    const std::string message = error.empty()
                        ? base
                        : (Str() << base << ", " << error)
                        ;
                    PrintParseError(runner, caller, message);

                    return ParseResult::Error;
                }
            }, [](){
                const std::optional<std::string> str = DefaultDescribe<T>();
                return str;
            });
            arg->default_value = DefaultValueToString(*target);
            return AddArgument(name, arg);
        }

        std::shared_ptr<SubParserGroup>
        AddSubParsers(const std::string& name="commands");

        std::shared_ptr<Argument>
        FindArgument(const std::string& name) const;

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

        SubParser
        (
            const std::string& d,
            ParserBase* p,
            Runner* r,
            const std::string& cn
        );

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
