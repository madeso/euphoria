#include "core/vfs.h"
#include "core/vfs_path.h"

#include "catch.hpp"


#define GTEST(X) TEST(filesystem, X)

using namespace eu::core::vfs;

namespace euco = eu::core;


struct AlwaysExistRoot : public ReadRoot
{
public:
    std::shared_ptr<euco::MemoryChunk>
    read_file(const FilePath&) override
    {
        // alloc some garbage
        return euco::MemoryChunk::allocate(32);
    }

    void
    add_description(std::vector<std::string>*) override
    {}

    FileList
    list_files(const DirPath&) override
    {
        FileList ret;
        return ret;
    }
};


struct NeverExistRoot : public ReadRoot
{
public:
    std::shared_ptr<euco::MemoryChunk>
    read_file(const FilePath&) override
    {
        return euco::MemoryChunk::create_null();
    }

    void
    add_description(std::vector<std::string>*) override
    {}

    FileList
    list_files(const DirPath&) override
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
        always.add_read_root(std::make_shared<AlwaysExistRoot>());
        REQUIRE(always.read_file(FilePath{"~/dog"}) != nullptr);
    }

    SECTION("never")
    {
        FileSystem never;
        never.add_read_root(std::make_shared<NeverExistRoot>());
        REQUIRE(never.read_file(FilePath{"~/dog"}) == nullptr);
    }
}


TEST_CASE("vfs-test_catalog_with_null", "[vfs]")
{
    FileSystem fs;
    auto catalog = ReadRootCatalog::create_and_add(&fs);
    catalog->register_file_string(FilePath{"~/dog"}, "happy");

    SECTION("can read stored file")
    {
        auto content = fs.read_file_to_string(FilePath{ "~/dog" });
        REQUIRE(content);
        REQUIRE(*content == "happy");
    }

    SECTION("error when trying to read missing file")
    {
        REQUIRE_FALSE(fs.read_file_to_string(FilePath{"~/cat"}));
    }
}

// todo(Gustav): add test with overriding files from different roots
