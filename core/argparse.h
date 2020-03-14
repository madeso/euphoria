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


    bool
    operator==(const ParseResult& lhs, const ParseResult& rhs);


    bool
    operator!=(const ParseResult& lhs, const ParseResult& rhs);


    struct Arguments
    {
        std::string              name;
        std::vector<std::string> arguments;

        Arguments(const std::string& n, const std::vector<std::string>& a);

        static Arguments
        Extract(int argc, char* argv[]);
    };


    struct ArgumentReader
    {
        Arguments arguments;
        int next_position;

        explicit ArgumentReader(const Arguments& a);

        bool
        HasMore() const;

        std::string
        Peek() const;

        std::string
        Read();
    };


    struct Printer
    {
        virtual ~Printer();

        virtual void
        PrintError(const std::string& line) = 0;

        virtual void
        PrintInfo(const std::string& line) = 0;
    };


    struct ConsolePrinter : public Printer
    {
        void
        PrintError(const std::string& line) override;

        void
        PrintInfo(const std::string& line) override;
    };


    struct Runner
    {
        ArgumentReader* arguments;
        std::shared_ptr<Printer> printer;
    };


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


    struct ArgumentAndName
    {
        Name name;
        std::shared_ptr<Argument> argument;

        ArgumentAndName(const Name& n, std::shared_ptr<Argument> a);
    };


    struct ArgumentNoValue : public Argument
    {
        using Callback = std::function<ParseResult (Runner*)>;
        Callback callback;

        explicit ArgumentNoValue(Callback cb);

        ParseResult
        Parse(Runner* runner) override;
    };


    struct SingleArgument : public Argument
    {
        using Callback = std::function<ParseResult (Runner*, const std::string&)>;
        Callback callback;

        explicit SingleArgument(Callback cb);

        ParseResult
        Parse(Runner* runner) override;
    };


    template<typename T>
    using ParseFunction = std::function<std::optional<T> (std::shared_ptr<Printer> printer, const std::string&)>;


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
            // todo(include argument name and matches here
            printer->PrintError(value + " is not accepted");
            return std::nullopt;
        }
    }


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


    struct SubParserNames
    {
        std::vector<std::string> names;

        SubParserNames(const char* str);
    };
    

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
        virtual ~ParserBase();

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


    struct SubParser : public ParserBase
    {
        Runner* runner;
        std::string calling_name;

        SubParser(const std::string& d, Runner* r, const std::string& cn);

        std::string
        GetCallingName(const Arguments& args) override;

        [[nodiscard]]
        ParseResult
        OnComplete(CompleteFunction com);
    };


    struct Parser : public ParserBase
    {
        explicit Parser(const std::string& d = "");

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

    int
    ParseFromMain(Parser* parser, int argc, char* argv[]);
}

#endif  // CORE_ARGPARSE_H
