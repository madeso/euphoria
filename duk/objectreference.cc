#include "duk/objectreference.h"

#include "duktape/duktape.h"

#include "core/assert.h"
#include "duk/context.h"
#include "duk/duk.h"
#include "duk/storedreference.h"

namespace duk
{
  ObjectReference::ObjectReference()
      : ptr(nullptr)
  {
  }

  ObjectReference::~ObjectReference()
  {
  }

  ObjectReference::ObjectReference(void* p)
      : ptr(p)
  {
  }

  bool
  ObjectReference::IsValid() const
  {
    return ptr != nullptr;
  }

  void
  ObjectReference::StoreReference(Duk* duk)
  {
    ASSERT(duk);
    if(reference == nullptr)
    {
      reference = std::make_shared<StoredReference>(ptr, &duk->references);
    }
  }

  void
  ObjectReference::SetFreeImpl(
      Context*           ctx,
      const std::string& name,
      void*              object,
      TypeId type CLASS_ARG(TypeName classname))
  {
    ASSERT(ctx);
    duk_push_heapptr(ctx->ctx, ptr);
    ctx->ReturnObject(object, type, nullptr, nullptr CLASS_ARG(classname));
    duk_put_prop_string(ctx->ctx, -2, name.c_str());
    duk_pop(ctx->ctx);
  }
}
