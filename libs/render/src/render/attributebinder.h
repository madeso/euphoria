#pragma once

#include "render/shaderattribute.h"

#include <vector>

namespace euphoria::render
{
    struct point_layout;

    void
    bind_attributes
    (
            const std::vector<shader_attribute>& attributes,
            point_layout* layout
    );
}

