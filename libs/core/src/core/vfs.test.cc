#include "core/vfs.h"
#include "core/vfs_path.h"

#include "catch.hpp"


#define GTEST(X) TEST(filesystem, X)

using namespace euphoria::core::vfs;

namespace euco = euphoria::core;


struct always_exist_root : public read_root
{
public:
    std::shared_ptr<euco::memory_chunk>
    read_file(const file_path&) override
    {
        // alloc some garbage
        return euco::memory_chunk::allocate(32);
    }

    void
    add_description(std::vector<std::string>*) override
    {}

    file_list
    list_files(const dir_path&) override
    {
        file_list ret;
        return ret;
    }
};


struct never_exist_root : public read_root
{
public:
    std::shared_ptr<euco::memory_chunk>
    read_file(const file_path&) override
    {
        return euco::memory_chunk::null();
    }

    void
    add_description(std::vector<std::string>*) override
    {}

    file_list
    list_files(const dir_path&) override
    {
        file_list ret;
        return ret;
    }
};


TEST_CASE("vfs-test_basic", "[vfs]")
{
    SECTION("always")
    {
        file_system always;
        always.add_read_root(std::make_shared<always_exist_root>());
        REQUIRE(always.read_file(file_path{"~/dog"}) != nullptr);
    }

    SECTION("never")
    {
        file_system never;
        never.add_read_root(std::make_shared<never_exist_root>());
        REQUIRE(never.read_file(file_path{"~/dog"}) == nullptr);
    }
}


TEST_CASE("vfs-test_catalog_with_null", "[vfs]")
{
    file_system fs;
    auto catalog = read_root_catalog::create_and_add(&fs);
    catalog->register_file_string(file_path{"~/dog"}, "happy");

    std::string content;

    SECTION("can read stored file")
    {
        REQUIRE(fs.read_file_to_string(file_path{"~/dog"}, &content));
        REQUIRE(content == "happy");
    }

    SECTION("error when trying to read missing file")
    {
        REQUIRE_FALSE(fs.read_file_to_string(file_path{"~/cat"}, &content));
    }
}

// todo(Gustav): add test with overriding files from different roots
