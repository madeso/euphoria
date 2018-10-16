#ifndef EUPHORIA_DUK_VALUE_H
#define EUPHORIA_DUK_VALUE_H

#include <string>
#include <memory>

#include "core/typeinfo.h"

#include "duk/config.h"

namespace duk
{
  class StoredReference;
  class Duk;
  class Context;

  // represents a object
  class ObjectReference
  {
   public:
    ObjectReference();
    explicit ObjectReference(void* p);

    ~ObjectReference();

    bool
    IsValid() const;

    // call this make the reference to be collected by the GC
    void
    StoreReference(Duk* duk);

    void
    SetFreeImpl(
        Context*           ctx,
        const std::string& name,
        void*              object,
        size_t type CLASS_ARG(const std::string& classname));

    template <typename T>
    void
    SetFree(Context* ctx, const std::string& name, T* t)
    {
      constexpr auto cpptype = TYPEID(T);
      SetFreeImpl(ctx, name, t, cpptype.id CLASS_ARG(cpptype.name));
    }

    void* ptr;

    std::shared_ptr<StoredReference> reference;
  };
}

#endif  // EUPHORIA_DUK_VALUE_H
