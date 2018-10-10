#ifndef EUPHORIA_DUK_REFERENCESTORAGE_H
#define EUPHORIA_DUK_REFERENCESTORAGE_H

#include <vector>

#include "duk/types.h"

class ReferenceStorage
{
 public:
  explicit ReferenceStorage(duk_context* c);

  using Index = unsigned int;

  ReferenceStorage::Index
  StoreReference(void* p);

  void
  ClearReference(Index index);

  void
  SetReference(void* p, Index index);

 private:
  duk_context*       ctx;
  std::vector<Index> free_indices;
  Index              reference_index = 0;
};


#endif  // EUPHORIA_DUK_REFERENCESTORAGE_H
