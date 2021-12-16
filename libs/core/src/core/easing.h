#pragma once

namespace euphoria::core::easing
{

enum class Function
{
    linear,
    
    // also called ease in
    // start slow, end quick
    smooth_start2, smooth_start3, smooth_start4,

    // also called ease out
    // start fast, end slow
    smooth_stop2, smooth_stop3, smooth_stop4,

    // also called ease in and out
    // start slow, end slow
    smooth_step2, smooth_step3, smooth_step4
};


float apply(Function f, float t);

}
