#include "core/cypher.h"

#include "catch.hpp"
#include "tests/utils.h"
#include "tests/falsestring.h"
#include "tests/vectorequals.h"
#include "tests/stringeq.h"


using namespace euphoria::core;
using namespace euphoria::tests;


TEST_CASE("cypher-rot13-wikipedia", "[cypher]")
{
    CHECK(is_string_equal(
            "Why did the chicken cross the road?",
            rot13("Jul qvq gur puvpxra pebff gur ebnq?")
    ));
    CHECK(is_string_equal(
            "Gb trg gb gur bgure fvqr!",
            rot13("To get to the other side!")
    ));
    CHECK(is_string_equal(
            rot13("Why did the chicken cross the road?"),
            "Jul qvq gur puvpxra pebff gur ebnq?"
    ));
    CHECK(is_string_equal(
            rot13("Gb trg gb gur bgure fvqr!"),
            "To get to the other side!"
    ));
}

