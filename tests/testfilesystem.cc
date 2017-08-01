#include "gtest/gtest.h"
#include "core/filesystem.h"

#define GTEST(X) TEST(filesystem, X)

class AlwaysExist : public FileSystem {
 public:
  bool LoadFileToString(const std::string& path, std::string* source) override {
    return true;
  }
};

class NeverExist : public FileSystem {
 public:
  bool LoadFileToString(const std::string& path, std::string* source) override {
    return false;
  }
};

GTEST(test_basic) {
  AlwaysExist always;
  EXPECT_TRUE(always.LoadFileToString("dog", nullptr));

  NeverExist never;
  EXPECT_FALSE(never.LoadFileToString("dog", nullptr));
}

GTEST(test_catalog_with_null) {
  CatalogFileSystem catalog {nullptr};
  catalog.RegisterFile("dog", "happy");

  std::string content;
  EXPECT_TRUE(catalog.LoadFileToString("dog", &content));
  EXPECT_EQ("happy", content);

  EXPECT_FALSE(catalog.LoadFileToString("cat", &content));
}


GTEST(test_catalog_with_backing_never) {
  NeverExist never;
  CatalogFileSystem catalog {&never};
  catalog.RegisterFile("dog", "happy");

  std::string content;
  EXPECT_TRUE(catalog.LoadFileToString("dog", &content));
  EXPECT_EQ("happy", content);

  EXPECT_FALSE(catalog.LoadFileToString("cat", &content));
}
