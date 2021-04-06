#ifndef EUPHORIA_DUK_STOREDREFERENCE_H
#define EUPHORIA_DUK_STOREDREFERENCE_H

// todo(Gustav): remove this include
#include "duk/referencestorage.h"

namespace euphoria::duk
{
    struct StoredReference
    {
    public:
        StoredReference(void* ptr, ReferenceStorage* duk);

        ~StoredReference();

    private:
        ReferenceStorage*       duk;
        ReferenceStorage::Index stored_index;
    };
}  // namespace euphoria::duk

#endif  // EUPHORIA_DUK_STOREDREFERENCE_H
