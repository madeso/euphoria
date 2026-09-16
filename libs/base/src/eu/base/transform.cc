#include "eu/base/transform.h"

#include <cmath>
#include <algorithm>

namespace eu
{
    namespace
    {
        template <typename T, size_t TSize>
        size_t argmax(const std::array<T, TSize>& array)
        {
            ASSERT(array.empty() == false);

            const auto max_it = std::max_element(array.begin(), array.end());
            return std::distance(array.begin(), max_it);
        }

        /// Implements the "Shepperd's method" as described in
        /// "3-D Computer Graphics A Mathematical Introduction with OpenGL" by Samuel R. Buss (2022)
        /// in section XII 3.6 Quaternion and rotation matrix conversions on page 465
        Q
        quat_from_rotation_matrix(const m4& m)
        {
            const auto m11 = m.get(0, 0);
            const auto m22 = m.get(1, 1);
            const auto m33 = m.get(2, 2);

            // aka trace
            const auto m00 = m11 + m22 + m33;

            switch (argmax(std::array{ m00, m11, m22, m33 }))
            {
            case 0:
            {
                const auto d = 0.5f * std::sqrt(m00 + 1.0f);

                const auto a = (m.get1(3, 2) - m.get1(2, 3)) / (4.0f * d);
                const auto b = (m.get1(1, 3) - m.get1(3, 1)) / (4.0f * d);
                const auto c = (m.get1(2, 1) - m.get1(1, 2)) / (4.0f * d);

                return Q{ d, {a, b, c} };
            }
            case 1:
            {
                const auto a = 0.5f * std::sqrt(2.0f * m11 - m00 + 1.0f);

                const auto d = (m.get1(3, 2) - m.get1(2, 3)) / (4.0f * a);
                const auto b = (m.get1(2, 1) + m.get1(1, 2)) / (4.0f * a);
                const auto c = (m.get1(1, 3) + m.get1(3, 1)) / (4.0f * a);

                return Q{ d, {a, b, c} };
            }
            case 2:
            {
                const auto b = 0.5f * std::sqrt(2.0f * m22 - m00 + 1.0f);

                const auto d = (m.get1(1, 3) - m.get1(3, 1)) / (4.0f * b);
                const auto a = (m.get1(2, 1) + m.get1(1, 2)) / (4.0f * b);
                const auto c = (m.get1(3, 2) + m.get1(2, 3)) / (4.0f * b);

                return Q{ d, {a, b, c} };
            }
            case 3:
            {
                const auto c = 0.5f * std::sqrt(2.0f * m33 - m00 + 1.0f);

                const auto d = (m.get1(2, 1) - m.get1(1, 2)) / (4.0f * c);
                const auto a = (m.get1(1, 3) + m.get1(3, 1)) / (4.0f * c);
                const auto b = (m.get1(3, 2) + m.get1(2, 3)) / (4.0f * c);

                return Q{ d, {a, b, c} };
            }
            default:
                DIE("shouldn't happen!!!");
                return q_identity;
            }
        }
    }


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
            .rotation = quat_from_rotation_matrix(rotation_matrix).get_normalized(),
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
