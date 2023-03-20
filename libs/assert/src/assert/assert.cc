#include "assert/assert.h"

#ifdef IMPLEMENT_ASSERT_LIB

#include <iostream>

#ifndef _MSC_VER
#include <cxxabi.h>
#include <execinfo.h>
#endif

#include <memory>
#include <vector>
#include <sstream>

namespace euphoria
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

                if(begin != symbol)
                {
                    const auto first_symbol = std::string(symbol, ++begin - symbol);
                    *end++ = '\0';
                    const auto second_symbol = demangle_symbol(begin);
                    ret.emplace_back("{}{}+{}"_format(first_symbol, second_symbol, end));
                }
                else
                {
                    ret.emplace_back(symbol);
                }
            }

            return ret;
        }
    }
#endif

}
    
namespace euphoria::assertlib
{
    bool&
    should_throw_variable()
    {
        static bool should_throw = false;
        return should_throw;
    }

    void
    begin_throwing()
    {
        should_throw_variable() = true;
    }

    bool
    is_throwing()
    {
        return should_throw_variable();
    }

    std::ostream& operator<<(std::ostream& ss, const AssertArgumentValue& val)
    {
        return ss << val.value;
    }

    void
    on_assert
    (
        const char* const expression,
        int line,
        const char* const file,
        const char* const argstr,
        const std::vector<AssertArgumentValue>& arguments,
        const char* function
    )
    {
        const auto start =
            "Assertion failed: {}\n"
            "Function: {}\n"
            "File: {}:{}\n"_format(expression, function, file, line);

        const auto with_arguments = arguments.empty()
            ? start
            : "{}({}) = {}\n"_format(start, argstr, arguments)
            ;

        const auto trace = run_backtrace(2);
        const auto message = trace.empty()
            ? with_arguments
            : "{}Backtrace:\n{}\n"_format(with_arguments, fmt::join(trace, "\n"))
            ;

        if(should_throw_variable())
        {
            throw std::move(message);
        }
        else
        {
            std::cerr << message;
            exit(-1);
        }
    }
}


#endif
