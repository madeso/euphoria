// ----------------------------------------------------------------
// Includes

#include "core/tracery.h"

#include <sstream>

#include "core/random.h"

#include "assert/assert.h"
#include "core/textfileparser.h"
#include "core/str.h"
#include "core/proto.h"
#include "core/stringutils.h"

#include "gaf_tracery.h"
#include "gaf_pugixml_tracery.h"


namespace euphoria::core::tracery
{
    // ----------------------------------------------------------------
    // Common (to be removed)

    template <typename Out>
    void
    split(const std::string& s, char delim, Out result)
    {
        std::stringstream ss;
        ss.str(s);
        std::string item;
        while(std::getline(ss, item, delim))
        {
            *(result++) = item;
        }
    }

    std::vector<std::string>
    split(const std::string& s, char delim)
    {
        std::vector<std::string> elems;
        split(s, delim, std::back_inserter(elems));
        return elems;
    }


    // ----------------------------------------------------------------
    // Private

    struct generator_argument
    {
        core::Random* generator = nullptr;
        const tracery::Grammar* grammar = nullptr;
        std::map<std::string, std::string> overridden_rules;
    };

    Result
    parse_json(Symbol* rule, const ::tracery::Rule& value)
    {
        for(const auto& v: value.text)
        {
            Result r = rule->add_rule(v);
            if(r == false)
            {
                return r;
            }
        }

        return Result::no_error;
    }


    Result
    from_json(Symbol* rule, const ::tracery::Rule& value)
    {
        auto r = parse_json(rule, value);
        if(r == false)
        {
            // todo(Gustav): add json error information
            r << "for symbol " << rule->key;
        }

        return r;
    }

    // ----------------------------------------------------------------

    Node::~Node() = default;

    // ----------------------------------------------------------------

    struct LiteralStringNode : public Node
    {
        LiteralStringNode(const std::string& t) : text(t) {}
        std::string text;

        Result
        flatten(generator_argument*) const override
        {
            return Result(Result::no_error) << text;
        }
    };

    // ----------------------------------------------------------------

    struct CallSymbolNode : public Node
    {
        CallSymbolNode() = default;

        struct ActionRule
        {
            std::string key;
            std::string symbol;
        };

        std::string symbol;
        std::vector<std::string> modifiers;
        std::vector<ActionRule> action_rules;

        void
        add_action_rule
        (
            const std::string& action_key,
            const std::string& action_symbol
        )
        {
            action_rules.push_back(ActionRule {action_key, action_symbol});
        }

        Result
        flatten(generator_argument* generator) const override
        {
            generator_argument arg = *generator;

            for(const auto& r: action_rules)
            {
                const auto result = arg.grammar->get_string_from_symbol(r.symbol, &arg);
                if(result == false)
                {
                    return result;
                }
                arg.overridden_rules[r.key] = result.get_text();
            }

            auto symbol_result = arg.grammar->get_string_from_symbol(symbol, &arg);
            if(symbol_result == false)
            {
                return symbol_result;
            }

            auto ret = symbol_result.get_text();

            for(const auto& f: modifiers)
            {
                auto r = generator->grammar->apply_modifier(f, ret);
                if(r == false)
                {
                    return r;
                }
                ret = r.get_text();
            }

            return Result{ Result::no_error } << ret;
        }
    };


    // ----------------------------------------------------------------
    // Result

    Result::Result(Error t) : error_type(t) {}

    Result&
    Result::operator<<(const std::string& t)
    {
        text.push_back(t);
        return *this;
    }

    Result::operator bool() const
    {
        return error_type == no_error;
    }

    std::string
    Result::get_text() const
    {
        std::ostringstream ss;
        for(const auto& s: text)
        {
            ss << s;
        }
        return ss.str();
    }

    std::ostream&
    operator<<(std::ostream& o, const Result& r)
    {
        switch(r.error_type)
        {
        case Result::no_error: o << "No error detected"; break;
        case Result::unable_to_open_file:
            o << "Unable to open file: " << r.get_text();
            break;
        case Result::json_parse:
            o << "JSON parse error: " << r.get_text();
            break;
        case Result::missing_rule:
            o << "Rule not found in grammar: " << r.get_text();
            break;
        case Result::rule_eof: o << "EOF in rule: " << r.get_text(); break;
        case Result::invalid_json: o << "Invalid json state."; break;
        case Result::invalid_modifier:
            o << "Invalid modifier: " << r.get_text();
            break;
        case Result::general_rule_parse_error:
            o << "Rule parse error: " << r.get_text();
            break;
        default: o << "Unhandled error"; break;
        }

        return o;
    }

    // ----------------------------------------------------------------

    Rule::Rule() = default;

    Result
    parse_error(TextfileParser* parser)
    {
        return Result(Result::general_rule_parse_error)
               << parser->peek_string() << " detected but ";
    }

