#include "core/textfileparser.h"

#include "catch.hpp"

TEST_CASE("textfileparser-test_basic_ident", "[textfileparser]")
{
  TextFileParser test{"Hello"};

  REQUIRE(test.HasMore());
  REQUIRE(test.ReadIdent() == "Hello");
  REQUIRE_FALSE(test.HasMore());
  REQUIRE(test.ReadIdent() == "");
  REQUIRE_FALSE(test.HasMore());
}

TEST_CASE("textfileparser-test_two_idents", "[textfileparser]")
{
  TextFileParser test{"Hello world"};

  REQUIRE(test.HasMore());
  REQUIRE(test.ReadIdent() == "Hello");
  REQUIRE(test.HasMore());
  REQUIRE(test.ReadIdent() == "");
  test.SkipSpaces(true);
  REQUIRE(test.ReadIdent() == "world");
  REQUIRE_FALSE(test.HasMore());
  REQUIRE(test.ReadIdent() == "");
  REQUIRE_FALSE(test.HasMore());
}

TEST_CASE("textfileparser-read_string_fail", "[textfileparser]")
{
  TextFileParser test{"Hello"};

  REQUIRE(test.HasMore());
  REQUIRE(test.ReadString() == "");
  REQUIRE(test.HasMore());
  REQUIRE(test.ReadIdent() == "Hello");
}

TEST_CASE("textfileparser-read_string", "[textfileparser]")
{
  TextFileParser test{"\"Hello\""};

  REQUIRE(test.HasMore());
  REQUIRE(test.ReadString() == "Hello");
  REQUIRE_FALSE(test.HasMore());
  REQUIRE(test.ReadString() == "");
}

TEST_CASE("textfileparser-read_to_eol", "[textfileparser]")
{
  TextFileParser test{"hello world\nhello dog"};

  REQUIRE(test.HasMore());
  REQUIRE(test.ReadToEndOfLine() == "hello world");
  REQUIRE(test.ReadIdent() == "hello");
  REQUIRE(test.ReadToEndOfLine() == " dog");
  REQUIRE_FALSE(test.HasMore());
}

TEST_CASE("textfileparser-peek_char", "[textfileparser]")
{
  TextFileParser test{"abc"};

  REQUIRE(test.HasMore());
  REQUIRE(test.PeekChar() == 'a');
  REQUIRE(test.PeekChar(0) == 'a');
  REQUIRE(test.PeekChar(1) == 'b');
  REQUIRE(static_cast<int>(test.PeekChar(3)) == 0);
}
