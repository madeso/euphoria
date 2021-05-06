#include "core/vfs.h"
#include "core/vfs_path.h"

#include "catch.hpp"


#define GTEST(X) TEST(filesystem, X)

using namespace euphoria::core::vfs;

namespace euco = euphoria::core;


struct AlwaysExist : public FileSystemReadRoot
{
public:
    std::shared_ptr<euco::memory_chunk>
    ReadFile(const FilePath&) override
    {
        // alloc some garbage
        return euco::memory_chunk::allocate(32);
    }

    void
    Describe(std::vector<std::string>*) override
    {}

    FileList
    ListFiles(const DirPath&) override
    {
        FileList ret;
        return ret;
    }
};


struct NeverExist : public FileSystemReadRoot
{
public:
    std::shared_ptr<euco::memory_chunk>
    ReadFile(const FilePath&) override
    {
        return euco::memory_chunk::null();
    }

    void
    Describe(std::vector<std::string>*) override
    {}

    FileList
    ListFiles(const DirPath&) override
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
        REQUIRE(always.ReadFile(FilePath{"~/dog"}) != nullptr);
    }

    SECTION("never")
    {
        FileSystem never;
        never.AddReadRoot(std::make_shared<NeverExist>());
        REQUIRE(never.ReadFile(FilePath{"~/dog"}) == nullptr);
    }
}


TEST_CASE("vfs-test_catalog_with_null", "[vfs]")
{
    FileSystem fs;
    auto catalog = FileSystemRootCatalog::AddRoot(&fs);
    catalog->RegisterFileString(FilePath{"~/dog"}, "happy");

    std::string content;

    SECTION("can read stored file")
    {
        REQUIRE(fs.ReadFileToString(FilePath{"~/dog"}, &content));
        REQUIRE(content == "happy");
    }

    SECTION("error when trying to read missing file")
    {
        REQUIRE_FALSE(fs.ReadFileToString(FilePath{"~/cat"}, &content));
    }
}

// todo(Gustav): add test with overriding files from different roots
