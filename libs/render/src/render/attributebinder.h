#ifndef EUPHORIA_ATTRIBUTEBINDER_H
#define EUPHORIA_ATTRIBUTEBINDER_H

#include "render/shaderattribute.h"

#include <vector>

namespace euphoria::render
{
    struct PointLayout;

    void
    BindAttributes(
            const std::vector<ShaderAttribute>& attributes,
            PointLayout*                        layout);
}  // namespace euphoria::render

#endif  // EUPHORIA_ATTRIBUTEBINDER_H
