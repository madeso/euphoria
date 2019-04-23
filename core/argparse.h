#ifndef CORE_ARGPARSE_H
#define CORE_ARGPARSE_H

#include <vector>
#include <string>
#include <memory>
#include <map>

#include "core/str.h"
#include "core/enumtostring.h"

namespace argparse
{
  /////////////////////////////////////////////////////////////////////////////////////////
  // Enum

  enum class Greedy
  {
    Yes, No
  };

  enum class ParseResult
  {
    // parsing was successfull, use arguments
    Ok,

    // some argumetns failed, please exit
    Failed,

    // parsing was successfull, but exit was requested, please exit
    Quit, 
  };

  /////////////////////////////////////////////////////////////////////////////////////////
  // Util

  struct Name
  {
    static bool IsOptional(const std::string& name);
    static std::string OptionalName(const std::string& name);

    Name();
    Name(const char* n);

    bool is_optional;
    std::vector<std::string> names;

    static Name Parse(const std::string& n);
    static Name Optional(std::initializer_list<std::string> names);
    static Name Positional(std::string& names);

    private:
      Name(bool o, const std::vector<std::string>& n);

      #ifdef _DEBUG
      void AssertValid();
      #endif
  };

  struct Output
  {
    virtual ~Output() {}

    virtual void OnError(const std::string& err) = 0;
    virtual void OnInfo(const std::string& info) = 0;
  };

  struct ConsoleOutput : public Output
  {
    void OnError(const std::string& err) override;
    void OnInfo(const std::string& info) override;
  };

  struct Running
  {
    std::string name;
    std::vector<std::string> arguments;
    Output* output = nullptr;

    size_t next_index = 0;

    bool HasMore() const;
    std::string Read();
    std::string Peek(size_t advance = 1) const;
  };

  struct Arg
  {
    Name name;
    std::string meta_var;
    std::string help;

    virtual ~Arg() {}
    virtual ParseResult Parse(const std::string& name, Running* running) = 0;
  };

  struct Extra
  {
    std::shared_ptr<Arg> arg;

    Extra& MetaVar(const std::string& m);
    Extra& Help(const std::string& h);
  };

  template<typename T>
  ParseResult SimpleParser(T* target, const std::string& name, const std::string& value, Output* output)
  {
    std::stringstream ss(value);
    ss >> *target;
    if(ss.fail())
    {
      output->OnError(Str() << value << " for " << name << " is not accepted.");
      return ParseResult::Failed;
    }
    else
    {
      return ParseResult::Ok;
    }
  }

  template<>
  ParseResult SimpleParser<std::string>(std::string* target, const std::string&, const std::string& value, Output*);

  template<typename T>
  struct SimpleArg : public Arg
  {
    T* target;

    ParseResult Parse(const std::string& cmd_name, Running* running) override
    {
      if(running->HasMore())
      {
        const auto value = running->Read();
        // todo: add validator (InRange, etc...)
        return SimpleParser<T>(target, cmd_name, value, running->output);
      }
      else
      {
        running->output->OnError(Str() << cmd_name << " missing value");
        return ParseResult::Failed;
      }
    }
  };

  template<typename T>
  struct FunctionArgument : public Arg
  {
    T t;

    FunctionArgument(T tt) : t(tt) { }
    ParseResult Parse(const std::string&, Running*) override
    {
      t();
      return ParseResult::Ok;
    }
  };

  template<typename T>
  struct VectorArg : public Arg
  {
    std::vector<T>* target;
    Greedy greedy = Greedy::No;

    ParseResult ParseOne(const std::string& cmd_name, Running* running)
    {
      if(!running->HasMore())
      {
        running->output->OnError(Str() << cmd_name << " missing value");
        return ParseResult::Failed;
      }
      const auto value = running->Read();
      T t;
      const auto r = SimpleParser<T>(&t, cmd_name, value, running->output);
      if(r == ParseResult::Ok)
      {
        // todo: add validator (InRange, etc...)
        target->emplace_back(t);
      }
      return r;
    }

    ParseResult Parse(const std::string& cmd_name, Running* running) override
    {
      if(greedy == Greedy::Yes)
      {
        while(running->HasMore())
        {
          auto r = ParseOne(cmd_name, running);
          if( r != ParseResult::Ok)
          {
            return r;
          }
        }

        return ParseResult::Ok;
      }
      else
      {
        return ParseOne(cmd_name, running);
      }
    }
  };

  /////////////////////////////////////////////////////////////////////////////////////////
  // Main

  struct Parser
  {
    explicit Parser(const std::string& n);

    // todo: add subparser
    // todo: get inspiration from 
    // https://stackoverflow.com/questions/491595/best-way-to-parse-command-line-arguments-in-c
    // https://github.com/clap-rs/clap
    // https://docs.python.org/3/library/argparse.html

    std::string display_name;

    // if null, use standard console output
    Output* output = nullptr;

    std::map<std::string, std::shared_ptr<Arg>> optional_arguments;
    std::vector<std::shared_ptr<Arg>> positional_arguments;

    Extra AddArgument(const Name& name, std::shared_ptr<Arg> arg);

    template<typename T>
    Extra AddSimple(const Name& name, T* var)
    {
      auto a = std::make_shared<SimpleArg<T>>();
      a->target = var;
      return AddArgument(name, a);
    }

    template<typename T>
    Extra AddSimpleFunction(const Name& name, T callback)
    {
      auto a = std::make_shared<FunctionArgument<T>>(callback);
      return AddArgument(name, a);
    }

    template<typename T>
    Extra AddVector(const Name& name, std::vector<T>* vec)
    {
      auto a = std::make_shared<VectorArg<T>>();
      a->target = vec;
      a->greedy = name.is_optional ? Greedy::No : Greedy::Yes;
      return AddArgument(name, a);
    }

    ParseResult Parse(int argc, char* argv[]) const;
    ParseResult Parse(const std::string& program_name, const std::vector<std::string>& args) const;
  };

}  // namespace argparse


BEGIN_ENUM_LIST(argparse::ParseResult)
  ENUM_VALUE(argparse::ParseResult, Ok)
  ENUM_VALUE(argparse::ParseResult, Failed)
  ENUM_VALUE(argparse::ParseResult, Quit)
END_ENUM_LIST()

#endif  // CORE_ARGPARSE_H
