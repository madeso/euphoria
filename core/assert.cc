#include "core/assert.h"

#include <iostream>

#include <memory>
#include <vector>
#include <sstream>
#include <cxxabi.h>
#include <execinfo.h>

namespace
{
  // backtrace code stolen from:
  // http://stackoverflow.com/questions/19190273/how-to-print-call-stack-in-c-c-more-beautifully

  std::string
  Demangle(const char* const symbol)
  {
    const std::unique_ptr<char, decltype(&std::free)> demangled(
        abi::__cxa_demangle(symbol, nullptr, nullptr, nullptr), &std::free);
    if(demangled)
    {
      return demangled.get();
    }

    return symbol;
  }

  std::vector<std::string>
  RunBacktrace(int skip)
  {
    // TODO: replace hardcoded limit?
    void*     addresses[256];
    const int n =
        ::backtrace(addresses, std::extent<decltype(addresses)>::value);
    const std::unique_ptr<char*, decltype(&std::free)> symbols(
        ::backtrace_symbols(addresses, n), &std::free);

    std::vector<std::string> ret;
    for(int i = skip; i < n; ++i)
    {
      // we parse the symbols retrieved from backtrace_symbols() to
      // extract the "real" symbols that represent the mangled names.
      char* const symbol = symbols.get()[i];
      char*       end    = symbol;
      while(*end != 0)
      {
        ++end;
      }
      // scanning is done backwards, since the module name
      // might contain both '+' or '(' characters.
      while(end != symbol && *end != '+')
      {
        --end;
      }
      char* begin = end;
      while(begin != symbol && *begin != '(')
      {
        --begin;
      }

      std::ostringstream ss;
      if(begin != symbol)
      {
        ss << std::string(symbol, ++begin - symbol);
        *end++ = '\0';
        ss << Demangle(begin) << '+' << end;
      }
      else
      {
        ss << symbol;
      }
      ret.push_back(ss.str());
    }

    return ret;
  }
}  // namespace

namespace assertlib
{
  bool&
  ShouldThrow()
  {
    static bool s_ShouldThrow = false;
    return s_ShouldThrow;
  }

  void
  StartThrowing()
  {
    ShouldThrow() = true;
  }

  void
  OnAssert(const char* const expression, int line, const char* const file,
           const char* const function)
  {
    std::cerr << "Assertion failed: " << expression << "\n"
              << "Function: " << function << "\n"
              << "File: " << file << ":" << line << "\n";

    const auto trace = RunBacktrace(2);
    if(!trace.empty())
    {
      std::cerr << "Backtrace:\n";
      for(const auto& b : trace)
      {
        std::cerr << b << "\n";
      }
    }

    if(ShouldThrow())
    {
      throw "assertion_failed";
    }

    exit(-1);
  }
}  // namespace assertlib
