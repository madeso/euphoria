#include "render/id.h"

#include "core/assert.h"

namespace euphoria::render
{
    Id::Id() : id_(0) {}

    Id::~Id() = default;

    gluint
    Id::GetId() const
    {
        return id_;
    }
}  // namespace euphoria::render
