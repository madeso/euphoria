#include "duk/referencestorage.h"

#include "duktape/duktape.h"
#include "core/log.h"

namespace euphoria::duk
{
    LOG_SPECIFY_DEFAULT_LOGGER("engine.duk.storage")

    ReferenceStorage::ReferenceStorage(duk_context* c) : ctx(c) {}

    ReferenceStorage::Index
    ReferenceStorage::StoreReference(void* p)
    {
        ReferenceStorage::Index index;
        if(free_indices.empty())
        {
            index = reference_index;
            reference_index += 1;
            LOG_INFO("Using new reference " << index);
        }
        else
        {
            index = *free_indices.rbegin();
            LOG_DEBUG("Reusing reference " << index);
            free_indices.pop_back();
        }
        SetReference(p, index);
        return index;
    }

    void
    ReferenceStorage::ClearReference(ReferenceStorage::Index index)
    {
        free_indices.emplace_back(index);
        SetReference(nullptr, index);
        LOG_DEBUG("Freeing reference " << index);
    }

    void
    ReferenceStorage::SetReference(void* p, ReferenceStorage::Index index)
    {
        // todo: store references in some sub object instead of directly at root?
        duk_push_heap_stash(ctx);  // heap
        if(p != nullptr)
        {
            duk_push_heapptr(ctx, p);  // heap ptr
        }
        else
        {
            duk_push_null(ctx);
        }
        duk_put_prop_index(ctx, -2, index);  // heap
        duk_pop(ctx);  //
    }
}  // namespace euphoria::duk
