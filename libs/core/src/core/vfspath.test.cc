#include "core/vfs_path.h"

#include "tests/utils.h"

#include "catch.hpp"


namespace vfs = euphoria::core::vfs;


TEST_CASE("vfspath-dir-root", "[vfspath]")
{
    SECTION("rooted")
    {
        const auto root = vfs::dir_path::from_root();
        REQUIRE_FALSE(root.is_relative());
        REQUIRE("~/" == root.path);
    }

    SECTION("sub")
    {
        const auto folder =
            vfs::dir_path::from_root().single_cd_copy("dog");
        REQUIRE_FALSE(folder.is_relative());
        REQUIRE("~/dog/" == folder.path);
    }
}


TEST_CASE("vfspath-dir-relative", "[vfspath]")
{
    SECTION("relative")
    {
        const auto relative = vfs::dir_path::from_relative();
        REQUIRE(relative.is_relative());
        REQUIRE("./" == relative.path);
    }

    SECTION("sub")
    {
        const auto folder =
            vfs::dir_path::from_relative().single_cd_copy("dog");
        REQUIRE(folder.is_relative());
        REQUIRE("./dog/" == folder.path);
    }
}


TEST_CASE("vfspath-dir-cd", "[vfspath]")
{
    SECTION("root")
    {
        const auto dir = vfs::dir_path::from_root();
        CHECK_THAT
        (
            dir.split_directories(),
            CATCH_IS_VECTOR(std::string, "~")
        );
    }

    SECTION("single_dir")
    {
        const auto dir = vfs::dir_path{"~/cat/"};
        CHECK_THAT
        (
            dir.split_directories(),
            CATCH_IS_VECTOR(std::string, "~", "cat")
        );
    }

    SECTION("double_dir")
    {
        const auto dir = vfs::dir_path{"~/cat/grumpy/"};
        CHECK_THAT
        (
            dir.split_directories(),
            CATCH_IS_VECTOR(std::string, "~", "cat", "grumpy")
        );
    }

    SECTION("relative_dir")
    {
        const auto dir = vfs::dir_path{"./../sub/"};
        CHECK_THAT
        (
            dir.split_directories(),
            CATCH_IS_VECTOR(std::string, ".", "..", "sub")
        );
    }
}


TEST_CASE("vfspath-dir-relative_test", "[vfspath]")
{
    SECTION("absolute")
    {
        const auto dir = vfs::dir_path("~/dog/../cat/");
        CHECK_FALSE(dir.is_relative());
        CHECK(dir.contains_relative());
    }

    SECTION("truly_absolute")
    {
        const auto dir = vfs::dir_path("~/dog/good/");
        CHECK_FALSE(dir.is_relative());
        CHECK_FALSE(dir.contains_relative());
    }

    SECTION("relative")
    {
        const auto dir = vfs::dir_path("./cat/");
        CHECK(dir.is_relative());
        CHECK(dir.contains_relative());
    }

    SECTION("also_relative")
    {
        const auto dir = vfs::dir_path("./dog/../cat/");
        CHECK(dir.is_relative());
        CHECK(dir.contains_relative());
    }
}


TEST_CASE("vfspath-dir-vector_constructor", "[vfspath]")
{
    SECTION("rooted")
    {
        const auto dir = vfs::dir_path::from_dirs({"~", "dog", "good"});
        CHECK(dir.path == "~/dog/good/");
    }

    SECTION("relative")
    {
        const auto dir = vfs::dir_path::from_dirs({".", "cat", "grumpy"});
        CHECK(dir.path == "./cat/grumpy/");
    }
}


TEST_CASE("vfspath-dir-resolve", "[vfspath]")
{
    const auto root = vfs::dir_path::from_root();
    const auto relative = vfs::dir_path::from_relative();
    const auto one_back = vfs::dir_path{"./../"};

    SECTION("absolute_root")
    {
        const auto odir = vfs::resolve_relative(root);
        REQUIRE(odir.has_value());
        const auto dir = odir.value();
        CHECK(dir.path == "~/");
    }

    SECTION("absolute_root_sub")
    {
        const auto odir = vfs::resolve_relative(root.single_cd_copy("dog"));
        REQUIRE(odir.has_value());
        const auto dir = odir.value();
        CHECK(dir.path == "~/dog/");
    }

    SECTION("root_and_relative")
    {
        const auto odir = vfs::resolve_relative(relative, root);
        REQUIRE(odir.has_value());
        const auto dir = odir.value();
        CHECK(dir.path == "~/");
    }

    SECTION("root_and_relative_sub")
    {
        const auto odir = vfs::resolve_relative
        (
            relative.single_cd_copy("dog"),
            root
        );
        REQUIRE(odir.has_value());
        const auto dir = odir.value();
        CHECK(dir.path == "~/dog/");
    }

    SECTION("root_sub_and_relative_sub")
    {
        const auto odir = vfs::resolve_relative
        (
            relative.single_cd_copy("grumpy"),
            root.single_cd_copy("cat")
        );
        REQUIRE(odir.has_value());
        const auto dir = odir.value();
        CHECK(dir.path == "~/cat/grumpy/");
    }

    SECTION("one_back_from_root")
    {
        const auto odir = vfs::resolve_relative(one_back, root);
        REQUIRE_FALSE(odir.has_value());
    }

    SECTION("one_back_from_sub")
    {
        const auto odir = vfs::resolve_relative
        (
            one_back,
            root.single_cd_copy("sub")
        );
        REQUIRE(odir.has_value());
        const auto dir = odir.value();
        CHECK(dir.path == "~/");
    }

    SECTION("complex")
    {
        const auto odir = vfs::resolve_relative
        (
            vfs::dir_path{"~/cat/grumpy/../../dog/bad/../good/"}
        );
        REQUIRE(odir.has_value());
        const auto dir = odir.value();
        CHECK(dir.path == "~/dog/good/");
    }
}



