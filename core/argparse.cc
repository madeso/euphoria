#include "core/argparse.h"

#include <iostream>

#include <string>
#include <sstream>
#include <functional>
#include <vector>
#include <map>
#include <memory>

#include "core/assert.h"
#include "core/stringutils.h"
#include "core/stringmerger.h"

namespace argparse
{
  /////////////////////////////////////////////////////////////////////////////////////////
  // Name

  bool Name::IsOptional(const std::string& arg)
  {
    if(arg.empty())
    {
      return false;
    }
    return arg[0] == '-';
  }

  std::string Name::OptionalName(const std::string& name)
  {
    ASSERTX(IsOptional(name), name);
    ASSERTX(!name.empty(), name);
    return name.substr(1);
  }

  Name::Name()
      : is_optional(false)
  { }

  Name::Name(const char* str)
      : is_optional(IsOptional(str))
      , names( Split(str, ',') ) 
  {
    if(is_optional)
    {
      names[0] = OptionalName(names[0]);
    }

    for(auto& n: names)
    {
      n = Trim(n);
    }

    #ifdef _DEBUG
    AssertValid();
    #endif
  }

  Name Name::Parse(const std::string& n)
  {
    return { n.c_str() };
  }
  
  Name Name::Optional(std::initializer_list<std::string> names)
  {
    return { true, names };
  }

  Name Name::Positional(std::string& name)
  {
    return {false, {name}};
  }

  Name::Name(bool o, const std::vector<std::string>& n)
      : is_optional(o)
      , names(n)
  {
    #ifdef _DEBUG
    AssertValid();
    #endif
  }

  #ifdef _DEBUG
  void Name::AssertValid()
  {
    if(is_optional)
    {
      for(unsigned int i=0; i<names.size(); i+=1)
      {
        ASSERTX(IsOptional(names[i])==false, i, names[i]);
      }
    }
    else
    {
      ASSERTX(names.size()==1, names.size());
    }

    // todo: assert that name only contains valid characters...
  }
  #endif

  /////////////////////////////////////////////////////////////////////////////////////////
  // ConsoleOutput

  void ConsoleOutput::OnError(const std::string& err)
  {
    std::cerr << err << "\n";
  }

  void ConsoleOutput::OnInfo(const std::string& info)
  {
    std::cout << info << "\n";
  }

  /////////////////////////////////////////////////////////////////////////////////////////
  // Running

  bool Running::HasMore() const
  {
    return next_index < arguments.size();
  }

  std::string Running::Read()
  {
    if(next_index < arguments.size())
    {
      const auto ret = arguments[next_index];
      next_index += 1;
      return ret;
    }
    else
    {
      return "";
    }
  }

  std::string Running::Peek(size_t advance) const
  {
    ASSERTX(advance > 0, advance);
    const auto suggested_index = next_index + advance - 1;
    if(suggested_index < arguments.size())
    {
      return arguments[suggested_index];
    }
    else
    {
      return "";
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////
  // Extra

  Extra& Extra::MetaVar(const std::string& m)
  {
    arg->meta_var = m;
    return *this;
  }

  Extra& Extra::Help(const std::string& h)
  {
    arg->help = h;
    return *this;
  }

  /////////////////////////////////////////////////////////////////////////////////////////
  // SimpleParser

  template<>
  ParseResult SimpleParser<std::string>(std::string* target, const std::string&, const std::string& value, Output*)
  {
    *target = value;
    return ParseResult::Ok;
  }

  /////////////////////////////////////////////////////////////////////////////////////////
  // Parser

  Parser::Parser(const std::string& n) : display_name(n)
  {
    // todo: add help command
  }

  Extra Parser::AddArgument(const Name& name, std::shared_ptr<Arg> arg)
  {
    arg->name = name;

    if(name.is_optional)
    {
      ASSERT(!name.names.empty());
      for(auto n: name.names)
      {
        ASSERTX(optional_arguments.find(n)==optional_arguments.end(), n);
        optional_arguments[n] = arg;
      }
    }
    else
    {
      positional_arguments.push_back(arg);
    }

    return Extra { arg };
  }

  ParseResult Parser::Parse(int argc, char* argv[]) const
  {
    auto args = std::vector<std::string>{};
    for (int i = 1; i < argc; i += 1)
    {
      args.push_back(argv[i]);
    }
    return Parse(argv[0], args);
  }

  ParseResult Parser::Parse(const std::string& program_name, const std::vector<std::string>& args) const
  {
    auto console = ConsoleOutput {};
    auto running = Running { program_name, args, output ? output : &console };

    size_t next_positional_index = 0;

    while(running.HasMore())
    {
      const auto is_optional = Name::IsOptional(running.Peek());
      const auto has_more_positionals = next_positional_index < positional_arguments.size();

      std::string arg_name;
      std::shared_ptr<Arg> arg = nullptr;

      if(has_more_positionals)
      {
        // if it is a valid optional, parse it, otherwise send to optional
        if(is_optional)
        {
          auto found = optional_arguments.find(Name::OptionalName(running.Peek()));
          if(found != optional_arguments.end())
          {
            arg = found->second;
          }
        }

        if(arg == nullptr)
        {
          arg = positional_arguments[next_positional_index];
          arg_name = arg->name.names[0];
          next_positional_index += 1;
        }
      }
      else
      {
        // no more positionals, must be a optional
        const auto optional_name = running.Read();
        if(!Name::IsOptional(optional_name))
        {
          running.output->OnError(Str() << "Got " << optional_name << " but all positionals are consumed");
          return ParseResult::Failed;
        }

        auto found = optional_arguments.find(Name::OptionalName(optional_name));
        if(found == optional_arguments.end())
        {
          running.output->OnError(Str() << "Not a valid argument: " << optional_name);
          return ParseResult::Failed;
        }
        arg_name = optional_name;
        arg = found->second;
      }

      auto r = arg->Parse(arg_name, &running);
      if( r != ParseResult::Ok)
      {
        return r;
      }
    }

    std::vector<std::string> missing_positionals;
    for(size_t i=next_positional_index; i<positional_arguments.size(); i+=1)
    {
      missing_positionals.push_back(positional_arguments[i]->name.names[0]);
    }

    if(!missing_positionals.empty())
    {
      running.output->OnError( Str() << "Positionals not consumed: " << StringMerger::EnglishAnd().Generate(missing_positionals));
      return ParseResult::Failed;
    }

    return ParseResult::Ok;
  }
}
