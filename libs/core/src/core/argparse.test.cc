#include "core/argparse.h"

#include "base/enumtostring.h"
#include "base/lrud.h"
#include "tests/utils.h"
#include "tests/stringeq.h"
#include "tests/vectorequals.h"

using namespace eu;
using namespace eu::core;
using namespace eu::core::argparse;

using namespace eu::tests;


namespace
{
    NameAndArguments
    make_arguments(const std::vector<std::string>& args)
    {
        return NameAndArguments{ "app", args };
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
    create_info(const std::string& text)
    {
        return {false, text};
    }


    Message
    create_error(const std::string& text)
    {
        return {true, text};
    }


    std::string from_message_to_string(const Message& m)
        { return fmt::format("{} {}", (m.error?"ERR":"INF"), m.text); }


    struct TestPrinter : public Printer
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
    check
    (
        const std::vector<std::string>& lhs,
        const std::vector<std::string>& rhs
    )
    {
        return eu::tests::is_vector_equal
        (
            lhs,
            rhs,
            [](const std::string &m) -> std::string
            {
                return m;
            },
            [](const std::string &alhs, const std::string &arhs) -> FalseString
            {
                return is_string_equal(alhs, arhs);
            }
        );
    }

    FalseString
    check
    (
        const std::vector<Message>& lhs,
        const std::vector<Message>& rhs
    )
    {
        return eu::tests::is_vector_equal
        (
            lhs,
            rhs,
            [](const Message &m) -> std::string
            {
                return fmt::to_string(m);
            },
            [](const Message &alhs, const Message &arhs) -> FalseString
            {
                const auto str = is_string_equal(alhs.text, arhs.text);
                if (str == false)
                { return str; }
                if (alhs.error == arhs.error)
                { return FalseString::create_true(); }
                return FalseString::create_false
                (
                    fmt::format("error diff: {} vs {}", alhs.error, arhs.error)
                );
            }
        );
    }




    enum class Animal
    {
        cat, dog, bird, none
    };
}


ADD_DEFAULT_FORMATTER(Message, std::string, from_message_to_string);


TEST_CASE("argparse", "[argparse]")
{
    auto parser = argparse::Parser{};
    auto output = std::make_shared<TestPrinter>();
    parser.printer = output;

    CHECK(fmt::to_string(Animal::cat) == "cat");

    SECTION("default")
    {
        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(make_arguments({}));
            CHECK(res == argparse::ok);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h]"),
                create_info(""),
                create_info("optional arguments:"),
                create_info("  -h, --help  show this help message and exit")
            }));
        }

        SECTION("positional error")
        {
            const auto res = parser.parse(make_arguments({"dog" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h]"),
                create_error("'dog' was unexpected")
            }));
        }

        SECTION("optional error")
        {
            const auto res = parser.parse(make_arguments({"--cat" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h]"),
                create_error("unknown argument: '--cat', did you mean '--help'?")
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
            const auto res = parser.parse(make_arguments({}));
            CHECK(res == argparse::ok);
            CHECK(var == "default");
        }

        SECTION("function called")
        {
            const auto res = parser.parse(make_arguments({"-f" }));
            CHECK(res == argparse::ok);
            CHECK(var == "called");
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f]"),
                create_info(""),
                create_info("optional arguments:"),
                create_info("  -h, --help  show this help message and exit"),
                create_info("  -f          call function")
            }));
        }

        SECTION("call function but die anyway")
        {
            const auto res = parser.parse(make_arguments({"-f", "dog" }));
            CHECK(res == argparse::error);
            CHECK(var == "called");
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f]"),
                create_error("'dog' was unexpected")
            }));
        }
    }

    SECTION("optional int")
    {
        int value = 0;
        parser.add("-f", &value).set_help("a int");

        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(make_arguments({}));
            CHECK(res == argparse::ok);
            CHECK(value == 0);
        }

        SECTION("parse 42")
        {
            const auto res = parser.parse(make_arguments({"-f", "42" }));
            CHECK(res == argparse::ok);
            CHECK(value == 42);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_info(""),
                create_info("optional arguments:"),
                create_info("  -h, --help  show this help message and exit"),
                create_info("  -f F        a int (default: 0)")
            }));
        }

        SECTION("missing value")
        {
            const auto res = parser.parse(make_arguments({"-f" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_error("missing value for '-f'")
            }));
        }

        SECTION("string is not a int")
        {
            const auto res = parser.parse(make_arguments({"-f", "dog" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_error("'dog' is not accepted for '-f'")
            }));
        }
    }

    SECTION("optional string")
    {
        std::string value = "default";
        parser.add("-f", &value).set_help("some string");

        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(make_arguments({}));
            CHECK(res == argparse::ok);
            CHECK(value == "default");
        }

        SECTION("parse dog")
        {
            const auto res = parser.parse(make_arguments({"-f", "dog" }));
            CHECK(res == argparse::ok);
            CHECK(value == "dog");
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_info(""),
                create_info("optional arguments:"),
                create_info("  -h, --help  show this help message and exit"),
                create_info("  -f F        some string (default: default)")
            }));
        }

        SECTION("missing value")
        {
            const auto res = parser.parse(make_arguments({"-f" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_error("missing value for '-f'")
            }));
        }
    }


    SECTION("optional enum")
    {
        Animal value = Animal::dog;
        parser.add("-f", &value).set_help("some animal");

        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(make_arguments({}));
            CHECK(res == argparse::ok);
            CHECK(value == Animal::dog);
        }

        SECTION("parse cat")
        {
            const auto res = parser.parse(make_arguments({"-f", "cat" }));
            CHECK(res == argparse::ok);
            CHECK(value == Animal::cat);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_info(""),
                create_info("optional arguments:"),
                create_info("  -h, --help  show this help message and exit"),
                create_info("  -f F        some animal (default: dog)"),
                create_info("              can be either 'cat', 'dog', 'bird' or 'none'")
            }));
        }

        SECTION("parse missing")
        {
            const auto res = parser.parse(make_arguments({"-f" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_error("missing value for '-f'")
            }));
        }

        SECTION("parse mouse")
        {
            const auto res = parser.parse(make_arguments({"-f", "mouse" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-f F]"),
                create_error("'mouse' is not accepted for '-f', did you mean 'none', 'dog', 'bird' or 'cat'?")
            }));
        }
    }

    SECTION("multi optional names")
    {
        Animal value = Animal::dog;
        parser.add("-a, --animal", &value).set_help("set the animal");

        SECTION("empty parser is ok")
        {
            const auto res = parser.parse(make_arguments({}));
            CHECK(res == argparse::ok);
            CHECK(value == Animal::dog);
        }

        SECTION("short name")
        {
            const auto res = parser.parse(make_arguments({"-a", "cat" }));
            CHECK(res == argparse::ok);
            CHECK(value == Animal::cat);
        }

        SECTION("long name")
        {
            const auto res = parser.parse(make_arguments({"--animal", "bird" }));
            CHECK(res == argparse::ok);
            CHECK(value == Animal::bird);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] [-a A]"),
                create_info(""),
                create_info("optional arguments:"),
                create_info("  -h, --help      show this help message and exit"),
                create_info("  -a, --animal A  set the animal (default: dog)"),
                create_info("                  can be either 'cat', 'dog', 'bird' or 'none'")
            }));
        }

        SECTION("--animal cookie error")
        {
            const auto res = parser.parse(make_arguments({"--animal", "cookie" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                    create_info("usage: app [-h] [-a A]"),
                    create_error("'cookie' is not accepted for '--animal', did you mean 'none', 'dog', 'cat' or 'bird'?")
            }));
        }
        SECTION("-a cake error")
        {
            const auto res = parser.parse(make_arguments({"-a", "cookie" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
            {
                    create_info("usage: app [-h] [-a A]"),
                    create_error("'cookie' is not accepted for '-a', did you mean 'none', 'dog', 'cat' or 'bird'?")
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
            const auto res = parser.parse(make_arguments({}));
            CHECK(res == argparse::error);
            CHECK(value == "default");
            CHECK(check(output->messages,
            {
                    create_info("usage: app [-h] F"),
                    create_error("Positional F was not specified.")
            }));
        }

        SECTION("parse 42")
        {
            const auto res = parser.parse(make_arguments({"dog" }));
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
            const auto res = parser.parse(make_arguments({"dog" }));
            CHECK(res == argparse::ok);
            CHECK(a == 42);
            CHECK(b == 42);
            CHECK(check(files,
            {
                "dog"
            }));
        }

        SECTION("parse many")
        {
            const auto res = parser.parse(make_arguments({"dog", "cat"}));
            CHECK(res == argparse::ok);
            CHECK(a == 42);
            CHECK(b == 42);
            CHECK(check(files,
            {
                "dog", "cat"
            }));
        }

        SECTION("set single short argument")
        {
            const auto res = parser.parse(make_arguments({"-a", "5", "dog" }));
            CHECK(res == argparse::ok);
            CHECK(a == 5);
            CHECK(b == 42);
            CHECK(check(files,
            {
                "dog"
            }));
        }

        SECTION("set single long argument")
        {
            const auto res = parser.parse(make_arguments({"--alpha", "7", "dog" }));
            CHECK(res == argparse::ok);
            CHECK(a == 7);
            CHECK(b == 42);
            CHECK(check(files,
            {
                "dog"
            }));
        }

        SECTION("set many argument")
        {
            const auto res = parser.parse(make_arguments({"-a", "5", "-b", "3", "dog" }));
            CHECK(res == argparse::ok);
            CHECK(a == 5);
            CHECK(b == 3);
            CHECK(check(files,
            {
                "dog"
            }));
        }
    }

    SECTION("subparser")
    {
        std::string a = "default";
        std::string b = "default";

        auto subs = parser.add_sub_parsers();

        subs->add("a", "do awesome stuff", [&](SubParser* sub)
        {
            std::string a_value = "dog";
            sub->add("-s", &a_value);

            return sub->on_complete([&]
            {
                a = a_value;
                return argparse::ok;
            });
        });

        subs->add("b", "do boring stuff", [&](SubParser*)
        {
            b = "bird";
            return argparse::ok;
        });

        SECTION("empty subparser = error")
        {
            const auto res = parser.parse(make_arguments({}));
            // INFO(output->messages);
            CHECK(res == argparse::error);
            CHECK(a == "default");
            CHECK(b == "default");
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] <command> [<args>]"),
                create_error("no subparser specified")
            }));
        }

        SECTION("call a")
        {
            const auto res = parser.parse
            (
                make_arguments
                ({
                    "a"
                })
            );
            // INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(a == "dog");
            CHECK(b == "default");
        }

        SECTION("call a with arg")
        {
            const auto res = parser.parse
            (
                    make_arguments
                ({
                    "a", "-s", "cat"
                })
            );
            // INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(a == "cat");
            CHECK(b == "default");
        }

        SECTION("call b")
        {
            const auto res = parser.parse
            (
                    make_arguments
                ({
                    "b"
                })
            );
            // INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(a == "default");
            CHECK(b == "bird");
        }

        SECTION("subcommand help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_info(""),
                    create_info("optional arguments:"),
                    create_info("  -h, --help  show this help message and exit"),
                    create_info(""),
                    create_info("commands:"),
                    create_info("  a           do awesome stuff"),
                    create_info("  b           do boring stuff"),
            }));
        }

        SECTION("bad subcommand")
        {
            const auto res = parser.parse(make_arguments({"cat" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_error("Invalid command 'cat', did you mean 'a' or 'b'?")
            }));
        }
        SECTION("invalid optional for root")
        {
            const auto res = parser.parse(make_arguments({"-f", "dog" }));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_error("unknown argument: '-f', did you mean '-h'?")
            }));
        }
    }

    SECTION("non greedy subparser/script like")
    {
        auto subs = parser.add_sub_parsers();
        parser.parser_style = SubParserStyle::fallback;
        std::string data;
        subs->add("add", "add something", [&](SubParser* sub)
        {
            std::string what;
            sub->add("what", &what);
            return sub->on_complete([&]
            {
                data += what;
                return argparse::ok;
            });
        });
        subs->add("double", "double the content", [&](SubParser* sub)
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
                    make_arguments
                ({
                    "add", "dog"
                })
            );
            // INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(data == "dog");
        }

        SECTION("twice")
        {
            const auto res = parser.parse
            (
                    make_arguments
                ({
                    "add", "cat",
                    "double"
                })
            );
            // INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(data == "catcat");
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_info(""),
                    create_info("optional arguments:"),
                    create_info("  -h, --help  show this help message and exit"),
                    create_info(""),
                    create_info("commands:"),
                    create_info("  add         add something"),
                    create_info("  double      double the content"),
            }));
        }

        SECTION("add and error")
        {
            const auto res = parser.parse(make_arguments({"add", "dog", "dog" }));
            CHECK(res == argparse::error);
            CHECK(data == "dog");
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_error("Invalid command 'dog', did you mean 'add' or 'double'?")
            }));
        }
    }

    SECTION("non root root")
    {
        auto root = parser.add_sub_parsers();
        std::string data;
        root->add("pretty", "be kind", [&](SubParser* pretty)
        {
            pretty->add_sub_parsers()->add("please", [&](SubParser* please)
            {
                please->parser_style = SubParserStyle::fallback;
                auto subs = please->add_sub_parsers();
                subs->add("add", [&](SubParser* sub)
                {
                    std::string what;
                    sub->add("what", &what);
                    return sub->on_complete([&]
                    {
                        data += what;
                        return argparse::ok;
                    });
                });
                subs->add("double", [&](SubParser* sub)
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
                    make_arguments
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
                make_arguments
                ({
                    "pretty", "please", "add", "cat",
                    "please", "double"
                });
            const auto res = parser.parse
            (
                arguments
            );
            // INFO(output->messages);
            // INFO(arguments);
            REQUIRE(data == "catcat");
            CHECK(res == argparse::ok);
        }

        SECTION("print help")
        {
            const auto res = parser.parse(make_arguments({"-h" }));
            CHECK(res == argparse::quit);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h] <command> [<args>]"),
                    create_info(""),
                    create_info("optional arguments:"),
                    create_info("  -h, --help  show this help message and exit"),
                    create_info(""),
                    create_info("commands:"),
                    create_info("  pretty      be kind"),
            }));
        }

        SECTION("error but no add")
        {
            const auto res = parser.parse(make_arguments({"pretty", "dog" }));
            CHECK(res == argparse::error);
            REQUIRE(data.empty());
            CHECK(check(output->messages,
                        {
                    create_info("usage: app pretty [-h] <command> [<args>]"),
                    create_error("Invalid command 'dog', did you mean 'please'?")
            }));
        }
    }
}


