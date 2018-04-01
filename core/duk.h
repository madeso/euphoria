#ifndef EUPHORIA_DUK_H
#define EUPHORIA_DUK_H


#include <string>

#include "duk_config.h"

class Duk
{
public:
  Duk();

  bool
  eval_string(const std::string& line, std::string* error, std::string* output);

  ~Duk();


  duk_context* ctx;
};


#endif //EUPHORIA_DUK_H
