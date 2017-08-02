#include "gtest/gtest.h"
#include "core/filesystem.h"

#define GTEST(X) TEST(filesystem, X)

class AlwaysExist : public FileSystemRoot {
 public:
  std::shared_ptr<MemoryChunk> ReadFile(const std::string& path) override {
    // alloc some garbage
    return MemoryChunk::Alloc(32);
  }
};

class NeverExist : public FileSystemRoot {
 public:
  std::shared_ptr<MemoryChunk> ReadFile(const std::string& path) override {
    return MemoryChunk::Null();
  }
};

GTEST(test_basic) {
  FileSystem always;
  always.AddRoot(std::make_shared<AlwaysExist>());
  EXPECT_NE(nullptr, always.ReadFile("dog"));

  FileSystem never;
  never.AddRoot(std::make_shared<NeverExist>());
  EXPECT_EQ(nullptr, never.ReadFile("dog"));
}

GTEST(test_catalog_with_null) {
  FileSystem fs;
  auto catalog = FileSystemRootCatalog::AddRoot(&fs);
  catalog->RegisterFileString("dog", "happy");

  std::string content;
  EXPECT_TRUE(fs.ReadFileToString("dog", &content));
  EXPECT_EQ("happy", content);

  EXPECT_FALSE(fs.ReadFileToString("cat", &content));
}


// todo: add test with overriding files from different roots
