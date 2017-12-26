#include "core/textparser.h"
#include "catch.hpp"


TEST_CASE("textfileparser-test", "[textparser]")
{
  TextParser parser;
  textparser::NodeCreatorBasic creator;

  SECTION("text")
  {
    parser.CreateText("hello", &creator);
    CHECK(textparser::VisitorDebugString::Visit(&parser) == "{text hello}");
  }

  SECTION("image")
  {
    CHECK(parser.CreateParse("@hello", &creator));
    CHECK(textparser::VisitorDebugString::Visit(&parser) == "{image hello}");
  }

  SECTION("var")
  {
    CHECK(parser.CreateParse("{hello}", &creator));
    CHECK(textparser::VisitorDebugString::Visit(&parser) == "{var hello}");
  }

  SECTION("example")
  {
    CHECK(parser.CreateParse("@abort Abort", &creator));
    CHECK(textparser::VisitorDebugString::Visit(&parser) == "{image abort}{text Abort}");
  }
}

