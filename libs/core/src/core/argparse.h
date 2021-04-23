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

#include "core/default_parse.h"
#include "core/stringmerger.h"
#include "core/result.h"


namespace euphoria::core::argparse
{
    struct parser_base;

    // enum to catch adding arguments during parsing in a callback
    enum class ParserState
    {
        adding, parsing
    };

    // option where the output can either be a single file or "infinite"
    struct file_output
    {
        explicit file_output(const std::string& o);

        std::string
        get_next_file(bool print = true);

        void
        create_dir_if_missing() const;

        std::string file;
        bool        single;
        int         index = 0;
    };


    struct parse_result
    {
        enum class type { error, ok, quit, custom };

        type internal_type;
        int return_value;

        static constexpr int
        default_return_value(type pr)
        {
            switch (pr)
            {
            case type::ok: return 0;
            case type::error: return -1;
            case type::quit: return 0;
            default: return -1;
            }
        }

        constexpr explicit parse_result(type t)
            : internal_type(t)
            , return_value(default_return_value(t))
        {
        }

        constexpr explicit parse_result(int rv)
            : internal_type(parse_result::type::custom)
            , return_value(rv)
        {
        }
    };

    // no error occurred
    constexpr parse_result error = parse_result{ parse_result::type::error };

    // all ok
    constexpr parse_result ok = parse_result{ parse_result::type::ok };

    // all ok, but quit requested
    constexpr parse_result quit = parse_result{ parse_result::type::quit };

    std::ostream&
    operator<<(std::ostream& o, const parse_result& pr);

    bool
    operator==(const parse_result& lhs, const parse_result& rhs);


    bool
    operator!=(const parse_result& lhs, const parse_result& rhs);


    // container for arguments passed to main
    struct name_and_arguments
    {
        std::string name;
        std::vector<std::string> arguments;

        name_and_arguments(const std::string& n, const std::vector<std::string>& a);

        static
        name_and_arguments
        extract(int argc, char* argv[]);
    };

    std::ostream&
    operator<<(std::ostream& stream, const name_and_arguments& arguments);


    // "file" like api for reading arguments
    struct argument_reader
    {
        name_and_arguments arguments;
        int next_position;

        explicit
        argument_reader(const name_and_arguments& a);

        [[nodiscard]] bool
        has_more() const;

        [[nodiscard]] [[nodiscard]] std::string
        peek() const;

        [[nodiscard]] std::string
        read();

        void
        undo_read();
    };

    // generic output class
    struct printer
    {
        printer() = default;
        virtual ~printer() = default;

        printer(const printer&) = delete;
        printer(printer&&) = delete;
        void operator=(const printer&) = delete;
        void operator=(printer&&) = delete;

        virtual void
        print_error(const std::string& line) = 0;

        virtual void
        print_info(const std::string& line) = 0;
    };

    // console output
    struct console_printer : public printer
    {
        void
        print_error(const std::string& line) override;

        void
        print_info(const std::string& line) override;
    };

    // shared data between parsing functions
    struct runner
    {
        argument_reader* arguments;
        std::shared_ptr<argparse::printer> printer;
    };

    void
    print_parse_error
    (
        runner* runner,
        parser_base* base,
        const std::string& error_message
    );


    // represents a command line argument
    // optional: start with either - or --
    //    can have many names
    // positional: doesn't start with -
    //    can only have a single name
    struct name
    {
        std::vector<std::string> names;

        name(const char* n);

        // return empty string if valid, return error message if not
        [[nodiscard]]
        std::string
        validate() const;

        [[nodiscard]] bool
        is_optional() const;
    };


    // base class for argument
    struct argument
    {
        argument() = default;
        virtual ~argument() = default;

        argument(const argument&) = delete;
        argument(argument&&) = delete;
        void operator=(const argument&) = delete;
        void operator=(argument&&) = delete;

        std::string help;
        std::string nargs;
        bool allow_before_positionals = false;

        // automatically filled
        std::string default_value;

        argument&
        set_nargs(const std::string& na);

        argument&
        set_help(const std::string& h);

        argument&
        set_allow_before_positionals();

        virtual
        std::optional<std::string>
        get_second_line() = 0;

        virtual
        bool
        have_nargs() = 0;

