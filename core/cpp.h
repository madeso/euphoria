#ifndef EUPHORIA_CPP_H
#define EUPHORIA_CPP_H

template <class T>
void
NotUsed(T const&)
{
}

template <typename T>
struct DefaultValue
{
  T value;
};

template <typename T>
T
Default()
{
  return DefaultValue<T>{}.value;
}

#endif  // EUPHORIA_CPP_H
