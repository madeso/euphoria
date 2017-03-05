#include "gmock/gmock.h"
#include "core/radix.h"

#include "testbase.h"

#include <vector>
#include <string>

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
  { }

  std::vector<Dummy> sorted1;
  std::vector<Dummy> unsorted1;
};

#define GTEST(X) TEST_F(RadixTest, X)

GTEST(already_sorted) {
  std::vector<Dummy> d = sorted1;
  RadixSort<Dummy, Dummy, Bucket10base<int>, int>(&d);
  ASSERT_THAT(d, ::testing::ElementsAreArray(sorted1));
}

GTEST(not_sorted) {
  std::vector<Dummy> d = unsorted1;
  RadixSort<Dummy, Dummy, Bucket10base<int>, int>(&d);
  ASSERT_THAT(d, ::testing::ElementsAreArray(sorted1));
}

