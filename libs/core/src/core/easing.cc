// source: Math for Game Programmers: Fast and Funky 1D Nonlinear Transformations
// https://www.youtube.com/watch?v=mr5xkf6zSzk&t=1383s
// easing functions on steroids
// exploring normalized utility functions

// easing functions
// filter functions (run a filter on it)
// tweening functions

#include "core/assert.h"

namespace euphoria::core::easing
{

enum class Function
{
    linear,
    
    // also called ease in
    smooth_start2, smooth_start3, smooth_start4,

    // also called ease out
    smooth_stop2, smooth_stop3, smooth_stop4,
    smooth_step2, smooth_step3, smooth_step4
};

template<int steps>
float smooth_start(float t)
{
    return t * smooth_start<steps-1>(t);
}


template<>
float smooth_start<1>(float t)
{
    return t;
}

float flip(float t)
{
    return 1 - t;
}

template<int steps>
float smooth_stop(float t)
{
    return flip(smooth_start<steps>(flip(t)));
}

float mix(float a, float b, float weight)
{
    return (1-weight) * a + weight*b;
}

template<int steps>
float smooth_step(float t)
{
    return mix(smooth_start<steps>(t), smooth_stop<steps>(t), t);
}


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

    default:
        DIE("Invalid case");
        return 0.0f;
    }
}

}
