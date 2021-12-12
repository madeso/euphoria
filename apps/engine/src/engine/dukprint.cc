#include "engine/dukprint.h"

#include <string>

#include "core/log.h"

#include "core/sol.h"


namespace euphoria::engine
{
    void
	add_print(LuaState* sol)
    {
        sol->state.set_function
        (
            "print",
            [](const std::string& str)
            {
                LOG_INFO(str);
            }
        );
    }
}
