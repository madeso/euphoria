#ifndef EUPHORIA_DUK_DUK_H
#define EUPHORIA_DUK_DUK_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

#include "duk/context.h"
#include "duk/value.h"
#include "duk/referencestorage.h"
#include "duk/config.h"
#include "duk/overload.h"

namespace duk
{
  class ObjectBinder;

  class ClassBinder;

  class BoundFunction;

  class Prototype;

  class Duk : private Context
  {
   public:
    Duk();

    Context*
    AsContext();

    ObjectReference
    CreateGlobal(const std::string& name);

    bool
    EvalString(
        const std::string& line,
        const std::string& file,
        std::string*       error,
        std::string*       output);

    void
    BindPrint(std::function<void(const std::string&)> on_print);

    void
    BindGlobalFunction(
        const std::string& name, const std::shared_ptr<Overload>& overload);

    void
    BindObject(const std::string& name, const ObjectBinder& bind);

    void
    BindClass(const std::string& name, const ClassBinder& bind);

    ~Duk();

    BoundFunction*
    CreateFunction(const std::shared_ptr<Overload>& overload);

    Prototype*
    TypeToProto(size_t id CLASS_ARG(const std::string& name));

    ReferenceStorage references;

    std::function<void(const std::string&)> on_print;

    std::vector<std::shared_ptr<BoundFunction>> functions;

    std::map<size_t, std::shared_ptr<Prototype>> classIds;
  };
}

#endif  // EUPHORIA_DUK_DUK_H
