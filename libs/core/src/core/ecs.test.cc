#include "core/ecs.h"

#include "tests/utils.h"

using namespace eu::core::ecs;
// using namespace eu::tests;

namespace
{
    struct Cat
    {
        int cat = 0;
    };

    struct Dog
    {
        int dog = 0;
    };
}

TEST_CASE("ecs2", "[ecs2]")
{
    Registry reg;

    SECTION("add and remove entity")
    {
        CHECK(reg.get_number_of_active_entities() == 0);

        auto a = reg.create();
        CHECK(reg.get_number_of_active_entities() == 1);

        reg.destroy(a);
        CHECK(reg.get_number_of_active_entities() == 0);
    }

    SECTION("add 2 components but use 1")
    {
        const std::string cat_name = "cat";
        const std::string dog_name = "dog";

        const auto cat = reg.register_component<Cat>(cat_name);
        const auto dog = reg.register_component<Dog>(dog_name);

        CHECK(reg.get_number_of_active_entities() == 0);

        auto a = reg.create();
        CHECK(reg.get_number_of_active_entities() == 1);
        CHECK(reg.view({cat}).empty());
        CHECK(reg.view({dog}).empty());

        reg.add_component(a, cat, Cat{42});

        const auto v = reg.view({cat});
        REQUIRE(v.size() == 1);
        CHECK(v[0] == a);
        CHECK(reg.view({dog}).empty());
        CHECK(reg.has_component(a, cat));
        CHECK_FALSE(reg.has_component(a, dog));
        CHECK(reg.get_component_or_null<Cat>(a, cat) != nullptr);
        CHECK(reg.get_component_or_null<Dog>(a, dog) == nullptr);

        CHECK(reg.get_component<Cat>(a, cat).cat == 42);
    }

    SECTION("use and reuse 2 components")
    {
        const std::string cat_name = "cat";
        const std::string dog_name = "dog";

        const auto cat = reg.register_component<Cat>(cat_name);
        const auto dog = reg.register_component<Dog>(dog_name);

        CHECK(reg.get_number_of_active_entities() == 0);

        auto a = reg.create();
        auto b = reg.create();

        reg.add_component(a, cat, Cat{42});
        reg.add_component(a, dog, Dog{10});

        reg.add_component(b, cat, Cat{24});
        reg.add_component(b, dog, Dog{20});

        CHECK(reg.get_number_of_active_entities() == 2);
        CHECK(reg.view({cat}).size() == 2);
        CHECK(reg.view({dog}).size() == 2);

        const auto v = reg.view({cat, dog});
        REQUIRE(v.size() == 2);
        CHECK(v[0] == a);
        CHECK(v[1] == b);

        CHECK(reg.get_component<Cat>(a, cat).cat == 42);
        CHECK(reg.get_component<Dog>(a, dog).dog == 10);

        CHECK(reg.get_component<Cat>(b, cat).cat == 24);
        CHECK(reg.get_component<Dog>(b, dog).dog == 20);

        // destroy the 2
        reg.destroy(a);
        reg.destroy(b);

        CHECK(reg.get_number_of_active_entities() == 0);
        CHECK(reg.view({cat}).empty());
        CHECK(reg.view({dog}).empty());

        // recreate 2 entities
        reg.create();
        reg.create();

        CHECK(reg.get_number_of_active_entities() == 2);
        CHECK(reg.view({cat}).empty());
        CHECK(reg.view({dog}).empty());
    }

    SECTION("2 name single compoent")
    {
        const std::string cat1_name = "cat-1";
        const std::string cat2_name = "cat-2";

        const auto cat1 = reg.register_component<Cat>(cat1_name);
        const auto cat2 = reg.register_component<Cat>(cat2_name);

        CHECK(reg.get_number_of_active_entities() == 0);

        auto a = reg.create();
        auto b = reg.create();

        reg.add_component(a, cat1, Cat{42});
        reg.add_component(a, cat2, Cat{10});

        reg.add_component(b, cat1, Cat{24});
        reg.add_component(b, cat2, Cat{20});

        CHECK(reg.get_number_of_active_entities() == 2);
        CHECK(reg.view({cat1}).size() == 2);
        CHECK(reg.view({cat2}).size() == 2);

        const auto v = reg.view({cat1, cat2});
        REQUIRE(v.size() == 2);
        CHECK(v[0] == a);
        CHECK(v[1] == b);

        CHECK(reg.get_component<Cat>(a, cat1).cat == 42);
        CHECK(reg.get_component<Cat>(a, cat2).cat == 10);

        CHECK(reg.get_component<Cat>(b, cat1).cat == 24);
        CHECK(reg.get_component<Cat>(b, cat2).cat == 20);

        // destroy the 2
        reg.destroy(a);
        reg.destroy(b);

        CHECK(reg.get_number_of_active_entities() == 0);
        CHECK(reg.view({cat1}).empty());
        CHECK(reg.view({cat2}).empty());

        // recreate 2 entities
        reg.create();
        reg.create();

        CHECK(reg.get_number_of_active_entities() == 2);
        CHECK(reg.view({cat1}).empty());
        CHECK(reg.view({cat2}).empty());
    }

    // todo(Gustav): add failing/asserting tests
}

TEST_CASE("ecs2-error", "[ecs2]")
{
    Registry reg;

    SECTION("assert when accessing with wrong type")
    {
        const std::string cat_name = "cat";
        const std::string dog_name = "dog";

        const auto cat = reg.register_component<Cat>(cat_name);
        reg.register_component<Dog>(dog_name);

        auto a = reg.create();
        reg.add_component(a, cat, Cat{42});

        REQUIRE_THROWS_WITH
        (
            reg.get_component<Dog>(a, cat),
            Catch::Contains("Assertion failed") && Catch::Contains("cat")
        );
    }
}