    std::string
    read_tracery_ident(TextfileParser* parser)
    {
        const std::string valid
                = "abcdefghijklmnopqrstuvwxyz"
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                  "0123456789"
                  "_-+";

        std::ostringstream ss;
        while(valid.find(parser->peek_char()) != std::string::npos)
        {
            ss << parser->read_char();
        }
        return ss.str();
    }

    Result
    Rule::compile(const std::string& s)
    {
#define EMPTY_STRING(str, err) \
    do \
    { \
        if(str.empty()) \
        { \
            return parse_error(&parser) << err; \
        } \
    } while(false)

#define EXPECT_CHAR(chr, err) \
    do \
    { \
        if(false == parser.expect_char(chr)) \
        { \
            return parse_error(&parser) << err; \
        } \
    } while(false)

        auto parser = TextfileParser::from_string(s);
        std::ostringstream buffer;
        while(parser.has_more())
        {
            switch(parser.peek_char())
            {
            case '\\':
                parser.read_char();
                buffer << parser.read_char();
                break;

            case '#': {
                parser.read_char();
                const auto text = buffer.str();
                buffer.str("");
                if(text.empty() == false)
                {
                    add(std::make_shared<LiteralStringNode>(text));
                }
                auto n = std::make_shared<CallSymbolNode>();
                while(parser.peek_char() == '[')
                {
                    parser.read_char();
                    const auto key_name = read_tracery_ident(&parser);
                    EMPTY_STRING(key_name, "got empty key");

                    EXPECT_CHAR(':', "expected : after key name");
                    if(parser.peek_char() == '#')
                    {
                        parser.read_char();
                        const auto symbol_name = read_tracery_ident(&parser);
                        EMPTY_STRING(symbol_name, "got empty symbol name");
                        EXPECT_CHAR('#', "expected # to end symbol name");
                        n->add_action_rule(key_name, symbol_name);
                    }
                    else
                    {
                        const auto command = read_tracery_ident(&parser);
                        EMPTY_STRING(command, "got empty command");
                    }
                    EXPECT_CHAR(']', "expected ]");
                }
                const auto symbol_name = read_tracery_ident(&parser);
                EMPTY_STRING(symbol_name, "Empty symbol name");
                n->symbol = symbol_name;
                bool run = true;
                while(run && parser.has_more())
                {
                    switch(parser.peek_char())
                    {
                    case '.': {
                        parser.read_char();
                        const auto mod = read_tracery_ident(&parser);
                        n->modifiers.push_back(mod);
                    }
                    break;

                    case '#':
                        parser.read_char();
                        run = false;
                        break;

                    default: {
                        const auto c = parser.read_char();
                        return Result(Result::general_rule_parse_error)
                               << "Unknown character inside ##: "
                               << (StringBuilder() << c);
                    }
                    }
                }
                add(n);
                if(run)
                {
                    return Result(Result::rule_eof);
                }
            }
            break;

            default: buffer << parser.read_char(); break;
            }
        }

        const auto text = buffer.str();
        if(text.empty() == false)
        {
            add(std::make_shared<LiteralStringNode>(text));
        }

        return Result(Result::no_error);
#undef EXPECT_CHAR
#undef EMPTY_STRING
    }

    Result
    Rule::flatten(generator_argument* gen) const
    {
        std::string ret;
        for(std::shared_ptr<Node> s: syntax)
        {
            const Result r = s->flatten(gen);
            if(r == false) { return r; }
            ret += r.get_text();
        }
        return Result(Result::no_error) << ret;
    }

    void
    Rule::add(std::shared_ptr<Node> p)
    {
        syntax.push_back(p);
    }


    // ----------------------------------------------------------------
    // Symbol
    Symbol::Symbol(const std::string& k) : key(k) {}

    Result
    Symbol::add_rule(const std::string& rule_code)
    {
        auto syntax = Rule{};
        Result r = syntax.compile(rule_code);
        if(r)
        {
            ruleset.push_back(syntax);
        }
        return r;
    }

    Result
    Symbol::flatten(generator_argument* gen) const
    {
        ASSERT(gen);
        ASSERTX(ruleset.empty() == false, key);

        return get_random_item_in_vector(gen->generator, ruleset).flatten(gen);
    }


    // ----------------------------------------------------------------
    // Modifier

    Modifier::~Modifier() = default;


    // ----------------------------------------------------------------
    // English


    namespace english
    {
        bool
        is_vowel(char c)
        {
            char c2 = to_lower_char(c);
            return (c2 == 'a') || (c2 == 'e') || (c2 == 'i') || (c2 == 'o') || (c2 == 'u');
        }

        bool
        is_alpha_num(char c)
        {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
        }


        std::string
        capitalize_all(const std::string& s)
        {
            std::string s2;
            bool capitalize_next = true;
            for(char c: s)
            {
                if(!is_alpha_num(c))
                {
                    capitalize_next = true;
                    s2 += c;
                }
                else
                {
                    if(!capitalize_next)
                    {
                        s2 += c;
                    }
                    else
                    {
                        s2 += to_upper_char(c);
                        capitalize_next = false;
                    }
                }
            }
            return s2;
        }

