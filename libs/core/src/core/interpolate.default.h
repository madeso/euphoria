#pragma once

namespace euphoria::core
{
template<typename T>
struct InterpolateDefault
{
};

template<typename T>
T default_interpolate(const T& from, float t, const T& to)
{
    return InterpolateDefault<T>::interpolate(from, t, to);
}

float float_transform(float from, float v, float to);

#define DEFAULT_INTERPOLATE(TYPE, FUNCTION) template<> struct InterpolateDefault<TYPE> {\
    static TYPE interpolate(const TYPE& from, float v, const TYPE& to) { return FUNCTION(from, v, to); }\
}

DEFAULT_INTERPOLATE(float, float_transform);

}
