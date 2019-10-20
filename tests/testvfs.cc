#include "core/vfs.h"
#include "catch.hpp"

#define GTEST(X) TEST(filesystem, X)

using namespace euphoria::core::vfs;

namespace euco = euphoria::core;

struct AlwaysExist : public FileSystemReadRoot
{
public:
    std::shared_ptr<euco::MemoryChunk>
    ReadFile(const std::string&) override
    {
        // alloc some garbage
        return euco::MemoryChunk::Alloc(32);
    }

    void
    Describe(std::vector<std::string>*) override
    {}

    FileList
    ListFiles(const Path&) override
    {
        FileList ret;
        return ret;
    }
};

struct NeverExist : public FileSystemReadRoot
{
public:
    std::shared_ptr<euco::MemoryChunk>
    ReadFile(const std::string&) override
    {
        return euco::MemoryChunk::Null();
    }


    void
    Describe(std::vector<std::string>*) override
    {}

    FileList
    ListFiles(const Path&) override
    {
        FileList ret;
        return ret;
    }
};

TEST_CASE("vfs-test_basic", "[vfs]")
{
    SECTION("always")
    {
        FileSystem always;
        always.AddReadRoot(std::make_shared<AlwaysExist>());
        REQUIRE(always.ReadFile("dog") != nullptr);
    }

    SECTION("never")
    {
        FileSystem never;
        never.AddReadRoot(std::make_shared<NeverExist>());
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
