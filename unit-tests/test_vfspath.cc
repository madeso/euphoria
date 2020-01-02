#include "core/vfs_path.h"

#include "catch.hpp"
#include "utils.h"

namespace vfs = euphoria::core::vfs;


TEST_CASE("vfspath-dir-root", "[vfspath]")
{
    SECTION("rooted")
    {
        const auto root = vfs::PathToDirectory::FromRoot();
        REQUIRE_FALSE(root.IsRelative());
        REQUIRE("~/" == root.path);
    }

    SECTION("sub")
    {
        const auto folder =
            vfs::PathToDirectory::FromRoot().SingleCdCopy("dog");
        REQUIRE_FALSE(folder.IsRelative());
        REQUIRE("~/dog/" == folder.path);
    }
}


TEST_CASE("vfspath-dir-relative", "[vfspath]")
{
    SECTION("relative")
    {
        const auto relative = vfs::PathToDirectory::FromRelative();
        REQUIRE(relative.IsRelative());
        REQUIRE("./" == relative.path);
    }

    SECTION("sub")
    {
        const auto folder =
            vfs::PathToDirectory::FromRelative().SingleCdCopy("dog");
        REQUIRE(folder.IsRelative());
        REQUIRE("./dog/" == folder.path);
    }
}


TEST_CASE("vfspath-dir-cd", "[vfspath]")
{
    SECTION("root")
    {
        const auto dir = vfs::PathToDirectory::FromRoot();
        CHECK_THAT
        (
            dir.SplitDirectories(),
            CATCH_IS_VECTOR(std::string, "~")
        );
    }

    SECTION("single_dir")
    {
        const auto dir = vfs::PathToDirectory{"~/cat/"};
        CHECK_THAT
        (
            dir.SplitDirectories(),
            CATCH_IS_VECTOR(std::string, "~", "cat")
        );
    }

    SECTION("double_dir")
    {
        const auto dir = vfs::PathToDirectory{"~/cat/grumpy/"};
        CHECK_THAT
        (
            dir.SplitDirectories(),
            CATCH_IS_VECTOR(std::string, "~", "cat", "grumpy")
        );
    }

    SECTION("relative_dir")
    {
        const auto dir = vfs::PathToDirectory{"./../sub/"};
        CHECK_THAT
        (
            dir.SplitDirectories(),
            CATCH_IS_VECTOR(std::string, ".", "..", "sub")
        );
    }
}


TEST_CASE("vfspath-dir-relative_test", "[vfspath]")
{
    SECTION("absolute")
    {
        const auto dir = vfs::PathToDirectory("~/dog/../cat/");
        CHECK_FALSE(dir.IsRelative());
        CHECK(dir.ContainsRelative());
    }

    SECTION("truly_absolute")
    {
        const auto dir = vfs::PathToDirectory("~/dog/good/");
        CHECK_FALSE(dir.IsRelative());
        CHECK_FALSE(dir.ContainsRelative());
    }

    SECTION("relative")
    {
        const auto dir = vfs::PathToDirectory("./cat/");
        CHECK(dir.IsRelative());
        CHECK(dir.ContainsRelative());
    }

    SECTION("also_relative")
    {
        const auto dir = vfs::PathToDirectory("./dog/../cat/");
        CHECK(dir.IsRelative());
        CHECK(dir.ContainsRelative());
    }
}


TEST_CASE("vfspath-dir-vector_constructor", "[vfspath]")
{
    SECTION("rooted")
    {
        const auto dir = vfs::PathToDirectory::FromDirs({"~", "dog", "good"});
        CHECK(dir.path == "~/dog/good/");
    }

    SECTION("relative")
    {
        const auto dir = vfs::PathToDirectory::FromDirs({".", "cat", "grumpy"});
        CHECK(dir.path == "./cat/grumpy/");
    }
}


