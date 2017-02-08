#include "render/id.h"

#include <cassert>

Id::Id() : id_(0) {}

Id::~Id() {}

gluint Id::id() const {
  assert(this);
  return id_;
}
