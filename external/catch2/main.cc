#include "catch2/catch_session.hpp"

#include "assert/assert.h"

int
main(int argc, char** argv)
{
    // todo(Gustav): loook into replacing this with CATCH_REGISTER_LISTENER
    // (but it currently doesn't work when it's registreed from within a static library)
#ifdef IMPLEMENT_ASSERT_LIB
    eu::assertlib::begin_throwing();
#endif

    int result = Catch::Session().run(argc, argv);
    return (result < 0xff ? result : 0xff);
}