        virtual
        parse_result
        parse_arguments
        (
            runner* runner,
            const std::string& name,
            parser_base* caller
        ) = 0;
    };

    // named tuple class for argument and name
    struct argument_and_name
    {
        argparse::name name;
        std::shared_ptr<argparse::argument> argument;

        argument_and_name(const argparse::name& n, std::shared_ptr<argparse::argument> a);
    };

    // a argument with no value, probably either a --set-true
    // or a void function like --help
    struct argument_no_value : public argument
    {
        using callback = std::function<parse_result (runner*)>;
        callback callback_function;

        explicit argument_no_value(callback cb);

        bool
        have_nargs() override;

        std::optional<std::string>
        get_second_line() override;

        parse_result
        parse_arguments
        (
            runner* runner,
            const std::string& name,
            parser_base* caller
        ) override;
    };

    // a single argument, probably either a --count 3 or positional input
    struct single_argument : public argument
    {
        using callback = std::function
        <
            parse_result
            (
                runner* runner,
                const std::string& name,
                parser_base* caller,
                const std::string& value
            )
        >;
        using describe = std::function<std::optional<std::string> ()>;

        callback callback_function;
        describe describe_function;

        single_argument(callback cb, describe d);

        bool
        have_nargs() override;

        std::optional<std::string>
        get_second_line() override;

        parse_result
        parse_arguments
        (
            runner* runner,
            const std::string& name,
            parser_base* caller
        ) override;
    };

    // a single argument, probably either a --count 3 or positional input
    struct multi_argument : public argument
    {
        using callback = std::function
        <
            parse_result
            (
                runner* runner,
                const std::string& name,
                parser_base* caller,
                const std::string& value
            )
        >;
        using describe = std::function<std::optional<std::string> ()>;
        callback callback_function;
        describe describe_function;

        multi_argument(callback cb, describe d);

        bool
        have_nargs() override;

        std::optional<std::string>
        get_second_line() override;

        parse_result
        parse_arguments
        (
            runner* runner,
            const std::string& name,
            parser_base* caller
        ) override;
    };

    // generic parse function
    template<typename T>
    using parse_function = std::function
    <
        Result<T>
        (
            const std::string& value
        )
    >;
    struct sub_parser;

    using sub_parser_callback = std::function
    <
        parse_result (sub_parser*)
    >;

    using complete_function = std::function
    <
        parse_result ()
    >;

    // valid names for a single subparser
    // example: [checkout, co] or [pull]
    struct sub_parser_names
    {
        std::vector<std::string> names;

        sub_parser_names(const char* str);
    };

    // data about a subparser
    struct sub_parser_container
    {
        sub_parser_names names;
        std::string help;
        sub_parser_callback callback;

        sub_parser_container
        (
            const sub_parser_names& n,
            const std::string& h,
            sub_parser_callback cb
        );
    };

    // subparsers can be grouped and this structs represents that group
    struct sub_parser_group
    {
        std::string title;
        parser_base* owner;
        std::vector<std::shared_ptr<sub_parser_container>> parsers;

        sub_parser_group
        (
            const std::string& t,
            parser_base* o
        );

        void
        add
        (
            const sub_parser_names& names,
            const std::string& desc,
            sub_parser_callback sub
        );

        void
        add
        (
            const sub_parser_names& names,
            sub_parser_callback sub
        );
    };

    // how the subparsing is handled, non-greedy are
    // useful for 'scripting' with subparsers
    enum class sub_parser_style
    {
        // parse all arguments
        greedy,

        // if argument is invalid, go back one step and try there
        fallback,

        // inherit from parent
        inherit
    };

    // base for the parser, start with Parser and add one or more subparsers
    struct parser_base
    {
        std::string description;

        std::vector<argument_and_name> positional_argument_list;

        std::map<std::string, std::shared_ptr<argument>> optional_arguments;
        std::vector<argument_and_name> optional_argument_list;

        EnumToStringImpl<std::shared_ptr<sub_parser_container>> subparsers;
        std::vector<std::shared_ptr<sub_parser_group>> subparser_groups;

        std::optional<complete_function> on_complete_function;
        ParserState parser_state = ParserState::adding;
        sub_parser_style parser_style = sub_parser_style::inherit;

