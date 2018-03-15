#ifndef EUPHORIA_POINTERORNOT_H
#define EUPHORIA_POINTERORNOT_H

template <typename T>
struct StorePointer
{
  typedef T* Storage;

  static T
  Get(const Storage& storage)
  {
    return *storage;
  }

  static void
  Set(Storage* storage, T value)
  {
    **storage = value;
  }
};

template <typename T>
struct StoreValue
{
  typedef T Storage;

  static T
  Get(const Storage& storage)
  {
    return storage;
  }

  static void
  Set(Storage* storage, T value)
  {
    *storage = value;
  }
};

#endif  // EUPHORIA_POINTERORNOT_H
