#pragma once

#include "eu/base/vec3.h"
#include "eu/base/quat.h"
#include "eu/base/mat4.h"

namespace eu
{
    // todo(Gustav): figure out a better base/terse name
    struct Transform
    {
        v3 position;
        Q rotation;
        v3 scale;
    };


    Transform transform_from_matrix(const m4& m);
    m4 matrix_from_transform(const Transform& t);
}
