#include "core/argparse.h"

#include "tests/utils.h"

namespace euco = euphoria::core;

namespace
{
    struct Entry
    {
        bool        ok;
        std::string text;

        bool
        operator!=(const Entry& rhs) const
        {
            return ok != rhs.ok || text != rhs.text;
        }
    };
}  // namespace

#define ERROR_LINE(err)                                                        \
    std::vector<Entry>                                                         \
    {                                                                          \
        Entry                                                                  \
        {                                                                      \
            false, err                                                         \
        }                                                                      \
    }

enum class MyEnum
{
    X,
    Cat,
    ReallyLongValue
};

DECLARE_ENUM_LIST(MyEnum)
BEGIN_ENUM_LIST(MyEnum)
ENUM_VALUE(MyEnum, X)
ENUM_VALUE(MyEnum, Cat)
ENUM_VALUE(MyEnum, ReallyLongValue)
END_ENUM_LIST()

std::ostream&
operator<<(std::ostream& s, const Entry& e)
{
    if(!e.ok)
        s << "(ERR): ";
    s << e.text;
    return s;
}

struct TestOutput : public euco::argparse::Output
{
    std::vector<Entry> out;

    void
    OnError(const std::string& err) override
    {
        out.push_back({false, err});
    }

    void
    OnInfo(const std::string& info) override
    {
        out.push_back({true, info});
    }
};

/////////////////////////////////////////////////////////////////////////////

#define IS_STRING_VECTOR(...)                                                  \
    Catch::Matchers::Equals(std::vector<std::string> {__VA_ARGS__})