TEST_CASE("argparse_error", "[argparse]")
{
    auto parser = argparse::Parser{};
    auto output = std::make_shared<TestPrinter>();
    parser.printer = output;

    SECTION("default")
    {
        SECTION("one positional")
        {
            const auto res = parser.parse(make_arguments({"dog"}));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h]"),
                    create_error("'dog' was unexpected")
            }));
        }
        SECTION("many positionals")
        {
            const auto res = parser.parse(make_arguments({"cat", "dog"}));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h]"),
                    create_error("'cat' was unexpected")
            }));
        }
        SECTION("optional 1 dash")
        {
            const auto res = parser.parse(make_arguments({"-o"}));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h]"),
                    create_error("unknown argument: '-o', did you mean '-h'?")
            }));
        }
        SECTION("optional 2 dashes")
        {
            const auto res = parser.parse(make_arguments({"--make-cool"}));
            CHECK(res == argparse::error);
            CHECK(check(output->messages,
                        {
                    create_info("usage: app [-h]"),
                    create_error("unknown argument: '--make-cool', did you mean '--help'?")
            }));
        }
    }

    SECTION("fourway test int")
    {
        using FF = Lrud<int>;
        auto ff = FF{0};
        parser.add("f", &ff);

        SECTION("one value")
        {
            const auto res = parser.parse(make_arguments({"4"}));
            // INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF{4});
        }
        SECTION("two values")
        {
            const auto res = parser.parse(make_arguments({"4/2"}));
            // INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF::from_lrud(2, 4));
        }
        SECTION("all values")
        {
            const auto res = parser.parse(make_arguments({"1/2/3/4"}));
            // INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF::from_lrud(4, 2, 1, 3));
        }
    }

    SECTION("fourway test enum")
    {
        using FF = Lrud<Animal>;
        auto ff = FF{Animal::none};
        parser.add("f", &ff);

        SECTION("one value")
        {
            const auto res = parser.parse(make_arguments({"cat"}));
            // INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF{Animal::cat});
        }
        SECTION("two values")
        {
            const auto res = parser.parse(make_arguments({"cat/none"}));
            // INFO(output->messages);
            CHECK(res == argparse::ok);
            CHECK(ff == FF::from_lrud(Animal::none, Animal::cat));
        }
    }

    SECTION("sub parser greedy")
    {
        auto sub = parser.add_sub_parsers();
        bool completed = false;
        parser.parser_style = SubParserStyle::greedy;
        sub->add("a", [&](SubParser* p)
        {
            return p->on_complete([&]
            {
                completed = true;
                return argparse::ok;
            });
        });

        SECTION("one positional")
        {
            const auto res = parser.parse(make_arguments({"a", "dog"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(check(output->messages,
            {
                create_info("usage: app a [-h]"),
                create_error("'dog' was unexpected")
            }));
        }
        SECTION("many positionals")
        {
            const auto res = parser.parse(make_arguments({"a", "cat", "dog"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(check(output->messages,
            {
                create_info("usage: app a [-h]"),
                create_error("'cat' was unexpected")
            }));
        }
        SECTION("optional 1 dash")
        {
            const auto res = parser.parse(make_arguments({"a", "-o"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(check(output->messages,
            {
                create_info("usage: app a [-h]"),
                create_error("unknown argument: '-o', did you mean '-h'?")
            }));
        }
        SECTION("optional 2 dashes")
        {
            const auto res = parser.parse(make_arguments({"a", "--make-cool"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(check(output->messages,
            {
                create_info("usage: app a [-h]"),
                create_error("unknown argument: '--make-cool', did you mean '--help'?")
            }));
        }
    }

    SECTION("sub parser fallback")
    {
        auto sub = parser.add_sub_parsers();
        bool completed = false;
        parser.parser_style = SubParserStyle::fallback;
        sub->add("a", [&](SubParser* p)
        {
            return p->on_complete([&]
            {
                completed = true;
                return argparse::ok;
            });
        });

        SECTION("one positional")
        {
            const auto res = parser.parse(make_arguments({"a", "dog"}));
            CHECK(res == argparse::error);
            CHECK(completed);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] <command> [<args>]"),
                create_error("Invalid command 'dog', did you mean 'a'?")
            }));
        }
        SECTION("many positionals")
        {
            const auto res = parser.parse(make_arguments({"a", "cat", "dog"}));
            CHECK(res == argparse::error);
            CHECK(completed);
            CHECK(check(output->messages,
            {
                create_info("usage: app [-h] <command> [<args>]"),
                create_error("Invalid command 'cat', did you mean 'a'?")
            }));
        }
        SECTION("optional 1 dash")
        {
            const auto res = parser.parse(make_arguments({"a", "-o"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(check(output->messages,
            {
                create_info("usage: app a [-h]"),
                create_error("unknown argument: '-o', did you mean '-h'?")
            }));
        }
        SECTION("optional 2 dashes")
        {
            const auto res = parser.parse(make_arguments({"a", "--make-cool"}));
            CHECK(res == argparse::error);
            CHECK_FALSE(completed);
            CHECK(check(output->messages,
            {
                create_info("usage: app a [-h]"),
                create_error("unknown argument: '--make-cool', did you mean '--help'?")
            }));
        }
    }
}

