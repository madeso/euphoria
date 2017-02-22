#include "core/assert.h"

#include <iostream>


namespace assertlib {

void OnAssert(const char* const expression, int line, const char* const file, const char* const function) {
  std::cerr << "Assertion failed: " << expression << "\n"
            << "Function: " << function << "\n"
            << "File: " << file << ":" << line << "\n";

  exit(-1);
}

}
