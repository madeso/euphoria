#include "render/id.h"

#include "core/assert.h"

namespace euphoria::render
{
    id::id()
        : id_(0)
    {
    }

    gluint
    id::get_id() const
    {
        return id_;
    }
}
