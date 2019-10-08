#include "duk/duk.h"

#include "duk/function.h"
#include "duk/bindclass.h"
#include "duk/bindobject.h"
#include "duk/functionreference.h"

#include "catch.hpp"

using namespace euphoria::duk;

int allocated_dogs = 0;

class Dog
{
public:
    Dog(const std::string& n) : name(n)
    {
        allocated_dogs += 1;
    }

    ~Dog()
    {
        allocated_dogs -= 1;
    }

    std::string name;
};
TYPEID_SETUP_TYPE(Dog);

TEST_CASE("duk-eval", "[duk]")
{
    Duk         duk;
    std::string error;
    std::string out;

    SECTION("empty")
    {
        REQUIRE(duk.EvalString("", "", &error, &out));
    }

    SECTION("test int")
    {
        REQUIRE(duk.EvalString("a = 3; a", "", &error, &out));
        REQUIRE(out == "3");
    }

    SECTION("test float")
    {
        REQUIRE(duk.EvalString("a = 3.3; a", "", &error, &out));
        REQUIRE(out == "3.3");
    }

    SECTION("test string")
    {
        REQUIRE(duk.EvalString("a = \"dog\"; a", "", &error, &out));
        REQUIRE(out == "dog");
    }

    SECTION("object")
    {
        REQUIRE(duk.EvalString("a = new Object(); a", "", &error, &out));
        // test output
        // REQUIRE(out == "aaa");
    }

    SECTION("function")
    {
        REQUIRE(duk.EvalString("a = function() {}; a", "", &error, &out));
        // test output
        // REQUIRE(out == "aaa");
    }

    SECTION("print tests")
    {
        std::string print_result;
        duk.BindPrint([&](const std::string& str) { print_result = str; });

        CAPTURE(error);
        CAPTURE(out);
        SECTION("print int")
        {
            REQUIRE(duk.EvalString("print(3);", "", &error, &out));
            REQUIRE(print_result == "3");
        }

        SECTION("print double")
        {
            REQUIRE(duk.EvalString("print(4.2);", "", &error, &out));
            REQUIRE(print_result == "4.2");
        }

        SECTION("print string")
        {
            REQUIRE(duk.EvalString("print(\"cat\");", "", &error, &out));
            REQUIRE(print_result == "cat");
        }

        SECTION("print int")
        {
            REQUIRE(duk.EvalString("print(3);", "", &error, &out));
            REQUIRE(print_result == "3");
        }

        SECTION("print int & double")
        {
            REQUIRE(duk.EvalString("print(1, 4.2);", "", &error, &out));
            REQUIRE(print_result == "1 4.2");
        }

        SECTION("print string int & double")
        {
            REQUIRE(duk.EvalString(
                    "print(\"hat\", 1, 4.2);", "", &error, &out));
            REQUIRE(print_result == "hat 1 4.2");
        }

        SECTION("print var")
        {
            REQUIRE(duk.EvalString(
                    "hat = \"cat\"; print(hat, 1, 4.2);", "", &error, &out));
            REQUIRE(print_result == "cat 1 4.2");
        }
    }

    SECTION("custom functions")
    {
        SECTION("test()")
        {
            static int value;
            value = 2;
            duk.BindGlobalFunction("test", MakeBind([](Context* ctx) -> int {
                                       value = 12;
                                       return ctx->ReturnVoid();
                                   }));
            REQUIRE(value == 2);
            REQUIRE(duk.EvalString("test();", "", &error, &out));
            REQUIRE(value == 12);
        }

        SECTION("test(int)")
        {
            int value = 12;
            duk.BindGlobalFunction(
                    "test", MakeBind<int>([&](Context* ctx, int i) -> int {
                        value = i;
                        return ctx->ReturnVoid();
                    }));
            REQUIRE(value == 12);
            REQUIRE(duk.EvalString("test(42);", "", &error, &out));
            REQUIRE(value == 42);
        }

        SECTION("test(unsigned int)")
        {
            unsigned int value = 12;
            duk.BindGlobalFunction(
                    "test",
                    MakeBind<unsigned int>(
                            [&](Context* ctx, unsigned int i) -> int {
                                value = i;
                                return ctx->ReturnVoid();
                            }));
            REQUIRE(value == 12);
            REQUIRE(duk.EvalString("test(42);", "", &error, &out));
            REQUIRE(value == 42);
        }


        SECTION("test(int, int)")
        {
            int value1 = 1;
            int value2 = 2;
            duk.BindGlobalFunction(
                    "test",
                    MakeBind<int, int>([&](Context* ctx, int a, int b) -> int {
                        value1 = a;
                        value2 = b;
                        return ctx->ReturnVoid();
                    }));
            REQUIRE(value1 == 1);
            REQUIRE(value2 == 2);
            REQUIRE(duk.EvalString("test(11, 22);", "", &error, &out));
            REQUIRE(value1 == 11);
            REQUIRE(value2 == 22);
        }

        SECTION("test(string)")
        {
            std::string value = "";
            duk.BindGlobalFunction(
                    "test",
                    MakeBind<std::string>(
                            [&](Context* ctx, const std::string& s) -> int {
                                value = s;
                                return ctx->ReturnVoid();
                            }));
            REQUIRE(value == "");
            REQUIRE(duk.EvalString("test(\"dog\");", "", &error, &out));
            REQUIRE(value == "dog");
        }

        SECTION("test overload")
        {
            int value = 0;
            duk.BindGlobalFunction(
                    "test",
                    MakeBind<Optional<int>>(
                            [&](Context* ctx, Optional<int> i) -> int {
                                if(i)
                                {
                                    value = i.value;
                                }
                                else
                                {
                                    value = 707;
                                }
                                return 0;
                            }));
            REQUIRE(value == 0);
            REQUIRE(duk.EvalString("test(42);", "", &error, &out));
            REQUIRE(value == 42);
            value = 0;
            REQUIRE(duk.EvalString("test();", "", &error, &out));
            REQUIRE(value == 707);
        }

        SECTION("return int")
        {
            duk.BindGlobalFunction("test", MakeBind<>([&](Context* ctx) -> int {
                                       return ctx->ReturnNumber(42);
                                   }));
            REQUIRE(duk.EvalString("r = test(); r", "", &error, &out));
            REQUIRE(out == "42");
        }

        SECTION("return func int")
        {
            duk.BindGlobalFunction("test", MakeBind<>([&](Context* ctx) -> int {
                                       return ctx->Return(42);
                                   }));
            REQUIRE(duk.EvalString("r = test(); r", "", &error, &out));
            REQUIRE(out == "42");
        }

        SECTION("return string")
        {
            duk.BindGlobalFunction("test", MakeBind<>([&](Context* ctx) -> int {
                                       return ctx->ReturnString("dog");
                                   }));
            REQUIRE(duk.EvalString("r = test(); r", "", &error, &out));
            REQUIRE(out == "dog");
        }

        SECTION("return func string")
        {
            duk.BindGlobalFunction("test", MakeBind<>([&](Context* ctx) -> int {
                                       return ctx->Return("dog");
                                   }));
            REQUIRE(duk.EvalString("r = test(); r", "", &error, &out));
            REQUIRE(out == "dog");
        }

        SECTION("take int array")
        {
            std::vector<int> ints;
            duk.BindGlobalFunction(
                    "test",
                    MakeBind<std::vector<int>>(
                            [&](Context*                ctx,
                                const std::vector<int>& arg) -> int {
                                ints = arg;
                                return ctx->ReturnVoid();
                            }));
            const auto eval
                    = duk.EvalString("test([1, 2, 3])", "", &error, &out);
            CAPTURE(out);
            CAPTURE(error);
            REQUIRE(eval);
            REQUIRE(ints == (std::vector<int> {1, 2, 3}));
        }

        SECTION("return int array")
        {
            duk.BindGlobalFunction(
                    "test",
                    MakeBind<int>([](Context* ctx, const int var) -> int {
                        std::vector<int> arr = {var, var + 1, var + 2};
                        return ctx->ReturnArray(arr);
                    }));
            const auto eval = duk.EvalString("test(0)", "", &error, &out);
            CAPTURE(out);
            CAPTURE(error);
            REQUIRE(eval);
            // REQUIRE(out == "[0, 1, 2]");
            REQUIRE(out
                    == "array, object and object coercible: [0: 0, 1: 1, 2: 2]");
        }

        SECTION("return int array")
        {
            duk.BindGlobalFunction(
                    "test",
                    MakeBind<long>([](Context* ctx, const long var) -> int {
                        std::vector<long> arr = {var, var + 1, var + 2};
                        return ctx->ReturnArray(arr);
                    }));
            const auto eval = duk.EvalString("test(0)", "", &error, &out);
            CAPTURE(out);
            CAPTURE(error);
            REQUIRE(eval);
            // REQUIRE(out == "[0, 1, 2]");
            REQUIRE(out
                    == "array, object and object coercible: [0: 0, 1: 1, 2: 2]");
        }

        SECTION("return string array")
        {
            duk.BindGlobalFunction(
                    "test",
                    MakeBind<std::string>(
                            [](Context* ctx, const std::string& var) -> int {
                                std::vector<std::string> arr
                                        = {var, "a " + var, "good " + var};
                                return ctx->ReturnArray(arr);
                            }));
            const auto eval = duk.EvalString("test(\"dog\")", "", &error, &out);
            CAPTURE(out);
            CAPTURE(error);
            REQUIRE(eval);
            // REQUIRE(out == "[dog, a dog, good dog]");
            REQUIRE(
          out ==
          "array, object and object coercible: [0: dog, 1: a dog, 2: good "
          "dog]");
        }

        SECTION("create object")
        {
            auto dog = duk.CreateGlobal("Dog");
            duk.BindGlobalFunction("test", MakeBind<>([&](Context* ctx) -> int {
                                       return ctx->Return(dog);
                                   }));
            const auto eval = duk.EvalString(
                    "Dog.name = \"Duke\"; test().name", "", &error, &out);
            CAPTURE(out);
            CAPTURE(error);
            REQUIRE(eval);
            REQUIRE(out == "Duke");
        }

        SECTION("take function")
        {
            int i = 0;
            duk.BindGlobalFunction(
                    "test",
                    MakeBind<FunctionReference>(
                            [&](Context*                 ctx,
                                const FunctionReference& func) -> int {
                                i = func.Call<int>(ctx);
                                return ctx->ReturnVoid();
                            }));
            const auto eval = duk.EvalString(
                    "test(function() {return 42;});", "", &error, &out);
            CAPTURE(out);
            CAPTURE(error);
            REQUIRE(eval);
            REQUIRE(i == 42);
        }

        SECTION("take function with arg")
        {
            std::string str;
            duk.BindGlobalFunction(
                    "test",
                    MakeBind<FunctionReference>(
                            [&](Context*                 ctx,
                                const FunctionReference& func) -> int {
                                str = func.Call<std::string>(ctx, "Duke");
                                return ctx->ReturnVoid();
                            }));
            const auto eval = duk.EvalString(
                    "test(function(x) {return x+\" the dog\";});",
                    "",
                    &error,
                    &out);
            CAPTURE(out);
            CAPTURE(error);
            REQUIRE(eval);
            REQUIRE(str == "Duke the dog");
        }

        SECTION("store function")
        {
            FunctionReference f;
            duk.BindGlobalFunction(
                    "test",
                    MakeBind<FunctionReference>(
                            [&](Context*                 ctx,
                                const FunctionReference& func) -> int {
                                f = func;
                                f.StoreReference(ctx);
                                return ctx->ReturnVoid();
                            }));
            REQUIRE(!f.IsValid());
            const auto eval = duk.EvalString(
                    "test(function(x) {return x+\" the dog\";});",
                    "",
                    &error,
                    &out);
            CAPTURE(out);
            CAPTURE(error);
            REQUIRE(eval);
            REQUIRE(f.IsValid());
            const auto str = f.Call<std::string>(duk.AsContext(), "Duke");
            REQUIRE(str == "Duke the dog");
        }

        SECTION("global object")
        {
            duk.BindObject(
                    "Dog",
                    BindObject().AddFunction(
                            "bark", MakeBind([](Context* ctx) -> int {
                                return ctx->ReturnString("woof!");
                            })));
            const auto eval = duk.EvalString("Dog.bark()", "", &error, &out);
            CAPTURE(out);
            CAPTURE(error);
            REQUIRE(eval);
            REQUIRE(out == "woof!");
        }

        SECTION("class")
        {
            duk.BindClass(
                    "Dog",
                    BindClass<Dog>()
                            .SetConstructor(MakeBind<std::string>(
                                    [](Context*           ctx,
                                       const std::string& name) -> int {
                                        return ctx->ReturnObject(
                                                std::make_shared<Dog>(name));
                                    }))
                            .AddMethod(
                                    "GetName",
                                    MakeBind<Dog>(
                                            [](Context*   ctx,
                                               const Dog& d) -> int {
                                                return ctx->ReturnString(
                                                        d.name);
                                            }))
                            .AddMethod(
                                    "SetName",
                                    MakeBind<Dog, std::string>(
                                            [](Context*           ctx,
                                               Dog&               d,
                                               const std::string& name) -> int {
                                                d.name = name;
                                                return ctx->ReturnVoid();
                                            }))
                            .AddProperty(
                                    "name",
                                    MakeBind<Dog>(
                                            [](Context*   ctx,
                                               const Dog& dog) -> int {
                                                return ctx->ReturnString(
                                                        dog.name);
                                            }),
                                    MakeBind<Dog, std::string>(
                                            [](Context*           ctx,
                                               Dog&               d,
                                               const std::string& name) -> int {
                                                d.name = name;
                                                return ctx->ReturnVoid();
                                            })));

            SECTION("Call member get fun")
            {
                Dog duke {"Duke"};
                duk.BindGlobalFunction(
                        "GetDog", MakeBind([&](Context* ctx) -> int {
                            return ctx->ReturnFreeObject(&duke);
                        }));
                const auto eval = duk.EvalString(
                        "dog = GetDog(); name = dog.GetName(); name",
                        "",
                        &error,
                        &out);
                CAPTURE(out);
                CAPTURE(error);
                REQUIRE(eval);
                REQUIRE(out == "Duke");
            }

            SECTION("Call member set fun")
            {
                Dog duke {"Duke"};
                duk.BindGlobalFunction(
                        "GetDog", MakeBind([&](Context* ctx) -> int {
                            return ctx->ReturnFreeObject(&duke);
                        }));
                const auto eval = duk.EvalString(
                        "dog = GetDog(); dog.SetName(\"Cat\");",
                        "",
                        &error,
                        &out);
                CAPTURE(out);
                CAPTURE(error);
                REQUIRE(eval);
                REQUIRE(duke.name == "Cat");
            }

            SECTION("Use property")
            {
                Dog duke {"Duke"};
                duk.BindGlobalFunction(
                        "GetDog", MakeBind([&](Context* ctx) -> int {
                            return ctx->ReturnFreeObject(&duke);
                        }));
                const auto eval = duk.EvalString(
                        "dog = GetDog(); dog.name = dog.name + \" the dog\";",
                        "",
                        &error,
                        &out);
                CAPTURE(out);
                CAPTURE(error);
                REQUIRE(eval);
                REQUIRE(duke.name == "Duke the dog");
            }

            SECTION("Dynamic/scoped class allocation")
            {
                const std::string code
                        = "name = function(){var d = GetDog(); return d.name;}(); name";

                SECTION("naked ptr allocation")
                {
                    Dog* duke = nullptr;
                    REQUIRE(allocated_dogs == 0);
                    duk.BindGlobalFunction(
                            "GetDog", MakeBind([&](Context* ctx) -> int {
                                duke = new Dog {"duke"};
                                return ctx->ReturnFreeObject(duke);
                            }));
                    const auto eval = duk.EvalString(code, "", &error, &out);
                    CAPTURE(out);
                    CAPTURE(error);
                    REQUIRE(eval);
                    REQUIRE(out == "duke");
                    REQUIRE(allocated_dogs == 1);
                    delete duke;
                    REQUIRE(allocated_dogs == 0);
                }

                SECTION("shared_ptr")
                {
                    REQUIRE(allocated_dogs == 0);
                    duk.BindGlobalFunction(
                            "GetDog", MakeBind([&](Context* ctx) -> int {
                                return ctx->ReturnObject(
                                        std::make_shared<Dog>("duke"));
                            }));
                    const auto eval = duk.EvalString(code, "", &error, &out);
                    CAPTURE(out);
                    CAPTURE(error);
                    REQUIRE(eval);
                    REQUIRE(out == "duke");
                    REQUIRE(allocated_dogs == 0);
                }
            }

            SECTION("Constructor")
            {
                const auto eval = duk.EvalString(
                        "dog = new Dog(\"Cat\"); dog.name", "", &error, &out);
                CAPTURE(out);
                CAPTURE(error);
                REQUIRE(eval);
                REQUIRE(out == "Cat");
            }
        }

#if 0
    // need to figure out how to test error messages in a sane way
    SECTION("missing")
    {
      SECTION("too many arguments to test(int)")
      {
        int value = 12;
        duk.BindGlobalFunction("test", MakeBind<int>([&](Context* ctx, int i) -> int {
          value = i;
          return ctx->ReturnVoid();
        }));
        REQUIRE(value == 12);
        REQUIRE_FALSE(duk.eval_string("test(\"\", 2);", "", &error, &out));
        // REQUIRE(error == "");
      }

      SECTION("invalid arguments to test(int)")
      {
        int value = 12;
        duk.BindGlobalFunction("test", MakeBind<int>([&](Context* ctx, int i) -> int {
          value = i;
          return ctx->ReturnVoid();
        }));
        REQUIRE(value == 12);
        REQUIRE_FALSE(duk.eval_string("test(\"\");", "", &error, &out));
        REQUIRE(error == "");
      }
    }
#endif
    }
}