        explicit parser_base(const std::string& d);
        virtual ~parser_base() = default;

        parser_base(const parser_base&) = delete;
        parser_base(parser_base&&) = delete;
        void operator=(const parser_base&) = delete;
        void operator=(parser_base&&) = delete;

        virtual
        parser_base*
        get_parent_or_null() = 0;

        std::string
        generate_usage_string(const name_and_arguments& args);

        virtual
        std::string
        get_calling_name(const name_and_arguments& args) = 0;

        void
        print_help(std::shared_ptr<printer> printer, const name_and_arguments& args);

        argument&
        add_argument(const name& name, std::shared_ptr<argument> argument);

        argument&
        add_void_function(const name& name, std::function<void()> void_function);

        void
        on_complete(complete_function com);

        template<typename T>
        argument&
        set_const(const name& name, T* target, T t)
        {
            return add_void_function(name, [target, t]()
            {
                *target = t;
            });
        }

        argument&
        set_true(const name& name, bool* target);

        argument&
        set_false(const name& name, bool* target);

        template<typename T>
        argument&
        add
        (
            const name& name,
            T* target,
            parse_function<T> parse_function = default_parse_function<T>
        )
        {
            auto arg = std::make_shared<single_argument>(
            [target, parse_function]
            (
                runner* runner,
                const std::string& argument_name,
                parser_base* caller,
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
                    const std::string base = Str()
                        << '\'' << value << "' is not accepted for '"
                        << argument_name << '\'';
                    const auto error = parsed.Error();
                    const std::string message = error.empty()
                        ? base
                        : (Str() << base << ", " << error)
                        ;
                    print_parse_error(runner, caller, message);

                    return argparse::error;
                }
            }, [](){
                const std::optional<std::string> str = default_describe<T>();
                return str;
            });
            arg->default_value = default_value_to_string(*target);
            return add_argument(name, arg);
        }

        // add greedy argument, currently also accepts zero
        // todo(Gustav): add option for non-greedy and error if empty
        template<typename T>
        argument&
        add_vector
        (
            const name& name,
            std::vector<T>* target,
            parse_function<T> parse_function = default_parse_function<T>
        )
        {
            auto arg = std::make_shared<multi_argument>(
            [target, parse_function]
            (
                runner* runner,
                const std::string& argument_name,
                parser_base* caller,
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
                    const std::string base = Str()
                        << '\'' << value << "' is not accepted for '"
                        << argument_name << '\'';
                    const auto error = parsed.Error();
                    const std::string message = error.empty()
                        ? base
                        : (Str() << base << ", " << error)
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
                values.emplace_back(default_value_to_string(t));
            }
            arg->default_value = StringMerger::Array().Generate(values);
            return add_argument(name, arg);
        }

        std::shared_ptr<sub_parser_group>
        add_sub_parsers(const std::string& name="commands");

        [[nodiscard]] std::shared_ptr<argument>
        find_argument(const std::string& name) const;

        [[nodiscard]]
        parse_result
        parse_args(runner* runner);
    };

    // subparser, don't create manually but add to a existing parser
    // AddSubParsers()->Add(...);
    struct sub_parser : public parser_base
    {
        parser_base* parent;
        argparse::runner* runner;
        std::string calling_name;

        sub_parser
        (
            const std::string& d,
            parser_base* p,
            argparse::runner* r,
            const std::string& cn
        );

        parser_base*
        get_parent_or_null() override;

        std::string
        get_calling_name(const name_and_arguments& args) override;

        [[nodiscard]]
        parse_result
        on_complete(complete_function com);
    };

    // root parser. start argumentparsing with this one
    struct parser : public parser_base
    {
        explicit parser(const std::string& d = "");

        parser_base*
        get_parent_or_null() override;

        parse_result
        parse(const name_and_arguments& args);

        // value = return from main
        // nullopt = continue, parsing was ok
        std::optional<int>
        parse(int argc, char* argv[]);

        std::string
        get_calling_name(const name_and_arguments& args) override;

        std::shared_ptr<argparse::printer> printer;
    };

    // helper function for parsing directly from main
    int
    parse_from_main(parser* parser, int argc, char* argv[]);
}

#endif  // CORE_ARGPARSE_H
