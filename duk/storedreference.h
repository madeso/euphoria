#ifndef EUPHORIA_DUK_STOREDREFERENCE_H
#define EUPHORIA_DUK_STOREDREFERENCE_H

// todo: remove this include
#include "duk/referencestorage.h"

namespace duk
{
  class StoredReference
  {
   public:
    StoredReference(void* ptr, ReferenceStorage* duk);

    ~StoredReference();

   private:
    ReferenceStorage*       duk;
    ReferenceStorage::Index stored_index;
  };
}

#endif  // EUPHORIA_DUK_STOREDREFERENCE_H
