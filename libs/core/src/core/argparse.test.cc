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
    name_and_arguments
    MakeArguments(const std::vector<std::string>& args)
    {
        return name_and_arguments{ "app", args };
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


    struct TestPrinter : public printer
    {
        std::vector<Message> messages;

        void
        print_error(const std::string& line) override
        {
            messages.emplace_back(true, line);
        }

        void
        print_info(const std::string& line) override
        {
            messages.emplace_back(false, line);
        }
    };


    FalseString
    Check
    (
        const std::vector<std::string>& lhs,
        const std::vector<std::string>& rhs
    )
    {
        return euphoria::tests::VectorEquals
        (
            lhs,
            rhs,
            [](const std::string& m) -> std::string
            {
                return m;
            },
            [](const std::string& lhs, const std::string& rhs) -> FalseString
            {
                return StringEq(lhs, rhs);
            }
        );
    }

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
                return string_builder() << m;
            },
            [](const Message& lhs, const Message& rhs) -> FalseString
            {
                const auto str = StringEq(lhs.text, rhs.text);
                if(str == false) { return str; }
                if(lhs.error == rhs.error) { return FalseString::True(); }
                return FalseString::False
                (
                    string_builder() << "error diff: "
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
        o << euphoria::core::enum_to_string(m);
        return o;
    }
}


