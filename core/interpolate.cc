#include "core/interpolate.h"
#include "aheasing/easing.h"

float FloatTransform::Transform(float from, float v, float to) {
  return from + (to-from) * v;
}

namespace easing {
#define FUN(NAME, FUNC) float NAME(float value) { return ::FUNC(value); }
// Linear interpolation (no easing)
FUN(Linear, LinearInterpolation)

// Quadratic easing; p^2
FUN(QuadIn, QuadraticEaseIn)
FUN(QuadOut, QuadraticEaseOut)
FUN(Quad, QuadraticEaseInOut)

// Cubic easing; p^3
FUN(CubicIn, CubicEaseIn)
FUN(CubicOut, CubicEaseOut)
FUN(Cubic, CubicEaseInOut)

// Quartic easing; p^4
FUN(QuartIn, QuarticEaseIn)
FUN(QuartOut, QuarticEaseOut)
FUN(Quart, QuarticEaseInOut)

// Quintic easing; p^5
FUN(QuintIn, QuinticEaseIn)
FUN(QuintOut, QuinticEaseOut)
FUN(Quint, QuinticEaseInOut)

// Sine wave easing; sin(p * PI/2)
FUN(SineIn, SineEaseIn)
FUN(SineOut, SineEaseOut)
FUN(Sine, SineEaseInOut)

// Circular easing; sqrt(1 - p^2)
FUN(CircIn, CircularEaseIn)
FUN(CircOut, CircularEaseOut)
FUN(Circ, CircularEaseInOut)

// Exponential easing, base 2
FUN(ExpIn, ExponentialEaseIn)
FUN(ExpOut, ExponentialEaseOut)
FUN(Exp, ExponentialEaseInOut)

// Exponentially-damped sine wave easing
FUN(ElasticIn, ElasticEaseIn)
FUN(ElasticOut, ElasticEaseOut)
FUN(Elastic, ElasticEaseInOut)

// Overshooting cubic easing;
FUN(BackIn, BackEaseIn)
FUN(BackOut, BackEaseOut)
FUN(Back, BackEaseInOut)

// Exponentially-decaying bounce easing
FUN(BounceIn, BounceEaseIn)
FUN(BounceOut, BounceEaseOut)
FUN(Bounce, BounceEaseInOut)
#undef FUN
}
