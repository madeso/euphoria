#include "gmock/gmock.h"
#include "gmock/gmock-matchers.h"
#include "core/tablelayout.h"

#define GTEST(X) TEST(tablelayout, X)
using namespace testing;

bool IsEqual(int lhs, int rhs) {
  return lhs == rhs;
}

template<typename T>
bool operator==(const std::vector<T>& lhs, const std::vector<T>& rhs) {
  const auto s = lhs.size();
  if( s != rhs.size() ) return false;
  for(typename std::vector<T>::size_type i = 0; i<s; ++i) {
    if( IsEqual(lhs[i], rhs[i]) == false ) return false;
  }
  return true;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& v)
{
  stream << "[";

  bool first = true;

  for(T t : v) {
    if( first ) {
      first = false;
    }
    else {
      stream << ", ";
    }
    stream << t;
  }

  stream << "]";
  return stream;
}

template<typename T>
std::vector<T> MakeVec(std::initializer_list<T> args) {
  std::vector<T> r {args};
  return r;
}

GTEST(basic) {
  EXPECT_EQ(MakeVec({3, 2}), PerformTableLayout<int>({3,2}, 12));
}

GTEST(one_expand) {
  EXPECT_EQ(MakeVec({3, 10, 2}), PerformTableLayout<int>({3, -1, 2}, 15));
}

GTEST(two_expand) {
  EXPECT_EQ(MakeVec({5.0f, 3.0f, 2.0f, 5.0f}), PerformTableLayout<float>({-1, 3, 2, -1}, 15));
}

GTEST(min_size) {
  EXPECT_EQ(MakeVec({5, 0, 5}), PerformTableLayout<int>({5, -1, 5}, 10));
}

GTEST(min_size_not_default) {
  EXPECT_EQ(MakeVec({5, -10, 5}), PerformTableLayout<int>({5, -1, 5}, 10, -10));
}

GTEST(smaller_size) {
  EXPECT_EQ(MakeVec({2.5f, 0.0f, 2.5f}), PerformTableLayout<float>({5, -1, 5}, 5));
}

GTEST(smaller_size_not_default) {
  EXPECT_EQ(MakeVec({2.5f, -2.0f, 2.5f}), PerformTableLayout<float>({5, -1, 5}, 5, -2));
}

