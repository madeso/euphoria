#include "core/filesystem.h"
#include "catch.hpp"

#define GTEST(X) TEST(filesystem, X)

class AlwaysExist : public FileSystemRoot
{
 public:
  std::shared_ptr<MemoryChunk>
  ReadFile(const std::string& path) override
  {
    // alloc some garbage
    return MemoryChunk::Alloc(32);
  }
};

class NeverExist : public FileSystemRoot
{
 public:
  std::shared_ptr<MemoryChunk>
  ReadFile(const std::string& path) override
  {
    return MemoryChunk::Null();
  }
};

TEST_CASE("vfs-test_basic", "[vfs]")
{
  SECTION("always")
  {
    FileSystem always;
    always.AddRoot(std::make_shared<AlwaysExist>());
    REQUIRE(always.ReadFile("dog") != nullptr);
  }

  SECTION("never")
  {
    FileSystem never;
    never.AddRoot(std::make_shared<NeverExist>());
    REQUIRE(never.ReadFile("dog") == nullptr);
  }
}

TEST_CASE("vfs-test_catalog_with_null", "[vfs]")
{
  FileSystem fs;
  auto       catalog = FileSystemRootCatalog::AddRoot(&fs);
  catalog->RegisterFileString("dog", "happy");

  std::string content;

  SECTION("can read stored file")
  {
    REQUIRE(fs.ReadFileToString("dog", &content));
    REQUIRE(content == "happy");
  }

  SECTION("error when trying to read missing file")
  {
    REQUIRE_FALSE(fs.ReadFileToString("cat", &content));
  }
}

// todo: add test with overriding files from different roots
