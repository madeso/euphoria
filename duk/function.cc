#include "core/duk_function.h"

#include "duktape/duktape.h"

#include "core/duk_duk.h"

FunctionVar::FunctionVar(void* ptr)
    : function(ptr)
{
}

bool
FunctionVar::IsValid() const
{
  return function != nullptr;
}

void
FunctionVar::StoreReference(Context* ctx)
{
  ctx->duk->references.StoreReference(function);
}

void
FunctionVar::BeginCall(Context* context) const
{
  ASSERT(IsValid());
  duk_push_heapptr(context->ctx, function);
}

bool
FunctionVar::CallFunction(Context* context, int arguments) const
{
  ASSERT(IsValid());
  return duk_pcall(context->ctx, arguments) == DUK_EXEC_SUCCESS;
}

std::string
FunctionVar::CollectError(Context* context) const
{
  std::string error = "unknown error";
  CollectDukError(context->ctx, &error);
  duk_pop(context->ctx);
  return error;
}

void
FunctionVar::DoneFunction(Context* context) const
{
  ASSERT(IsValid());
  duk_pop(context->ctx);
}