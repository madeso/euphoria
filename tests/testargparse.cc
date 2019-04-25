#include "core/argparse.h"

#include "catch.hpp"

struct Entry
{
  bool ok;
  std::string text;

  bool operator!=(const Entry& rhs) const
  {
    return ok != rhs.ok || text != rhs.text;
  }
};

enum class MyEnum
{
  X, Cat, ReallyLongValue
};

BEGIN_ENUM_LIST(MyEnum)
  ENUM_VALUE(MyEnum, X)
  ENUM_VALUE(MyEnum, Cat)
  ENUM_VALUE(MyEnum, ReallyLongValue)
END_ENUM_LIST()

std::ostream& operator<<(std::ostream& s, const Entry& e)
{
  if(!e.ok) s << "(ERR): ";
  s << e.text;
  return s;
}

struct TestOutput : public argparse::Output
{
  std::vector<Entry> out;

  void OnError(const std::string& err) override
  {
    out.push_back({false, err});
  }

  void OnInfo(const std::string& info) override
  {
    out.push_back({true, info});
  }
};

/////////////////////////////////////////////////////////////////////////////

#define IS_STRING_VECTOR(...) Catch::Matchers::Equals(std::vector<std::string>{__VA_ARGS__} )

TEST_CASE("name tests", "[argparse]")
{
  using argparse::Name;

  SECTION("optional 1")
  {
    auto n = Name {"-int"};
    CHECK(n.is_optional);
    CHECK_THAT(n.names, IS_STRING_VECTOR("int"));
  }

  SECTION("optional 2")
  {
    auto n = Name {"-int,-i"};
    CHECK(n.is_optional);
    CHECK_THAT(n.names, IS_STRING_VECTOR("int", "i"));
  }

  SECTION("positional")
  {
    // todo: this looks weird and is unexpected...
    auto n = Name {"dog"};
    CHECK_FALSE(n.is_optional);
    CHECK_THAT(n.names, IS_STRING_VECTOR("dog"));
  }
}


TEST_CASE("argparse", "[argparse]")
{
  const std::string app          = "hello.exe";
  const auto        empty_output = std::vector<Entry>{};
  auto              output       = TestOutput{};
  auto              parser       = argparse::Parser{"description"};
  parser.output                  = &output;

  SECTION("test empty")
  {
    auto r = parser.Parse(app, {});
    CHECK(r == argparse::ParseResult::Ok);
		REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));
  }

  SECTION("optional enum")
  {
    auto e = MyEnum::ReallyLongValue;
    parser.AddEnum("-e", &e);

    SECTION("default")
    {
      auto r = parser.Parse(app, {});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));

      CHECK(e == MyEnum::ReallyLongValue);
    }

    SECTION("supplied")
    {
      auto r = parser.Parse(app, {"-e", "x"});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));

      CHECK(e == MyEnum::X);
    }
  }

  SECTION("optional int")
  {
    int i = 4;
    parser.AddSimple("-int", &i);

    SECTION("default")
    {
      auto r = parser.Parse(app, {});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));

      CHECK(i == 4);
    }
    SECTION("supplied")
    {
      auto r = parser.Parse(app, {"-int", "42"});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));

      CHECK(i == 42);
    }
  }

  SECTION("optional bool")
  {
    bool t = true;
    bool f = false;
    parser.AddSimple("-t", &t);
    parser.AddSimple("-f", &f);

    SECTION("default")
    {
      auto r = parser.Parse(app, {});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));

      CHECK(t);
      CHECK_FALSE(f);
    }

    SECTION("supplied 01")
    {
      auto r = parser.Parse(app, {"-t", "0", "-f", "1"});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));
      CHECK_FALSE(t);
      CHECK(f);
    }

    SECTION("supplied yn")
    {
      auto r = parser.Parse(app, {"-t", "n", "-f", "y"});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));
      CHECK_FALSE(t);
      CHECK(f);
    }

    SECTION("supplied yes/no")
    {
      auto r = parser.Parse(app, {"-t", "no", "-f", "yes"});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));
      CHECK_FALSE(t);
      CHECK(f);
    }
  }

  SECTION("optional string")
  {
    std::string str = "dog";
    parser.AddSimple("-str", &str);

    SECTION("default")
    {
      auto r = parser.Parse(app, {});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));

      CHECK(str == "dog");
    }
    SECTION("supplied")
    {
      auto r = parser.Parse(app, {"-str", "cat"});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));

      CHECK(str == "cat");
    }
  }

  SECTION("optional vector")
  {
    std::vector<int> v;
    parser.AddVector("-v", &v);

    SECTION("default")
    {
      auto r = parser.Parse(app, {});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));

      CHECK_THAT(v, Catch::Matchers::Equals( std::vector<int>{} ));
    }
    SECTION("supplied 1")
    {
      auto r = parser.Parse(app, {"-v", "43"});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));

      CHECK_THAT(v, Catch::Matchers::Equals( std::vector<int>{43} ));
    }
    SECTION("supplied 3")
    {
      auto r = parser.Parse(app, {"-v", "1", "-v", "2", "-v", "3"});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));

      CHECK_THAT(v, Catch::Matchers::Equals( std::vector<int>{1, 2, 3} ));
    }
  }

  SECTION("optional function")
  {
    bool called = false;
    parser.AddSimpleFunction("-f", [&called]() {called = true;});

    SECTION("empty")
    {
      auto r = parser.Parse(app, {});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));
      CHECK_FALSE(called);
    }

    SECTION("called")
    {
      auto r = parser.Parse(app, {"-f"});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));
      CHECK(called);
    }
  }

  ///////////////////////////////////////////////////////////////////////////////////
  // positional

  SECTION("positional vector")
  {
    std::vector<int> v;
    parser.AddVector("v", &v);

    SECTION("default")
    {
      auto r = parser.Parse(app, {});
      CHECK(r == argparse::ParseResult::Failed);
      // todo: verify error
      // REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));

      CHECK_THAT(v, Catch::Matchers::Equals( std::vector<int>{} ));
    }
    SECTION("supplied 1")
    {
      auto r = parser.Parse(app, {"43"});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));

      CHECK_THAT(v, Catch::Matchers::Equals( std::vector<int>{43} ));
    }
    SECTION("supplied 3")
    {
      auto r = parser.Parse(app, {"1", "2", "3"});
      CHECK(r == argparse::ParseResult::Ok);
      REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));

      CHECK_THAT(v, Catch::Matchers::Equals( std::vector<int>{1, 2, 3} ));
    }
  }
}
