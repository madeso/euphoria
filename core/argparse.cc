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

namespace
{
  /// internal function.
  /// @returns true if arg is to be considered as an optional
  bool
  IsOptional(const std::string& arg)
  {
    if(arg.empty())
    {
      return false;
    }  // todo: assert this?
    return arg[0] == '-';
  }
}

namespace argparse
{
  ParserError::ParserError(const std::string& error)
      : runtime_error("error: " + error)
  {
  }

  ////////////////////////////////////////////////////////////////////////////

  Arguments::Arguments(const std::vector<std::string>& arguments)
      : args(arguments)
  {
  }

  const std::string&
  Arguments::PeekFirst() const
  {
    return args[0];
  }

  const bool
  Arguments::IsEmpty() const
  {
    return args.empty();
  }

  const std::string
  Arguments::GetFirst(const std::string& error)
  {
    if(IsEmpty())
    {
      throw ParserError(error);
    }
    const std::string r = args[0];
    args.erase(args.begin());
    return r;
  }

  ////////////////////////////////////////////////////////////////////////////

  Count::Count(size_t c)
      : count(c)
      , type(Const)
  {
  }

  Count::Count(Type t)
      : count(0)
      , type(t)
  {
    ASSERT(t != Const);
  }

  ////////////////////////////////////////////////////////////////////////////

  Running::Running(const std::string& aapp)
      : app(aapp)
      , run(true)
  {
  }

  ////////////////////////////////////////////////////////////////////////////

  Argument::~Argument()
  {
  }

  ////////////////////////////////////////////////////////////////////////////

  FunctionArgument::FunctionArgument(const ArgumentCallback& func)
      : function(func)
  {
  }

  void
  FunctionArgument::Parse(
      Running& r, Arguments& args, const std::string& argname)
  {
    function(r, args, argname);
  }

  ////////////////////////////////////////////////////////////////////////////

  ArgumentBase::ArgumentBase() = default;

  void
  ArgumentBase::Parse(Running&, Arguments& args, const std::string& argname)
  {
    const auto& count = extra.count;
    switch(count.type)
    {
      case Count::Const:
        for(size_t i = 0; i < count.count; ++i)
        {
          std::stringstream ss;
          ss << "argument " << argname << ": expected ";
          if(count.count == 1)
          {
            ss << "one argument";
          }
          else
          {
            ss << count.count << " argument(s), " << i << " already given";
          }
          Combine(args.GetFirst(ss.str()));

          // abort on optional?
        }
        return;
      case Count::MoreThanOne:
        Combine(args.GetFirst(
            "argument " + argname + ": expected at least one argument"));
      case Count::ZeroOrMore:
        while(!args.IsEmpty() && !IsOptional(args.PeekFirst()))
        {
          Combine(args.GetFirst("internal error"));
        }
        return;
      case Count::Optional:
        if(args.IsEmpty())
        {
          return;
        }
        if(IsOptional(args.PeekFirst()))
        {
          return;
        }
        Combine(args.GetFirst("internal error"));
        return;
      case Count::None:
        return;
      default:
        DIE("internal error, ArgumentT::parse invalid Count");
        throw "internal error, ArgumentT::parse invalid Count";
    }
  }

  ////////////////////////////////////////////////////////////////////////////


  Extra::Extra()
      : count(1)
  {
  }

  Extra&
  Extra::Help(const std::string& h)
  {
    help = h;
    return *this;
  }

  Extra&
  Extra::Count(const argparse::Count& c)
  {
    count = c;
    return *this;
  }