TEST_CASE("vfspath-dir-resolve", "[vfspath]")
{
    const auto root = vfs::PathToDirectory::FromRoot();
    const auto relative = vfs::PathToDirectory::FromRelative();
    const auto one_back = vfs::PathToDirectory{"./../"};

    SECTION("absolute_root")
    {
        const auto odir = vfs::ResolveRelative(root);
        REQUIRE(odir.has_value());
        const auto dir = odir.value();
        CHECK(dir.path == "~/");
    }

    SECTION("absolute_root_sub")
    {
        const auto odir = vfs::ResolveRelative(root.SingleCdCopy("dog"));
        REQUIRE(odir.has_value());
        const auto dir = odir.value();
        CHECK(dir.path == "~/dog/");
    }

    SECTION("root_and_relative")
    {
        const auto odir = vfs::ResolveRelative(relative, root);
        REQUIRE(odir.has_value());
        const auto dir = odir.value();
        CHECK(dir.path == "~/");
    }

    SECTION("root_and_relative_sub")
    {
        const auto odir = vfs::ResolveRelative
        (
            relative.SingleCdCopy("dog"),
            root
        );
        REQUIRE(odir.has_value());
        const auto dir = odir.value();
        CHECK(dir.path == "~/dog/");
    }

    SECTION("root_sub_and_relative_sub")
    {
        const auto odir = vfs::ResolveRelative
        (
            relative.SingleCdCopy("grumpy"),
            root.SingleCdCopy("cat")
        );
        REQUIRE(odir.has_value());
        const auto dir = odir.value();
        CHECK(dir.path == "~/cat/grumpy/");
    }

    SECTION("one_back_from_root")
    {
        const auto odir = vfs::ResolveRelative(one_back, root);
        REQUIRE_FALSE(odir.has_value());
    }

    SECTION("one_back_from_sub")
    {
        const auto odir = vfs::ResolveRelative
        (
            one_back,
            root.SingleCdCopy("sub")
        );
        REQUIRE(odir.has_value());
        const auto dir = odir.value();
        CHECK(dir.path == "~/");
    }

    SECTION("complex")
    {
        const auto odir = vfs::ResolveRelative
        (
            vfs::PathToDirectory{"~/cat/grumpy/../../dog/bad/../good/"}
        );
        REQUIRE(odir.has_value());
        const auto dir = odir.value();
        CHECK(dir.path == "~/dog/good/");
    }
}




TEST_CASE("path-test_empty_create_guess", "[path]")
{
    auto p = vfs::Path::FromGuess("");
    REQUIRE_FALSE(p.IsFile());
    REQUIRE(p.IsDirectory());
    REQUIRE("" == p.GetAbsolutePath());
    REQUIRE("" == p.GetParentDirectory().GetAbsolutePath());
}

TEST_CASE("path-test_empty_create_dir", "[path]")
{
    auto p = vfs::Path::FromDirectory("");
    REQUIRE_FALSE(p.IsFile());
    REQUIRE(p.IsDirectory());
    REQUIRE("" == p.GetAbsolutePath());
    REQUIRE("" == p.GetParentDirectory().GetAbsolutePath());
}

TEST_CASE("path-test_create_file", "[path]")
{
    auto p = vfs::Path::FromFile("hello");
    REQUIRE(p.IsFile());
    REQUIRE_FALSE(p.IsDirectory());
    REQUIRE("hello" == p.GetAbsolutePath());
    REQUIRE("" == p.GetDirectory().GetAbsolutePath());
}

TEST_CASE("path-test_create_directory", "[path]")
{
    auto p = vfs::Path::FromDirectory("hello");
    REQUIRE_FALSE(p.IsFile());
    REQUIRE(p.IsDirectory());
    REQUIRE("hello/" == p.GetAbsolutePath());
    REQUIRE("" == p.GetParentDirectory().GetAbsolutePath());
}

TEST_CASE("path-test_create_sub_directory", "[path]")
{
    auto p = vfs::Path::FromDirectory("hello/world");
    REQUIRE_FALSE(p.IsFile());
    REQUIRE(p.IsDirectory());
    REQUIRE("hello/world/" == p.GetAbsolutePath());
    REQUIRE("hello/" == p.GetParentDirectory().GetAbsolutePath());
}

TEST_CASE("path-test_get_sub_directory", "[path]")
{
    auto p = vfs::Path::FromDirectory("hello");
    p      = p.GetSubDirectory("world");
    REQUIRE_FALSE(p.IsFile());
    REQUIRE(p.IsDirectory());
    REQUIRE("hello/world/" == p.GetAbsolutePath());
    REQUIRE("hello/" == p.GetParentDirectory().GetAbsolutePath());
}

TEST_CASE("path-test_get_file", "[path]")
{
    auto p = vfs::Path::FromDirectory("hello");
    p      = p.GetFile("world");
    REQUIRE(p.IsFile());
    REQUIRE_FALSE(p.IsDirectory());
    REQUIRE("hello/world" == p.GetAbsolutePath());
    REQUIRE("hello/" == p.GetDirectory().GetAbsolutePath());
}
