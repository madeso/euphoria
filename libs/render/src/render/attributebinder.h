#pragma once

#include "render/shaderattribute.h"



namespace euphoria::render
{
    struct PointLayout;

    void
    bind_attributes
    (
        const std::vector<ShaderAttribute>& attributes,
        PointLayout* layout
    );
}

