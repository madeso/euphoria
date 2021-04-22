#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "core/assert.h"

int
main(int argc, char** argv)
{
#ifdef IMPLEMENT_ASSERT_LIB
    euphoria::core::assertlib::start_throwing();
#endif

    int result = Catch::Session().run(argc, argv);
    return (result < 0xff ? result : 0xff);
}
