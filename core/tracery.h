#ifndef CORE_TRACERY_H
#define CORE_TRACERY_H

#include <vector>
#include <string>
#include <memory>
#include <map>

class Result
{
 public:
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
  operator<<(const std::string& t) ;

  operator bool() const ;


  std::string
  GetText() const ;
};

std::ostream& operator<<(std::ostream& o, const Result& r);

class GeneratorArgument;


class SyntaxPart
{
 public:
  virtual ~SyntaxPart();

  virtual Result
  Flatten(GeneratorArgument* generator) = 0;
};


class Modifier
{
 public:
  virtual ~Modifier();

  virtual Result
  ApplyModifier(const std::string& input) = 0;
};

class Syntax
{
 public:
  Syntax();
  
  Result
  Compile(const std::string& s);

  Result
  Flatten(GeneratorArgument* gen);

  void
  Add(SyntaxPart* s);

  std::vector<std::shared_ptr<SyntaxPart>> syntax;
};

class Rule
{
 public:
  explicit Rule(const std::string& n);

  std::string name;
  std::vector<Syntax> rules;

  Result
  AddRule(const std::string& rule);

  Result
  Flatten(GeneratorArgument* gen);
};

class Grammar
{
 public:
  Grammar();

  void
  RegisterEnglish();

  Result
  LoadFromString(const std::string& data);

  Result
  GetStringFromRule(const std::string& rule, GeneratorArgument* generator);

  Grammar&
  RegisterModifier(const std::string& name, Modifier* m);

  Result
  ApplyModifier(const std::string& name, const std::string& data);

  Result
  Flatten(const std::string& rule);

  std::map<std::string, Rule>                      rules;
  std::map<std::string, std::shared_ptr<Modifier>> modifiers;
};

#endif  // CORE_TRACERY_H
