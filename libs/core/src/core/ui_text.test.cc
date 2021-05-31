#include "core/ui_text.h"
#include "catch.hpp"

namespace euco = euphoria::core;
namespace textparser = euphoria::core::textparser;

TEST_CASE("textfileparser-test", "[textparser]")
{
    euco::ui_text parser;

    SECTION("text")
    {
        parser.init_with_text("hello");
        CHECK(textparser::visitor_debug_string::accept_all_nodes(&parser) == "{text hello}");
    }

    SECTION("image")
    {
        CHECK(parser.init_by_parsing_source("@hello"));
        CHECK(textparser::visitor_debug_string::accept_all_nodes(&parser)
              == "{image hello}");
    }

    SECTION("begin and end")
    {
        CHECK(parser.init_by_parsing_source("{hello}"));
        CHECK(textparser::visitor_debug_string::accept_all_nodes(&parser)
              == "{begin}{text hello}{end}");
    }

    SECTION("begin begin")
    {
        CHECK(parser.init_by_parsing_source("{{"));
        CHECK(textparser::visitor_debug_string::accept_all_nodes(&parser)
              == "{begin}{begin}");
    }

    SECTION("begin image end")
    {
        CHECK(parser.init_by_parsing_source("{@dog}"));
        CHECK(textparser::visitor_debug_string::accept_all_nodes(&parser)
              == "{begin}{image dog}{end}");
    }

    SECTION("example")
    {
        CHECK(parser.init_by_parsing_source("@abort Abort"));
        CHECK(textparser::visitor_debug_string::accept_all_nodes(&parser)
              == "{image abort}{text Abort}");
    }

    SECTION("no escape")
    {
        CHECK(parser.init_by_parsing_source("my@email"));
        CHECK(textparser::visitor_debug_string::accept_all_nodes(&parser)
              == "{text my}{image email}");
    }

    SECTION("with escape")
    {
        CHECK(parser.init_by_parsing_source("my\\@email"));
        CHECK(textparser::visitor_debug_string::accept_all_nodes(&parser)
              == "{text my@email}");
    }
}