        std::string
        capitalize(const std::string& s)
        {
            char c = to_upper_char(s[0]);
            std::string a = std::string(1, c);
            std::string b = s.substr(1);
            std::string cr = a + b;
            return cr;
        }

        std::string
        a(const std::string& s)
        {
            if(s.length() > 0)
            {
                if(to_lower_char(s[0]) == 'u')
                {
                    if(s.length() > 2)
                    {
                        if(to_lower_char(s[2]) == 'i')
                        {
                            return "a " + s;
                        }
                    }
                }

                if(is_vowel(s[0]))
                {
                    return "an " + s;
                }
            }

            return "a " + s;
        }

        std::string
        s(const std::string& s)
        {
            switch(s[s.length() - 1])
            {
            case 's':
            case 'h':
            case 'x':
                return s + "es";
            case 'y':
                if(!is_vowel(s[s.length() - 2]))
                {
                    return s.substr(0, s.length() - 1) + "ies";
                }
                else
                {
                    return s + "s";
                }
            default: return s + "s";
            }
        }

        std::string
        ed(const std::string& s)
        {
            switch(s[s.length() - 1])
            {
            case 'e': return s + "d";
            case 's': case 'h': case 'x':
                return s + "ed";
            case 'y':
                if(!is_vowel(s[s.length() - 2]))
                {
                    return s.substr(0, s.length() - 1) + "ied";
                }
                else
                {
                    return s + "d";
                }
            default: return s + "ed";
            }
        }


        template <typename TFunction>
        struct FunctionModifier : public Modifier
        {
            TFunction func;
            FunctionModifier(TFunction f) : func(f) {}

            Result
            apply_modifier(const std::string& input) override
            {
                std::string r = func(input);
                return Result(Result::no_error) << r;
            }
        };

        template <typename T>
        std::shared_ptr<Modifier>
        new_modifier(T func)
        {
            return std::shared_ptr<Modifier>{new FunctionModifier<T>(func)};
        }


        void
        register_on_grammar(Grammar* g)
        {
            g->register_modifier("capitalizeAll", new_modifier(capitalize_all));
            g->register_modifier("capitalize", new_modifier(capitalize));
            g->register_modifier("a", new_modifier(a));
            g->register_modifier("s", new_modifier(s));
            g->register_modifier("ed", new_modifier(ed));
        }
    }

    // ----------------------------------------------------------------
    // Grammar

    Grammar::Grammar() = default;

    void
    Grammar::register_english()
    {
        english::register_on_grammar(this);
    }

    Result
    Grammar::load_from_string(const std::string& filename, const std::string& data)
    {
        const auto loaded = get_optional_and_log_errors
        (
            read_xml_source_to_gaf_struct<::tracery::Tracery>(filename, data, ::tracery::ReadXmlElementTracery)
        );
        if(loaded.has_value() == false)
        {
            // todo(Gustav): handle logging error better...
            return Result(Result::json_parse) << "unable to load file";
        }
        const auto& message = *loaded;
        // ::tracery::Tracery message;
        // const auto loaded_json_error = read_xml_source_to_gaf_struct_or_get_error_message(data, &message);
        // if(loaded_json_error.empty() == false)
        // {
        //     return result(result::json_parse) << loaded_json_error;
        // }

        for(const auto& json_rule: message.rule)
        {
            auto rule = Symbol{json_rule.name};
            const auto rule_loaded = from_json(&rule, json_rule);
            if(rule_loaded == false)
            {
                return rule_loaded;
            }
            rules.insert(std::make_pair(json_rule.name, rule));
        }

        return Result(Result::no_error);
    }

    Result
    Grammar::get_string_from_symbol(const std::string& rule, generator_argument* generator) const
    {
        const auto has_overridden = generator->overridden_rules.find(rule);
        if(has_overridden != generator->overridden_rules.end())
        {
            return Result(Result::no_error) << has_overridden->second;
        }

        const auto& found = rules.find(rule);
        if(found == rules.end())
        {
            // todo(Gustav): handle errors better
            return Result(Result::missing_rule) << rule;
        }
        return found->second.flatten(generator);
    }

    Grammar&
    Grammar::register_modifier(const std::string& name, std::shared_ptr<Modifier> mod)
    {
        modifiers.insert(std::make_pair(name, mod));
        return *this;
    }

    Result
    Grammar::apply_modifier(const std::string& name, const std::string& data) const
    {
        auto r = modifiers.find(name);
        if(r == modifiers.end())
        {
            return Result(Result::invalid_modifier) << name;
        }
        return r->second->apply_modifier(data);
    }

    Result
    Grammar::flatten(core::Random* random, const std::string& rule_code) const
    {
        generator_argument generator;
        generator.grammar = this;
        generator.generator = random;
        Rule syntax;
        syntax.compile(rule_code);
        return syntax.flatten(&generator);
    }
}
