#ifndef CORE_ARGPARSE_H
#define CORE_ARGPARSE_H

// todo(Gustav): optimize headers

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <optional>
#include <functional>
#include <type_traits>

#include "core/enumtostring.h"


namespace euphoria::core::argparse
{
    enum class ParseResult
    {
        // no error occured
        Error,
        // all ok
        Ok,
        // all ok, but quit requested
        Quit
    };


    int
    ReturnValue(ParseResult pr);


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


    struct Name
    {
        std::string name;

        // optional: start with either - or --
        // positional: doesn't start with -

        Name(const std::string& n);

        Name(const char* n);

        bool
        IsOptional() const;
    };


    struct Argument
    {
        virtual ~Argument();

        std::string help;

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
        std::enable_if_t<!std::is_enum<T>::value == true, int> = 0
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
        std::enable_if_t<std::is_enum<T>::type, int> = 0
        >
        std::optional<T>
        DefaultParseFunction(std::shared_ptr<Printer> printer, const std::string& value)
    {
        auto stream = std::istringstream{ value.c_str() };
        T t;
        stream >> t;
        if (stream.fail() || !stream.eof())
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


    struct SubParser;
    using SubParserCallback = std::function
    <
        ParseResult (SubParser*)
    >;


    using CompleteFunction = std::function
    <
        void ()
    >;


    struct SubParserContainer
    {
        // todo: add description
        SubParserContainer(SubParserCallback cb);

        SubParserCallback callback;
    };


    struct Parser
    {
        std::vector<ArgumentAndName> positional_argument_list;

        std::map<std::string, std::shared_ptr<Argument>> optional_arguments;
        std::vector<ArgumentAndName> optional_argument_list;

        EnumToStringImpl<std::shared_ptr<SubParserContainer>> subparsers;

        std::optional<CompleteFunction> on_complete;

        Argument&
        AddArgument(const Name& name, std::shared_ptr<Argument> argument);

        Argument&
        AddVoidFunction(const Name& name, std::function<void()> void_function);

        void
        OnComplete(CompleteFunction com);

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
            return AddArgument(name, arg);
        }

        void
        AddSubParser
        (
            const std::string& name,
            SubParserCallback sub
        );

        std::shared_ptr<Argument>
        FindArgument(const std::string& name);

        [[nodiscard]]
        ParseResult
        ParseArgs(Runner* runner);
    };


    struct SubParser : public Parser
    {
        Runner* runner;

        explicit SubParser(Runner* r);

        [[nodiscard]]
        ParseResult
        OnComplete(CompleteFunction com);
    };


    struct RootParser : public Parser
    {
        RootParser(const std::string& d = "");

        void
        PrintHelp();

        ParseResult
        ParseArgs(const Arguments& args);

        std::string description;

        std::shared_ptr<Printer> printer;
    };
}

#endif  // CORE_ARGPARSE_H
