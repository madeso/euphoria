#pragma once



#include <memory>
#include <map>

#include "core/noncopyable.h"


namespace euphoria::core
{
    struct Random;
}


namespace euphoria::core::tracery
{
    struct Result
    {
        enum Error
        {
            unable_to_open_file,
            no_error,
            json_parse,
            missing_rule,
            rule_eof,
            invalid_json,
            invalid_modifier,
            general_rule_parse_error
        };

        Error error_type;
        std::vector<std::string> text;

        Result(Error t);

        Result&
        operator<<(const std::string& t);

        operator bool() const;

        [[nodiscard]] std::string
        get_text() const;
    };

    std::ostream&
    operator<<(std::ostream& o, const Result& r);

    struct generator_argument;


    struct Node
    {
        Node() = default;
        virtual ~Node();

        NONCOPYABLE(Node);

        virtual Result
        flatten(generator_argument* generator) const = 0;
    };


    struct Modifier
    {
        Modifier() = default;
        virtual ~Modifier();

        NONCOPYABLE(Modifier);

        virtual Result
        apply_modifier(const std::string& input) = 0;
    };

    struct Rule
    {
        Rule();

        Result
        compile(const std::string& s);

        Result
        flatten(generator_argument* gen) const;

        void
        add(std::shared_ptr<Node> p);

        std::vector<std::shared_ptr<Node>> syntax;
    };

    struct Symbol
    {
        explicit Symbol(const std::string& k);

        std::string key;
        std::vector<Rule> ruleset;

        Result
        add_rule(const std::string& rule);

        Result
        flatten(generator_argument* gen) const;
    };

    struct Grammar
    {
        Grammar();

        void
        register_english();

        [[nodiscard]] Result
        load_from_string(const std::string& filename, const std::string& data);

        [[nodiscard]] Result
        load_from_gaf_string(const std::string& filename, const std::string& data);

        [[nodiscard]] Result
        get_string_from_symbol(const std::string& rule, generator_argument* generator) const;

        Grammar&
        register_modifier(const std::string& name, std::shared_ptr<Modifier> m);

        [[nodiscard]] Result
        apply_modifier(const std::string& name, const std::string& data) const;

        [[nodiscard]] Result
        flatten(core::Random* random, const std::string& rule) const;

        std::map<std::string, Symbol> rules;
        std::map<std::string, std::shared_ptr<Modifier>> modifiers;
    };

}
