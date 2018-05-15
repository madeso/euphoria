#ifndef CORE_ARGPARSE_H
#define CORE_ARGPARSE_H

#include <stdexcept>
#include <vector>
#include <string>
#include <functional>
#include <sstream>
#include <memory>
#include <map>
#include <sstream>


namespace argparse
{
  ////////////////////////////////////////////////////////////////////////////

  class ParserError : public std::runtime_error
  {
   public:
    ParserError(const std::string& error);
  };

  ////////////////////////////////////////////////////////////////////////////

  class Arguments
  {
   public:
    explicit Arguments(const std::vector<std::string>& arguments);

    const std::string&
    PeekFirst() const;

    const bool
    IsEmpty() const;

    const std::string
    GetFirst(const std::string& error = "no more arguments available");

   private:
    std::vector<std::string> args;
  };

  ////////////////////////////////////////////////////////////////////////////

  class Count
  {
   public:
    enum Type
    {
      Const,
      MoreThanOne,
      Optional,
      None,
      ZeroOrMore
    };

    Count(size_t c);
    Count(Type t);

    size_t count;
    Type   type;
  };

  ////////////////////////////////////////////////////////////////////////////

  // basic class for passing along variables that only exist when parsing
  struct Running
  {
   public:
    explicit Running(const std::string& aapp);

    const std::string  app;
    std::ostringstream o;
    std::ostringstream error;
    bool               run;

   private:
    Running(const Running&);

    void
    operator=(const Running&);
  };

  ////////////////////////////////////////////////////////////////////////////

  // Utility class to provide optional arguments for the commandline arguments.
  class Extra
  {
   public:
    Extra();

    /// set the extended help for the argument
    Extra&
    Help(const std::string& h);

    /// the number of values a argument can support
    Extra&
    Count(const Count& c);

    // the meta variable, used in usage display and help display for the
    // arguments
    Extra&
    MetaVar(const std::string& the_metavar);

    std::string     help;
    argparse::Count count;
    std::string     metavar;
  };

  ////////////////////////////////////////////////////////////////////////////

  class Argument
  {
   public:
    Extra extra;

    virtual ~Argument();

    virtual void
    Parse(Running& r, Arguments& args, const std::string& argname) = 0;
  };

  ////////////////////////////////////////////////////////////////////////////

  typedef std::function<void(Running& r, Arguments&, const std::string&)>
      ArgumentCallback;

  ////////////////////////////////////////////////////////////////////////////

  class FunctionArgument : public Argument
  {
   public:
    FunctionArgument(const ArgumentCallback& func);

    void
    Parse(Running& r, Arguments& args, const std::string& argname) override;

   private:
    ArgumentCallback function;
  };

  ////////////////////////////////////////////////////////////////////////////

  class ArgumentBase : public Argument
  {
   public:
    ArgumentBase();

    virtual void
    Combine(const std::string& value) = 0;

    virtual void
    Parse(Running&, Arguments& args, const std::string& argname) override;
  };

  ////////////////////////////////////////////////////////////////////////////

  class Help
  {
   public:
    Help(const std::string& aname, Extra* e);

    const std::string
    GetUsage() const;

    const std::string
    GetMetaVarRepresentation() const;

    const std::string
    GetMetaVarName() const;

    const std::string
    GetHelpCommand() const;

    const std::string&
    GetHelpDescription() const;

   private:
    std::string name;
    Extra*      extra;
  };

  ////////////////////////////////////////////////////////////////////////////

  template <typename V>
  using ConverterFunction = std::function<V(const std::string&)>;

  template <typename T, typename V>
  using CombinerFunction = std::function<void(T& t, const V&)>;

  ////////////////////////////////////////////////////////////////////////////

  template <typename A, typename B>
  void
  Assign(A& a, const B& b)
  {
    a = b;
  }

  template <typename T>
  void
  PushBackVector(std::vector<T>& vec, const T& t)
  {
    vec.push_back(t);
  }

  template <typename T>
  T
  StandardConverter(const std::string& type)
  {
    std::istringstream ss(type);
    T                  t;
    if(ss >> t)
    {
      return t;
    }
    else
    {
      throw ParserError("Failed to parse " + type);
    }
  }

  template <typename T>
  struct Convert
  {
    Convert()
    {
      // todo: to lowercase?
      // todo: almost matching?
    }

    Convert&
    operator()(const std::string& a, T t)
    {
      map[a] = t;
      return *this;
    }

    T
    operator()(const std::string& type) const
    {
      auto found = map.find(type);
      if(found == map.end())
      {
        throw ParserError("Failed to parse " + type);
      }
      else
      {
        return found->second;
      }
    }

    std::map<std::string, T> map;
  };

  template <typename T, typename V>
  class ArgumentT : public ArgumentBase
  {
   public:
    ArgumentT(T& t, CombinerFunction<T, V> com, ConverterFunction<V> c)
        : target(t)
        , combiner(com)
        , converter(c)
    {
    }

    void
    Combine(const std::string& value) override
    {
      combiner(target, converter(value));
    }

   private:
    T& target;
    CombinerFunction<T, V> combiner;
    ConverterFunction<V> converter;
  };

  ////////////////////////////////////////////////////////////////////////////

  struct ParseStatus
  {
    enum Result
    {
      Failed,
      Complete,
      ShouldExit
    };
    ParseStatus(const Running& running, Result r);
    std::string out;
    std::string error;
    Result      result;
  };

  /// main entry class that contains all arguments and does all the parsing.
  class Parser
  {
   public:
    Parser(const std::string& d, const std::string& aappname = "");

    template <typename T>
    Extra&
    AddSimple(
        const std::string&   name,
        T&                   var,
        ConverterFunction<T> co = StandardConverter<T>,
        CombinerFunction<T, T> combiner = Assign<T, T>)
    {
      return Add<T, T>(name, var, combiner, co);
    }

    template <typename T>
    Extra&
    AddVector(const std::string& name, std::vector<T>& var)
    {
      return Add<std::vector<T>, T>(name, var, argparse::PushBackVector<T>)
          .Count(argparse::Count::MoreThanOne);
    }

    template <typename T, typename V>
    Extra&
    Add(const std::string& name,
        T&                 var,
        CombinerFunction<T, V> combiner = Assign<T, V>,
        ConverterFunction<V> co = StandardConverter<V>)
    {
      ArgumentPtr arg(new ArgumentT<T, V>(var, combiner, co));
      return insert(name, arg);
    }

    Extra&
    AddFunction(const std::string& name, ArgumentCallback func);

    ParseStatus
    Parse(const std::string& name, const std::vector<std::string>& arguments)
        const;

    void
    WriteHelp(Running& r) const;

    void
    WriteUsage(Running& r) const;

   private:
    typedef std::shared_ptr<Argument> ArgumentPtr;

    Extra&
    insert(const std::string& name, ArgumentPtr arg);

    std::string description;
    std::string appname;

    typedef std::map<std::string, ArgumentPtr> Optionals;
    Optionals optionals;

    typedef std::vector<ArgumentPtr> Positionals;
    Positionals                      positionals;
    mutable size_t
        positionalIndex;  // todo: mutable or change parseArgs to nonconst?

    std::vector<Help> helpOptional;
    std::vector<Help> helpPositional;
  };
}


#endif  // CORE_ARGPARSE_H
