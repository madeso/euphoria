#include "core/template.h"
#include "core/vfs.h"
#include "core/vfs_path.h"

#include "catch.hpp"

namespace euco = euphoria::core;
namespace vfs  = euphoria::core::vfs;


TEST_CASE("template-test_replace", "[template]")
{
    euco::Template t {"Hello {{@sender}}!"};
    REQUIRE_FALSE(t.errors.HasErrors());
    REQUIRE(t.errors.GetCombinedErrors() == "");

    euco::Defines defines;
    defines.Define("sender", "Buffy");
    REQUIRE(t.Evaluate(defines) == "Hello Buffy!");
    REQUIRE(t.errors.GetCombinedErrors() == "");
}


TEST_CASE("template-test_if", "[template]")
{
    euco::Template t {"{{ifdef sender}}Hello {{@sender}}!{{end}}"};
    REQUIRE_FALSE(t.errors.HasErrors());
    REQUIRE(t.errors.GetCombinedErrors() == "");

    euco::Defines defines_with_sender;
    defines_with_sender.Define("sender", "Buffy");
    euco::Defines empty_define;

    REQUIRE(t.Evaluate(empty_define) == "");
    REQUIRE(t.errors.GetCombinedErrors() == "");

    REQUIRE(t.Evaluate(defines_with_sender) == "Hello Buffy!");
    REQUIRE(t.errors.GetCombinedErrors() == "");
}


TEST_CASE("template-test_define", "[template]")
{
    euco::Template t {"{{set sender \"Buffy\"}}Hello {{@sender}}!"};
    REQUIRE_FALSE(t.errors.HasErrors());
    REQUIRE(t.errors.GetCombinedErrors() == "");

    euco::Defines defines;
    REQUIRE(t.Evaluate(defines) == "Hello Buffy!");
    REQUIRE_FALSE(defines.IsDefined("sender"));
    REQUIRE(t.errors.GetCombinedErrors() == "");
}


TEST_CASE("template-test_only_code", "[template]")
{
    euco::Template t {"{{set sender \"Buffy\" @sender}}"};
    REQUIRE_FALSE(t.errors.HasErrors());
    REQUIRE(t.errors.GetCombinedErrors() == "");

    euco::Defines defines;
    REQUIRE(t.Evaluate(defines) == "Buffy");
    REQUIRE_FALSE(defines.IsDefined("sender"));
    REQUIRE(t.errors.GetCombinedErrors() == "");
}


TEST_CASE("template-test_basic_filesystem", "[template]")
{
    vfs::FileSystem filesys;
    auto catalog = vfs::FileSystemRootCatalog::AddRoot(&filesys);
    catalog->RegisterFileString(vfs::FilePath{"~/main"}, "main");

    euco::Template t {&filesys, vfs::FilePath{"~/main"}};
    REQUIRE_FALSE(t.errors.HasErrors());
    REQUIRE(t.errors.GetCombinedErrors() == "");

    euco::Defines defines;
    REQUIRE(t.Evaluate(defines) == "main");
    REQUIRE(t.errors.GetCombinedErrors() == "");
}


TEST_CASE("template-test_include_filesystem", "[template]")
{
    vfs::FileSystem filesys;
    auto catalog = vfs::FileSystemRootCatalog::AddRoot(&filesys);
    catalog->RegisterFileString
    (
        vfs::FilePath{"~/main"},
        "{{include \"included\"}}"
    );
    catalog->RegisterFileString
    (
        vfs::FilePath{"~/included"},
        "included"
    );

    euco::Template t {&filesys, vfs::FilePath{"~/main"}};
    REQUIRE(t.errors.GetCombinedErrors() == "");
    REQUIRE_FALSE(t.errors.HasErrors());

    euco::Defines defines;
    REQUIRE(t.Evaluate(defines) == "included");
    REQUIRE(t.errors.GetCombinedErrors() == "");
}


TEST_CASE("template-test_scoping_filesystem", "[template]")
{
    vfs::FileSystem filesys;
    auto catalog = vfs::FileSystemRootCatalog::AddRoot(&filesys);
    catalog->RegisterFileString
    (
        vfs::FilePath{"~/main"},
        "{{include \"included\"}} {{@var}}!"
    );
    catalog->RegisterFileString
    (
        vfs::FilePath{"~/included"},
        "{{set var \"hello\" @var}}"
    );

    euco::Template t {&filesys, vfs::FilePath{"~/main"}};
    REQUIRE(t.errors.GetCombinedErrors() == "");
    REQUIRE_FALSE(t.errors.HasErrors());

    euco::Defines defines;
    defines.Define("var", "world");
    REQUIRE(t.Evaluate(defines) == "hello world!");
    REQUIRE(t.errors.GetCombinedErrors() == "");
}
