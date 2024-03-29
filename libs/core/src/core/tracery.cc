#include "core/tracery.h"

#include "jsonh/jsonh.h"

#include "assert/assert.h"

#include "base/stringutils.h"
#include "base/stringbuilder.h"

#include "io/json.h"

#include "base/random.h"
#include "core/textfileparser.h"



namespace eu::core::tracery
{
    // ----------------------------------------------------------------
    // Private

    struct GeneratorArgument
    {
        Random* generator = nullptr;
        const tracery::Grammar* grammar = nullptr;
        std::map<std::string, std::string> overridden_rules;
    };

    // ----------------------------------------------------------------

    Result
    from_json(Symbol* rule, const jsonh::Value& value, const jsonh::Document* doc)
    {
        auto r = [&]() -> Result {
            if (const auto* str = value.AsString(doc); str != nullptr)
            {
                return rule->add_rule(str->value);
            }
            else if (const auto* arr = value.AsArray(doc); arr != nullptr)
            {
                for (const auto& v : arr->array)
                {
                    if (const auto* vstr = v.AsString(doc); vstr != nullptr)
                    {
                        Result res = rule->add_rule(vstr->value);
                        if (res == false)
                        {
                            return res;
                        }
                    }
                    else
                    {
                        return { Result::invalid_json };
                    }
                }

                return { Result::no_error };
            }
            else
            {
                return { Result::invalid_json };
            }
        }();

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
        std::string text;

        LiteralStringNode(const std::string& t)
            : text(t)
        {
        }

        Result flatten(GeneratorArgument*) const override
        {
            return Result{Result::no_error} << text;
        }
    };


    // ----------------------------------------------------------------

    struct ActionRule
    {
        std::string key;
        std::string symbol;
    };

    struct CallSymbolNode : public Node
    {
        std::string symbol;
        std::vector<std::string> modifiers;
        std::vector<ActionRule> action_rules;

        CallSymbolNode() = default;

        void add_action_rule
        (
            const std::string& action_key,
            const std::string& action_symbol
        )
        {
            action_rules.push_back(ActionRule {action_key, action_symbol});
        }

        Result flatten(GeneratorArgument* generator) const override
        {
            GeneratorArgument arg = *generator;

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
        auto ss = StringBuilder{};
        for(const auto& s: text)
        {
            ss.add_string(s);
        }
        return ss.to_string();
    }


    std::string to_string(const Result& r)
    {
        switch(r.error_type)
        {
        case Result::no_error:
            return "No error detected";
        case Result::unable_to_open_file:
            return fmt::format("Unable to open file: ", r.get_text());
        case Result::json_parse:
            return fmt::format("JSON parse error: ", r.get_text());
        case Result::missing_rule:
            return fmt::format("Rule not found in grammar: ", r.get_text());
        case Result::rule_eof:
            return fmt::format("EOF in rule: ", r.get_text());
        case Result::invalid_json:
            return "Invalid json state.";
        case Result::invalid_modifier:
            return fmt::format("Invalid modifier: ", r.get_text());
        case Result::general_rule_parse_error:
            return fmt::format("Rule parse error: ", r.get_text());
        default:
            return "Unhandled error";
        }
    }


    // ----------------------------------------------------------------


    Rule::Rule() = default;


    Result parse_error(TextfileParser* parser)
    {
        return Result{Result::general_rule_parse_error}
               << parser->peek_string() << " detected but ";
    }


    std::string read_tracery_ident(TextfileParser* parser)
    {
        const std::string valid
                = "abcdefghijklmnopqrstuvwxyz"
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                  "0123456789"
                  "_-+";

        auto ss = StringBuilder{};
        while(valid.find(parser->peek_char()) != std::string::npos)
        {
            ss.add_char(parser->read_char());
        }
        return ss.to_string();
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
        auto buffer = StringBuilder{};
        while(parser.has_more())
        {
            switch(parser.peek_char())
            {
            case '\\':
                parser.read_char();
                buffer.add_char(parser.read_char());
                break;

            case '#': {
                parser.read_char();
                const auto text = buffer.to_string();
                buffer.clear();
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
                    case '.':
                        {
                        parser.read_char();
                        const auto mod = read_tracery_ident(&parser);
                        n->modifiers.push_back(mod);
                        }
                        break;

                    case '#':
                        parser.read_char();
                        run = false;
                        break;

                    default:
                        {
                        const auto c = parser.read_char();
                        return Result{Result::general_rule_parse_error}
                                << "Unknown character inside ##: "
                                << fmt::to_string(c);
                        }
                    }
                }
                add(n);
                if(run)
                {
                    return {Result::rule_eof};
                }
            }
            break;

            default:
                buffer.add_char(parser.read_char());
                break;
            }
        }

