#ifndef CORE_ASSERT_H
#define CORE_ASSERT_H

#if 0

#include <cassert>
#define Assert(x) assert(x)

#else

namespace assertlib {
void StartThrowing();
void OnAssert(const char* const expression, int line, const char* const file, const char* const function);
}

// todo: stb libraries and rapidjson aren't using our assert
#define Assert(x) do { if(x) {} else { ::assertlib::OnAssert(#x, __LINE__, __FILE__, __PRETTY_FUNCTION__); } } while(false)

#endif

#endif  // CORE_ASSERT_H
