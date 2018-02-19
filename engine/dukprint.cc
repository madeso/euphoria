#include "engine/dukprint.h"

#include <string>

#include "dukglue/dukglue.h"

#include "core/log.h"

#include "engine/duk.h"


LOG_SPECIFY_DEFAULT_LOGGER("duk")


void
Print(const std::string& str)
{
  LOG_INFO(str);
}

void
AddPrint(Duk* duk)
{
  dukglue_register_function(duk->ctx, &Print, "print");
}
