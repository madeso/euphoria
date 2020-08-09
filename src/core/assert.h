#ifndef CORE_ASSERT_H
#define CORE_ASSERT_H

#include <string>
#include <vector>
#include <sstream>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#define BREAK_IN_DEBUG() __debugbreak()
#else
#define BREAK_IN_DEBUG()                                                       \
    do                                                                         \
    {                                                                          \
    } while(false)
#endif

#ifdef RELEASE

// todo: implement assert for windows...

#define ASSERT(x)
#define ASSERTX(x, ...)
#define DIE(message)

#else

#define IMPLEMENT_ASSERT_LIB

// todo: stb libraries and rapidjson aren't using our assert
#define ASSERT(x)                                                              \
    do                                                                         \
    {                                                                          \
        if(x)                                                                  \
        {                                                                      \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            if(::euphoria::core::assertlib::IsThrowing() == false) { BREAK_IN_DEBUG(); }                                                  \
            ::euphoria::core::assertlib::OnAssert(                             \
                    #x,                                                        \
                    __LINE__,                                                  \
                    __FILE__,                                                  \
                    "",                                                        \
                    {},                                                        \
                    __PRETTY_FUNCTION__);      \
        }                                                                      \
    } while(false)

#define ASSERTX(x, ...)                                                        \
    do                                                                         \
    {                                                                          \
        if(x)                                                                  \
        {                                                                      \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            if(::euphoria::core::assertlib::IsThrowing() == false) { BREAK_IN_DEBUG(); }                                                  \
            ::euphoria::core::assertlib::OnAssert(                             \
                    #x,                                                        \
                    __LINE__,                                                  \
                    __FILE__,                                                  \
                    #__VA_ARGS__,                                              \
                    {__VA_ARGS__},                                             \
                    __PRETTY_FUNCTION__);      \
        }                                                                      \
    } while(false)

#define DIE(message)                                                           \
    ::euphoria::core::assertlib::OnAssert(                                     \
            message,                                                           \
            __LINE__,                                                          \
            __FILE__,                                                          \
            "",                                                                \
            {},                                                                \
            __PRETTY_FUNCTION__)

#endif  // _MSC_VER

#ifdef IMPLEMENT_ASSERT_LIB

namespace euphoria::core
{
    namespace assertlib
    {
        struct AssertValueArg
        {
            std::string value;
            template <typename T>
            AssertValueArg(const T& t)
            {
                std::ostringstream ss;
                ss << t;
                value = ss.str();
            }
        };

        void
        StartThrowing();

        bool
        IsThrowing();

        void
        OnAssert(
                const char* const                  expression,
                int                                line,
                const char* const                  file,
                const char* const                  argstr,
                const std::vector<AssertValueArg>& arguments,
                const char* const                  function);
    }  // namespace assertlib

}  // namespace euphoria::core
#endif  // IMPLEMENT_ASSERT_LIB


#endif  // CORE_ASSERT_H
