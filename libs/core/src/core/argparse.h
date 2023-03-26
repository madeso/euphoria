#pragma once


#include <map>


#include <memory>
#include <optional>
#include <functional>
#include <type_traits>

#include "core/default_parse.h"
#include "core/stringmerger.h"
#include "core/result.h"


namespace euphoria::core::argparse
{
    struct ParserBase;

    /// enum to catch adding arguments during parsing in a callback
    enum class State
    {
        adding, parsing
    };

    /// option where the output can either be a single file or "infinite"
    struct FileOutput
    {
        std::string file;
        bool single;
        int index = 0;

        explicit FileOutput(const std::string& o);

        std::string get_next_file(bool print = true);
        void create_dir_if_missing() const;
    };


    struct ParseResult
    {
        enum class Type { error, ok, quit, custom };

        Type internal_type;
        int return_value;

        constexpr explicit ParseResult(Type t)
            : internal_type(t)
            , return_value(get_default_return_value(t))
        {
        }

        constexpr explicit ParseResult(int rv)
            : internal_type(ParseResult::Type::custom)
            , return_value(rv)
        {
        }

        static constexpr int get_default_return_value(Type pr)
        {
            switch (pr)
            {
            case Type::ok: return 0;
            case Type::error: return -1;
            case Type::quit: return 0;
            default: return -1;
            }
        }
    };

    /// no error occurred
    constexpr ParseResult error = ParseResult{ ParseResult::Type::error };

    /// all ok
    constexpr ParseResult ok = ParseResult{ ParseResult::Type::ok };

    /// all ok, but quit requested
    constexpr ParseResult quit = ParseResult{ ParseResult::Type::quit };
    
    std::string to_string(const ParseResult& pr);
}

ADD_DEFAULT_FORMATTER(euphoria::core::argparse::ParseResult, std::string, euphoria::core::argparse::to_string);

namespace euphoria::core::argparse
{
    bool
    operator==(const ParseResult& lhs, const ParseResult& rhs);


    bool
    operator!=(const ParseResult& lhs, const ParseResult& rhs);


    /// container for arguments passed to main
    struct NameAndArguments
    {
        std::string name;
        std::vector<std::string> arguments;

        NameAndArguments(const std::string& n, const std::vector<std::string>& a);

        static
        NameAndArguments
        extract(int argc, char* argv[]);
    };
    std::string to_string(const NameAndArguments& arguments);
}

ADD_DEFAULT_FORMATTER(euphoria::core::argparse::NameAndArguments, std::string, euphoria::core::argparse::to_string);

namespace euphoria::core::argparse
{
    /// "file" like api for reading arguments
    struct ArgumentReader
    {
        NameAndArguments arguments;
        int next_position;

        explicit ArgumentReader(const NameAndArguments& a);

        [[nodiscard]] std::string read();
        void undo_read();

        [[nodiscard]] bool has_more() const;
        [[nodiscard]] std::string peek() const;
    };

    /// generic output class
    struct Printer
    {
        Printer() = default;
        virtual ~Printer() = default;

        Printer(const Printer&) = delete;
        Printer(Printer&&) = delete;
        void operator=(const Printer&) = delete;
        void operator=(Printer&&) = delete;

        virtual void
        print_error(const std::string& line) = 0;

        virtual void
        print_info(const std::string& line) = 0;
    };

    /// console output
    struct ConsolePrinter : public Printer
    {
        void
        print_error(const std::string& line) override;

        void
        print_info(const std::string& line) override;
    };

    /// shared data between parsing functions
    struct Runner
    {
        ArgumentReader* arguments;
        std::shared_ptr<argparse::Printer> printer;
    };

    void
    print_parse_error
    (
        Runner* runner,
        ParserBase* base,
        const std::string& error_message
    );


    /// represents a command line argument
    /// optional:
    ///    * start with either - or --
    ///    * can have many names
    /// positional:
    ///    * doesn't start with -
    ///    * can only have a single name
    struct Name
    {
        std::vector<std::string> names;

        Name(const char* nn);

        // return empty string if valid, return error message if not
        [[nodiscard]]
        std::string
        validate() const;

        [[nodiscard]] bool
        is_optional() const;
    };


    /// base class for argument
    struct Argument
    {
        std::string help;
        std::string nargs;
        bool allow_before_positionals = false;

        // automatically filled
        std::string default_value;

        Argument() = default;
        virtual ~Argument() = default;

