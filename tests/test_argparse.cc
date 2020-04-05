#include "core/argparse.h"

#include "core/enumtostring.h"
#include "core/fourway.h"
#include "tests/utils.h"
#include "tests/stringeq.h"
#include "tests/vectorequals.h"

using namespace euphoria::core;
using namespace euphoria::core::argparse;

using namespace euphoria::tests;


namespace
{
    Arguments
    MakeArguments(const std::vector<std::string>& args)
    {
        return Arguments{ "app", args };
    }


    struct Message
    {
        bool error;
        std::string text;

        Message(bool e, const std::string& t)
            : error(e)
            , text(t)
        {
        }
    };


    Message
    Inf(const std::string& text)
    {
        return {false, text};
    }


    Message
    Err(const std::string& text)
    {
        return {true, text};
    }


    template<typename T>
    std::ostream&
    operator<<(std::ostream& o, const std::vector<T>& m)
    {
        o << "[";
        bool first = true;
        for(const auto& t: m)
        {
            if(first) {first = false;}
            else { o << ", "; }
            o << t;
        }
        o << "]";
        return o;
    }


    std::ostream&
    operator<<(std::ostream& o, const Message& m)
    {
        o << (m.error?"ERR":"INF");
        o << " " << m.text;
        return o;
    }


    struct TestPrinter : public Printer
    {
        std::vector<Message> messages;

        void
        PrintError(const std::string& line) override
        {
            messages.emplace_back(true, line);
        }

        void
        PrintInfo(const std::string& line) override
        {
            messages.emplace_back(false, line);
        }
    };


    FalseString
    Check
    (
        const std::vector<Message>& lhs,
        const std::vector<Message>& rhs
    )
    {
        return euphoria::tests::VectorEquals
        (
            lhs,
            rhs,
            [](const Message& m) -> std::string
            {
                return Str() << m;
            },
            [](const Message& lhs, const Message& rhs) -> FalseString
            {
                const auto str = StringEq(lhs.text, rhs.text);
                if(str == false) { return str; }
                if(lhs.error == rhs.error) { return FalseString::True(); }
                return FalseString::False
                (
                    Str() << "error diff: "
                    << lhs.error << " vs "
                    << rhs.error
                );
            }
        );
    }




    enum class Animal
    {
        Cat, Dog, Bird, None
    };


    std::ostream&
    operator<<(std::ostream& o, const Animal& m)
    {
        o << euphoria::core::EnumToString(m);
        return o;
    }
}


