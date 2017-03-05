#include "gmock/gmock.h"
#include "core/radix.h"

#include "testbase.h"

#include <vector>
#include <string>

struct IntType {
  static int GetId(int d) {
    return d;
  }
};

struct Dummy {
  int id;
  std::string name;

  Dummy(int i, const std::string& n) : id(i), name(n) {}

  static int GetId(const Dummy& d) {
    return d.id;
  }

  bool operator==(const Dummy& rhs) const {
    return id == rhs.id && name == rhs.name;
  }
};
void PrintTo(const Dummy& d, ::std::ostream* os) {
  *os << d.id << ": " << d.name;
}

template<typename T>
struct TBuilder {
  TBuilder<T>& operator<< (T t) {
    data.push_back(t);
    return *this;
  }

  operator const std::vector<T>&() const {
    return data;
  }

  std::vector<T> data;
};

struct Builder {
  Builder& operator()(int i, const std::string& n) {
    data.push_back(Dummy{i, n});
    return *this;
  }

  operator const std::vector<Dummy>&() const {
    return data;
  }

  std::vector<Dummy> data;
};

struct RadixTest : public ::testing::Test {
  RadixTest()
  : sorted1( Builder()(1, "dog")(2, "cat")(5, "pony")(24, "bird")(25, "horse")(123, "fish")(8390, "badger") )
  , unsorted1( Builder()(25, "horse")(24, "bird")(5, "pony")(1, "dog")(8390, "badger")(123, "fish")(2, "cat") )
  , sorted2( TBuilder<int>() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 11 << 12 << 13 << 14 << 15 << 16 << 17 << 18 << 19 << 20 << 21 << 22 << 23 << 24 << 25)
  , unsorted2( TBuilder<int>() << 5 << 12 << 13 << 24 << 4 << 21 << 8 << 16 << 6 << 20 << 15 << 25 << 17 << 3 << 22 << 14 << 9 << 10 << 18 << 19 << 2 << 11 << 23 << 7 << 1 )
  { }

  std::vector<Dummy> sorted1;
  std::vector<Dummy> unsorted1;

  std::vector<int> sorted2;
  std::vector<int> unsorted2;
};

#define GTEST(X) TEST_F(RadixTest, X)

GTEST(bitshift_test) {
  EXPECT_EQ(0, Bucket16base<int>::GetIndex(0, 0));
  EXPECT_EQ(0, Bucket16base<int>::GetIndex(16, 0));
  EXPECT_EQ(1, Bucket16base<int>::GetIndex(16, 1));
  EXPECT_EQ(4, Bucket16base<int>::GetIndex(4, 0));
  EXPECT_EQ(1, Bucket16base<int>::GetIndex(17, 0));
}

GTEST(already_sorted10) {
  std::vector<Dummy> d = sorted1;
  RadixSort<Dummy, Dummy, Bucket10base<int>, int>(&d);
  ASSERT_THAT(d, ::testing::ElementsAreArray(sorted1));
}

GTEST(not_sorted10) {
  std::vector<Dummy> d = unsorted1;
  RadixSort<Dummy, Dummy, Bucket10base<int>, int>(&d);
  ASSERT_THAT(d, ::testing::ElementsAreArray(sorted1));
}

GTEST(already_sorted16) {
  std::vector<Dummy> d = sorted1;
  RadixSort<Dummy, Dummy, Bucket16base<int>, int>(&d);
  ASSERT_THAT(d, ::testing::ElementsAreArray(sorted1));
}

GTEST(not_sorted16) {
  std::vector<Dummy> d = unsorted1;
  RadixSort<Dummy, Dummy, Bucket16base<int>, int>(&d);
  ASSERT_THAT(d, ::testing::ElementsAreArray(sorted1));
}

////////////////////////////////////////////////////////////////////////////////

GTEST(already_sorted10int) {
  std::vector<int> d = sorted2;
  RadixSort<int, IntType, Bucket10base<int>, int>(&d);
  ASSERT_THAT(d, ::testing::ElementsAreArray(sorted2));
}

GTEST(not_sorted10int) {
  std::vector<int> d = unsorted2;
  RadixSort<int, IntType, Bucket10base<int>, int>(&d);
  ASSERT_THAT(d, ::testing::ElementsAreArray(sorted2));
}

GTEST(already_sorted16int) {
  std::vector<int> d = sorted2;
  RadixSort<int, IntType, Bucket16base<int>, int>(&d);
  ASSERT_THAT(d, ::testing::ElementsAreArray(sorted2));
}

GTEST(not_sorted16int) {
  std::vector<int> d = unsorted2;
  RadixSort<int, IntType, Bucket16base<int>, int>(&d);
  ASSERT_THAT(d, ::testing::ElementsAreArray(sorted2));
}

