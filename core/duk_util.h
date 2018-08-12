#ifndef EUPHORIA_DUK_UTIL_H
#define EUPHORIA_DUK_UTIL_H

#include <string>
#include "core/duk_types.h"

std::string
VarToString(duk_context* ctx, int index);

void
FatalHandler(void*, const char* msg);

void
CollectDukError(duk_context* ctx, std::string* error);

#endif  // EUPHORIA_DUK_UTIL_H
