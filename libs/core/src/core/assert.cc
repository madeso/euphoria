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
    // todo(Gustav): implement backtrace for windows?
    namespace
    {
        std::vector<std::string>
        run_backtrace(int)
        {
            return {};
        }
    }

#else
    namespace
    {
        // backtrace code stolen from:
        // http://stackoverflow.com/questions/19190273/how-to-print-call-stack-in-c-c-more-beautifully

        std::string
        demangle_symbol(const char* const symbol)
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
        run_backtrace(int skip)
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
                char* end = symbol;
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
                    ss << demangle_symbol(begin) << '+' << end;
                }
                else
                {
                    ss << symbol;
                }
                ret.push_back(ss.str());
            }

            return ret;
        }
    }
#endif

    namespace assertlib
    {
        bool&
        should_throw_variable()
        {
            static bool ShouldThrow = false;
            return ShouldThrow;
        }

        void
        start_throwing()
        {
            should_throw_variable() = true;
        }

        bool
        is_throwing()
        {
            return should_throw_variable();
        }

        void
        on_assert
        (
            const char* const expression,
            int line,
            const char* const file,
            const char* const argstr,
            const std::vector<AssertValueArg>& arguments,
            const char* function
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
                   << ") = " << string_mergers::array.merge(args) << "\n";
            }

            const auto trace = run_backtrace(2);
            if(!trace.empty())
            {
                ss << "Backtrace:\n";
                for(const auto& b: trace)
                {
                    ss << b << "\n";
                }
            }

            if(should_throw_variable())
            {
                throw ss.str();
            }
            else
            {
                std::cerr << ss.str();
            }

            exit(-1);
        }
    }

}

#endif