TEST_CASE("argparse", "[argparse]")
{
    auto parser = Parser{};
    auto output = std::make_shared<TestPrinter>();
    parser.printer = output;


    SECTION("default")
    {
        SECTION("empty parser is ok")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Ok);
        }

        SECTION("print help")
        {
            const auto res = parser.Parse(MakeArguments({ "-h" }));
            CHECK(res == ParseResult::Quit);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h]"),
                Inf(""),
                Inf("optional arguments:"),
                Inf("  -h, --help  show this help message and exit")
            }));
        }

        SECTION("positional error")
        {
            const auto res = parser.Parse(MakeArguments({ "dog" }));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h]"),
                Err("'dog' was unexpected")
            }));
        }

        SECTION("optional error")
        {
            const auto res = parser.Parse(MakeArguments({ "--cat" }));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h]"),
                Err("unknown argument: '--cat', did you mean '--help'?")
            }));
        }
    }

    SECTION("void function")
    {
        std::string var = "default";
        parser.AddVoidFunction("-f", [&]
        {
            var = "called";
        }).Help("call function");

        SECTION("empty parser is ok")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Ok);
            CHECK(var == "default");
        }

        SECTION("function called")
        {
            const auto res = parser.Parse(MakeArguments({ "-f" }));
            CHECK(res == ParseResult::Ok);
            CHECK(var == "called");
        }

        SECTION("print help")
        {
            const auto res = parser.Parse(MakeArguments({ "-h" }));
            CHECK(res == ParseResult::Quit);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-f]"),
                Inf(""),
                Inf("optional arguments:"),
                Inf("  -h, --help  show this help message and exit"),
                Inf("  -f          call function")
            }));
        }

        SECTION("call function but die anyway")
        {
            const auto res = parser.Parse(MakeArguments({ "-f", "dog" }));
            CHECK(res == ParseResult::Error);
            CHECK(var == "called");
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-f]"),
                Err("'dog' was unexpected")
            }));
        }
    }

    SECTION("optional int")
    {
        int value = 0;
        parser.Add("-f", &value).Help("a int");

        SECTION("empty parser is ok")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Ok);
            CHECK(value == 0);
        }

        SECTION("parse 42")
        {
            const auto res = parser.Parse(MakeArguments({ "-f", "42" }));
            CHECK(res == ParseResult::Ok);
            CHECK(value == 42);
        }

        SECTION("print help")
        {
            const auto res = parser.Parse(MakeArguments({ "-h" }));
            CHECK(res == ParseResult::Quit);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-f F]"),
                Inf(""),
                Inf("optional arguments:"),
                Inf("  -h, --help  show this help message and exit"),
                Inf("  -f F        a int (default: 0)")
            }));
        }

        SECTION("missing value")
        {
            const auto res = parser.Parse(MakeArguments({ "-f" }));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-f F]"),
                Err("missing value for '-f'")
            }));
        }

        SECTION("string is not a int")
        {
            const auto res = parser.Parse(MakeArguments({ "-f", "dog" }));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-f F]"),
                Err("'dog' is not accepted for '-f'")
            }));
        }
    }

    SECTION("optional string")
    {
        std::string value = "default";
        parser.Add("-f", &value).Help("some string");

        SECTION("empty parser is ok")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Ok);
            CHECK(value == "default");
        }

        SECTION("parse dog")
        {
            const auto res = parser.Parse(MakeArguments({ "-f", "dog" }));
            CHECK(res == ParseResult::Ok);
            CHECK(value == "dog");
        }

        SECTION("print help")
        {
            const auto res = parser.Parse(MakeArguments({ "-h" }));
            CHECK(res == ParseResult::Quit);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-f F]"),
                Inf(""),
                Inf("optional arguments:"),
                Inf("  -h, --help  show this help message and exit"),
                Inf("  -f F        some string (default: default)")
            }));
        }

        SECTION("missing value")
        {
            const auto res = parser.Parse(MakeArguments({ "-f" }));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-f F]"),
                Err("missing value for '-f'")
            }));
        }
    }


    SECTION("optional enum")
    {
        Animal value = Animal::Dog;
        parser.Add("-f", &value).Help("some animal");

        SECTION("empty parser is ok")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Ok);
            CHECK(value == Animal::Dog);
        }

        SECTION("parse cat")
        {
            const auto res = parser.Parse(MakeArguments({ "-f", "cat" }));
            CHECK(res == ParseResult::Ok);
            CHECK(value == Animal::Cat);
        }

        SECTION("print help")
        {
            const auto res = parser.Parse(MakeArguments({ "-h" }));
            CHECK(res == ParseResult::Quit);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-f F]"),
                Inf(""),
                Inf("optional arguments:"),
                Inf("  -h, --help  show this help message and exit"),
                Inf("  -f F        some animal (default: Dog)"),
                Inf("              can be either 'Cat', 'Dog', 'Bird' or 'None'")
            }));
        }

        SECTION("parse missing")
        {
            const auto res = parser.Parse(MakeArguments({ "-f" }));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-f F]"),
                Err("missing value for '-f'")
            }));
        }

        SECTION("parse mouse")
        {
            const auto res = parser.Parse(MakeArguments({ "-f", "mouse" }));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-f F]"),
                Err("'mouse' is not accepted for '-f', did you mean 'None', 'Dog', 'Bird' or 'Cat'?")
            }));
        }
    }

    SECTION("multi optional names")
    {
        Animal value = Animal::Dog;
        parser.Add("-a, --animal", &value).Help("set the animal");

        SECTION("empty parser is ok")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Ok);
            CHECK(value == Animal::Dog);
        }

        SECTION("short name")
        {
            const auto res = parser.Parse(MakeArguments({ "-a", "cat" }));
            CHECK(res == ParseResult::Ok);
            CHECK(value == Animal::Cat);
        }

        SECTION("long name")
        {
            const auto res = parser.Parse(MakeArguments({ "--animal", "bird" }));
            CHECK(res == ParseResult::Ok);
            CHECK(value == Animal::Bird);
        }

        SECTION("print help")
        {
            const auto res = parser.Parse(MakeArguments({ "-h" }));
            CHECK(res == ParseResult::Quit);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-a A]"),
                Inf(""),
                Inf("optional arguments:"),
                Inf("  -h, --help      show this help message and exit"),
                Inf("  -a, --animal A  set the animal (default: Dog)"),
                Inf("                  can be either 'Cat', 'Dog', 'Bird' or 'None'")
            }));
        }

        SECTION("--animal cookie error")
        {
            const auto res = parser.Parse(MakeArguments({ "--animal", "cookie" }));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-a A]"),
                Err("'cookie' is not accepted for '--animal', did you mean 'None', 'Dog', 'Bird' or 'Cat'?")
            }));
        }
        SECTION("-a cake error")
        {
            const auto res = parser.Parse(MakeArguments({ "-a", "cookie" }));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-a A]"),
                Err("'cookie' is not accepted for '-a', did you mean 'None', 'Dog', 'Bird' or 'Cat'?")
            }));
        }
    }

    SECTION("positional string")
    {
        std::string value = "default";
        parser.Add("f", &value);

        // todo(Gustav): add int parse error test

        SECTION("positional missing = error")
        {
            const auto res = parser.Parse(MakeArguments({}));
            CHECK(res == ParseResult::Error);
            CHECK(value == "default");
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] F"),
                Err("Positional F was not specified.")
            }));
        }

        SECTION("parse 42")
        {
            const auto res = parser.Parse(MakeArguments({ "dog" }));
            CHECK(res == ParseResult::Ok);
            CHECK(value == "dog");
        }
    }

    SECTION("subparser")
    {
        std::string a = "default";
        std::string b = "default";

        auto sub = parser.AddSubParsers();

        sub->Add("a", "do awesome stuff", [&](SubParser* parser)
        {
            std::string a_value = "dog";
            parser->Add("-s", &a_value);

            return parser->OnComplete([&]
            {
                a = a_value;
                return ParseResult::Ok;
            });
        });

        sub->Add("b", "do boring stuff", [&](SubParser*)
        {
            b = "bird";
            return ParseResult::Ok;
        });

        SECTION("empty subparser = error")
        {
            const auto res = parser.Parse(MakeArguments({}));
            INFO(output->messages);
            CHECK(res == ParseResult::Error);
            CHECK(a == "default");
            CHECK(b == "default");
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] <command> [<args>]"),
                Err("no subparser specified")
            }));
        }

        SECTION("call a")
        {
            const auto res = parser.Parse
            (
                MakeArguments
                ({
                    "a"
                })
            );
            INFO(output->messages);
            CHECK(res == ParseResult::Ok);
            CHECK(a == "dog");
            CHECK(b == "default");
        }

        SECTION("call a with arg")
        {
            const auto res = parser.Parse
            (
                MakeArguments
                ({
                    "a", "-s", "cat"
                })
            );
            INFO(output->messages);
            CHECK(res == ParseResult::Ok);
            CHECK(a == "cat");
            CHECK(b == "default");
        }

        SECTION("call b")
        {
            const auto res = parser.Parse
            (
                MakeArguments
                ({
                    "b"
                })
            );
            INFO(output->messages);
            CHECK(res == ParseResult::Ok);
            CHECK(a == "default");
            CHECK(b == "bird");
        }

        SECTION("subcommand help")
        {
            const auto res = parser.Parse(MakeArguments({ "-h" }));
            CHECK(res == ParseResult::Quit);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] <command> [<args>]"),
                Inf(""),
                Inf("optional arguments:"),
                Inf("  -h, --help  show this help message and exit"),
                Inf(""),
                Inf("commands:"),
                Inf("  a           do awesome stuff"),
                Inf("  b           do boring stuff"),
            }));
        }

        SECTION("bad subcommand")
        {
            const auto res = parser.Parse(MakeArguments({ "cat" }));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] <command> [<args>]"),
                Err("Invalid command 'cat', did you mean 'a' or 'b'?")
            }));
        }
        SECTION("invalid optional for root")
        {
            const auto res = parser.Parse(MakeArguments({ "-f", "dog" }));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] <command> [<args>]"),
                Err("unknown argument: '-f', did you mean '-h'?")
            }));
        }
    }

    SECTION("non greedy subparser/script like")
    {
        auto sub = parser.AddSubParsers();
        parser.parser_style = SubParserStyle::Fallback;
        std::string data;
        sub->Add("add", "add something", [&](SubParser* sub)
        {
            std::string what;
            sub->Add("what", &what);
            return sub->OnComplete([&]
            {
                data += what;
                return ParseResult::Ok;
            });
        });
        sub->Add("double", "double the content", [&](SubParser* sub)
        {
            return sub->OnComplete([&]
            {
                data += data;
                return ParseResult::Ok;
            });
        });

        SECTION("once")
        {
            const auto res = parser.Parse
            (
                MakeArguments
                ({
                    "add", "dog"
                })
            );
            INFO(output->messages);
            CHECK(res == ParseResult::Ok);
            CHECK(data == "dog");
        }

        SECTION("twice")
        {
            const auto res = parser.Parse
            (
                MakeArguments
                ({
                    "add", "cat",
                    "double"
                })
            );
            INFO(output->messages);
            CHECK(res == ParseResult::Ok);
            CHECK(data == "catcat");
        }

        SECTION("print help")
        {
            const auto res = parser.Parse(MakeArguments({ "-h" }));
            CHECK(res == ParseResult::Quit);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] <command> [<args>]"),
                Inf(""),
                Inf("optional arguments:"),
                Inf("  -h, --help  show this help message and exit"),
                Inf(""),
                Inf("commands:"),
                Inf("  add         add something"),
                Inf("  double      double the content"),
            }));
        }

        SECTION("add and error")
        {
            const auto res = parser.Parse(MakeArguments({ "add", "dog", "dog" }));
            CHECK(res == ParseResult::Error);
            CHECK(data == "dog");
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] <command> [<args>]"),
                Err("Invalid command 'dog', did you mean 'add' or 'double'?")
            }));
        }
    }

    SECTION("non root root")
    {
        auto sub = parser.AddSubParsers();
        std::string data;
        sub->Add("pretty", "be kind", [&](SubParser* pretty)
        {
            pretty->AddSubParsers()->Add("please", [&](SubParser* please)
            {
                please->parser_style = SubParserStyle::Fallback;
                auto sub = please->AddSubParsers();
                sub->Add("add", [&](SubParser* sub)
                {
                    std::string what;
                    sub->Add("what", &what);
                    return sub->OnComplete([&]
                    {
                        data += what;
                        return ParseResult::Ok;
                    });
                });
                sub->Add("double", [&](SubParser* sub)
                {
                    return sub->OnComplete([&]
                    {
                        data += data;
                        return ParseResult::Ok;
                    });
                });
                return please->OnComplete([]{return ParseResult::Ok;});
            });
            return pretty->OnComplete([]{return ParseResult::Ok;});
        });

        SECTION("once")
        {
            const auto res = parser.Parse
            (
                MakeArguments
                ({
                    "pretty", "please", "add", "dog"
                })
            );
            CHECK(res == ParseResult::Ok);
            CHECK(data == "dog");
        }

        SECTION("twice")
        {
            const auto arguments = 
                MakeArguments
                ({
                    "pretty", "please", "add", "cat",
                    "please", "double"
                });
            const auto res = parser.Parse
            (
                arguments
            );
            INFO(output->messages);
            INFO(arguments);
            REQUIRE(data == "catcat");
            CHECK(res == ParseResult::Ok);
        }

        SECTION("print help")
        {
            const auto res = parser.Parse(MakeArguments({ "-h" }));
            CHECK(res == ParseResult::Quit);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] <command> [<args>]"),
                Inf(""),
                Inf("optional arguments:"),
                Inf("  -h, --help  show this help message and exit"),
                Inf(""),
                Inf("commands:"),
                Inf("  pretty      be kind"),
            }));
        }

        SECTION("error but no add")
        {
            const auto res = parser.Parse(MakeArguments({ "pretty", "dog" }));
            CHECK(res == ParseResult::Error);
            REQUIRE(data == "");
            CHECK(Check(output->messages,
            {
                Inf("usage: app pretty [-h] <command> [<args>]"),
                Err("Invalid command 'dog', did you mean 'please'?")
            }));
        }
    }
}


