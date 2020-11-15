#include "duk/functionreference.h"

#include "duktape/duktape.h"

#include "duk/duk.h"

namespace euphoria::duk
{
    FunctionReference::FunctionReference(void* ptr) : function(ptr) {}

    bool
    FunctionReference::IsValid() const
    {
        return function != nullptr;
    }

    void
    FunctionReference::StoreReference(Context* ctx)
    {
        ASSERT(IsValid());
        ctx->duk->references.StoreReference(function);
    }

    void
    FunctionReference::BeginCall(Context* context) const
    {
        ASSERT(IsValid());
        duk_push_heapptr(context->ctx, function);
    }

    bool
    FunctionReference::CallFunction(Context* context, int arguments) const
    {
        ASSERT(IsValid());
        return duk_pcall(context->ctx, arguments) == DUK_EXEC_SUCCESS;
    }

    std::string
    FunctionReference::CollectError(Context* context) const
    {
        std::string error = "unknown error";
        CollectDukError(context->ctx, &error);
        duk_pop(context->ctx);
        return error;
    }

    void
    FunctionReference::DoneFunction(Context* context) const
    {
        ASSERT(IsValid());
        duk_pop(context->ctx);
    }
}  // namespace euphoria::duk
