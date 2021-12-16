#include "render/id.h"

#include "core/assert.h"

namespace euphoria::render
{
    Id::Id()
        : id_(0)
    {
    }

    OpenglUint
    Id::get_id() const
    {
        return id_;
    }
}
