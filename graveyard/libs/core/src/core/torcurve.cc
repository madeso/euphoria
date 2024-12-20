#include "core/torcurve.h"

#include <cmath>

#include "base/numeric.h"


namespace eu::core
{



float calc_tor_curve(float x, float a, float b, float c)
{
    // pinch c near 0.5, smoothing out its effect
    c = c < 0.5f
        ? -c * c
        : c * c
        ;
    x = max(0.0f, min(1.0f, x)); //clamp input to [0-1], behavior is undefined otherwise
    const float s = std::exp(a); //could be any exponential like 2^a or 3^a, or just linear
    const float s2 = 1.0f / s;
    const float t = max(0.0f, min(1.0f, b));
    const float u = c; //should normally be clamped but creates possibly useful results outside of the 0-1 range
    const float eps = 0.00001f; //protect against div/0

    float res = 0.0f;
    float c1 = 0.0f; //normal 1/x
    float c2 = 0.0f; //flat-center
    float c3 = 0.0f; //flat-end
    if (x < t) {
        c1 = (t * x) / (x + s * (t - x) + eps);
        c2 = t - std::pow(1 / (t + eps), s2 - 1.0f) * std::pow(abs(x - t), s2);
        c3 = std::pow(1.0f / (t + eps), s - 1.0f) * std::pow(x, s);
    }
    else {
        c1 = (1.0f - t) * (x - 1.0f) / (1.0f - x - s * (t - x) + eps) + 1;
        c2 = std::pow(1.0f / ((1.0f - t) + eps), s2 - 1.0f) * std::pow(abs(x - t), s2) + t;
        c3 = 1.0f - std::pow(1 / ((1.0f - t) + eps), s - 1.0f) * std::pow(1.0f - x, s);
    }
    if (u <= 0) {
        res = (-u) * c2 + (1.0f + u) * c1;
    }
    else {
        res = (u)*c3 + (1.0f - u) * c1;
    }

    // todo(Gustav): return 0 if inifinite
    return res;
}




}
