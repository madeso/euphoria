#include "duk/value.h"

#include "duktape/duktape.h"

#include "core/assert.h"
#include "duk/context.h"
#include "duk/duk.h"
#include "duk/storedreference.h"

DukValue::DukValue()
    : ptr(nullptr)
{
}

DukValue::~DukValue()
{
}

DukValue::DukValue(void* p)
    : ptr(p)
{
}

bool
DukValue::IsValid() const
{
  return ptr != nullptr;
}

void
DukValue::StoreReference(Duk* duk)
{
  ASSERT(duk);
  if(reference == nullptr)
  {
    reference = std::make_shared<StoredReference>(ptr, &duk->references);
  }
}

void
DukValue::SetFreeImpl(
    Context*           ctx,
    const std::string& name,
    void*              object,
    size_t type CLASS_ARG(const std::string& classname))
{
  ASSERT(ctx);
  duk_push_heapptr(ctx->ctx, ptr);
  ctx->ReturnObject(object, type, nullptr, nullptr CLASS_ARG(classname));
  duk_put_prop_string(ctx->ctx, -2, name.c_str());
  duk_pop(ctx->ctx);
}
