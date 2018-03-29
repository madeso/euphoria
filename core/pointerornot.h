#ifndef EUPHORIA_POINTERORNOT_H
#define EUPHORIA_POINTERORNOT_H

#include "core/assert.h"

template <typename T>
bool IsValid(T);

template <>
bool
IsValid(int);

template <>
bool
IsValid(float f);

template <typename T>
struct StoreValue
{
  typedef T Storage;

  static T
  Get(const Storage& storage)
  {
    ASSERTX(IsValid(storage), storage);
    return storage;
  }

  static void
  Set(Storage* storage, T value)
  {
    ASSERTX(::IsValid(value), value);
    *storage = value;
  }

  static bool
  IsValid(const Storage& storage)
  {
    return ::IsValid(storage);
  }
};

template <typename T>
struct StorePointer
{
  typedef T* Storage;

  static T
  Get(const Storage& storage)
  {
    ASSERTX(IsValid(storage), storage);
    return *storage;
  }

  static void
  Set(Storage* storage, T value)
  {
    ASSERTX(::IsValid(value), value);
    **storage = value;
  }

  static bool
  IsValid(const Storage& storage)
  {
    return storage != nullptr && ::IsValid(*storage);
  }
};

#endif  // EUPHORIA_POINTERORNOT_H
