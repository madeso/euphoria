#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "assert/assert.h"

int
main(int argc, char** argv)
{
#ifdef IMPLEMENT_ASSERT_LIB
    eu::assertlib::begin_throwing();
#endif

    int result = Catch::Session().run(argc, argv);
    return (result < 0xff ? result : 0xff);
}

