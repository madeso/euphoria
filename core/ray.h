#ifndef CORE_RAY_H
#define CORE_RAY_H

#include "core/vec3.h"

namespace euphoria::core
{
    struct UnitRay3f
    {
        UnitRay3f(const vec3f& from, const unit3f& dir);

        [[nodiscard]] static UnitRay3f
        FromTo(const vec3f& from, const vec3f& to);

        [[nodiscard]] static UnitRay3f
        FromDirection(const unit3f& dir);

        vec3f
        GetPoint(float at) const;

        vec3f from;
        unit3f  dir;
    };


    struct Ray2f
    {
        Ray2f(const vec2f& p, const vec2f& d);

        [[nodiscard]] static Ray2f
        FromTo(const vec2f& from, const vec2f& to);

        [[nodiscard]] static Ray2f
        FromDirection(const vec2f& direction, const vec2f& pos = vec2f::Zero());

        vec2f
        GetPosition(float d) const;

        vec2f position;
        vec2f direction;
    };

    struct Ray3f
    {
        Ray3f(const vec3f& from, const vec3f& dir);

        [[nodiscard]] static Ray3f
        FromTo(const vec3f& from, const vec3f& to);

        [[nodiscard]] static Ray3f
        FromDirection(const vec3f& dir);

        vec3f
        GetPoint(float at) const;

        UnitRay3f
        GetNormalized() const;

        vec3f from;
        vec3f dir;
    };

}  // namespace euphoria::core

#endif  // CORE_RAY_H
