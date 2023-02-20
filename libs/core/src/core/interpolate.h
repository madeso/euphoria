#pragma once

#include "core/easing.h"
#include "assert/assert.h"


namespace euphoria::core
{
    struct FloatTransform
    {
        static float
        transform(float from, float v, float to);
    };


    // Transform should have a function Transform(T from, float zeroToOne, T to)
    template <typename Type, typename Transformer>
    struct Interpolate
    {
        using Self = Interpolate<Type, Transformer>;

        static constexpr float transition_ended = 2.0f;

        Type from;
        Type to;
        Type value;

        float t = 1.0f;
        float speed = 1.0f;
        easing::Function easing_function = easing::Function::linear;

        explicit Interpolate(Type v)
            : from(v)
            , to(v)
            , value(v)
        {
        }

        Self& clear()
        {
            t = transition_ended;
            return *this;
        }

        Self& set(const Type& new_value)
        {
            value = new_value;
            t = transition_ended;
            return *this;
        }

        void set(const Type& new_value, easing::Function f, float time)
        {
            ASSERT(time >= 0.0f);

            from = value;
            to = new_value;
            easing_function = f;
            t = 0.0f;
            speed = 1.0f / time;
        }

        void set(easing::Function f, const Type& new_value, float time)
        {
            set(new_value, f, time);
        }

        void update(float dt)
        {
            if(t >= 1.0f)
            {
                return;
            }
            
            t += dt * speed;

            if (is_active() == false)
            {
                value = to;
                t = transition_ended;
            }
            else
            {
                const auto f = easing::apply(easing_function, t);
                value = Transformer::transform(from, f, to);
            }
        }

        [[nodiscard]] bool is_active() const
        {
            return t < 1.0f;
        }
    };

    using FloatInterpolate = Interpolate<float, FloatTransform>;
}
