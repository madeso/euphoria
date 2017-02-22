#ifndef CORE_ASSERT_H
#define CORE_ASSERT_H

namespace assertlib {
void OnAssert(const char* const expression, int line, const char* const file, const char* const function);
}

#define assert(x) do { if(x) {} else { ::assertlib::OnAssert(#x, __LINE__, __FILE__, __FUNCTION__); } } while(false)

#endif  // CORE_ASSERT_H
