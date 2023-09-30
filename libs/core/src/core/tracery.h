#pragma once



#include <memory>
#include <map>


namespace eu
{
    struct Random;
}


namespace eu::core::tracery
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

        [[nodiscard]] std::string get_text() const;

        // todo(Gustav): replace with chaining member function
        Result& operator<<(const std::string& t);
        operator bool() const;
    };


    std::string to_string(const Result& r);


    struct GeneratorArgument;


    struct Node
    {
        Node() = default;
        virtual ~Node();

        Node(const Node&) = delete;
        Node(Node&&) = delete;
        void operator=(const Node&) = delete;
        void operator=(Node&&) = delete;

        virtual Result flatten(GeneratorArgument* generator) const = 0;
    };


    struct Modifier
    {
        Modifier() = default;
        virtual ~Modifier();

        Modifier(const Modifier&) = delete;
        Modifier(Modifier&&) = delete;
        void operator=(const Modifier&) = delete;
        void operator=(Modifier&&) = delete;

        virtual Result apply_modifier(const std::string& input) = 0;
    };


    struct Rule
    {
        std::vector<std::shared_ptr<Node>> syntax;

        Rule();

        Result compile(const std::string& s);
        void add(std::shared_ptr<Node> p);

        Result flatten(GeneratorArgument* gen) const;
    };


    struct Symbol
    {
        std::string key;
        std::vector<Rule> ruleset;

        explicit Symbol(const std::string& k);

        Result add_rule(const std::string& rule);
        Result flatten(GeneratorArgument* gen) const;
    };


    struct Grammar
    {
        std::map<std::string, Symbol> rules;
        std::map<std::string, std::shared_ptr<Modifier>> modifiers;


        Grammar();

        void register_english();

        [[nodiscard]] Result load_from_string(const std::string& filename, const std::string& data);

        Grammar& register_modifier(const std::string& name, std::shared_ptr<Modifier> m);

        [[nodiscard]] Result get_string_from_symbol(const std::string& rule, GeneratorArgument* generator) const;
        [[nodiscard]] Result apply_modifier(const std::string& name, const std::string& data) const;
        [[nodiscard]] Result flatten(Random* random, const std::string& rule) const;
    };

}

ADD_DEFAULT_FORMATTER(eu::core::tracery::Result, std::string, eu::core::tracery::to_string);
