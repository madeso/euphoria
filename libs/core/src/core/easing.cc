#include "core/easing.h"


// source: Math for Game Programmers: Fast and Funky 1D Nonlinear Transformations
// https://www.youtube.com/watch?v=mr5xkf6zSzk&t=1383s
// easing functions on steroids
// exploring normalized utility functions

// easing functions
// filter functions (run a filter on it)
// tweening functions

#include "assert/assert.h"

namespace eu::core::easing
{

// easing "techniques"

template<int steps> constexpr float ipow   (float t) { return t * ipow<steps-1>(t); }
template<>          constexpr float ipow<1>(float t) { return t; }

constexpr float flip(float t) { return 1 - t; }

constexpr float blend(float a, float b, float weight) { return (1-weight) * a + weight*b; }

// ipow<2.3>(x) -> fracpow<2>(0.3f, x)
template<int steps> constexpr float fracpow(float frac, float t) { return blend(ipow<steps>(t), ipow<steps+1>(t), frac);}

constexpr float fabs(float t) { return t < 0 ? -t : t; }

constexpr float bounce_clamp_top(float t) { return fabs(t); }

constexpr float bounce_clamp_bottom(float t) { return 1 - fabs(1 - t); }

constexpr float bounce_clamp(float t) { return bounce_clamp_top(bounce_clamp_bottom(t)); }

constexpr float normalized_bezier_curve3(float b, float c, float t)
{
    const auto s = 1 - t;
    const auto t2 = t*t;
    const auto s2 = s*s;
    const auto t3 = t2*t;
    return 3*b*s2*t + 3*c*s*t2 + t3;
}


// library of easing functions


template<int steps>
float smooth_start(float t) { return ipow<steps>(t); }

template<int steps>
float smooth_stop(float t) { return flip(ipow<steps>(flip(t))); }

template<int steps>
float smooth_step(float t) { return blend(smooth_start<steps>(t), smooth_stop<steps>(t), t); }


float apply(Function f, float t)
{
    switch(f)
    {
    case Function::linear: return t;

    case Function::smooth_start2: return smooth_start<2>(t);
    case Function::smooth_start3: return smooth_start<3>(t);
    case Function::smooth_start4: return smooth_start<4>(t);

    case Function::smooth_stop2: return smooth_stop<2>(t);
    case Function::smooth_stop3: return smooth_stop<3>(t);
    case Function::smooth_stop4: return smooth_stop<4>(t);

    case Function::smooth_step2: return smooth_step<2>(t);
    case Function::smooth_step3: return smooth_step<3>(t);
    case Function::smooth_step4: return smooth_step<4>(t);

    default:
        DIE("Invalid case");
        return 0.0f;
    }
}



std::vector<std::pair<const char*, Function>> get_all_values()
{
    return
    {
#define VALUE(x) {#x, Function::x}
        {"Linear", Function::linear},

        {"Smooth Start 2", Function::smooth_start2},
        {"Smooth Start 3", Function::smooth_start3},
        {"Smooth Start 4", Function::smooth_start4},

        {"Smooth Stop 2", Function::smooth_stop2},
        {"Smooth Stop 3", Function::smooth_stop3},
        {"Smooth Stop 4", Function::smooth_stop4},

        {"Smooth Step 2", Function::smooth_step2},
        {"Smooth Step 3", Function::smooth_step3},
        {"Smooth Step 4", Function::smooth_step4}
#undef VALUE
    };
}

}
