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
  Generator generator;
  Grammar*  grammar;

  GeneratorArgument()
  {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator.seed(seed);
  }
};

Result
FromJson(Symbol* rule, const rapidjson::Value& value)
{
  if(value.IsString())
  {
    return rule->AddRule(value.GetString());
  }
  else if(value.IsArray())
  {
    for(const auto& v : value.GetArray())
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


// ----------------------------------------------------------------

SyntaxPart::~SyntaxPart()
{
}

// ----------------------------------------------------------------

struct SyntaxPartText : public SyntaxPart
{
  SyntaxPartText(const std::string& t)
      : text(t)
  {
  }
  std::string text;

  Result
  Flatten(GeneratorArgument* generator) override
  {
    return Result(Result::NO_ERROR) << text;
  }
};

// ----------------------------------------------------------------

struct SyntaxPartRule : public SyntaxPart
{
  SyntaxPartRule(const std::string& d)
      : rule(split(d, '.'))
  {
  }
  std::vector<std::string> rule;

  Result
  Flatten(GeneratorArgument* generator) override
  {
    Result result = generator->grammar->GetStringFromSymbol(rule[0], generator);
    if(result == false)
    {
      return result;
    }

    std::string ret = result.GetText();

    bool functions = false;
    for(const std::string& f : rule)
    {
      if(functions)
      {
        Result r = generator->grammar->ApplyModifier(f, ret);
        if(r == false)
        {
          return r;
        }
        ret = r.GetText();
      }
      else
      {
        functions = true;
      }
    }

    return Result(Result::NO_ERROR) << ret;
  }
};


// ----------------------------------------------------------------
// Result

Result::Result(Type t)
    : type(t)
{
}

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
  assert(text.size() == 1);
  if(text.size() == 0)
  {
    return "";
  }
  return text[0];
}

std::ostream&
operator<<(std::ostream& o, const Result& r)
{
  switch(r.type)
  {
    case Result::NO_ERROR:
      o << "No error detected";
      break;
    case Result::UNABLE_TO_OPEN_FILE:
      o << "Unable to open file: " << r.GetText();
      break;
    case Result::JSON_PARSE:
      o << "JSON parse error: " << r.GetText();
      break;
    case Result::MISSING_RULE:
      o << "Rule not found in grammar: " << r.GetText();
      break;
    case Result::RULE_EOF:
      o << "EOF in rule: " << r.GetText();
      break;
    case Result::INVALID_JSON:
      o << "Invalid json state.";
      break;
    case Result::INVALID_MODIFIER:
      o << "Invalid modifier: " << r.GetText();
      break;
    default:
      o << "Unhandled error";
      break;
  }

  return o;
}

// ----------------------------------------------------------------

Syntax::Syntax()
{
}

Result
Syntax::Compile(const std::string& s)
{
  const char  START_CHAR = '#';
  const char  END_CHAR   = '#';
  bool        escape     = false;
  bool        rule       = false;
  std::string buffer     = "";
  for(char c : s)
  {
    if(rule)
    {
      // todo: parse rules in a prettier way
      if(c == END_CHAR)
      {
        rule = false;
        if(buffer.empty() == false)
        {
          Add(new SyntaxPartRule(buffer));
          buffer = "";
        }
      }
      else
      {
        buffer += c;
      }
    }
    else
    {
      if(escape)
      {
        escape = false;
        buffer += c;
      }
      else
      {
        if(c == '\\')
        {
          escape = true;
        }
        else if(c == START_CHAR)
        {
          rule = true;
          if(buffer.empty() == false)
          {
            Add(new SyntaxPartText(buffer));
            buffer = "";
          }
        }
        else
        {
          buffer += c;
        }
      }
    }
  }

  if(rule)
  {
    return Result(Result::RULE_EOF) << s;
  }
  else
  {
    if(buffer.empty() == false)
    {
      Add(new SyntaxPartText(buffer));
      buffer = "";
    }
  }

  return Result(Result::NO_ERROR);
}

Result
Syntax::Flatten(GeneratorArgument* gen)
{
  std::string ret;
  for(std::shared_ptr<SyntaxPart> s : syntax)
  {
    const Result r = s->Flatten(gen);
    if(r == false)
      return r;
    ret += r.GetText();
  }
  return Result(Result::NO_ERROR) << ret;
}

void
Syntax::Add(SyntaxPart* s)
{
  std::shared_ptr<SyntaxPart> p(s);
  syntax.push_back(p);
}


// ----------------------------------------------------------------
// Symbol
Symbol::Symbol(const std::string& k)
    : key(k)
{
}

Result
Symbol::AddRule(const std::string& rule)
{
  Syntax syntax;
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
  std::uniform_int_distribution<size_t> distribution(0, ruleset.size() - 1);
  size_t                                index = distribution(gen->generator);
  return ruleset[index].Flatten(gen);
}


// ----------------------------------------------------------------
// Modifier

Modifier::~Modifier()
{
}


// ----------------------------------------------------------------
// English


namespace english
{
  void
  Register(Grammar* g);

  bool
  isVowel(char c)
  {
    char c2 = tolower(c);
    return (c2 == 'a') || (c2 == 'e') || (c2 == 'i') || (c2 == 'o') ||
           (c2 == 'u');
  }

  char
  isAlphaNum(char c)
  {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9');
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
      case 's':
        return s + "es";
        break;
      case 'h':
        return s + "es";
        break;
      case 'x':
        return s + "es";
        break;
      case 'y':
        if(!isVowel(s[s.length() - 2]))
          return s.substr(0, s.length() - 1) + "ies";
        else
          return s + "s";
        break;
      default:
        return s + "s";
    }
  }

  /*
  std::string firstS(const std::string& s) {
  auto s2 = split(s, ' ');

  var finished = s(s2[0]) + " " + s2.slice(1).join(" ");
  console.log(finished);
  return finished;
  }*/

  std::string
  ed(const std::string& s)
  {
    switch(s[s.length() - 1])
    {
      case 's':
        return s + "ed";
      case 'e':
        return s + "d";
      case 'h':
        return s + "ed";
      case 'x':
        return s + "ed";
      case 'y':
        if(!isVowel(s[s.length() - 2]))
          return s.substr(0, s.length() - 1) + "ied";
        else
          return s + "d";
        break;
      default:
        return s + "ed";
    }
  }


  struct FuncModifier : public Modifier
  {
    std::function<std::string(std::string)> func;
    FuncModifier(std::function<std::string(std::string)> f)
        : func(f)
    {
    }

    virtual Result
    ApplyModifier(const std::string& input) override
    {
      std::string r = func(input);
      return Result(Result::NO_ERROR) << r;
    }
  };


  void
  Register(Grammar* g)
  {
    g->RegisterModifier("capitalizeAll", new FuncModifier(capitalizeAll))
        .RegisterModifier("capitalize", new FuncModifier(capitalize))
        .RegisterModifier("a", new FuncModifier(a))
        .RegisterModifier("s", new FuncModifier(s))
        .RegisterModifier("ed", new FuncModifier(ed));
  }
}  // namespace english

// ----------------------------------------------------------------
// Grammar

Grammar::Grammar()
{
}

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
    return Result(Result::JSON_PARSE) << rapidjson::GetParseError_En(ok.Code());
  }

  for(rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin();
      itr != doc.MemberEnd();
      ++itr)
  {
    const std::string ruleName = itr->name.GetString();
    Symbol            rule{ruleName};
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
    const std::string& rule, GeneratorArgument* generator)
{
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
  GeneratorArgument generator;
  generator.grammar = this;
  Syntax syntax;
  syntax.Compile(rule);
  return syntax.Flatten(&generator);
}
