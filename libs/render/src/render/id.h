#pragma once

#include "render/gltypes.h"


namespace eu::render
{
    struct Id
    {
        Id();
        ~Id() = default;

        Id(const Id&) = delete;
        Id(Id&&) = delete;
        void operator=(const Id&) = delete;
        void operator=(Id&&) = delete;

        // todo(Gustav): remove and make id public or remove class altogether
        [[nodiscard]] gl::Uint get_id() const;

    protected:
        // nolint regarding protected visibility
        gl::Uint id; // NOLINT
    };
}
