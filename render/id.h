#ifndef RENDER_ID_H
#define RENDER_ID_H

#include "core/noncopyable.h"
#include "render/gltypes.h"

namespace euphoria::render
{
    class Id : core::Noncopyable
    {
    public:
        Id();
        ~Id();

        gluint
        GetId() const;

    protected:
        gluint id_;
    };
}  // namespace euphoria::render

#endif  // RENDER_ID_H
