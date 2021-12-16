#pragma once

#include "core/noncopyable.h"

#include "render/gltypes.h"


namespace euphoria::render
{
    struct Id
    {
    public:
        Id();
        ~Id() = default;

        NONCOPYABLE(Id);

        // todo(Gustav): remove and make id public or remove class altogether
        [[nodiscard]] OpenglUint
        get_id() const;

    protected:
        // nolint regarding protected visibility
        OpenglUint id_; // NOLINT
    };
}
