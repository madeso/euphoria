#include "render/id.h"

#include "core/assert.h"

namespace euphoria::render
{
    Id::Id()
        : id_(0)
    {
    }

    gluint
    Id::GetId() const
    {
        return id_;
    }
}
