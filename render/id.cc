#include "render/id.h"

#include "core/assert.h"

Id::Id() : id_(0) {}

Id::~Id() {}

gluint Id::id() const {
  Assert(this);
  return id_;
}
