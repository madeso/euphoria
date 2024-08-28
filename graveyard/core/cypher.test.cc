#include "core/cypher.h"

#include "catch2/catch_all.hpp"
#include "tests/utils.h"
#include "tests/falsestring.h"
#include "tests/vectorequals.h"
#include "tests/stringeq.h"


using namespace eu;
using namespace eu::core;
using namespace eu::tests;


TEST_CASE("cypher-get_rot13-wikipedia", "[cypher]")
{
    CHECK(is_string_equal(
            "Why did the chicken cross the road?",
            get_rot13("Jul qvq gur puvpxra pebff gur ebnq?")
    ));
    CHECK(is_string_equal(
            "Gb trg gb gur bgure fvqr!",
            get_rot13("To get to the other side!")
    ));
    CHECK(is_string_equal(
            get_rot13("Why did the chicken cross the road?"),
            "Jul qvq gur puvpxra pebff gur ebnq?"
    ));
    CHECK(is_string_equal(
            get_rot13("Gb trg gb gur bgure fvqr!"),
            "To get to the other side!"
    ));
}

