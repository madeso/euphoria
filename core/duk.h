#ifndef EUPHORIA_DUK_H
#define EUPHORIA_DUK_H


#include <string>

// #include "duk_config.h"

extern "C" {
struct duk_hthread;
typedef struct duk_hthread duk_context;
}

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

  ~Duk();


  duk_context* ctx;
};


#endif  // EUPHORIA_DUK_H
