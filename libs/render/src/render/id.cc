#include "render/id.h"

#include "assert/assert.h"

namespace euphoria::render
{
    Id::Id()
        : id_(0)
    {
    }

    gl::Uint
    Id::get_id() const
    {
        return id_;
    }
}
