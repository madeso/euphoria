#ifndef EUPHORIA_DUK_H
#define EUPHORIA_DUK_H


#include <string>
#include <functional>
#include <map>
#include <memory>
#include <vector>

// #include "duk_config.h"

extern "C" {
struct duk_hthread;
typedef struct duk_hthread duk_context;
}

class Overload
{
 public:
  Overload()          = default;
  virtual ~Overload() = default;

  virtual bool
  IsValid(duk_context* ctx) = 0;

  virtual int
  Call(duk_context* ctx) = 0;

  virtual std::string
  Describe() const = 0;
};

class Function
{
 public:
  Function()          = default;
  virtual ~Function() = default;

  std::vector<std::shared_ptr<Overload>> overloads;
};

class Duk
{
 public:
  Duk();

  bool
  eval_string(
      const std::string& line,
      const std::string& file,
      std::string*       error,
      std::string*       output);

  void
  bind_print(std::function<void(const std::string&)> on_print);

  void
  bind(const std::string& name);

  ~Duk();


  duk_context* ctx;

  std::function<void(const std::string&)> on_print;

  std::map<std::string, std::shared_ptr<Function>> functions;
};


#endif  // EUPHORIA_DUK_H