        const auto text = buffer.to_string();
        if(text.empty() == false)
        {
            add(std::make_shared<LiteralStringNode>(text));
        }

        return {Result::no_error};
#undef EXPECT_CHAR
#undef EMPTY_STRING
    }


    Result
    Rule::flatten(GeneratorArgument* gen) const
    {
        std::string ret;
        for(std::shared_ptr<Node> s: syntax)
        {
            const Result r = s->flatten(gen);
            if(r == false) { return r; }
            ret += r.get_text();
        }
        return Result{Result::no_error} << ret;
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
    Symbol::flatten(GeneratorArgument* gen) const
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
        bool is_vowel(char c)
        {
            char c2 = to_lower_char(c);
            return (c2 == 'a') || (c2 == 'e') || (c2 == 'i') || (c2 == 'o') || (c2 == 'u');
        }


        bool is_alpha_num(char c)
        {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
        }


        std::string capitalize_all(const std::string& s)
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


        std::string capitalize(const std::string& s)
        {
            char c = to_upper_char(s[0]);
            std::string a = std::string(1, c);
            std::string b = s.substr(1);
            std::string cr = a + b;
            return cr;
        }

        // useful for sconverting cat -> "a cat"
        std::string add_prefix_with_a_or_an(const std::string& s)
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

        // useful fornverting dog -> dogs
        std::string add_postfix_plural(const std::string& s)
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

        /// useful for converting walk -> walked
        std::string add_postfix_ed(const std::string& s)
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


        template <typename TFunc>
        struct FunctionModifier : public Modifier
        {
            TFunc func;
            FunctionModifier(TFunc f) : func(f) {}

            Result apply_modifier(const std::string& input) override
            {
                std::string r = func(input);
                return Result{Result::no_error} << r;
            }
        };


        template <typename T>
        std::shared_ptr<Modifier> make_modifier(T func)
        {
            return std::shared_ptr<Modifier>{new FunctionModifier<T>(func)};
        }


        void register_on_grammar(Grammar* g)
        {
            g->register_modifier("capitalizeAll", make_modifier(capitalize_all));
            g->register_modifier("capitalize", make_modifier(capitalize));
            g->register_modifier("a", make_modifier(add_prefix_with_a_or_an));
            g->register_modifier("s", make_modifier(add_postfix_plural));
            g->register_modifier("ed", make_modifier(add_postfix_ed));
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
        const auto result = jsonh::Parse(data, jsonh::parse_flags::Json);
        if(result.HasError())
        {
            auto ret = Result{ Result::json_parse };
            for(const auto& e: result.errors)
            {
                ret << fmt::format("{}({}:{}): {}", filename, e.location.line, e.location.column, e.message);
            }
            return ret;
        }

        const auto* const root = result.root->AsObject(&result.doc);

        if (root == nullptr)
        {
            return Result{ Result::json_parse } << "root was not a object";
        }

        for(const auto& mem: root->object)
        {
            const auto& name_of_rule = mem.first;
            Symbol rule {name_of_rule};
            Result r = from_json(&rule, mem.second, &result.doc);
            if(r == false)
            {
                return r;
            }
            rules.insert(std::make_pair(name_of_rule, rule));
        }

        return {Result::no_error};
    }


    Result
    Grammar::get_string_from_symbol(const std::string& rule, GeneratorArgument* generator) const
    {
        const auto has_overridden = generator->overridden_rules.find(rule);
        if(has_overridden != generator->overridden_rules.end())
        {
            return Result{Result::no_error} << has_overridden->second;
        }

        const auto& found = rules.find(rule);
        if(found == rules.end())
        {
            // todo(Gustav): handle errors better
            return Result{Result::missing_rule} << rule;
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
            return Result{Result::invalid_modifier} << name;
        }
        return r->second->apply_modifier(data);
    }


    Result
    Grammar::flatten(Random* random, const std::string& rule_code) const
    {
        GeneratorArgument generator;
        generator.grammar = this;
        generator.generator = random;
        Rule syntax;
        syntax.compile(rule_code);
        return syntax.flatten(&generator);
    }
}
