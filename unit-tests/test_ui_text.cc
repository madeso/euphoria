#include "core/ui_text.h"
#include "catch.hpp"

namespace euco       = euphoria::core;
namespace textparser = euphoria::core::textparser;

TEST_CASE("textfileparser-test", "[textparser]")
{
    euco::UiText parser;

    SECTION("text")
    {
        parser.CreateText("hello");
        CHECK(textparser::VisitorDebugString::Visit(&parser) == "{text hello}");
    }

    SECTION("image")
    {
        CHECK(parser.CreateParse("@hello"));
        CHECK(textparser::VisitorDebugString::Visit(&parser)
              == "{image hello}");
    }

    SECTION("begin and end")
    {
        CHECK(parser.CreateParse("{hello}"));
        CHECK(textparser::VisitorDebugString::Visit(&parser)
              == "{begin}{text hello}{end}");
    }

    SECTION("begin begin")
    {
        CHECK(parser.CreateParse("{{"));
        CHECK(textparser::VisitorDebugString::Visit(&parser)
              == "{begin}{begin}");
    }

    SECTION("begin image end")
    {
        CHECK(parser.CreateParse("{@dog}"));
        CHECK(textparser::VisitorDebugString::Visit(&parser)
              == "{begin}{image dog}{end}");
    }

    SECTION("example")
    {
        CHECK(parser.CreateParse("@abort Abort"));
        CHECK(textparser::VisitorDebugString::Visit(&parser)
              == "{image abort}{text Abort}");
    }

    SECTION("no escape")
    {
        CHECK(parser.CreateParse("my@email"));
        CHECK(textparser::VisitorDebugString::Visit(&parser)
              == "{text my}{image email}");
    }

    SECTION("with escape")
    {
        CHECK(parser.CreateParse("my\\@email"));
        CHECK(textparser::VisitorDebugString::Visit(&parser)
              == "{text my@email}");
    }
}
