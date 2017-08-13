#include "render/id.h"

#include "core/assert.h"

Id::Id()
    : id_(0)
{
}

Id::~Id() = default;

gluint
Id::id() const
{
  return id_;
}
