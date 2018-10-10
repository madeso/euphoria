#include "duk/context.h"

#include "duktape/duktape.h"

#include "core/assert.h"

#include "duk/duk.h"
#include "duk/prototype.h"

Context::Context(duk_context* c, Duk* d)
    : ctx(c)
    , duk(d)
{
}

int
Context::GetNumberOfArguments() const
{
  return duk_get_top(ctx);
}

////////////////////////////////////////////////////////////////////////////////

bool
Context::IsNumber(int index) const
{
  return duk_is_number(ctx, index) == 1;
}

double
Context::GetNumber(int index)
{
  return duk_get_number(ctx, index);
}

////////////////////////////////////////////////////////////////////////////////

bool
Context::IsString(int index) const
{
  return duk_is_string(ctx, index) == 1;
}

std::string
Context::GetString(int index)
{
  return duk_get_string(ctx, index);
}

////////////////////////////////////////////////////////////////////////////////

bool
Context::IsObject(int index)
{
  return duk_is_object(ctx, index) == 1;
}

Prototype*
Context::GetObjectType(int index)
{
  duk_get_prop_string(ctx, index, DUK_HIDDEN_SYMBOL("type"));
  const auto ptr = duk_get_pointer(ctx, -1);
  duk_pop(ctx);
  return static_cast<Prototype*>(ptr);
}

Prototype*
Context::TypeToProto(size_t id CLASS_ARG(const std::string& name))
{
  return duk->TypeToProto(id CLASS_ARG(name));
}

void*
Context::GetObjectPtr(int index)
{
  duk_get_prop_string(ctx, index, DUK_HIDDEN_SYMBOL("ptr"));
  void* data = duk_get_pointer(ctx, -1);
  duk_pop(ctx);
  return data;
}

////////////////////////////////////////////////////////////////////////////////

void*
Context::GetFunctionPtr(int index)
{
  return duk_get_heapptr(ctx, index);
}

bool
Context::IsFunction(int index)
{
  return duk_is_function(ctx, index) == 1;
}

////////////////////////////////////////////////////////////////////////////////

bool
Context::IsArray(int index)
{
  return duk_is_array(ctx, index) == 1;
}

int
Context::GetArrayLength(int index)
{
  // duk_inspect_value(ctx, index);
  duk_get_prop_string(ctx, index, "length");
  const auto size = duk_to_int(ctx, -1);
  // duk_pop_2(ctx);
  duk_pop(ctx);
  return size;
}

void
Context::GetArrayIndex(int array, int index)
{
  const auto put = duk_get_prop_index(ctx, array, index);
  ASSERT(put == 1);
}

void
Context::StopArrayIndex()
{
  duk_pop(ctx);
}

int
Context::PushArray()
{
  return duk_push_array(ctx);
}

void
Context::PutArrayIndex(int arr, unsigned int i)
{
  duk_put_prop_index(ctx, arr, i);
}

////////////////////////////////////////////////////////////////////////////////

int
Context::ReturnVoid()
{
  return 0;
}

int
Context::Return(int value)
{
  return ReturnNumber(value);
}

int
Context::Return(const std::string& value)
{
  return ReturnString(value);
}

int
Context::Return(DukValue val)
{
  duk_push_heapptr(ctx, val.ptr);
  return 1;
}

int
Context::ReturnNumber(double num)
{
  duk_push_number(ctx, num);
  return 1;
}

int
Context::ReturnBool(bool num)
{
  duk_push_boolean(ctx, num ? 1 : 0);
  return 1;
}

int
Context::ReturnString(const std::string& str)
{
  duk_push_string(ctx, str.c_str());
  return 1;
}

int
Context::ReturnObject(
    void*          object,
    size_t         type,
    duk_c_function finalizer,
    void* data CLASS_ARG(const std::string& name))
{
  if(object == nullptr)
  {
    duk_push_null(ctx);
    return 1;
  }
  Prototype* proto     = duk->TypeToProto(type CLASS_ARG(name));
  const auto object_id = duk_push_object(ctx);  // object

  // prototype
  duk_push_heapptr(ctx, proto->prototype);  // object proto
  duk_set_prototype(ctx, object_id);        // object

  // type id
  duk_push_pointer(ctx, proto);                                    // object id
  duk_put_prop_string(ctx, object_id, DUK_HIDDEN_SYMBOL("type"));  // object

  // object ptr
  duk_push_pointer(ctx, object);                                  // object ptr
  duk_put_prop_string(ctx, object_id, DUK_HIDDEN_SYMBOL("ptr"));  // object

  if(finalizer)
  {
    duk_push_pointer(ctx, data);  // object data
    duk_put_prop_string(ctx, object_id, DUK_HIDDEN_SYMBOL("data"));  // object

    duk_push_c_function(ctx, finalizer, 1);  // object finalzer
    duk_set_finalizer(ctx, object_id);       // object
  }

  return 1;
}