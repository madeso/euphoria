#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>

#include "core/noncopyable.h"


namespace euphoria::core
{
    struct random;
}


namespace euphoria::core::tracery
{
    struct result
    {
        enum error
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

        error error_type;
        std::vector<std::string> text;

        result(error t);

        result&
        operator<<(const std::string& t);

        operator bool() const;

        [[nodiscard]] std::string
        get_text() const;
    };

    std::ostream&
    operator<<(std::ostream& o, const result& r);

    struct generator_argument;


    struct node
    {
        node() = default;
        virtual ~node();

        NONCOPYABLE(node);

        virtual result
        flatten(generator_argument* generator) const = 0;
    };


    struct modifier
    {
        modifier() = default;
        virtual ~modifier();

        NONCOPYABLE(modifier);

        virtual result
        apply_modifier(const std::string& input) = 0;
    };

    struct rule
    {
        rule();

        result
        compile(const std::string& s);

        result
        flatten(generator_argument* gen) const;

        void
        add(std::shared_ptr<node> p);

        std::vector<std::shared_ptr<node>> syntax;
    };

    struct symbol
    {
        explicit symbol(const std::string& k);

        std::string key;
        std::vector<rule> ruleset;

        result
        add_rule(const std::string& rule);

        result
        flatten(generator_argument* gen) const;
    };

    struct grammar
    {
        grammar();

        void
        register_english();

        result
        load_from_string(const std::string& data);

        result
        get_string_from_symbol(const std::string& rule, generator_argument* generator) const;

        grammar&
        register_modifier(const std::string& name, std::shared_ptr<modifier> m);

        result
        apply_modifier(const std::string& name, const std::string& data) const;

        result
        flatten(core::random* random, const std::string& rule) const;

        std::map<std::string, symbol> rules;
        std::map<std::string, std::shared_ptr<modifier>> modifiers;
    };

}
