#include "core/texttemplate.h"
#include "io/vfs.h"
#include "io/vfs_path.h"

#include "catch.hpp"

namespace euco = eu::core;


TEST_CASE("template-test_replace", "[template]")
{
    euco::CompiledTextTemplate t {"Hello {{@sender}}!"};
    REQUIRE_FALSE(t.errors.has_errors());
    REQUIRE(t.errors.get_combined_errors().empty());

    euco::Defines defines;
    defines.set("sender", "Buffy");
    REQUIRE(t.build_string(defines) == "Hello Buffy!");
    REQUIRE(t.errors.get_combined_errors().empty());
}


TEST_CASE("template-test_if", "[template]")
{
    euco::CompiledTextTemplate t {"{{ifdef sender}}Hello {{@sender}}!{{end}}"};
    REQUIRE_FALSE(t.errors.has_errors());
    REQUIRE(t.errors.get_combined_errors().empty());

    euco::Defines defines_with_sender;
    defines_with_sender.set("sender", "Buffy");
    euco::Defines empty_define;

    REQUIRE(t.build_string(empty_define).empty());
    REQUIRE(t.errors.get_combined_errors().empty());

    REQUIRE(t.build_string(defines_with_sender) == "Hello Buffy!");
    REQUIRE(t.errors.get_combined_errors().empty());
}


TEST_CASE("template-test_define", "[template]")
{
    euco::CompiledTextTemplate t {"{{set sender \"Buffy\"}}Hello {{@sender}}!"};
    REQUIRE_FALSE(t.errors.has_errors());
    REQUIRE(t.errors.get_combined_errors().empty());

    euco::Defines defines;
    REQUIRE(t.build_string(defines) == "Hello Buffy!");
    REQUIRE_FALSE(defines.is_defined("sender"));
    REQUIRE(t.errors.get_combined_errors().empty());
}


TEST_CASE("template-test_only_code", "[template]")
{
    euco::CompiledTextTemplate t {"{{set sender \"Buffy\" @sender}}"};
    REQUIRE_FALSE(t.errors.has_errors());
    REQUIRE(t.errors.get_combined_errors().empty());

    euco::Defines defines;
    REQUIRE(t.build_string(defines) == "Buffy");
    REQUIRE_FALSE(defines.is_defined("sender"));
    REQUIRE(t.errors.get_combined_errors().empty());
}


TEST_CASE("template-test_basic_filesystem", "[template]")
{
    eu::io::FileSystem filesys;
    auto catalog = eu::io::ReadRootCatalog::create_and_add(&filesys);
    catalog->register_file_string(eu::io::FilePath{"~/main"}, "main");

    euco::CompiledTextTemplate t {&filesys, eu::io::FilePath{"~/main"}};
    REQUIRE_FALSE(t.errors.has_errors());
    REQUIRE(t.errors.get_combined_errors().empty());

    euco::Defines defines;
    REQUIRE(t.build_string(defines) == "main");
    REQUIRE(t.errors.get_combined_errors().empty());
}


TEST_CASE("template-test_include_filesystem", "[template]")
{
    eu::io::FileSystem filesys;
    auto catalog = eu::io::ReadRootCatalog::create_and_add(&filesys);
    catalog->register_file_string
    (
        eu::io::FilePath{"~/main"},
        "{{include \"included\"}}"
    );
    catalog->register_file_string
    (
        eu::io::FilePath{"~/included"},
        "included"
    );

    euco::CompiledTextTemplate t {&filesys, eu::io::FilePath{"~/main"}};
    REQUIRE(t.errors.get_combined_errors().empty());
    REQUIRE_FALSE(t.errors.has_errors());

    euco::Defines defines;
    REQUIRE(t.build_string(defines) == "included");
    REQUIRE(t.errors.get_combined_errors().empty());
}


TEST_CASE("template-test_scoping_filesystem", "[template]")
{
    eu::io::FileSystem filesys;
    auto catalog = eu::io::ReadRootCatalog::create_and_add(&filesys);
    catalog->register_file_string
    (
        eu::io::FilePath{"~/main"},
        "{{include \"included\"}} {{@var}}!"
    );
    catalog->register_file_string
    (
        eu::io::FilePath{"~/included"},
        "{{set var \"hello\" @var}}"
    );

    euco::CompiledTextTemplate t {&filesys, eu::io::FilePath{"~/main"}};
    REQUIRE(t.errors.get_combined_errors().empty());
    REQUIRE_FALSE(t.errors.has_errors());

    euco::Defines defines;
    defines.set("var", "world");
    REQUIRE(t.build_string(defines) == "hello world!");
    REQUIRE(t.errors.get_combined_errors().empty());
}
