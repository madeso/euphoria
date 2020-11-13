#include "core/assert.h"
#include "stringmerger.h"

#ifdef IMPLEMENT_ASSERT_LIB

#include <iostream>

#ifndef _MSC_VER
#include <cxxabi.h>
#include <execinfo.h>
#endif

#include <memory>
#include <vector>
#include <sstream>

namespace euphoria::core
{
#ifdef _MSC_VER
    // todo: implement backtrace for windows?
    namespace
    {
        std::vector<std::string>
        RunBacktrace(int)
        {
            return {};
        }
    }  // namespace

#else
    namespace
    {
        // backtrace code stolen from:
        // http://stackoverflow.com/questions/19190273/how-to-print-call-stack-in-c-c-more-beautifully

        std::string
        Demangle(const char* const symbol)
        {
            const std::unique_ptr<char, decltype(&std::free)> demangled
            (
                abi::__cxa_demangle(symbol, nullptr, nullptr, nullptr),
                &std::free
            );
            if(demangled)
            {
                return demangled.get();
            }

            return symbol;
        }

        std::vector<std::string>
        RunBacktrace(int skip)
        {
            // todo replace hardcoded limit?
            void* addresses[256];
            const int n = ::backtrace
            (
                static_cast<void**>(addresses),
                std::extent<decltype(addresses)>::value
            );
            const std::unique_ptr<char*, decltype(&std::free)> symbols
            (
                ::backtrace_symbols(static_cast<void* const*>(addresses), n),
                &std::free
            );

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
#endif

    namespace assertlib
    {
        bool&
        ShouldThrow()
        {
            static bool ShouldThrow = false;
            return ShouldThrow;
        }

        void
        StartThrowing()
        {
            ShouldThrow() = true;
        }

        bool
        IsThrowing()
        {
            return ShouldThrow();
        }

        void
        OnAssert
        (
            const char* const                  expression,
            int                                line,
            const char* const                  file,
            const char* const                  argstr,
            const std::vector<AssertValueArg>& arguments,
            const char* const                  function
        )
        {
            std::ostringstream ss;
            ss << "Assertion failed: " << expression << "\n"
               << "Function: " << function << "\n"
               << "File: " << file << ":" << line << "\n";

            if(!arguments.empty())
            {
                std::vector<std::string> args;
                for(const auto& a: arguments)
                {
                    args.push_back(a.value);
                }
                ss << "(" << argstr
                   << ") = " << StringMerger::Array().Generate(args) << "\n";
            }

            const auto trace = RunBacktrace(2);
            if(!trace.empty())
            {
                ss << "Backtrace:\n";
                for(const auto& b: trace)
                {
                    ss << b << "\n";
                }
            }

            if(ShouldThrow())
            {
                throw ss.str();
            }
            else
            {
                std::cerr << ss.str();
            }

            exit(-1);
        }
    }  // namespace assertlib

}  // namespace euphoria::core

#endif  // IMPLEMENT_ASSERT_LIB