        Argument(const Argument&) = delete;
        Argument(Argument&&) = delete;
        void operator=(const Argument&) = delete;
        void operator=(Argument&&) = delete;

        Argument&
        set_nargs(const std::string& na);

        Argument&
        set_help(const std::string& h);

        Argument&
        set_allow_before_positionals();

        virtual
        std::optional<std::string>
        get_second_line() = 0;

        virtual
        bool
        have_nargs() = 0;

        virtual
        ParseResult
        parse_arguments
        (
            Runner* runner,
            const std::string& name,
            ParserBase* caller
        ) = 0;
    };

    /// named tuple class for argument and name
    struct ArgumentAndName
    {
        argparse::Name name;
        std::shared_ptr<argparse::Argument> argument;

        ArgumentAndName(const argparse::Name& n, std::shared_ptr<argparse::Argument> a);
    };

    /// a argument with no value, probably either a --set-true
    /// or a void function like --help
    struct ArgumentNoValue : public Argument
    {
        using CallbackFunction = std::function<ParseResult (Runner*)>;
        
        CallbackFunction callback_function;

        explicit ArgumentNoValue(CallbackFunction cb);

        bool have_nargs() override;

        std::optional<std::string> get_second_line() override;

        ParseResult parse_arguments
        (
            Runner* runner,
            const std::string& name,
            ParserBase* caller
        ) override;
    };

    /// a single argument, probably either a --count 3 or positional input
    struct SingleArgument : public Argument
    {
        using CallbackFunction = std::function
        <
            ParseResult
            (
                Runner* runner,
                const std::string& name,
                ParserBase* caller,
                const std::string& value
            )
        >;
        using DescribeFunction = std::function<std::optional<std::string> ()>;

        CallbackFunction callback_function;
        DescribeFunction describe_function;

        SingleArgument(CallbackFunction cb, DescribeFunction d);

        bool have_nargs() override;
        std::optional<std::string> get_second_line() override;

        ParseResult parse_arguments
        (
            Runner* runner,
            const std::string& name,
            ParserBase* caller
        ) override;
    };

    /// a single argument, probably either a --count 3 or positional input
    struct MultiArgument : public Argument
    {
        using CallbackFunction = std::function
        <
            ParseResult
            (
                Runner* runner,
                const std::string& name,
                ParserBase* caller,
                const std::string& value
            )
        >;
        using DescribeFunction = std::function<std::optional<std::string> ()>;

        CallbackFunction callback_function;
        DescribeFunction describe_function;

        MultiArgument(CallbackFunction cb, DescribeFunction d);

        bool have_nargs() override;

        std::optional<std::string> get_second_line() override;

        ParseResult parse_arguments
        (
            Runner* runner,
            const std::string& name,
            ParserBase* caller
        ) override;
    };

    /// generic parse function
    template<typename T>
    using ParseFunction = std::function<Result<T> (const std::string& value)>;
    
    struct SubParser;

    using SubParserCallback = std::function<ParseResult (SubParser*)>;
    using CompleteFunction = std::function<ParseResult ()>;

    /// valid names for a single subparser
    /// example: [checkout, co] or [pull]
    struct SubParserNames
    {
        std::vector<std::string> names;

        SubParserNames(const char* str);
    };

    /// data about a subparser
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

    /// subparsers can be grouped and this structs represents that group
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

        void add
        (
            const SubParserNames& names,
            const std::string& desc,
            SubParserCallback sub
        );

