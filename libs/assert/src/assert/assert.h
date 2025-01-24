#pragma once

#include <string>
#include <vector>

/// implements a break in debug.
/// \hideinitializer
#define BREAK_IN_DEBUG() \
    do \
    { \
    } while(false)


#ifdef RELEASE

// todo(Gustav): implement assert for windows...

#define ASSERT(x)
#define ASSERTX(x, ...)
#define DIE(message)

#else

/// Defines that the assert support library should be implemented.
/// @todo Expose as as cmake variable instead of looking at the build flag.
#define IMPLEMENT_ASSERT_LIB

// todo(Gustav): stb libraries and rapidjson aren't using our assert
/// Assert that a value is true.
/// @param x the value to check.
/// \hideinitializer
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

/// Assert that a value is true.
/// @param x the value to check.
/// @param ... any extra value to attach to the assert message
/// \hideinitializer
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

/// Assert that something bad has happened.
/// @param message the reason to fail.
/// \hideinitializer
#define DIE(message) \
    ::eu::assertlib::on_assert( \
            message, \
            __LINE__, \
            __FILE__, \
            "", \
            {}, \
            __PRETTY_FUNCTION__)

/// Assert that something bad has happened.
/// @param message the reason to fail.
/// @param ... any extra value to attach to the assert message
/// \hideinitializer
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

/// \private
namespace eu::assertlib
{
    /// \private
    struct AssertArgumentValue
    {
        std::string value;

        template <typename T>
        AssertArgumentValue(const T& t)
            : value(fmt::to_string(t))
            {}
    };

    /// \private
    void
    begin_throwing();

    /// \private
    bool
    is_throwing();

    /// \private
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

