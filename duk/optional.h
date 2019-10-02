#ifndef EUPHORIA_DUK_OPTIONAL_H
#define EUPHORIA_DUK_OPTIONAL_H


namespace euphoria::duk
{
  template <typename T>
  struct Optional
  {
    Optional() = default;

    explicit Optional(const T& t)
        : value(t)
        , parsed(true)
    {
    }

    explicit Optional(T&& t)
        : value(t)
        , parsed(true)
    {
    }

    operator bool() const
    {
      return parsed;
    }

    T* operator->()
    {
      return &value;
    }

    const T* operator->() const
    {
      return &value;
    }

    T    value;
    bool parsed = false;
  };

  template <typename T>
  struct IsOptional
  {
    static constexpr bool value = false;
  };

  template <typename T>
  struct IsOptional<Optional<T>>
  {
    static constexpr bool value = true;
  };
}


#endif  // EUPHORIA_DUK_OPTIONAL_H
