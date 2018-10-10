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

void*
GetVoidFunctionProperty(duk_context* ctx, const char* name);

void*
GetVoidProperty(duk_context* ctx, duk_idx_t index, const char* name);

void*
GetHiddenProperty(duk_context* ctx, duk_idx_t index, const std::string& name);

#endif  // EUPHORIA_DUK_UTIL_H
