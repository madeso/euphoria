#include "engine/dukprint.h"

#include <string>

#include "core/log.h"

#include "core/duk.h"


LOG_SPECIFY_DEFAULT_LOGGER("duk")

void
AddPrint(Duk* duk)
{
  duk->bind_print([](const std::string& str) { LOG_INFO(str); });
}
