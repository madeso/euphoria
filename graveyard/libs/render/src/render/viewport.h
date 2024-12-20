#pragma once


namespace eu::core
{
    struct Viewport;
}

namespace eu::render
{
    /** Sets the gl viewport.
     */
    void
    activate(const core::Viewport& vp);
}
