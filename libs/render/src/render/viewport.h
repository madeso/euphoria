#pragma once


namespace euphoria::core
{
    struct Viewport;
}

namespace euphoria::render
{
    /** Sets the gl viewport.
     */
    void
    activate(const core::Viewport& vp);
}