        void add
        (
            const SubParserNames& names,
            SubParserCallback sub
        );
    };

    /// how the subparsing is handled, non-greedy are
    /// useful for 'scripting' with subparsers
    enum class SubParserStyle
    {
        /// parse all arguments
        greedy,

        /// if argument is invalid, go back one step and try there
        fallback,

        /// inherit from parent
        inherit
    };

    /// base for the parser, start with Parser and add one or more subparsers
    struct ParserBase
    {
        std::string description;
        std::vector<ArgumentAndName> positional_argument_list;
        std::map<std::string, std::shared_ptr<Argument>> optional_arguments;
        std::vector<ArgumentAndName> optional_argument_list;
        EnumToStringImplementation<std::shared_ptr<SubParserContainer>> subparsers;
        std::vector<std::shared_ptr<SubParserGroup>> subparser_groups;
        std::optional<CompleteFunction> on_complete_function;
        State parser_state = State::adding;
        SubParserStyle parser_style = SubParserStyle::inherit;

        explicit ParserBase(const std::string& d);
        virtual ~ParserBase() = default;

        ParserBase(const ParserBase&) = delete;
        ParserBase(ParserBase&&) = delete;
        void operator=(const ParserBase&) = delete;
        void operator=(ParserBase&&) = delete;

        std::string generate_usage_string(const NameAndArguments& args);
        void print_help(std::shared_ptr<Printer> printer, const NameAndArguments& args);
        void on_complete(CompleteFunction com);

        Argument& add_argument(const Name& name, std::shared_ptr<Argument> argument);
        Argument& add_void_function(const Name& name, std::function<void()> void_function);
        Argument& set_true(const Name& name, bool* target);
        Argument& set_false(const Name& name, bool* target);
        std::shared_ptr<SubParserGroup> add_sub_parsers(const std::string& name = "commands");
        [[nodiscard]] ParseResult parse_args(Runner* runner);

        template<typename T>
        Argument& set_const(const Name& name, T* target, T t)
        {
            return add_void_function(name, [target, t]()
            {
                *target = t;
            });
        }

        template<typename T>
        Argument& add
        (
            const Name& name,
            T* target,
            ParseFunction<T> parse_function = default_parse_function<T>
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
                    return argparse::ok;
                }
                else
                {
                    const std::string base = fmt::format("'{}' is not accepted for '{}'", value, argument_name);
                    const auto parsed_error = parsed.get_error();
                    const std::string message = parsed_error.empty()
                        ? base
                        : fmt::format("{}, {}", base, parsed_error)
                        ;
                    print_parse_error(runner, caller, message);

                    return argparse::error;
                }
            }, [](){
                const std::optional<std::string> str = default_describe<T>();
                return str;
            });
            arg->default_value = from_default_value_to_string(*target);
            return add_argument(name, arg);
        }

        /// add greedy argument, currently also accepts zero
        // todo(Gustav): add option for non-greedy and error if empty
        template<typename T>
        Argument& add_vector
        (
            const Name& name,
            std::vector<T>* target,
            ParseFunction<T> parse_function = default_parse_function<T>
        )
        {
            auto arg = std::make_shared<MultiArgument>(
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
                    target->emplace_back(*parsed);
                    return argparse::ok;
                }
                else
                {
                    const std::string base = fmt::format("\'{}' is not accepted for '{}'", value, argument_name);
                    const auto parsed_error = parsed.get_error();
                    const std::string message = parsed_error.empty()
                        ? base
                        : fmt::format("{}, {}", base, parsed_error)
                        ;
                    print_parse_error(runner, caller, message);

                    return argparse::error;
                }
            }, [](){
                const std::optional<std::string> str = default_describe<T>();
                return str;
            });
            std::vector<std::string> values;
            for(const auto& t: *target)
            {
                values.emplace_back(from_default_value_to_string(t));
            }
            arg->default_value = string_mergers::array.merge(values);
            return add_argument(name, arg);
        }


        [[nodiscard]] std::shared_ptr<Argument> find_argument(const std::string& name) const;

        virtual std::string get_calling_name(const NameAndArguments& args) = 0;
        virtual ParserBase* get_parent_or_null() = 0;
    };

    /// subparser, don't create manually but add to a existing parser
    /// AddSubParsers()->Add(...);
    struct SubParser : public ParserBase
    {
        ParserBase* parent;
        argparse::Runner* runner;
        std::string calling_name;

        SubParser
        (
            const std::string& d,
            ParserBase* p,
            argparse::Runner* r,
            const std::string& cn
        );

        [[nodiscard]] ParseResult on_complete(CompleteFunction com);

        ParserBase* get_parent_or_null() override;
        std::string get_calling_name(const NameAndArguments& args) override;
    };

    /// root parser. start argumentparsing with this one
    struct Parser : public ParserBase
    {
        std::shared_ptr<argparse::Printer> printer;

        explicit Parser(const std::string& d = "");
        
        ParseResult parse(const NameAndArguments& args);

        /// value = return from main
        /// nullopt = continue, parsing was ok
        std::optional<int> parse(int argc, char* argv[]);

        ParserBase* get_parent_or_null() override;
        std::string get_calling_name(const NameAndArguments& args) override;
    };

    /// helper function for parsing directly from main
    int parse_from_main(Parser* parser, int argc, char* argv[]);
}