TEST_CASE("argparse", "[argparse]")
{
    auto parser = argparse::parser{};
    auto output = std::make_shared<TestPrinter>();
    parser.printer = output;


    SECTION("default")
    {
        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(MakeArguments({}));
            CHECK(res == argparse::ok);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(MakeArguments({ "-h" }));
            CHECK(res == argparse::quit);
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
            const auto res = parser.parse(MakeArguments({ "dog" }));
            CHECK(res == argparse::error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h]"),
                Err("'dog' was unexpected")
            }));
        }

        SECTION("optional error")
        {
            const auto res = parser.parse(MakeArguments({ "--cat" }));
            CHECK(res == argparse::error);
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
        parser.add_void_function("-f", [&]
        {
            var = "called";
        }).set_help("call function");

        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(MakeArguments({}));
            CHECK(res == argparse::ok);
            CHECK(var == "default");
        }

        SECTION("function called")
        {
            const auto res = parser.parse(MakeArguments({ "-f" }));
            CHECK(res == argparse::ok);
            CHECK(var == "called");
        }

        SECTION("print help")
        {
            const auto res = parser.parse(MakeArguments({ "-h" }));
            CHECK(res == argparse::quit);
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
            const auto res = parser.parse(MakeArguments({ "-f", "dog" }));
            CHECK(res == argparse::error);
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
        parser.add("-f", &value).set_help("a int");

        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(MakeArguments({}));
            CHECK(res == argparse::ok);
            CHECK(value == 0);
        }

        SECTION("parse 42")
        {
            const auto res = parser.parse(MakeArguments({ "-f", "42" }));
            CHECK(res == argparse::ok);
            CHECK(value == 42);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(MakeArguments({ "-h" }));
            CHECK(res == argparse::quit);
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
            const auto res = parser.parse(MakeArguments({ "-f" }));
            CHECK(res == argparse::error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-f F]"),
                Err("missing value for '-f'")
            }));
        }

        SECTION("string is not a int")
        {
            const auto res = parser.parse(MakeArguments({ "-f", "dog" }));
            CHECK(res == argparse::error);
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
        parser.add("-f", &value).set_help("some string");

        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(MakeArguments({}));
            CHECK(res == argparse::ok);
            CHECK(value == "default");
        }

        SECTION("parse dog")
        {
            const auto res = parser.parse(MakeArguments({ "-f", "dog" }));
            CHECK(res == argparse::ok);
            CHECK(value == "dog");
        }

        SECTION("print help")
        {
            const auto res = parser.parse(MakeArguments({ "-h" }));
            CHECK(res == argparse::quit);
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
            const auto res = parser.parse(MakeArguments({ "-f" }));
            CHECK(res == argparse::error);
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
        parser.add("-f", &value).set_help("some animal");

        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(MakeArguments({}));
            CHECK(res == argparse::ok);
            CHECK(value == Animal::Dog);
        }

        SECTION("parse cat")
        {
            const auto res = parser.parse(MakeArguments({ "-f", "cat" }));
            CHECK(res == argparse::ok);
            CHECK(value == Animal::Cat);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(MakeArguments({ "-h" }));
            CHECK(res == argparse::quit);
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
            const auto res = parser.parse(MakeArguments({ "-f" }));
            CHECK(res == argparse::error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-f F]"),
                Err("missing value for '-f'")
            }));
        }

        SECTION("parse mouse")
        {
            const auto res = parser.parse(MakeArguments({ "-f", "mouse" }));
            CHECK(res == argparse::error);
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
        parser.add("-a, --animal", &value).set_help("set the animal");

        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(MakeArguments({}));
            CHECK(res == argparse::ok);
            CHECK(value == Animal::Dog);
        }

        SECTION("short name")
        {
            const auto res = parser.parse(MakeArguments({ "-a", "cat" }));
            CHECK(res == argparse::ok);
            CHECK(value == Animal::Cat);
        }

        SECTION("long name")
        {
            const auto res = parser.parse(MakeArguments({ "--animal", "bird" }));
            CHECK(res == argparse::ok);
            CHECK(value == Animal::Bird);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(MakeArguments({ "-h" }));
            CHECK(res == argparse::quit);
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
            const auto res = parser.parse(MakeArguments({ "--animal", "cookie" }));
            CHECK(res == argparse::error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] [-a A]"),
                Err("'cookie' is not accepted for '--animal', did you mean 'None', 'Dog', 'Bird' or 'Cat'?")
            }));
        }
        SECTION("-a cake error")
        {
            const auto res = parser.parse(MakeArguments({ "-a", "cookie" }));
            CHECK(res == argparse::error);
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
        parser.add("f", &value);

        // todo(Gustav): add int parse error test

        SECTION("positional missing = error")
        {
            const auto res = parser.parse(MakeArguments({}));
            CHECK(res == argparse::error);
            CHECK(value == "default");
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] F"),
                Err("Positional F was not specified.")
            }));
        }

        SECTION("parse 42")
        {
            const auto res = parser.parse(MakeArguments({ "dog" }));
            CHECK(res == argparse::ok);
            CHECK(value == "dog");
        }
    }

    SECTION("greedy")
    {
        int a = 42;
        int b = 42;
        std::vector<std::string> files;

        parser.add("-a, --alpha", &a).set_allow_before_positionals().set_help("set a");
        parser.add("-b, --beta", &b).set_allow_before_positionals().set_help("set b");
        parser.add_vector("files", &files).set_help("the files");

        SECTION("parse single")
        {
            const auto res = parser.parse(MakeArguments({ "dog" }));
            CHECK(res == argparse::ok);
            CHECK(a == 42);
            CHECK(b == 42);
            CHECK(Check(files,
            {
                "dog"
            }));
        }

        SECTION("parse many")
        {
            const auto res = parser.parse(MakeArguments({ "dog", "cat"}));
            CHECK(res == argparse::ok);
            CHECK(a == 42);
            CHECK(b == 42);
            CHECK(Check(files,
            {
                "dog", "cat"
            }));
        }

        SECTION("set single short argument")
        {
            const auto res = parser.parse(MakeArguments({ "-a", "5", "dog" }));
            CHECK(res == argparse::ok);
            CHECK(a == 5);
            CHECK(b == 42);
            CHECK(Check(files,
            {
                "dog"
            }));
        }

        SECTION("set single long argument")
        {
            const auto res = parser.parse(MakeArguments({ "--alpha", "7", "dog" }));
            CHECK(res == argparse::ok);
            CHECK(a == 7);
            CHECK(b == 42);
            CHECK(Check(files,
            {
                "dog"
            }));
        }

        SECTION("set many argument")
        {
            const auto res = parser.parse(MakeArguments({ "-a", "5", "-b", "3", "dog" }));
            CHECK(res == argparse::ok);
            CHECK(a == 5);
            CHECK(b == 3);
            CHECK(Check(files,
            {
                "dog"
            }));
        }
    }

    SECTION("subparser")
    {
        std::string a = "default";
        std::string b = "default";

        auto sub = parser.add_sub_parsers();

        sub->add("a", "do awesome stuff", [&](sub_parser* parser)
        {
            std::string a_value = "dog";
            parser->add("-s", &a_value);

            return parser->on_complete([&]
            {
                a = a_value;
                return argparse::ok;
            });
        });

        sub->add("b", "do boring stuff", [&](sub_parser*)
        {
            b = "bird";
            return argparse::ok;
        });

        SECTION("empty subparser = error")
        {
            const auto res = parser.parse(MakeArguments({}));
            INFO(output->messages);
            CHECK(res == argparse::error);
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
            const auto res = parser.parse
            (
                MakeArguments
                ({
                    "a"
                })
            );
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(a == "dog");
            CHECK(b == "default");
        }

        SECTION("call a with arg")
        {
            const auto res = parser.parse
            (
                MakeArguments
                ({
                    "a", "-s", "cat"
                })
            );
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(a == "cat");
            CHECK(b == "default");
        }

        SECTION("call b")
        {
            const auto res = parser.parse
            (
                MakeArguments
                ({
                    "b"
                })
            );
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(a == "default");
            CHECK(b == "bird");
        }

        SECTION("subcommand help")
        {
            const auto res = parser.parse(MakeArguments({ "-h" }));
            CHECK(res == argparse::quit);
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
            const auto res = parser.parse(MakeArguments({ "cat" }));
            CHECK(res == argparse::error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] <command> [<args>]"),
                Err("Invalid command 'cat', did you mean 'a' or 'b'?")
            }));
        }
        SECTION("invalid optional for root")
        {
            const auto res = parser.parse(MakeArguments({ "-f", "dog" }));
            CHECK(res == argparse::error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] <command> [<args>]"),
                Err("unknown argument: '-f', did you mean '-h'?")
            }));
        }
    }

    SECTION("non greedy subparser/script like")
    {
        auto sub = parser.add_sub_parsers();
        parser.parser_style = sub_parser_style::fallback;
        std::string data;
        sub->add("add", "add something", [&](sub_parser* sub)
        {
            std::string what;
            sub->add("what", &what);
            return sub->on_complete([&]
            {
                data += what;
                return argparse::ok;
            });
        });
        sub->add("double", "double the content", [&](sub_parser* sub)
        {
            return sub->on_complete([&]
            {
                data += data;
                return argparse::ok;
            });
        });

        SECTION("once")
        {
            const auto res = parser.parse
            (
                MakeArguments
                ({
                    "add", "dog"
                })
            );
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(data == "dog");
        }

        SECTION("twice")
        {
            const auto res = parser.parse
            (
                MakeArguments
                ({
                    "add", "cat",
                    "double"
                })
            );
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(data == "catcat");
        }

        SECTION("print help")
        {
            const auto res = parser.parse(MakeArguments({ "-h" }));
            CHECK(res == argparse::quit);
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
            const auto res = parser.parse(MakeArguments({ "add", "dog", "dog" }));
            CHECK(res == argparse::error);
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
        auto sub = parser.add_sub_parsers();
        std::string data;
        sub->add("pretty", "be kind", [&](sub_parser* pretty)
        {
            pretty->add_sub_parsers()->add("please", [&](sub_parser* please)
            {
                please->parser_style = sub_parser_style::fallback;
                auto sub = please->add_sub_parsers();
                sub->add("add", [&](sub_parser* sub)
                {
                    std::string what;
                    sub->add("what", &what);
                    return sub->on_complete([&]
                    {
                        data += what;
                        return argparse::ok;
                    });
                });
                sub->add("double", [&](sub_parser* sub)
                {
                    return sub->on_complete([&]
                    {
                        data += data;
                        return argparse::ok;
                    });
                });
                return please->on_complete([]{return argparse::ok;});
            });
            return pretty->on_complete([]{return argparse::ok;});
        });

        SECTION("once")
        {
            const auto res = parser.parse
            (
                MakeArguments
                ({
                    "pretty", "please", "add", "dog"
                })
            );
            CHECK(res == argparse::ok);
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
            const auto res = parser.parse
            (
                arguments
            );
            INFO(output->messages);
            INFO(arguments);
            REQUIRE(data == "catcat");
            CHECK(res == argparse::ok);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(MakeArguments({ "-h" }));
            CHECK(res == argparse::quit);
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
            const auto res = parser.parse(MakeArguments({ "pretty", "dog" }));
            CHECK(res == argparse::error);
            REQUIRE(data.empty());
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
    auto parser = argparse::parser{};
    auto output = std::make_shared<TestPrinter>();
    parser.printer = output;

    SECTION("default")
    {
        SECTION("one positional")
        {
            const auto res = parser.parse(MakeArguments({"dog"}));
            CHECK(res == argparse::error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h]"),
                Err("'dog' was unexpected")
            }));
        }
        SECTION("many positionals")
        {
            const auto res = parser.parse(MakeArguments({"cat", "dog"}));
            CHECK(res == argparse::error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h]"),
                Err("'cat' was unexpected")
            }));
        }
        SECTION("optional 1 dash")
        {
            const auto res = parser.parse(MakeArguments({"-o"}));
            CHECK(res == argparse::error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h]"),
                Err("unknown argument: '-o', did you mean '-h'?")
            }));
        }
        SECTION("optional 2 dashes")
        {
            const auto res = parser.parse(MakeArguments({"--make-cool"}));
            CHECK(res == argparse::error);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h]"),
                Err("unknown argument: '--make-cool', did you mean '--help'?")
            }));
        }
    }

    SECTION("fourway test int")
    {
        using FF = fourway<int>;
        auto ff = FF{0};
        parser.add("f", &ff);

        SECTION("one value")
        {
            const auto res = parser.parse(MakeArguments({"4"}));
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF{4});
        }
        SECTION("two values")
        {
            const auto res = parser.parse(MakeArguments({"4/2"}));
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF::from_lrud(2, 4));
        }
        SECTION("all values")
        {
            const auto res = parser.parse(MakeArguments({"1/2/3/4"}));
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF::from_lrud(4, 2, 1, 3));
        }
    }

    SECTION("fourway test enum")
    {
        using FF = fourway<Animal>;
        auto ff = FF{Animal::None};
        parser.add("f", &ff);

        SECTION("one value")
        {
            const auto res = parser.parse(MakeArguments({"cat"}));
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF{Animal::Cat});
        }
        SECTION("two values")
        {
            const auto res = parser.parse(MakeArguments({"cat/none"}));
            INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF::from_lrud(Animal::None, Animal::Cat));
        }
    }

    SECTION("sub parser greedy")
    {
        auto sub = parser.add_sub_parsers();
        bool completed = false;
        parser.parser_style = sub_parser_style::greedy;
        sub->add("a", [&](sub_parser* parser)
        {
            return parser->on_complete([&]
            {
                completed = true;
                return argparse::ok;
            });
        });

        SECTION("one positional")
        {
            const auto res = parser.parse(MakeArguments({"a", "dog"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app a [-h]"),
                Err("'dog' was unexpected")
            }));
        }
        SECTION("many positionals")
        {
            const auto res = parser.parse(MakeArguments({"a", "cat", "dog"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app a [-h]"),
                Err("'cat' was unexpected")
            }));
        }
        SECTION("optional 1 dash")
        {
            const auto res = parser.parse(MakeArguments({"a", "-o"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app a [-h]"),
                Err("unknown argument: '-o', did you mean '-h'?")
            }));
        }
        SECTION("optional 2 dashes")
        {
            const auto res = parser.parse(MakeArguments({"a", "--make-cool"}));
            CHECK(res == argparse::error);
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
        auto sub = parser.add_sub_parsers();
        bool completed = false;
        parser.parser_style = sub_parser_style::fallback;
        sub->add("a", [&](sub_parser* parser)
        {
            return parser->on_complete([&]
            {
                completed = true;
                return argparse::ok;
            });
        });

        SECTION("one positional")
        {
            const auto res = parser.parse(MakeArguments({"a", "dog"}));
            CHECK(res == argparse::error);
            CHECK(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] <command> [<args>]"),
                Err("Invalid command 'dog', did you mean 'a'?")
            }));
        }
        SECTION("many positionals")
        {
            const auto res = parser.parse(MakeArguments({"a", "cat", "dog"}));
            CHECK(res == argparse::error);
            CHECK(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app [-h] <command> [<args>]"),
                Err("Invalid command 'cat', did you mean 'a'?")
            }));
        }
        SECTION("optional 1 dash")
        {
            const auto res = parser.parse(MakeArguments({"a", "-o"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app a [-h]"),
                Err("unknown argument: '-o', did you mean '-h'?")
            }));
        }
        SECTION("optional 2 dashes")
        {
            const auto res = parser.parse(MakeArguments({"a", "--make-cool"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(Check(output->messages,
            {
                Inf("usage: app a [-h]"),
                Err("unknown argument: '--make-cool', did you mean '--help'?")
            }));
        }
    }
}

