#include "engine/dukprint.h"

#include <string>

#include "core/log.h"

#include "duk/duk.h"

LOG_SPECIFY_DEFAULT_LOGGER("engine.duk")

namespace euphoria::engine
{
  void
  AddPrint(duk::Duk* duk)
  {
    duk->BindPrint([](const std::string& str) { LOG_INFO(str); });
  }
}
