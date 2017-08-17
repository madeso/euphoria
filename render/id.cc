#include "render/id.h"

#include "core/assert.h"

Id::Id()
    : id_(0)
{
}

Id::~Id() = default;

gluint
Id::GetId() const
{
  return id_;
}
