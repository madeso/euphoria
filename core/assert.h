#ifndef CORE_ASSERT_H
#define CORE_ASSERT_H

#include <string>
#include <vector>
#include <sstream>

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#define BREAK_IN_DEBUG() __debugbreak()
#else
#define BREAK_IN_DEBUG() do { } while(false)
#endif

#ifdef RELEASE

// todo: implement assert for windows...

#define ASSERT(x)
#define ASSERTX(x, ...)
#define DIE(message)

#else

#define IMPLEMENT_ASSERT_LIB

// todo: stb libraries and rapidjson aren't using our assert
#define ASSERT(x)                                               \
  do                                                            \
  {                                                             \
    if(x)                                                       \
    {                                                           \
    }                                                           \
    else                                                        \
    {                                                           \
      BREAK_IN_DEBUG();                                         \
      ::assertlib::OnAssert(                                    \
          #x,                                                   \
          __LINE__,                                             \
          __FILE__,                                             \
          "",                                                   \
          {},                                                   \
          static_cast<const char* const>(__PRETTY_FUNCTION__)); \
    }                                                           \
  } while(false)

#define ASSERTX(x, ...)                                         \
  do                                                            \
  {                                                             \
    if(x)                                                       \
    {                                                           \
    }                                                           \
    else                                                        \
    {                                                           \
      BREAK_IN_DEBUG();                                         \
      ::assertlib::OnAssert(                                    \
          #x,                                                   \
          __LINE__,                                             \
          __FILE__,                                             \
          #__VA_ARGS__,                                         \
          {__VA_ARGS__},                                        \
          static_cast<const char* const>(__PRETTY_FUNCTION__)); \
    }                                                           \
  } while(false)

#define DIE(message)     \
  ::assertlib::OnAssert( \
      message,           \
      __LINE__,          \
      __FILE__,          \
      "",                \
      {},                \
      static_cast<const char* const>(__PRETTY_FUNCTION__))

#endif  // _MSC_VER

#ifdef IMPLEMENT_ASSERT_LIB
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
  void
  OnAssert(
      const char* const                  expression,
      int                                line,
      const char* const                  file,
      const char* const                  argstr,
      const std::vector<AssertValueArg>& arguments,
      const char* const                  function);
}  // namespace assertlib
#endif  // IMPLEMENT_ASSERT_LIB


#endif  // CORE_ASSERT_H
