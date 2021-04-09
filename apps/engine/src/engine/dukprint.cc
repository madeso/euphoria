#include "engine/dukprint.h"

#include <string>

#include "core/log.h"

#include "core/sol.h"


LOG_SPECIFY_DEFAULT_LOGGER("engine.duk")

namespace euphoria::engine
{
    void
	AddPrint(Sol* sol)
    {
	  sol->lua.set_function("print", [](const std::string& str) { LOG_INFO(str); });
    }
}  // namespace euphoria::engine
