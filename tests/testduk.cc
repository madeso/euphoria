#include "core/duk.h"

#include "catch.hpp"

int allocated_dogs = 0;

class Dog
{
 public:
  Dog(const std::string& n)
      : name(n)
  {
    allocated_dogs += 1;
  }

  ~Dog()
  {
    allocated_dogs -= 1;
  }

  std::string name;
};

TEST_CASE("duk-eval", "[duk]")
{
  Duk         duk;
  std::string error;
  std::string out;

  SECTION("empty")
  {
    REQUIRE(duk.eval_string("", "", &error, &out));
  }

  SECTION("test int")
  {
    REQUIRE(duk.eval_string("a = 3; a", "", &error, &out));
    REQUIRE(out == "3");
  }

  SECTION("test float")
  {
    REQUIRE(duk.eval_string("a = 3.3; a", "", &error, &out));
    REQUIRE(out == "3.3");
  }

  SECTION("test string")
  {
    REQUIRE(duk.eval_string("a = \"dog\"; a", "", &error, &out));
    REQUIRE(out == "dog");
  }

  SECTION("object")
  {
    REQUIRE(duk.eval_string("a = new Object(); a", "", &error, &out));
    // test output
    // REQUIRE(out == "aaa");
  }

  SECTION("function")
  {
    REQUIRE(duk.eval_string("a = function() {}; a", "", &error, &out));
    // test output
    // REQUIRE(out == "aaa");
  }

  SECTION("print tests")
  {
    std::string print_result;
    duk.bind_print([&](const std::string& str) { print_result = str; });

    CAPTURE(error);
    CAPTURE(out);
    SECTION("print int")
    {
      REQUIRE(duk.eval_string("print(3);", "", &error, &out));
      REQUIRE(print_result == "3");
    }

    SECTION("print double")
    {
      REQUIRE(duk.eval_string("print(4.2);", "", &error, &out));
      REQUIRE(print_result == "4.2");
    }

    SECTION("print string")
    {
      REQUIRE(duk.eval_string("print(\"cat\");", "", &error, &out));
      REQUIRE(print_result == "cat");
    }

    SECTION("print int")
    {
      REQUIRE(duk.eval_string("print(3);", "", &error, &out));
      REQUIRE(print_result == "3");
    }

    SECTION("print int & double")
    {
      REQUIRE(duk.eval_string("print(1, 4.2);", "", &error, &out));
      REQUIRE(print_result == "1 4.2");
    }

    SECTION("print string int & double")
    {
      REQUIRE(duk.eval_string("print(\"hat\", 1, 4.2);", "", &error, &out));
      REQUIRE(print_result == "hat 1 4.2");
    }

    SECTION("print var")
    {
      REQUIRE(duk.eval_string(
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
      duk.BindGlobalFunction("test", Bind{}.bind<>([](Context* ctx) -> int {
        value = 12;
        return ctx->ReturnVoid();
      }));
      REQUIRE(value == 2);
      REQUIRE(duk.eval_string("test();", "", &error, &out));
      REQUIRE(value == 12);
    }

    SECTION("test(int)")
    {
      int value = 12;
      duk.BindGlobalFunction(
          "test", Bind{}.bind<int>([&](Context* ctx, int i) -> int {
            value = i;
            return ctx->ReturnVoid();
          }));
      REQUIRE(value == 12);
      REQUIRE(duk.eval_string("test(42);", "", &error, &out));
      REQUIRE(value == 42);
    }

    SECTION("test(int, int)")
    {
      int value1 = 1;
      int value2 = 2;
      duk.BindGlobalFunction(
          "test", Bind{}.bind<int, int>([&](Context* ctx, int a, int b) -> int {
            value1 = a;
            value2 = b;
            return ctx->ReturnVoid();
          }));
      REQUIRE(value1 == 1);
      REQUIRE(value2 == 2);
      REQUIRE(duk.eval_string("test(11, 22);", "", &error, &out));
      REQUIRE(value1 == 11);
      REQUIRE(value2 == 22);
    }

    SECTION("test(string)")
    {
      std::string value = "";
      duk.BindGlobalFunction(
          "test",
          Bind{}.bind<std::string>(
              [&](Context* ctx, const std::string& s) -> int {
                value = s;
                return ctx->ReturnVoid();
              }));
      REQUIRE(value == "");
      REQUIRE(duk.eval_string("test(\"dog\");", "", &error, &out));
      REQUIRE(value == "dog");
    }

    SECTION("test overload")
    {
      int value = 0;
      duk.BindGlobalFunction(
          "test",
          Bind{}
              .bind<int>([&](Context* ctx, int i) -> int {
                value = i;
                return 0;
              })
              .bind<>([&](Context* ctx) -> int {
                value = 707;
                return ctx->ReturnVoid();
              }));
      REQUIRE(value == 0);
      REQUIRE(duk.eval_string("test(42);", "", &error, &out));
      REQUIRE(value == 42);
      value = 0;
      REQUIRE(duk.eval_string("test();", "", &error, &out));
      REQUIRE(value == 707);
    }

    SECTION("return int")
    {
      duk.BindGlobalFunction("test", Bind{}.bind<>([&](Context* ctx) -> int {
        return ctx->ReturnNumber(42);
      }));
      REQUIRE(duk.eval_string("r = test(); r", "", &error, &out));
      REQUIRE(out == "42");
    }

    SECTION("return string")
    {
      duk.BindGlobalFunction("test", Bind{}.bind<>([&](Context* ctx) -> int {
        return ctx->ReturnString("dog");
      }));
      REQUIRE(duk.eval_string("r = test(); r", "", &error, &out));
      REQUIRE(out == "dog");
    }

    SECTION("take int array")
    {
      std::vector<int> ints;
      duk.BindGlobalFunction(
          "test",
          Bind{}.bind<std::vector<int>>(
              [&](Context* ctx, const std::vector<int>& arg) -> int {
                ints = arg;
                return ctx->ReturnVoid();
              }));
      const auto eval = duk.eval_string("test([1, 2, 3])", "", &error, &out);
      CAPTURE(out);
      CAPTURE(error);
      REQUIRE(eval);
      REQUIRE(ints == (std::vector<int>{1, 2, 3}));
    }

    SECTION("take function")
    {
      int i = 0;
      duk.BindGlobalFunction(
          "test",
          Bind{}.bind<FunctionVar>(
              [&](Context* ctx, const FunctionVar& func) -> int {
                i = func.Call<int>(ctx);
                return ctx->ReturnVoid();
              }));
      const auto eval =
          duk.eval_string("test(function() {return 42;});", "", &error, &out);
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
          Bind{}.bind<FunctionVar>(
              [&](Context* ctx, const FunctionVar& func) -> int {
                str = func.Call<std::string>(ctx, std::string("Duke"));
                return ctx->ReturnVoid();
              }));
      const auto eval = duk.eval_string(
          "test(function(x) {return x+\" the dog\";});", "", &error, &out);
      CAPTURE(out);
      CAPTURE(error);
      REQUIRE(eval);
      REQUIRE(str == "Duke the dog");
    }

    SECTION("store function")
    {
      FunctionVar f;
      duk.BindGlobalFunction(
          "test",
          Bind{}.bind<FunctionVar>(
              [&](Context* ctx, const FunctionVar& func) -> int {
                f = func;
                // todo: store func so the GC wont collect it
                return ctx->ReturnVoid();
              }));
      REQUIRE(!f.IsValid());
      const auto eval = duk.eval_string(
          "test(function(x) {return x+\" the dog\";});", "", &error, &out);
      CAPTURE(out);
      CAPTURE(error);
      REQUIRE(eval);
      REQUIRE(f.IsValid());
      const auto str =
          f.Call<std::string>(duk.AsContext(), std::string("Duke"));
      REQUIRE(str == "Duke the dog");
    }

    SECTION("class")
    {
      duk.BindClass(
          "Dog",
          BindClass<Dog>()
              .SetConstructor(Bind{}.bind<std::string>(
                  [](Context* ctx, const std::string& name) -> int {
                    return ctx->ReturnObject(std::make_shared<Dog>(name));
                  }))
              .AddMethod(
                  "GetName",
                  Bind{}.bind<Dog>([](Context* ctx, const Dog& d) -> int {
                    return ctx->ReturnString(d.name);
                  }))
              .AddMethod(
                  "SetName",
                  Bind{}.bind<Dog, std::string>(
                      [](Context* ctx, Dog& d, const std::string& name) -> int {
                        d.name = name;
                        return ctx->ReturnVoid();
                      }))
              .AddProperty(
                  "name",
                  Bind{}.bind<Dog>([](Context* ctx, const Dog& dog) -> int {
                    return ctx->ReturnString(dog.name);
                  }),
                  Bind{}.bind<Dog, std::string>(
                      [](Context* ctx, Dog& d, const std::string& name) -> int {
                        d.name = name;
                        return ctx->ReturnVoid();
                      })));

      SECTION("Call member get fun")
      {
        Dog duke{"Duke"};
        duk.BindGlobalFunction("GetDog", Bind{}.bind([&](Context* ctx) -> int {
          return ctx->ReturnFreeObject(&duke);
        }));
        const auto eval = duk.eval_string(
            "dog = GetDog(); name = dog.GetName(); name", "", &error, &out);
        CAPTURE(out);
        CAPTURE(error);
        REQUIRE(eval);
        REQUIRE(out == "Duke");
      }

      SECTION("Call member set fun")
      {
        Dog duke{"Duke"};
        duk.BindGlobalFunction("GetDog", Bind{}.bind([&](Context* ctx) -> int {
          return ctx->ReturnFreeObject(&duke);
        }));
        const auto eval = duk.eval_string(
            "dog = GetDog(); dog.SetName(\"Cat\");", "", &error, &out);
        CAPTURE(out);
        CAPTURE(error);
        REQUIRE(eval);
        REQUIRE(duke.name == "Cat");
      }

      SECTION("Use property")
      {
        Dog duke{"Duke"};
        duk.BindGlobalFunction("GetDog", Bind{}.bind([&](Context* ctx) -> int {
          return ctx->ReturnFreeObject(&duke);
        }));
        const auto eval = duk.eval_string(
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
        const std::string code =
            "name = function(){var d = GetDog(); return d.name;}(); name";

        SECTION("naked ptr allocation")
        {
          Dog* duke = nullptr;
          REQUIRE(allocated_dogs == 0);
          duk.BindGlobalFunction(
              "GetDog", Bind{}.bind([&](Context* ctx) -> int {
                duke = new Dog{"duke"};
                return ctx->ReturnFreeObject(duke);
              }));
          const auto eval = duk.eval_string(code, "", &error, &out);
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
              "GetDog", Bind{}.bind([&](Context* ctx) -> int {
                return ctx->ReturnObject(std::make_shared<Dog>("duke"));
              }));
          const auto eval = duk.eval_string(code, "", &error, &out);
          CAPTURE(out);
          CAPTURE(error);
          REQUIRE(eval);
          REQUIRE(out == "duke");
          REQUIRE(allocated_dogs == 0);
        }
      }

      SECTION("Constructor")
      {
        const auto eval = duk.eval_string(
            "dog = new Dog(\"Cat\"); dog.name", "", &error, &out);
        CAPTURE(out);
        CAPTURE(error);
        REQUIRE(eval);
        REQUIRE(out == "Cat");
      }
    }

#if 0
    // need to figure out how to test error messages is a sane way
    SECTION("missing")
    {
      SECTION("too many arguments to test(int)")
      {
        int value = 12;
        duk.BindGlobalFunction("test", Bind{}.bind<int>([&](Context* ctx, int i) -> int {
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
        duk.BindGlobalFunction("test", Bind{}.bind<int>([&](Context* ctx, int i) -> int {
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