TEST_CASE("vfspath-file-constructor", "[vfspath]")
{
    SECTION("relative-file-txt")
    {
        const auto file = vfs::file_path{"./file.txt"};
        const auto [dir, name] = file.split_directories_and_file();
        CHECK(dir.path == "./");
        CHECK(name == "file.txt");
        CHECK(file.get_filename_without_extension() == "file");
        CHECK(file.get_extension() == "txt");
    }

    SECTION("dotdot-file-txt")
    {
        const auto file = vfs::file_path{"./../file.txt"};
        const auto [dir, name] = file.split_directories_and_file();
        CHECK(dir.path == "./../");
        CHECK(name == "file.txt");
        CHECK(file.get_filename_without_extension() == "file");
        CHECK(file.get_extension() == "txt");
    }

    SECTION("relative-no-ext")
    {
        const auto file = vfs::file_path{"./cat"};
        const auto [dir, name] = file.split_directories_and_file();
        CHECK(dir.path == "./");
        CHECK(name == "cat");
        CHECK(file.get_filename_without_extension() == "cat");
        CHECK(file.get_extension().empty());
    }

    SECTION("absolute-gitignore")
    {
        const auto file = vfs::file_path{"~/.gitignore"};
        const auto [dir, name] = file.split_directories_and_file();
        CHECK(dir.path == "~/");
        CHECK(name == ".gitignore");
        CHECK(file.get_filename_without_extension() == ".gitignore");
        CHECK(file.get_extension().empty());
    }

    SECTION("absolute-targz")
    {
        const auto file = vfs::file_path{"~/cats.tar.gz"};
        const auto [dir, name] = file.split_directories_and_file();
        CHECK(dir.path == "~/");
        CHECK(name == "cats.tar.gz");
        CHECK(file.get_filename_without_extension() == "cats");
        CHECK(file.get_extension() == "tar.gz");
    }
}


TEST_CASE("vfspath-file-from-dir", "[vfspath]")
{
    SECTION("basic")
    {
        const auto file = vfs::dir_path{"~/dogs/"}.get_file("good.dog");
        CHECK(file.path == "~/dogs/good.dog");
    }
}


TEST_CASE("vfspath-file-change_ext", "[vfspath]")
{
    SECTION("sub with ext")
    {
        const auto file = vfs::file_path{"~/dogs/dog.exe"}.set_extension_copy
        (
            "good"
        );
        CHECK(file.path == "~/dogs/dog.good");
    }
    SECTION("sub no ext")
    {
        const auto file = vfs::file_path{"~/dogs/dog"}.set_extension_copy
        (
            "good"
        );
        CHECK(file.path == "~/dogs/dog.good");
    }
    SECTION("dot dot ext")
    {
        const auto file = vfs::file_path{"./../dog.exe"}.set_extension_copy
        (
            "good"
        );
        CHECK(file.path == "./../dog.good");
    }
    SECTION("root with ext")
    {
        const auto file = vfs::file_path{"~/dog.exe"}.set_extension_copy
        (
            "good"
        );
        CHECK(file.path == "~/dog.good");
    }
    SECTION("root no ext")
    {
        const auto file = vfs::file_path{"~/dog"}.set_extension_copy
        (
            "good"
        );
        CHECK(file.path == "~/dog.good");
    }
}


TEST_CASE("vfspath-file-resolve-join", "[vfspath]")
{
    SECTION("basic")
    {
        const auto root = vfs::dir_path{"~/dogs/"};
        const auto relative = vfs::file_path{"./good.dog"};

        const auto joined = vfs::join(root, relative);
        CHECK(joined.path == "~/dogs/good.dog");

        const auto resolved = vfs::resolve_relative(relative, root);
        REQUIRE(resolved.has_value());
        CHECK(resolved.value().path == "~/dogs/good.dog");
    }

    SECTION("back")
    {
        const auto root = vfs::dir_path{"~/cats/"};
        const auto relative = vfs::file_path{"./../dogs/good.dog"};

        const auto joined = vfs::join(root, relative);
        CHECK(joined.path == "~/cats/../dogs/good.dog");

        const auto resolved = vfs::resolve_relative(relative, root);
        REQUIRE(resolved.has_value());
        CHECK(resolved.value().path == "~/dogs/good.dog");
    }

    SECTION("invalid")
    {
        const auto root = vfs::dir_path{"~/"};
        const auto relative = vfs::file_path{"./../dogs/good.dog"};

        const auto joined = vfs::join(root, relative);
        CHECK(joined.path == "~/../dogs/good.dog");

        const auto resolved = vfs::resolve_relative(relative, root);
        REQUIRE_FALSE(resolved.has_value());
    }
}
