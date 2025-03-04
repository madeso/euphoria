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

namespace eu
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
                std::extent_v<decltype(addresses)>
            );
            const std::unique_ptr<char*, decltype(&std::free)> symbols
            (
                ::backtrace_symbols(static_cast<void* const*>(addresses), n),
                &std::free
            );

            std::vector<std::string> ret;
            for(int backtrace_index = skip; backtrace_index < n; backtrace_index += 1)
            {
                // we parse the symbols retrieved from backtrace_symbols() to
                // extract the "real" symbols that represent the mangled names.
                char* const symbol = symbols.get()[backtrace_index];
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
                    ret.emplace_back(fmt::format("{}{}+{}", first_symbol, second_symbol, end));
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

template <> struct fmt::formatter<eu::assertlib::AssertArgumentValue> : formatter<std::string>
{
    template <typename FormatContext>
    auto format(const eu::assertlib::AssertArgumentValue& c, FormatContext& ctx) const
    {
        return formatter<string_view>::format(c.value, ctx);
    }
};
   
namespace eu::assertlib
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
        const auto start = fmt::format(
            "Assertion failed: {}\n"
            "Function: {}\n"
            "File: {}:{}\n", expression, function, file, line);

        const auto with_arguments = arguments.empty()
            ? start
            : fmt::format("{}({}) = {}\n", start, argstr, fmt::join(arguments, ", "))
            ;

        const auto trace = run_backtrace(2);
        const auto message = trace.empty()
            ? with_arguments
            : fmt::format("{}Backtrace:\n{}\n", with_arguments, fmt::join(trace, "\n"))
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
