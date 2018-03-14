#ifndef EUPHORIA_POINTERORNOT_H
#define EUPHORIA_POINTERORNOT_H

template <typename T>
struct StorePointer
{
  typedef T* Storage;
};

template <typename T>
struct StoreValue
{
  typedef T Storage;
};

#endif  // EUPHORIA_POINTERORNOT_H
