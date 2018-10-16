#ifndef EUPHORIA_DUK_CONTEXT_H
#define EUPHORIA_DUK_CONTEXT_H

#include <string>
#include <vector>
#include <memory>

#include "core/typeinfo.h"

#include "duk/types.h"
#include "duk/config.h"
#include "duk/objectreference.h"
#include "duk/util.h"

namespace duk
{
  class RegisteredClass;

  class Duk;

  class Context
  {
   public:
    Context(duk_context* c, Duk* d);

    int
    GetNumberOfArguments() const;

    // number argument

    bool
    IsNumber(int index) const;

    double
    GetNumber(int index);

    // string argument

    bool
    IsString(int index) const;

    std::string
    GetString(int index);

    // object argument

    bool
    IsObject(int index);

    RegisteredClass*
    GetObjectType(int index);

    RegisteredClass*
    TypeToProto(size_t id CLASS_ARG(const std::string& name));

    void*
    GetObjectPtr(int index);

    // function

    void*
    GetFunctionPtr(int index);

    bool
    IsFunction(int index);

    // array handling
    bool
    IsArray(int index);

    int
    GetArrayLength(int index);

    void
    GetArrayIndex(int array, int index);

    void
    StopArrayIndex();

    // array return

    int
    PushArray();

    void
    PutArrayIndex(int arr, unsigned int i);

    // return handling

    int
    ReturnVoid();

    int
    Return(int value);

    int
    Return(const std::string& value);

    int
    Return(ObjectReference val);

    template <typename T>
    int
    ReturnArray(const std::vector<T> array);

    int
    ReturnNumber(double num);

    int
    ReturnBool(bool num);

    int
    ReturnString(const std::string& str);

    int
    ReturnObject(
        void*          object,
        size_t         type,
        duk_c_function finalizer,
        void* data CLASS_ARG(const std::string& name));

    template <typename T>
    int
    ReturnFreeObject(T* t)
    {
      return ReturnObject(
          t, TYPEID_ID(T), nullptr, nullptr CLASS_ARG(TYPEID_NAME(T)));
    }

    template <typename T>
    int
    ReturnObject(std::shared_ptr<T> t)
    {
      if(t.get() == nullptr)
      {
        return ReturnFreeObject<T>(nullptr);
      }
      auto* ptr = new std::shared_ptr<T>(t);
      return ReturnObject(
          ptr->get(),
          TYPEID_ID(T),
          // this needs to conform to a duktape c function pointer
          // if needed move to global scope and make a template
          [](duk_context* ctx) -> duk_ret_t {
            void* data   = GetHiddenProperty(ctx, 0, "data");
            auto* my_ptr = reinterpret_cast<std::shared_ptr<T>*>(data);
            delete my_ptr;
            return 0;
          },
          ptr CLASS_ARG(TYPEID_NAME(T)));
    }

    duk_context* ctx;
    Duk*         duk;
  };

  template <typename T>
  int
  Context::ReturnArray(const std::vector<T> array)
  {
    const auto   arr = PushArray();
    unsigned int i   = 0;
    for(const T& t : array)
    {
      PushVar(this, t);
      PutArrayIndex(arr, i);
      i += 1;
    }

    return 1;
  }
}

#endif  // EUPHORIA_DUK_CONTEXT_H
