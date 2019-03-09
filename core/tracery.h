#ifndef CORE_TRACERY_H
#define CORE_TRACERY_H

#include <vector>
#include <string>
#include <memory>
#include <map>

struct Result
{
  enum Type
  {
    UNABLE_TO_OPEN_FILE,
    NO_ERROR,
    JSON_PARSE,
    MISSING_RULE,
    RULE_EOF,
    INVALID_JSON,
    INVALID_MODIFIER
  };

  Type                     type;
  std::vector<std::string> text;

  Result(Type t);

  Result&
  operator<<(const std::string& t);

  operator bool() const;


  std::string
  GetText() const;
};

std::ostream&
operator<<(std::ostream& o, const Result& r);

struct GeneratorArgument;


struct Node
{
  virtual ~Node();

  virtual Result
  Flatten(GeneratorArgument* generator) = 0;
};


struct Modifier
{
  virtual ~Modifier();

  virtual Result
  ApplyModifier(const std::string& input) = 0;
};

struct Rule
{
  Rule();

  Result
  Compile(const std::string& s);

  Result
  Flatten(GeneratorArgument* gen);

  void
  Add(Node* s);

  std::vector<std::shared_ptr<Node>> syntax;
};

struct Symbol
{
  explicit Symbol(const std::string& k);

  std::string         key;
  std::vector<Rule> ruleset;

  Result
  AddRule(const std::string& rule);

  Result
  Flatten(GeneratorArgument* gen);
};

struct Grammar
{
  Grammar();

  void
  RegisterEnglish();

  Result
  LoadFromString(const std::string& data);

  Result
  GetStringFromSymbol(const std::string& rule, GeneratorArgument* generator);

  Grammar&
  RegisterModifier(const std::string& name, Modifier* m);

  Result
  ApplyModifier(const std::string& name, const std::string& data);

  Result
  Flatten(const std::string& rule);

  std::map<std::string, Symbol>                    rules;
  std::map<std::string, std::shared_ptr<Modifier>> modifiers;
};

#endif  // CORE_TRACERY_H
