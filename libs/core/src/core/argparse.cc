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
        is_optional_argument(const std::string& name)
        {
            return name[0] == '-';
        }

        bool
        is_valid_argument(const std::string& str)
        {
            if (str.empty())
            {
                return false;
            }

            constexpr auto valid_argument_name_characters = "abcdefghijklmnopqrstuvwxyz-_0123456789";
            const auto first_invalid_character = to_lower(str).find_first_not_of(valid_argument_name_characters);
            if (first_invalid_character != std::string::npos)
            {
                return false;
            }

            return true;
        }
    }


    file_output::file_output(const std::string& o) : file(o), single(!(ends_with(o, "/") || ends_with(o, "\\")))
    {}


    std::string
    file_output::get_next_file(bool print)
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
    file_output::create_dir_if_missing() const
    {
        if(single) { return; }
        // std::filesystem::create_directories(file);
    }


    std::ostream&
    operator<<(std::ostream& o, const parse_result& pr)
    {
        o << enum_to_string(pr.internal_type) << "(" << pr.return_value << ")";
        return o;
    }

    bool
    operator==(const parse_result& lhs, const parse_result& rhs)
    {
        return lhs.internal_type == rhs.internal_type && lhs.return_value == rhs.return_value;
    }


    bool
    operator!=(const parse_result& lhs, const parse_result& rhs)
    {
        return !(lhs == rhs);
    }


    name_and_arguments::name_and_arguments(const std::string& n, const std::vector<std::string>& a)
        : name(n)
        , arguments(a)
    {
    }


    name_and_arguments
    name_and_arguments::extract(int argc, char* argv[])
    {
        auto ret = name_and_arguments{argv[0] , {}};

        for (int i = 1; i < argc; i += 1)
        {
            ret.arguments.emplace_back(argv[i]);
        }

        return ret;
    }


    std::ostream&
    operator<<(std::ostream& o, const euphoria::core::argparse::name_and_arguments& args)
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


    argument_reader::argument_reader(const name_and_arguments& a)
        : arguments(a)
        , next_position(0)
    {
    }


    bool
    argument_reader::has_more() const
    {
        return next_position < Csizet_to_int(arguments.arguments.size());
    }


    std::string
    argument_reader::peek() const
    {
        if (has_more())
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
    argument_reader::read()
    {
        const auto arg = peek();
        next_position += 1;
        return arg;
    }


    void
    argument_reader::undo_read()
    {
        ASSERT(next_position > 0);
        next_position -= 1;
    }







    void
    console_printer::print_error(const std::string& line)
    {
        std::cerr << line << "\n";
    }


    void
    console_printer::print_info(const std::string& line)
    {
        std::cout << line << "\n";
    }


    void
    print_parse_error
    (
        runner* runner,
        parser_base* base,
        const std::string& error_message
    )
    {
        runner->printer->print_info
        (
            base->generate_usage_string
            (
                runner->arguments->arguments
            )
        );
        runner->printer->print_error
        (
            error_message
        );
    }


    name::name(const char* nn)
        : names(split(nn, ','))
    {
        for (auto& n : names)
        {
            n = trim(n);
        }

        ASSERTX(validate().empty(), validate());
    }


    std::string
    name::validate() const
    {
        if (names.empty()) { return "no names found"; }
        if (is_optional())
        {
            for (const auto& n : names)
            {
                if (is_optional_argument(n) == false)
                {
                    return "arguments are optional but '" + n + "' is not";
                }

                if (is_valid_argument(n) == false)
                {
                    return "optional arguments '" + n + "' is not considered valid";
                }
            }
        }
        else
        {
            const auto n = names[0];
            if (is_valid_argument(n) == false)
            {
                return "positional arguments '" + n + "' is not considered valid";
            }
        }

        return "";
    }


    bool
    name::is_optional() const
    {
        if (names.empty())
        {
            return false;
        }

        if (names.size() > 1)
        {
            return true;
        }

        return is_optional_argument(names[0]);
    }






    argument&
    argument::set_nargs(const std::string& na)
    {
        nargs = na;
        return *this;
    }


    argument&
    argument::set_help(const std::string& h)
    {
        help = h;
        return *this;
    }


    argument&
    argument::set_allow_before_positionals()
    {
        allow_before_positionals = true;
        return *this;
    }


    argument_and_name::argument_and_name(const argparse::name& n, std::shared_ptr<argparse::argument> a)
        : name(n)
        , argument(a)
    {
    }


    argument_no_value::argument_no_value(callback cb)
        : callback_function(cb)
    {
    }


    bool
    argument_no_value::have_nargs()
    {
        return false;
    }


    std::optional<std::string>
    argument_no_value::get_second_line()
    {
        return std::nullopt;
    }



    parse_result
    argument_no_value::parse_arguments
    (
        runner* runner,
        const std::string&,
        parser_base*
    )
    {
        return callback_function(runner);
    }


    multi_argument::multi_argument(callback cb, describe d)
        : callback_function(cb)
        , describe_function(d)
    {
    }


    bool
    multi_argument::have_nargs()
    {
        return true;
    }


    std::optional<std::string>
    multi_argument::get_second_line()
    {
        return describe_function();
    }


    parse_result
    multi_argument::parse_arguments
    (
        runner* runner,
        const std::string& name,
        parser_base* caller
    )
    {
        // eat all arguments!
        while (runner->arguments->has_more())
        {
            auto res = callback_function
            (
                runner,
                name,
                caller,
                runner->arguments->read()
            );
            if(res != argparse::ok)
            {
                return res;
            }
        }
        return argparse::ok;
    }


    single_argument::single_argument(callback cb, describe d)
        : callback_function(cb)
        , describe_function(d)
    {
    }


    bool
    single_argument::have_nargs()
    {
        return true;
    }


    std::optional<std::string>
    single_argument::get_second_line()
    {
        return describe_function();
    }


    parse_result
    single_argument::parse_arguments
    (
        runner* runner,
        const std::string& name,
        parser_base* caller
    )
    {
        if (runner->arguments->has_more())
        {
            auto res = callback_function
            (
                runner,
                name,
                caller,
                runner->arguments->read()
            );
            return res;
        }
        else
        {
            print_parse_error
            (
                runner,
                caller,
                string_builder() << "missing value for '" << name << '\''
            );
            return argparse::error;
        }
    }


    sub_parser_names::sub_parser_names(const char* str)
    {
        names.emplace_back(str);
    }


    sub_parser_container::sub_parser_container
    (
        const sub_parser_names& n,
        const std::string& h,
        sub_parser_callback cb
    )
        : names(n)
        , help(h)
        , callback(cb)
    {
    }


    sub_parser_group::sub_parser_group
    (
        const std::string& t,
        parser_base* o
    )
        : title(t)
        , owner(o)
    {
    }


    void
    sub_parser_group::add
    (
        const sub_parser_names& names,
        const std::string& desc,
        sub_parser_callback sub
    )
    {
        auto container = std::make_shared<sub_parser_container>(names, desc, sub);
        parsers.emplace_back(container);
        for(const auto& name : names.names)
        {
            owner->subparsers.add(name, container);
        }
    }


    void
    sub_parser_group::add
    (
        const sub_parser_names& names,
        sub_parser_callback sub
    )
    {
        add(names, "", sub);
    }


    parser_base::parser_base(const std::string& d)
        : description(d)
    {
        add_argument
        (
            "-h, --help",
            std::make_shared<argument_no_value>
            (
                [this](runner* runner)
                {
                    print_help(runner->printer, runner->arguments->arguments);
                    return argparse::quit;
                }
            )
        )
        .set_allow_before_positionals()
        .set_help("show this help message and exit")
        ;
    }






    std::string
    parser_base::generate_usage_string(const name_and_arguments& args)
    {
        auto arg_to_string = [](const argument_and_name& aa)
        {
            if( aa.name.is_optional() )
            {
                std::ostringstream ss;
                ss << "[" << aa.name.names[0];
                if(aa.argument->have_nargs())
                {
                    ss << " " << aa.argument->nargs;
                }
                ss << "]";
                // todo(Gustav): include any attributes (vector input)
                return ss.str();
            }
            else
            {
                return to_upper(aa.name.names[0]);
            }
        };
        auto ret = std::vector<std::string>{"usage:", get_calling_name(args)};

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

        return string_mergers::space.merge(ret);
    }

    namespace
    {
        // msvc and clang disagrees on if capturing
        // constexpr in lambdas is necessary or not
        constexpr int global_max_name_length = 20;
        constexpr int global_max_help_length = 80;
    }

    void
    parser_base::print_help(std::shared_ptr<printer> printer, const name_and_arguments& args)
    {
        using StringTable = table<std::string>;

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
            table->new_row(values);
            if(second_line)
            {
                table->new_row({"", *second_line});
            }

            max_name_length = std::min
            (
                std::max
                (
                    max_name_length,
                    Csizet_to_int(name.length())
                ),
                global_max_name_length
            );
        };

        const auto print = [printer, &max_name_length](const StringTable& table)
        {
            auto t = StringTable{};
            for(int y=0; y<table.get_height(); y+=1)
            {
                const auto names = word_wrap
                (
                    table(0, y),
                    [max_name_length](const std::string& s)
                    {
                        return Csizet_to_int(s.size()) <= max_name_length;
                    }
                );
                const auto helps = word_wrap
                (
                    table(1, y),
                    [](const std::string& s)
                    {
                        return s.size() <= global_max_help_length;
                    }
                );
                const auto rows = zip_longest(names, helps);
                for(auto [name,help]: rows)
                {
                    t.new_row({name, help});
                }
            }
            for(int y=0; y<t.get_height(); y+=1)
            {
                constexpr auto indent = "  ";
                constexpr auto space = "  ";

                const auto name = t(0, y);
                const auto help = t(1, y);

                const auto name_length = Csizet_to_int(name.length());
                if(name_length > max_name_length)
                {
                    printer->print_info(indent + name);
                    const auto extra_space = std::string(max_name_length, ' ');
                    printer->print_info(indent + extra_space + space + help);
                }
                else
                {
                    const auto extra_space = std::string(max_name_length - name_length, ' ');
                    printer->print_info(indent + name + extra_space + space + help);
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
                a.argument->get_second_line()
            );
        }

        auto optionals = StringTable{};
        for (auto& a : optional_argument_list)
        {
            const auto names = string_mergers::comma.merge(a.name.names);
            const auto names_with_narg = a.argument->have_nargs() == false
                ? names
                : string_builder() << names << " " << a.argument->nargs
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
                a.argument->get_second_line()
            );
        }

        auto subs = std::vector<StringTable>{};
        for(const auto& group: subparser_groups)
        {
            auto sub = StringTable{};
            for(const auto& parser: group->parsers)
            {
                const auto names = string_mergers::comma.merge
                (
                    parser->names.names
                );
                add(&sub, names, parser->help, std::nullopt);
            }
            subs.emplace_back(sub);
        }

        // print all tables now
        printer->print_info(generate_usage_string(args));
        if (description.empty() == false)
        {
            printer->print_info("");
            printer->print_info(description);
        }

        if (positionals.get_height() != 0)
        {
            printer->print_info("");
            printer->print_info("positional arguments:");
            print(positionals);
        }

        if(optionals.get_height() != 0)
        {
            printer->print_info("");
            printer->print_info("optional arguments:");
            print(optionals);
        }

        if(!subs.empty())
        {
            bool is_first_group = true;
            printer->print_info("");
            auto sub = subs.begin();
            for(const auto& group: subparser_groups)
            {
                if(is_first_group == false)
                {
                    printer->print_info("");
                }

                printer->print_info(group->title + ":");

                print(*sub);
                sub +=1 ;

                is_first_group = false;
            }
        }
    }


    std::string
    create_default_nargs(const name& name)
    {
        const auto n = trim_left(name.names[0], "-");
        return to_upper(n);
    }


    argument&
    parser_base::add_argument(const name& name, std::shared_ptr<argument> argument)
    {
        ASSERT
        (
                parser_state == state::adding &&
            "It looks like you are adding argument during parsing... "
            "are you using the wrong parser in a OnComplete?"
        );
        argument->nargs = create_default_nargs(name);
        if (name.is_optional())
        {
            for (const auto& n : name.names)
            {
                optional_arguments[n] = argument;
            }
            optional_argument_list.emplace_back(name, argument);
        }
        else
        {
            ASSERT(argument->have_nargs());
            positional_argument_list.emplace_back(name, argument);
        }
        return *argument;
    }


    argument&
    parser_base::add_void_function(const name& name, std::function<void()> void_function)
    {
        return add_argument(name, std::make_shared<argument_no_value>([void_function](runner*) {void_function(); return argparse::ok; }));
    }


    argument&
    parser_base::set_true(const name& name, bool* target)
    {
        *target = false;
        return set_const(name, target, true);
    }


    argument&
    parser_base::set_false(const name& name, bool* target)
    {
        *target = true;
        return set_const(name, target, false);
    }


    void
    parser_base::on_complete(complete_function cf)
    {
        on_complete_function = cf;
    }


    std::shared_ptr<sub_parser_group>
    parser_base::add_sub_parsers(const std::string& name)
    {
        ASSERT(parser_state == state::adding);
        auto group = std::make_shared<sub_parser_group>(name, this);
        subparser_groups.emplace_back(group);
        return group;
    }


    std::shared_ptr<argument>
    parser_base::find_argument(const std::string& name) const
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
        struct argument_parser
        {
            parser_base* base = nullptr;
            argparse::runner* runner = nullptr;
            int positional_index = 0;
            bool found_subparser = false;

            [[nodiscard]] bool
            has_more_positionals() const;

            [[nodiscard]] std::vector<std::string>
            get_positionals_left() const;

            void
            print_error(const std::string& error_message) const;

            [[nodiscard]] std::optional<parse_result>
            try_parse_important_optional() const;

            std::optional<parse_result>
            parse_one_positional();

            std::optional<parse_result>
            parse_sub_command(const std::string& arg);

            std::optional<parse_result>
            parse_one_optional();

            std::optional<parse_result>
            parse_one_arg();
        };


        bool
        argument_parser::has_more_positionals() const
        {
            const auto positionals_size = Csizet_to_int
            (
                base->positional_argument_list.size()
            );
            return positional_index < positionals_size;
        }


        std::vector<std::string>
        argument_parser::get_positionals_left() const
        {
            if(has_more_positionals() == false) { return {}; }

            std::vector<std::string> positionals;

            const auto& list = base->positional_argument_list;
            for
            (
                auto pos = list.begin()+positional_index;
                pos != list.end();
                pos += 1
            )
            {
                positionals.emplace_back(to_upper(pos->name.names[0]));
            }

            return positionals;
        }


        void
        argument_parser::print_error(const std::string& error_message) const
        {
            print_parse_error(runner, base, error_message);
        }


        std::optional<parse_result>
        argument_parser::try_parse_important_optional() const
        {
            // first, peek at the next commandline argument
            // and check for optionals that are allowed before positionals
            const auto arg = base->find_argument(runner->arguments->peek());
            if(arg != nullptr && arg->allow_before_positionals)
            {
                // we have matched a optional, read and parse it!
                const auto matched_name = runner->arguments->read();
                auto arg_parse_result = arg->parse_arguments
                (
                    runner,
                    matched_name,
                    base
                );
                return arg_parse_result;
            }
            else
            {
                // the peeked argument isn't a important optional
                // handle positional
                return std::nullopt;
            }
        }

        std::optional<parse_result>
        argument_parser::parse_one_positional()
        {
            auto match = base->positional_argument_list[positional_index];
            auto arg_parse_result = match.argument->parse_arguments
            (
                runner,
                match.name.names[0],
                base
            );
            if (arg_parse_result != argparse::ok)
            {
                return arg_parse_result;
            }
            positional_index += 1;

            return std::nullopt;
        }

        sub_parser_style
        get_parser_style(parser_base* base, bool from_self=true)
        {
            if(base == nullptr)
            {
                return sub_parser_style::greedy;
            }

            parser_base* parent = base->get_parent_or_null();
            if(from_self && parent==nullptr)
            {
                return sub_parser_style::greedy;
            }

            if(base->parser_style == sub_parser_style::inherit)
            {
                return get_parser_style(parent, false);
            }
            else
            {
                return base->parser_style;
            }
        }


        std::optional<parse_result>
        argument_parser::parse_sub_command(const std::string& arg)
        {
            auto match = base->subparsers.match(arg, 3);
            if(match.single_match == false)
            {
                // todo(Gustav): check if this accepts invalid and
                // calls on_complete() on invalid input
                if(get_parser_style(base) == sub_parser_style::greedy)
                {
                    if(match.names.empty())
                    {
                        print_error
                        (
                            string_builder() << '\'' << arg << "' was unexpected"
                        );
                        return argparse::error;
                    }

                    const std::string invalid_command = string_builder()
                        << "Invalid command '" << arg << "'";
                    const auto names = quote_and_combine_english_or(match.names);

                    // todo(Gustav): switch between 'did you mean' and
                    // 'could be either' depending on how big the
                    // edit distance is?
                    print_error
                    (
                        string_builder() << invalid_command <<
                        ", did you mean " << names << '?'
                    );
                    return argparse::error;
                }
                else
                {
                    // go back one step
                    runner->arguments->undo_read();

                    if(base->on_complete_function.has_value())
                    {
                        return base->on_complete_function.value()();
                    }

                    return argparse::ok;
                }
            }
            found_subparser = true;

            auto container = match.values[0];
            const auto calling_name = base->get_calling_name
            (
                runner->arguments->arguments
            );
            auto sub = sub_parser
            {
                container->help,
                base,
                runner,
                calling_name + " " + arg
            };
            const auto subresult = container->callback(&sub);
            if
            (
                subresult == argparse::ok &&
                        get_parser_style(base) == sub_parser_style::greedy
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

        std::optional<parse_result>
        argument_parser::parse_one_optional()
        {
            const auto arg = runner->arguments->read();
            if(is_optional_argument(arg))
            {
                auto match = base->find_argument(arg);
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
                                return edit_distance(arg, lhs.first) <
                                       edit_distance(arg, rhs.first) ;
                            }
                        );

                    // todo(Gustav): if we have already detected it,
                    // change the text to 'invalid argument' but in this case
                    // it's actually a unknown argument, not a 'invalid' one
                    if(closest_match == base->optional_arguments.end())
                    {
                        print_error
                        (
                            string_builder() << "unknown argument: '" << arg << '\''
                        );
                    }
                    else
                    {
                        const auto closest_match_name = closest_match->first;
                        print_error
                        (
                            string_builder() << "unknown argument: '" << arg <<
                            "', did you mean '" << closest_match_name << "'?"
                        );
                    }

                    return argparse::error;
                }
                auto arg_parse_result = match->parse_arguments
                (
                    runner,
                    arg,
                    base
                );
                if (arg_parse_result != argparse::ok)
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
                return parse_sub_command(arg);
            }
        }


        std::optional<parse_result>
        argument_parser::parse_one_arg()
        {
            if (has_more_positionals())
            {
                // null from tryparse = the parsed optional wasnt important
                // null from this = abort this parsing and return to parent
                auto opt = try_parse_important_optional();
                if(opt.has_value())
                {
                    if(*opt == argparse::ok)
                    {
                        // ok... continue parsing
                        return std::nullopt;
                    }
                    else
                    {
                        // error... abort!
                        return opt;
                    }
                }
                else
                {
                    // not important optional... parse a positional
                    return parse_one_positional();
                }
            }
            else
            {
                // no more positionals... then it can ob ly be optionals
                return parse_one_optional();
            }
        }
    }

    parse_result
    parser_base::parse_args(runner* runner)
    {
        parser_state = state::parsing;
        auto parser = argument_parser{this, runner};

        while (runner->arguments->has_more())
        {
            auto r = parser.parse_one_arg();
            if(r)
            {
                return *r;
            }
        }

        if (parser.has_more_positionals())
        {
            const auto missing = parser.get_positionals_left();
            const auto text = string_mergers::english_and.merge(missing);
            parser.print_error
            (
                missing.size() == 1 ?
                (
                    string_builder() << "Positional " << text << " was not specified."
                )
                :
                (
                    string_builder() << "Positionals " << text << " were not specified."
                )
            );
            return argparse::error;
        }

        // not sure if should keep or not
        // should handle required subparser, but doesn't
        if(subparsers.size != 0 && parser.found_subparser == false)
        {
            parser.print_error("no subparser specified");
            return argparse::error;
        }

        if(on_complete_function.has_value())
        {
            return on_complete_function.value()();
        }

        return argparse::ok;
    }


    sub_parser::sub_parser
    (
        const std::string& d,
        parser_base* p,
        argparse::runner* r,
        const std::string& cn
    )
        : parser_base(d)
        , parent(p)
        , runner(r)
        , calling_name(cn)
    {
    }


    parser_base*
    sub_parser::get_parent_or_null()
    {
        return parent;
    }


    std::string
    sub_parser::get_calling_name(const name_and_arguments&)
    {
        return calling_name;
    }


    parse_result
    sub_parser::on_complete(complete_function com)
    {
        parser_base::on_complete(com);
        return parse_args(runner);
    }


    parser::parser(const std::string& d)
        : parser_base(d)
        , printer(std::make_shared<console_printer>())
    {
    }


    parser_base*
    parser::get_parent_or_null()
    {
        return nullptr;
    }


    parse_result
    parser::parse(const name_and_arguments& args)
    {
        auto reader = argument_reader{ args };
        auto runner = argparse::runner{ &reader, printer };
        return parser_base::parse_args(&runner);
    }


    std::optional<int>
    parser::parse(int argc, char* argv[])
    {
        const auto args = name_and_arguments::extract(argc, argv);
        const auto res = parse(args);
        if (res == argparse::ok)
        {
            return std::nullopt;
        }
        else
        {
            return res.return_value;
        }
    }


    std::string
    parser::get_calling_name(const name_and_arguments& args)
    {
        return args.name;
    }


    int
    parse_from_main(parser* parser, int argc, char* argv[])
    {
        const auto args = name_and_arguments::extract(argc, argv);
        const auto res = parser->parse(args);
        return res.return_value;
    }
}
