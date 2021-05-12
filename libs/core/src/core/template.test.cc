#include "core/texttemplate.h"
#include "core/vfs.h"
#include "core/vfs_path.h"

#include "catch.hpp"

namespace euco = euphoria::core;
namespace vfs  = euphoria::core::vfs;


TEST_CASE("template-test_replace", "[template]")
{
    euco::compiled_text_template t {"Hello {{@sender}}!"};
    REQUIRE_FALSE(t.errors.has_errors());
    REQUIRE(t.errors.get_combined_errors().empty());

    euco::defines defines;
    defines.define("sender", "Buffy");
    REQUIRE(t.evaluate(defines) == "Hello Buffy!");
    REQUIRE(t.errors.get_combined_errors().empty());
}


TEST_CASE("template-test_if", "[template]")
{
    euco::compiled_text_template t {"{{ifdef sender}}Hello {{@sender}}!{{end}}"};
    REQUIRE_FALSE(t.errors.has_errors());
    REQUIRE(t.errors.get_combined_errors().empty());

    euco::defines defines_with_sender;
    defines_with_sender.define("sender", "Buffy");
    euco::defines empty_define;

    REQUIRE(t.evaluate(empty_define).empty());
    REQUIRE(t.errors.get_combined_errors().empty());

    REQUIRE(t.evaluate(defines_with_sender) == "Hello Buffy!");
    REQUIRE(t.errors.get_combined_errors().empty());
}


TEST_CASE("template-test_define", "[template]")
{
    euco::compiled_text_template t {"{{set sender \"Buffy\"}}Hello {{@sender}}!"};
    REQUIRE_FALSE(t.errors.has_errors());
    REQUIRE(t.errors.get_combined_errors().empty());

    euco::defines defines;
    REQUIRE(t.evaluate(defines) == "Hello Buffy!");
    REQUIRE_FALSE(defines.is_defined("sender"));
    REQUIRE(t.errors.get_combined_errors().empty());
}


TEST_CASE("template-test_only_code", "[template]")
{
    euco::compiled_text_template t {"{{set sender \"Buffy\" @sender}}"};
    REQUIRE_FALSE(t.errors.has_errors());
    REQUIRE(t.errors.get_combined_errors().empty());

    euco::defines defines;
    REQUIRE(t.evaluate(defines) == "Buffy");
    REQUIRE_FALSE(defines.is_defined("sender"));
    REQUIRE(t.errors.get_combined_errors().empty());
}


TEST_CASE("template-test_basic_filesystem", "[template]")
{
    vfs::file_system filesys;
    auto catalog = vfs::read_root_catalog::create_and_add(&filesys);
    catalog->register_file_string(vfs::file_path{"~/main"}, "main");

    euco::compiled_text_template t {&filesys, vfs::file_path{"~/main"}};
    REQUIRE_FALSE(t.errors.has_errors());
    REQUIRE(t.errors.get_combined_errors().empty());

    euco::defines defines;
    REQUIRE(t.evaluate(defines) == "main");
    REQUIRE(t.errors.get_combined_errors().empty());
}


TEST_CASE("template-test_include_filesystem", "[template]")
{
    vfs::file_system filesys;
    auto catalog = vfs::read_root_catalog::create_and_add(&filesys);
    catalog->register_file_string
    (
        vfs::file_path{"~/main"},
        "{{include \"included\"}}"
    );
    catalog->register_file_string
    (
        vfs::file_path{"~/included"},
        "included"
    );

    euco::compiled_text_template t {&filesys, vfs::file_path{"~/main"}};
    REQUIRE(t.errors.get_combined_errors().empty());
    REQUIRE_FALSE(t.errors.has_errors());

    euco::defines defines;
    REQUIRE(t.evaluate(defines) == "included");
    REQUIRE(t.errors.get_combined_errors().empty());
}


TEST_CASE("template-test_scoping_filesystem", "[template]")
{
    vfs::file_system filesys;
    auto catalog = vfs::read_root_catalog::create_and_add(&filesys);
    catalog->register_file_string
    (
        vfs::file_path{"~/main"},
        "{{include \"included\"}} {{@var}}!"
    );
    catalog->register_file_string
    (
        vfs::file_path{"~/included"},
        "{{set var \"hello\" @var}}"
    );

    euco::compiled_text_template t {&filesys, vfs::file_path{"~/main"}};
    REQUIRE(t.errors.get_combined_errors().empty());
    REQUIRE_FALSE(t.errors.has_errors());

    euco::defines defines;
    defines.define("var", "world");
    REQUIRE(t.evaluate(defines) == "hello world!");
    REQUIRE(t.errors.get_combined_errors().empty());
}