TEST_CASE("argparse_error", "[argparse]")
{
    auto parser = Parser{};
    auto output = std::make_shared<TestPrinter>();
    parser.printer = output;

    SECTION("default")
    {
        SECTION("one positional")
        {
            const auto res = parser.Parse(MakeArguments({"dog"}));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h]"),
                Err("'dog' was unexpected")
            }));
        }
        SECTION("many positionals")
        {
            const auto res = parser.Parse(MakeArguments({"cat", "dog"}));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h]"),
                Err("'cat' was unexpected")
            }));
        }
        SECTION("optional 1 dash")
        {
            const auto res = parser.Parse(MakeArguments({"-o"}));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h]"),
                Err("unknown argument: '-o', did you mean '-h'?")
            }));
        }
        SECTION("optional 2 dashes")
        {
            const auto res = parser.Parse(MakeArguments({"--make-cool"}));
            CHECK(res == ParseResult::Error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h]"),
                Err("unknown argument: '--make-cool', did you mean '--help'?")
            }));
        }
    }

    SECTION("fourway test int")
    {
        using FF = Fourway<int>;
        auto ff = FF{0};
        parser.Add("f", &ff);

        SECTION("one value")
        {
            const auto res = parser.Parse(MakeArguments({"4"}));
            INFO(output->messages);
            CHECK(res == ParseResult::Ok);
            CHECK(ff == FF{4});
        }
        SECTION("two values")
        {
            const auto res = parser.Parse(MakeArguments({"4/2"}));
            INFO(output->messages);
            CHECK(res == ParseResult::Ok);
            CHECK(ff == FF::FromLrud(2, 4));
        }
        SECTION("all values")
        {
            const auto res = parser.Parse(MakeArguments({"1/2/3/4"}));
            INFO(output->messages);
            CHECK(res == ParseResult::Ok);
            CHECK(ff == FF::FromLrud(4, 2, 1, 3));
        }
    }

    SECTION("fourway test enum")
    {
        using FF = Fourway<Animal>;
        auto ff = FF{Animal::None};
        parser.Add("f", &ff);

        SECTION("one value")
        {
            const auto res = parser.Parse(MakeArguments({"cat"}));
            INFO(output->messages);
            CHECK(res == ParseResult::Ok);
            CHECK(ff == FF{Animal::Cat});
        }
        SECTION("two values")
        {
            const auto res = parser.Parse(MakeArguments({"cat/none"}));
            INFO(output->messages);
            CHECK(res == ParseResult::Ok);
            CHECK(ff == FF::FromLrud(Animal::None, Animal::Cat));
        }
    }

    SECTION("sub parser greedy")
    {
        auto sub = parser.AddSubParsers();
        bool completed = false;
        parser.parser_style = SubParserStyle::Greedy;
        sub->Add("a", [&](SubParser* parser)
        {
            return parser->OnComplete([&]
            {
                completed = true;
                return ParseResult::Ok;
            });
        });

        SECTION("one positional")
        {
            const auto res = parser.Parse(MakeArguments({"a", "dog"}));
            CHECK(res == ParseResult::Error);
            CHECK_FALSE(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app a [-h]"),
                Err("'dog' was unexpected")
            }));
        }
        SECTION("many positionals")
        {
            const auto res = parser.Parse(MakeArguments({"a", "cat", "dog"}));
            CHECK(res == ParseResult::Error);
            CHECK_FALSE(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app a [-h]"),
                Err("'cat' was unexpected")
            }));
        }
        SECTION("optional 1 dash")
        {
            const auto res = parser.Parse(MakeArguments({"a", "-o"}));
            CHECK(res == ParseResult::Error);
            CHECK_FALSE(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app a [-h]"),
                Err("unknown argument: '-o', did you mean '-h'?")
            }));
        }
        SECTION("optional 2 dashes")
        {
            const auto res = parser.Parse(MakeArguments({"a", "--make-cool"}));
            CHECK(res == ParseResult::Error);
            CHECK_FALSE(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app a [-h]"),
                Err("unknown argument: '--make-cool', did you mean '--help'?")
            }));
        }
    }

    SECTION("sub parser fallback")
    {
        auto sub = parser.AddSubParsers();
        bool completed = false;
        parser.parser_style = SubParserStyle::Fallback;
        sub->Add("a", [&](SubParser* parser)
        {
            return parser->OnComplete([&]
            {
                completed = true;
                return ParseResult::Ok;
            });
        });

        SECTION("one positional")
        {
            const auto res = parser.Parse(MakeArguments({"a", "dog"}));
            CHECK(res == ParseResult::Error);
            CHECK(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] <command> [<args>]"),
                Err("Invalid command 'dog', did you mean 'a'?")
            }));
        }
        SECTION("many positionals")
        {
            const auto res = parser.Parse(MakeArguments({"a", "cat", "dog"}));
            CHECK(res == ParseResult::Error);
            CHECK(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] <command> [<args>]"),
                Err("Invalid command 'cat', did you mean 'a'?")
            }));
        }
        SECTION("optional 1 dash")
        {
            const auto res = parser.Parse(MakeArguments({"a", "-o"}));
            CHECK(res == ParseResult::Error);
            CHECK_FALSE(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app a [-h]"),
                Err("unknown argument: '-o', did you mean '-h'?")
            }));
        }
        SECTION("optional 2 dashes")
        {
            const auto res = parser.Parse(MakeArguments({"a", "--make-cool"}));
            CHECK(res == ParseResult::Error);
            CHECK_FALSE(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app a [-h]"),
                Err("unknown argument: '--make-cool', did you mean '--help'?")
            }));
        }
    }
}

