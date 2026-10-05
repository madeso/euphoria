#include "eu/base/transform.h"

#include <cmath>
#include <algorithm>

namespace eu
{
    Transform
    transform_from_matrix(const m4& m)
    {
        // heavily inspired by DecomposeMatrixToComponents from imguizmo
        const auto right = m.get_column(0).to_vec3(0.0f);
        const auto up = m.get_column(1).to_vec3(0.0f);
        const auto out = m.get_column(2).to_vec3(0.0f);

        const auto scale = v3
        {
            right.get_length(),
            up.get_length(),
            out.get_length()
        };

        // orthonormalize the basis to drop the scale
        const auto normalized_right = right.get_normalized().value_or(kk::right);
        const auto normalized_up = up.get_normalized().value_or(kk::up);
        const auto normalized_out = out.get_normalized().value_or(kk::out);
        
        const auto rotation_matrix = m4::from_basis(normalized_right, normalized_up, normalized_out);

        return
        {
            .position = m.get_translation(),
            .rotation = Q::from_rotation_matrix(rotation_matrix).get_normalized(),
            .scale = scale
        };
    }


    m4
    matrix_from_transform(const Transform& t)
    {
        // heavily inspired by RecomposeMatrixFromComponents from imguizmo

        const auto rotation = m4::from_basis
        (
            t.rotation.get_local_right(),
            t.rotation.get_local_up(),
            t.rotation.get_local_out()
        );

        return m4::from_translation(t.position) * rotation * m4::from_scale(t.scale);
    }
}
