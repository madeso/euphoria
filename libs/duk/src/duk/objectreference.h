#ifndef EUPHORIA_DUK_VALUE_H
#define EUPHORIA_DUK_VALUE_H

#include <string>
#include <memory>

#include "core/typeinfo.h"

#include "duk/config.h"

namespace euphoria::duk
{
    struct StoredReference;
    struct Duk;
    struct Context;

    // represents a object
    struct ObjectReference
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
                core::TypeId type  CLASS_ARG(core::TypeName classname));

        template <typename T>
        void
        SetFree(Context* ctx, const std::string& name, T* t)
        {
            SetFreeImpl(ctx, name, t, TYPEID_ID(T) CLASS_ARG(TYPEID_NAME(T)));
        }

        void* ptr;

        std::shared_ptr<StoredReference> reference;
    };
}  // namespace euphoria::duk

#endif  // EUPHORIA_DUK_VALUE_H
