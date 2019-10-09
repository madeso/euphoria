// ----------------------------------------------------------------
// Includes

#include "core/tracery.h"

#include <sstream>

// for random
#include <random>
#include <chrono>

// json parsing
#include "rapidjson/document.h"
#include "rapidjson/error/error.h"
#include "rapidjson/error/en.h"

#include "core/assert.h"
#include "core/textfileparser.h"
#include "core/str.h"

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

    typedef std::default_random_engine Generator;

    struct GeneratorArgument
    {
        Generator*                         generator;
        Grammar*                           grammar;
        std::map<std::string, std::string> overridden_rules;
    };

    Result
    ParseJson(Symbol* rule, const rapidjson::Value& value)
    {
        if(value.IsString())
        {
            return rule->AddRule(value.GetString());
        }
        else if(value.IsArray())
        {
            for(const auto& v: value.GetArray())
            {
                if(v.IsString())
                {
                    Result r = rule->AddRule(v.GetString());
                    if(r == false)
                    {
                        return r;
                    }
                }
                else
                {
                    return Result(Result::INVALID_JSON);
                }
            }

            return Result(Result::NO_ERROR);
        }
        else
        {
            return Result(Result::INVALID_JSON);
        }
    }


    Result
    FromJson(Symbol* rule, const rapidjson::Value& value)
    {
        auto r = ParseJson(rule, value);
        if(r == false)
        {
            // todo: add json error information
            r << "for symbol " << rule->key;
        }

        return r;
    }

    // ----------------------------------------------------------------

    Node::~Node() {}

    // ----------------------------------------------------------------

    struct LiteralStringNode : public Node
    {
        LiteralStringNode(const std::string& t) : text(t) {}
        std::string text;

        Result
        Flatten(GeneratorArgument*) override
        {
            return Result(Result::NO_ERROR) << text;
        }
    };

    // ----------------------------------------------------------------

    struct CallSymbolNode : public Node
    {
        CallSymbolNode() {}

        struct ActionRule
        {
            std::string key;
            std::string symbol;
        };

        std::string              symbol;
        std::vector<std::string> modifiers;
        std::vector<ActionRule>  action_rules;

        void
        AddActionRule(const std::string& key, const std::string& symbol)
        {
            action_rules.push_back(ActionRule {key, symbol});
        }

        Result
        Flatten(GeneratorArgument* generator) override
        {
            GeneratorArgument arg = *generator;

            for(const auto& r: action_rules)
            {
                const Result result
                        = arg.grammar->GetStringFromSymbol(r.symbol, &arg);
                if(result == false)
                {
                    return result;
                }
                arg.overridden_rules[r.key] = result.GetText();
            }

            Result result = arg.grammar->GetStringFromSymbol(symbol, &arg);
            if(result == false)
            {
                return result;
            }

            std::string ret = result.GetText();

            for(const std::string& f: modifiers)
            {
                Result r = generator->grammar->ApplyModifier(f, ret);
                if(r == false)
                {
                    return r;
                }
                ret = r.GetText();
            }

            return Result(Result::NO_ERROR) << ret;
        }
    };


    // ----------------------------------------------------------------
    // Result

    Result::Result(Type t) : type(t) {}

    Result&
    Result::operator<<(const std::string& t)
    {
        text.push_back(t);
        return *this;
    }

    Result::operator bool() const
    {
        return type == NO_ERROR;
    }

    std::string
    Result::GetText() const
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
        switch(r.type)
        {
        case Result::NO_ERROR: o << "No error detected"; break;
        case Result::UNABLE_TO_OPEN_FILE:
            o << "Unable to open file: " << r.GetText();
            break;
        case Result::JSON_PARSE:
            o << "JSON parse error: " << r.GetText();
            break;
        case Result::MISSING_RULE:
            o << "Rule not found in grammar: " << r.GetText();
            break;
        case Result::RULE_EOF: o << "EOF in rule: " << r.GetText(); break;
        case Result::INVALID_JSON: o << "Invalid json state."; break;
        case Result::INVALID_MODIFIER:
            o << "Invalid modifier: " << r.GetText();
            break;
        case Result::GENERAL_RULE_PARSE_ERROR:
            o << "Rule parse error: " << r.GetText();
            break;
        default: o << "Unhandled error"; break;
        }

        return o;
    }

    // ----------------------------------------------------------------

    Rule::Rule() {}

    Result
    ParseError(TextFileParser* parser)
    {
        return Result(Result::GENERAL_RULE_PARSE_ERROR)
               << parser->PeekString() << " detected but ";
    }

    std::string
    ReadTraceryIdent(TextFileParser* parser)
    {
        const std::string valid
                = "abcdefghijklmnopqrstuvwxyz"
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                  "0123456789"
                  "_-+";

        std::ostringstream ss;
        while(valid.find(parser->PeekChar()) != std::string::npos)
        {
            ss << parser->ReadChar();
        }
        return ss.str();
    }

    Result
    Rule::Compile(const std::string& s)
    {
#define EMPTY_STRING(str, err)                                                 \
    do                                                                         \
    {                                                                          \
        if(str.empty())                                                        \
        {                                                                      \
            return ParseError(&parser) << err;                                 \
        }                                                                      \
    } while(false)

#define EXPECT_CHAR(chr, err)                                                  \
    do                                                                         \
    {                                                                          \
        if(false == parser.ExpectChar(chr))                                    \
        {                                                                      \
            return ParseError(&parser) << err;                                 \
        }                                                                      \
    } while(false)

        auto               parser = TextFileParser {s};
        std::ostringstream buffer;
        while(parser.HasMore())
        {
            switch(parser.PeekChar())
            {
            case '\\':
                parser.ReadChar();
                buffer << parser.ReadChar();
                break;

            case '#':
            {
                parser.ReadChar();
                const auto text = buffer.str();
                buffer.str("");
                if(text.empty() == false)
                {
                    Add(std::make_shared<LiteralStringNode>(text));
                }
                auto n = std::make_shared<CallSymbolNode>();
                while(parser.PeekChar() == '[')
                {
                    parser.ReadChar();
                    const auto key_name = ReadTraceryIdent(&parser);
                    EMPTY_STRING(key_name, "got empty key");

                    EXPECT_CHAR(':', "expected : after key name");
                    if(parser.PeekChar() == '#')
                    {
                        parser.ReadChar();
                        const auto symbol_name = ReadTraceryIdent(&parser);
                        EMPTY_STRING(symbol_name, "got empty symbol name");
                        EXPECT_CHAR('#', "expected # to end symbol name");
                        n->AddActionRule(key_name, symbol_name);
                    }
                    else
                    {
                        const auto command = ReadTraceryIdent(&parser);
                        EMPTY_STRING(command, "got empty command");
                    }
                    EXPECT_CHAR(']', "expected ]");
                }
                const auto symbol_name = ReadTraceryIdent(&parser);
                EMPTY_STRING(symbol_name, "Empty symbol name");
                n->symbol = symbol_name;
                bool run  = true;
                while(run && parser.HasMore())
                {
                    switch(parser.PeekChar())
                    {
                    case '.':
                    {
                        parser.ReadChar();
                        const auto mod = ReadTraceryIdent(&parser);
                        n->modifiers.push_back(mod);
                    }
                    break;

                    case '#':
                        parser.ReadChar();
                        run = false;
                        break;

                    default:
                    {
                        const auto c = parser.ReadChar();
                        return Result(Result::GENERAL_RULE_PARSE_ERROR)
                               << "Unknown character inside ##: "
                               << (Str() << c);
                    }
                    }
                }
                Add(n);
                if(run)
                {
                    return Result(Result::RULE_EOF);
                }
            }
            break;

            default: buffer << parser.ReadChar(); break;
            }
        }

        const auto text = buffer.str();
        if(text.empty() == false)
        {
            Add(std::make_shared<LiteralStringNode>(text));
        }

        return Result(Result::NO_ERROR);
#undef EXPECT_CHAR
#undef EMPTY_STRING
    }

    Result
    Rule::Flatten(GeneratorArgument* gen)
    {
        std::string ret;
        for(std::shared_ptr<Node> s: syntax)
        {
            const Result r = s->Flatten(gen);
            if(r == false)
                return r;
            ret += r.GetText();
        }
        return Result(Result::NO_ERROR) << ret;
    }

    void
    Rule::Add(std::shared_ptr<Node> p)
    {
        syntax.push_back(p);
    }


    // ----------------------------------------------------------------
    // Symbol
    Symbol::Symbol(const std::string& k) : key(k) {}

    Result
    Symbol::AddRule(const std::string& rule)
    {
        Rule   syntax;
        Result r = syntax.Compile(rule);
        if(r)
        {
            ruleset.push_back(syntax);
        }
        return r;
    }

    Result
    Symbol::Flatten(GeneratorArgument* gen)
    {
        ASSERT(gen);
        ASSERTX(ruleset.empty() == false, key);
        std::uniform_int_distribution<size_t> distribution(
                0, ruleset.size() - 1);
        size_t index = distribution(*gen->generator);
        return ruleset[index].Flatten(gen);
    }


    // ----------------------------------------------------------------
    // Modifier

    Modifier::~Modifier() {}


    // ----------------------------------------------------------------
    // English


    namespace english
    {
        bool
        isVowel(char c)
        {
            char c2 = tolower(c);
            return (c2 == 'a') || (c2 == 'e') || (c2 == 'i') || (c2 == 'o')
                   || (c2 == 'u');
        }

        char
        isAlphaNum(char c)
        {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
                   || (c >= '0' && c <= '9');
        }


        std::string
        capitalizeAll(const std::string& s)
        {
            std::string s2      = "";
            bool        capNext = true;
            for(unsigned int i = 0; i < s.length(); i++)
            {
                if(!isAlphaNum(s[i]))
                {
                    capNext = true;
                    s2 += s[i];
                }
                else
                {
                    if(!capNext)
                    {
                        s2 += s[i];
                    }
                    else
                    {
                        s2 += toupper(s[i]);
                        capNext = false;
                    }
                }
            }
            return s2;
        }

        std::string
        capitalize(const std::string& s)
        {
            char        c  = toupper(s[0]);
            std::string a  = std::string(1, c);
            std::string b  = s.substr(1);
            std::string cr = a + b;
            return cr;
        }

        std::string
        a(const std::string& s)
        {
            if(s.length() > 0)
            {
                if(tolower(s[0]) == 'u')
                {
                    if(s.length() > 2)
                    {
                        if(tolower(s[2]) == 'i')
                            return "a " + s;
                    }
                }

                if(isVowel(s[0]))
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
            case 's': return s + "es"; break;
            case 'h': return s + "es"; break;
            case 'x': return s + "es"; break;
            case 'y':
                if(!isVowel(s[s.length() - 2]))
                    return s.substr(0, s.length() - 1) + "ies";
                else
                    return s + "s";
                break;
            default: return s + "s";
            }
        }

        std::string
        ed(const std::string& s)
        {
            switch(s[s.length() - 1])
            {
            case 's': return s + "ed";
            case 'e': return s + "d";
            case 'h': return s + "ed";
            case 'x': return s + "ed";
            case 'y':
                if(!isVowel(s[s.length() - 2]))
                    return s.substr(0, s.length() - 1) + "ied";
                else
                    return s + "d";
                break;
            default: return s + "ed";
            }
        }


        template <typename Func>
        struct FuncModifier : public Modifier
        {
            Func func;
            FuncModifier(Func f) : func(f) {}

            virtual Result
            ApplyModifier(const std::string& input) override
            {
                std::string r = func(input);
                return Result(Result::NO_ERROR) << r;
            }
        };

        template <typename T>
        Modifier*
        NewModifier(T func)
        {
            return new FuncModifier<T>(func);
        }


        void
        Register(Grammar* g)
        {
            g->RegisterModifier("capitalizeAll", NewModifier(capitalizeAll))
                    .RegisterModifier("capitalize", NewModifier(capitalize))
                    .RegisterModifier("a", NewModifier(a))
                    .RegisterModifier("s", NewModifier(s))
                    .RegisterModifier("ed", NewModifier(ed));
        }
    }  // namespace english

    // ----------------------------------------------------------------
    // Grammar

    Grammar::Grammar() {}

    void
    Grammar::RegisterEnglish()
    {
        english::Register(this);
    }

    Result
    Grammar::LoadFromString(const std::string& data)
    {
        rapidjson::Document doc;
        doc.Parse(data.c_str());
        const rapidjson::ParseResult ok = doc;
        if(!ok)
        {
            return Result(Result::JSON_PARSE)
                   << rapidjson::GetParseError_En(ok.Code());
        }

        for(rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin();
            itr != doc.MemberEnd();
            ++itr)
        {
            const std::string ruleName = itr->name.GetString();
            Symbol            rule {ruleName};
            Result            r = FromJson(&rule, itr->value);
            if(r == false)
            {
                return r;
            }
            rules.insert(std::make_pair(ruleName, rule));
        }

        return Result(Result::NO_ERROR);
    }

    Result
    Grammar::GetStringFromSymbol(
            const std::string& rule,
            GeneratorArgument* generator)
    {
        const auto has_overridden = generator->overridden_rules.find(rule);
        if(has_overridden != generator->overridden_rules.end())
        {
            return Result(Result::NO_ERROR) << has_overridden->second;
        }

        const auto found = rules.find(rule);
        if(found == rules.end())
        {
            // todo: handle errors better
            return Result(Result::MISSING_RULE) << rule;
        }
        return found->second.Flatten(generator);
    }

    Grammar&
    Grammar::RegisterModifier(const std::string& name, Modifier* m)
    {
        std::shared_ptr<Modifier> mod(m);
        modifiers.insert(std::make_pair(name, mod));
        return *this;
    }

    Result
    Grammar::ApplyModifier(const std::string& name, const std::string& data)
    {
        auto r = modifiers.find(name);
        if(r == modifiers.end())
        {
            return Result(Result::INVALID_MODIFIER) << name;
        }
        return r->second->ApplyModifier(data);
    }

    Result
    Grammar::Flatten(const std::string& rule)
    {
        const auto seed
                = std::chrono::system_clock::now().time_since_epoch().count();
        Generator gen;
        gen.seed(seed);
        GeneratorArgument generator;
        generator.grammar   = this;
        generator.generator = &gen;
        Rule syntax;
        syntax.Compile(rule);
        return syntax.Flatten(&generator);
    }

}  // namespace euphoria::core::tracery
