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

    const std::string& operator[](int index) const;

    const bool
    empty() const;

    const size_t
    size() const;

    const std::string
    get(const std::string& error = "no more arguments available");

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

    Type
    type() const;

    size_t
    count() const;

   private:
    size_t mCount;
    Type   mType;
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
    help(const std::string& h);
    const std::string&
    help() const;

    /// the number of values a argument can support
    Extra&
    count(const Count c);
    const Count&
    count() const;

    // the meta variable, used in usage display and help display for the
    // arguments
    Extra&
    metavar(const std::string& metavar);

    const std::string&
    metavar() const;

   private:
    std::string mHelp;
    Count       mCount;
    std::string mMetavar;
  };

  ////////////////////////////////////////////////////////////////////////////

  class Argument
  {
   public:
    Extra extra;

    virtual ~Argument();

    virtual void
    parse(Running& r, Arguments& args, const std::string& argname) = 0;
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
    parse(Running& r, Arguments& args, const std::string& argname) override;

   private:
    ArgumentCallback function;
  };

  ////////////////////////////////////////////////////////////////////////////

  class ArgumentBase : public Argument
  {
   public:
    ArgumentBase();

    virtual void
    combine(const std::string& value) = 0;

    virtual void
    parse(Running&, Arguments& args, const std::string& argname) override;
  };

  ////////////////////////////////////////////////////////////////////////////

  class Help
  {
   public:
    Help(const std::string& aname, Extra* e);

    const std::string
    usage() const;

    const std::string
    metavarrep() const;

    const std::string
    metavarname() const;

    const std::string
    helpCommand() const;

    const std::string&
    helpDescription() const;

   private:
    std::string name;
    Extra*      extra;
  };

////////////////////////////////////////////////////////////////////////////

#define ConverterFunction(V) std::function<V(const std::string&)>
#define CombinerFunction(T, V) std::function<void(T & t, const V&)>

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
    ArgumentT(T& t, CombinerFunction(T, V) com, ConverterFunction(V) c)
        : target(t)
        , combiner(com)
        , converter(c)
    {
    }

    void
    combine(const std::string& value) override
    {
      combiner(target, converter(value));
    }

   private:
    T& target;
    CombinerFunction(T, V) combiner;
    ConverterFunction(V) converter;
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
    add_simple(
        const std::string& name,
        T&                 var,
        ConverterFunction(T) co = StandardConverter<T>,
        CombinerFunction(T, T) combiner = Assign<T, T>)
    {
      return add<T, T>(name, var, combiner, co);
    }

    template <typename T>
    Extra&
    add_vector(const std::string& name, std::vector<T>& var)
    {
      return add<std::vector<T>, T>(name, var, argparse::PushBackVector<T>)
          .count(argparse::Count::MoreThanOne);
    }

    template <typename T, typename V>
    Extra&
    add(const std::string& name,
        T&                 var,
        CombinerFunction(T, V) combiner = Assign<T, V>,
        ConverterFunction(V) co = StandardConverter<V>)
    {
      ArgumentPtr arg(new ArgumentT<T, V>(var, combiner, co));
      return insert(name, arg);
    }

    Extra&
    addFunction(const std::string& name, ArgumentCallback func);

    ParseStatus
    parse(const std::string& name, const std::vector<std::string>& arguments)
        const;

    void
    writeHelp(Running& r) const;

    void
    writeUsage(Running& r) const;

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