  Extra&
  Extra::MetaVar(const std::string& the_metavar)
  {
    metavar = the_metavar;
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////////

  Help::Help(const std::string& aname, Extra* e)
      : name(aname)
      , extra(e)
  {
  }

  const std::string
  Help::GetUsage() const
  {
    if(IsOptional(name))
    {
      return "[" + name + " " + GetMetaVarRepresentation() + "]";
    }
    else
    {
      return GetMetaVarRepresentation();
    }
  }

  const std::string
  Help::GetMetaVarRepresentation() const
  {
    switch(extra->count.type)
    {
      case Count::None:
        return "";
      case Count::MoreThanOne:
        return GetMetaVarName() + " [" + GetMetaVarName() + " ...]";
      case Count::Optional:
        return "[" + GetMetaVarName() + "]";
      case Count::ZeroOrMore:
        return "[" + GetMetaVarName() + " [" + GetMetaVarName() + " ...]]";
      case Count::Const:
      {
        std::ostringstream ss;
        ss << "[";
        for(size_t i = 0; i < extra->count.count; ++i)
        {
          if(i != 0)
          {
            ss << " ";
          }
          ss << GetMetaVarName();
        }
        ss << "]";
        return ss.str();
      }
      default:
        DIE("missing case");
        throw "invalid count type";
    }
  }

  const std::string
  Help::GetMetaVarName() const
  {
    const auto metavar = extra->metavar;
    if(metavar.empty() == false)
    {
      return metavar;
    }
    else
    {
      if(IsOptional(name))
      {
        return ToUpper(name.substr(1));
      }
      else
      {
        return name;
      }
    }
  }

  const std::string
  Help::GetHelpCommand() const
  {
    if(IsOptional(name))
    {
      return name + " " + GetMetaVarRepresentation();
    }
    else
    {
      return GetMetaVarName();
    }
  }

  const std::string&
  Help::GetHelpDescription() const
  {
    return extra->help;
  }

  ////////////////////////////////////////////////////////////////////////////

  ParseStatus::ParseStatus(const Running& running, ParseStatus::Result r)
      : out(running.o.str())
      , error(running.error.str())
      , result(r)
  {
  }

  ////////////////////////////////////////////////////////////////////////////

  namespace
  {
    struct CallHelp
    {
      explicit CallHelp(Parser* on)
          : parser(on)
      {
      }

      void
      operator()(Running& r, Arguments& args, const std::string& argname)
      {
        // todo: fix non const references here.
        parser->WriteHelp(r);
        r.run = false;
      }

      Parser* parser;
    };
  }

  Parser::Parser(const std::string& d, const std::string& aappname)
      : description(d)
      , appname(aappname)
      , positionalIndex(0)
  {
    AddFunction("-h", CallHelp(this)).Help("show this help message and exit.");
  }

  Extra&
  Parser::AddFunction(const std::string& name, ArgumentCallback func)
  {
    ArgumentPtr arg(new FunctionArgument(func));
    return insert(name, arg).Count(Count::None);
  }

  ParseStatus
  Parser::Parse(
      const std::string& name, const std::vector<std::string>& arguments) const
  {
    Arguments          args(arguments);
    const std::string& app = name;
    Running            running{app};

    try
    {
      while(!args.IsEmpty())
      {
        if(IsOptional(args.PeekFirst()))
        {
          // optional
          const std::string arg = args.GetFirst();
          auto              r   = optionals.find(arg);
          if(r == optionals.end())
          {
            throw ParserError(
                "Unknown optional argument: " +
                arg);  // todo: implement partial matching of arguments?
          }
          r->second->Parse(running, args, arg);
        }
        else
        {
          if(positionalIndex >= positionals.size())
          {
            throw ParserError(
                "All positional arguments have been consumed: " +
                args.PeekFirst());
          }
          ArgumentPtr p = positionals[positionalIndex];
          ++positionalIndex;
          p->Parse(
              running,
              args,
              "POSITIONAL");  // todo: give better name or something
        }
      }

      if(positionalIndex != positionals.size())
      {
        throw ParserError(
            "too few arguments");  // todo: list a few missing arguments...
      }

      return ParseStatus{running, ParseStatus::Complete};
    }
    catch(ParserError& p)
    {
      WriteUsage(running);
      running.error << app << ": " << p.what() << std::endl << std::endl;
      return ParseStatus{running, ParseStatus::Failed};
    }
  }

  void
  Parser::WriteHelp(Running& r) const
  {
    WriteUsage(r);
    r.o << std::endl << description << std::endl << std::endl;

    const std::string sep = "\t";
    const std::string ins = "  ";

    if(!helpPositional.empty())
    {
      r.o << "positional arguments:\n";
      for(const Help& positional : helpPositional)
      {
        r.o << ins << positional.GetHelpCommand() << sep
            << positional.GetHelpDescription() << "\n";
      }

      r.o << "\n";
    }

    if(!helpOptional.empty())
    {
      r.o << "optional arguments:\n";
      for(const Help& optional : helpOptional)
      {
        r.o << ins << optional.GetHelpCommand() << sep
            << optional.GetHelpDescription() << "\n";
      }
    }

    r.o << "\n";
  }

  void
  Parser::WriteUsage(Running& r) const
  {
    r.o << "usage: " << r.app;
    for(const Help& optional : helpOptional)
    {
      r.o << " " << optional.GetUsage();
    }

    for(const Help& positional : helpPositional)
    {
      r.o << " " << positional.GetUsage();
    }
    r.o << "\n";
  }

  Extra&
  Parser::insert(const std::string& name, ArgumentPtr arg)
  {
    if(IsOptional(name))
    {
      optionals.insert(Optionals::value_type(name, arg));
      helpOptional.emplace_back(name, &arg->extra);
      return arg->extra;
    }
    else
    {
      positionals.push_back(arg);
      helpPositional.emplace_back(name, &arg->extra);
      return arg->extra;
    }
  }
}
