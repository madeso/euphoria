#ifndef EUPHORIA_DUK_VALUE_H
#define EUPHORIA_DUK_VALUE_H

#include <string>
#include <memory>

#include "duk/config.h"

class StoredReference;
class Duk;
class Context;

// represents a object
class DukValue
{
 public:
  DukValue();
  explicit DukValue(void* p);

  ~DukValue();

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
    constexpr auto& cpptype = typeid(T);
    SetFreeImpl(ctx, name, t, cpptype.hash_code() CLASS_ARG(cpptype.name()));
  }

  void* ptr;

  std::shared_ptr<StoredReference> reference;
};

#endif  // EUPHORIA_DUK_VALUE_H
