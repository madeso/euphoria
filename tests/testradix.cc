#include "gtest/gtest.h"
#include "core/radix.h"
#include <vector>

#define GTEST(X) TEST(radix, X)

struct Dummy {
  int id;

  Dummy(int i) : id(i) {}

  static int GetId(const Dummy& d) {
    return d.id;
  }
};

GTEST(already_sorted) {
  std::vector<Dummy> d;
  d.push_back(3);
  d.push_back(5);
  d.push_back(9);
  RadixSort<Dummy, Dummy, Bucket10base<int>, int>(&d);
  EXPECT_EQ(3, d[0].id);
  EXPECT_EQ(5, d[1].id);
  EXPECT_EQ(9, d[2].id);
}

GTEST(not_sorted) {
  std::vector<Dummy> d;
  d.push_back(9);
  d.push_back(3);
  d.push_back(5);

  RadixSort<Dummy, Dummy, Bucket10base<int>, int>(&d);
  EXPECT_EQ(3, d[0].id);
  EXPECT_EQ(5, d[1].id);
  EXPECT_EQ(9, d[2].id);
}

