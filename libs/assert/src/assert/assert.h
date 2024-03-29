#pragma once

#include <string>
#include <vector>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#define BREAK_IN_DEBUG() __debugbreak()
#else
#define BREAK_IN_DEBUG() \
    do \
    { \
    } while(false)
#endif

#ifdef RELEASE

// todo(Gustav): implement assert for windows...

#define ASSERT(x)
#define ASSERTX(x, ...)
#define DIE(message)

#else

#define IMPLEMENT_ASSERT_LIB

// todo(Gustav): stb libraries and rapidjson aren't using our assert
#define ASSERT(x) \
    do \
    { \
        if(x) \
        { \
        } \
        else \
        { \
            if(::eu::assertlib::is_throwing() == false) { BREAK_IN_DEBUG(); } \
            ::eu::assertlib::on_assert( \
                    #x, \
                    __LINE__, \
                    __FILE__, \
                    "", \
                    {}, \
                    __PRETTY_FUNCTION__); \
        } \
    } while(false)

#define ASSERTX(x, ...) \
    do \
    { \
        if(x) \
        { \
        } \
        else \
        { \
            if(::eu::assertlib::is_throwing() == false) { BREAK_IN_DEBUG(); } \
            ::eu::assertlib::on_assert( \
                    #x, \
                    __LINE__, \
                    __FILE__, \
                    #__VA_ARGS__, \
                    {__VA_ARGS__}, \
                    __PRETTY_FUNCTION__); \
        } \
    } while(false)

#define DIE(message) \
    ::eu::assertlib::on_assert( \
            message, \
            __LINE__, \
            __FILE__, \
            "", \
            {}, \
            __PRETTY_FUNCTION__)

#define DIEX(message, ...) \
    ::eu::assertlib::on_assert( \
            message, \
            __LINE__, \
            __FILE__, \
            #__VA_ARGS__, \
            {__VA_ARGS__}, \
            __PRETTY_FUNCTION__)

#endif // _MSC_VER

#ifdef IMPLEMENT_ASSERT_LIB

namespace eu::assertlib
{
    struct AssertArgumentValue
    {
        std::string value;

        template <typename T>
        AssertArgumentValue(const T& t)
            : value(fmt::to_string(t))
            {}
    };

    void
    begin_throwing();

    bool
    is_throwing();

    void
    on_assert
    (
        const char* expression,
        int line,
        const char* file,
        const char* argstr,
        const std::vector<AssertArgumentValue>& arguments,
        const char* function
    );
}
#endif // IMPLEMENT_ASSERT_LIB