TEST_CASE("name tests", "[argparse]")
{
    using euco::argparse::Name;

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
    const auto        empty_output = std::vector<Entry> {};
    auto              output       = TestOutput {};
    auto              parser       = euco::argparse::Parser {"description"};
    parser.output                  = &output;

    SECTION("test empty")
    {
        auto r = parser.Parse(app, {});
        CHECK(r == euco::argparse::ParseResult::Ok);
        REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
    }

    SECTION("optional enum")
    {
        auto e = MyEnum::ReallyLongValue;
        parser.AddEnum("-e", &e);

        SECTION("default")
        {
            auto r = parser.Parse(app, {});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));

            CHECK(e == MyEnum::ReallyLongValue);
        }

        SECTION("supplied")
        {
            auto r = parser.Parse(app, {"-e", "x"});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));

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
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));

            CHECK(i == 4);
        }
        SECTION("supplied")
        {
            auto r = parser.Parse(app, {"-int", "42"});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));

            CHECK(i == 42);
        }

        SECTION("passed string")
        {
            auto r = parser.Parse(app, {"-int", "cat"});
            CHECK(r == euco::argparse::ParseResult::Failed);
            REQUIRE_THAT(
                    output.out,
                    Catch::Matchers::Equals(
                            ERROR_LINE("cat for -int is not accepted.")));
            CHECK(i == 4);
        }

        SECTION("passed int+string")
        {
            auto r = parser.Parse(app, {"-int", "42cat"});
            CHECK(r == euco::argparse::ParseResult::Failed);
            CHECK_THAT(
                    output.out,
                    Catch::Matchers::Equals(
                            ERROR_LINE("42cat for -int is not accepted.")));
            CHECK(i == 4);
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
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));

            CHECK(t);
            CHECK_FALSE(f);
        }

        SECTION("supplied 01")
        {
            auto r = parser.Parse(app, {"-t", "0", "-f", "1"});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK_FALSE(t);
            CHECK(f);
        }

        SECTION("supplied yn")
        {
            auto r = parser.Parse(app, {"-t", "n", "-f", "y"});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK_FALSE(t);
            CHECK(f);
        }

        SECTION("supplied yes/no")
        {
            auto r = parser.Parse(app, {"-t", "no", "-f", "yes"});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
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
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));

            CHECK(str == "dog");
        }
        SECTION("supplied")
        {
            auto r = parser.Parse(app, {"-str", "cat"});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));

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
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));

            CHECK_THAT(v, Catch::Matchers::Equals(std::vector<int> {}));
        }
        SECTION("supplied 1")
        {
            auto r = parser.Parse(app, {"-v", "43"});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));

            CHECK_THAT(v, Catch::Matchers::Equals(std::vector<int> {43}));
        }
        SECTION("supplied 3")
        {
            auto r = parser.Parse(app, {"-v", "1", "-v", "2", "-v", "3"});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));

            CHECK_THAT(v, Catch::Matchers::Equals(std::vector<int> {1, 2, 3}));
        }
    }

    SECTION("optional function")
    {
        bool called = false;
        parser.AddSimpleFunction("-f", [&called]() { called = true; });

        SECTION("empty")
        {
            auto r = parser.Parse(app, {});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK_FALSE(called);
        }

        SECTION("called")
        {
            auto r = parser.Parse(app, {"-f"});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(called);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////
    // count

    SECTION("count")
    {
        int c = 0;
        parser.AddCount("-c", &c);

        SECTION("zero")
        {
            auto r = parser.Parse(app, {});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(c == 0);
        }

        SECTION("one")
        {
            auto r = parser.Parse(app, {"-c"});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(c == 1);
        }

        SECTION("many")
        {
            auto r = parser.Parse(app, {"-c", "-c"});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(c == 2);
        }
    }

    SECTION("set true/false")
    {
        bool t = false;
        bool f = true;
        parser.SetTrue("-t", &t);
        parser.SetFalse("-f", &f);

        SECTION("empty")
        {
            CHECK(parser.Parse(app, {}) == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK_FALSE(t);
            CHECK(f);
        }

        SECTION("set")
        {
            CHECK(parser.Parse(app, {"-t", "-f"})
                  == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(t);
            CHECK_FALSE(f);
        }
    }

    SECTION("combine")
    {
        bool a = false;
        bool b = false;
        bool c = false;
        parser.SetTrue("-a", &a);
        parser.SetTrue("-b", &b);
        parser.SetTrue("-c", &c);
        auto args = GENERATE_AS(std::string, "-abc", "-bac", "-cba");
        CHECK(parser.Parse(app, {args}) == euco::argparse::ParseResult::Ok);
        REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
        CHECK(a);
        CHECK(b);
        CHECK(c);
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
            CHECK(r == euco::argparse::ParseResult::Failed);
            // todo: verify error
            // REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));

            CHECK_THAT(v, Catch::Matchers::Equals(std::vector<int> {}));
        }
        SECTION("supplied 1")
        {
            auto r = parser.Parse(app, {"43"});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));

            CHECK_THAT(v, Catch::Matchers::Equals(std::vector<int> {43}));
        }
        SECTION("supplied 3")
        {
            auto r = parser.Parse(app, {"1", "2", "3"});
            CHECK(r == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));

            CHECK_THAT(v, Catch::Matchers::Equals(std::vector<int> {1, 2, 3}));
        }
    }

    // sub parser

    SECTION("sub parsers")
    {
        std::string called = "none";

        parser.AddSubParser("update", [&called] { called = "update"; });
        parser.AddSubParser("pull", [&called] { called = "pull"; });

        SECTION("empty")
        {
            CHECK(parser.Parse(app, {}) == euco::argparse::ParseResult::Failed);
            // todo: verify error
        }

        SECTION("update")
        {
            CHECK(parser.Parse(app, {"update"})
                  == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(called == "update");
        }

        SECTION("pull")
        {
            CHECK(parser.Parse(app, {"pull"})
                  == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(called == "pull");
        }
    }

    SECTION("send data to correct parser")
    {
        std::string first  = "";
        std::string second = "";
        parser.AddSimple("-a", &first);
        parser.AddSubParser("b", [] {})->AddSimple("-a", &second);

        SECTION("first")
        {
            CHECK(parser.Parse(app, {"-a", "dog", "b"})
                  == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(first == "dog");
            CHECK(second == "");
        }

        SECTION("second")
        {
            CHECK(parser.Parse(app, {"b", "-a", "cat"})
                  == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(first == "");
            CHECK(second == "cat");
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////
    // complex combinations
    SECTION("too many uses")
    {
        int i = 0;
        parser.AddSimple("-i", &i);
        CHECK(parser.Parse(app, {"-i", "2", "-i", "3"})
              == euco::argparse::ParseResult::Failed);
        CHECK_THAT(
                output.out,
                Catch::Matchers::Equals(ERROR_LINE("-i specified earlier")));
        // the first i is read, the second errors
        CHECK(i == 2);
    }

    SECTION("mixing positional and optional")
    {
        std::string pos = "not read";
        std::string opt = "not read";

        parser.AddSimple("pos", &pos);
        parser.AddSimple("-a, --animal", &opt);

        SECTION("position and optional")
        {
            CHECK(parser.Parse(app, {"abc", "-a", "cat"})
                  == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(opt == "cat");
            CHECK(pos == "abc");
        }

        SECTION("optional and position")
        {
            CHECK(parser.Parse(app, {"-a", "cat", "abc"})
                  == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(opt == "cat");
            CHECK(pos == "abc");
        }

        SECTION("negative position and optional")
        {
            CHECK(parser.Parse(app, {"-abc", "-a", "cat"})
                  == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(opt == "cat");
            CHECK(pos == "-abc");
        }

        SECTION("optional and negative position")
        {
            CHECK(parser.Parse(app, {"-a", "cat", "-abc"})
                  == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(opt == "cat");
            CHECK(pos == "-abc");
        }

        SECTION("optional and positional named as optional")
        {
            CHECK(parser.Parse(app, {"-a", "cat", "-a"})
                  == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(opt == "cat");
            CHECK(pos == "-a");
        }
    }

    SECTION("mixing positional and 3 bool")
    {
        bool a = false;
        bool b = false;
        bool c = false;
        parser.SetTrue("-a", &a);
        parser.SetTrue("-b", &b);
        parser.SetTrue("-c", &c);

        std::string pos = "not read";
        parser.AddSimple("pos", &pos);

        SECTION("empty")
        {
            CHECK(parser.Parse(app, {}) == euco::argparse::ParseResult::Failed);
            // REQUIRE_THAT(output.out, Catch::Matchers::Equals( empty_output ));
            CHECK_FALSE(a);
            CHECK_FALSE(b);
            CHECK_FALSE(c);
            CHECK(pos == "not read");
        }

        SECTION("before")
        {
            CHECK(parser.Parse(app, {"-abc", "cat"})
                  == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(a);
            CHECK(b);
            CHECK(c);
            CHECK(pos == "cat");
        }

        SECTION("after")
        {
            CHECK(parser.Parse(app, {"cat", "-abc"})
                  == euco::argparse::ParseResult::Ok);
            REQUIRE_THAT(output.out, Catch::Matchers::Equals(empty_output));
            CHECK(a);
            CHECK(b);
            CHECK(c);
            CHECK(pos == "cat");
        }
    }
}
