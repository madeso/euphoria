#ifndef CORE_LINE2_H
#define CORE_LINE2_H

#include "core/vec2.h"

namespace euphoria::core
{
    template <typename T>
    class line2
    {
    public:
        typedef line2<T> Self;
        typedef vec2<T>  vec;
        typedef vec2<T>  tpoint;

        [[nodiscard]]
        static Self
        FromDirection(const vec& direction, const tpoint& pos = tpoint::Zero())
        {
            return Self(pos, direction);
        }

        [[nodiscard]]
        static Self
        FromTo(const tpoint& from, const tpoint& to)
        {
            return Self(from, vec::FromTo(from, to));
        }

        vec
        GetPosition(T d) const
        {
            return position + d * direction;
        }

        struct Collision
        {
        public:
            [[nodiscard]]
            static Collision
            Parallel()
            {
                Collision c;
                c.is_parallel = true;
                return c;
            }

            [[nodiscard]]
            static Collision
            NoCollision()
            {
                Collision c;
                return c;
            }

            [[nodiscard]]
            static Collision
            Collided(const tpoint& p, T a, T b)
            {
                Collision c {p, a, b};
                return c;
            }

            [[nodiscard]]
            static Collision
            GetClosestCollision(const Collision& a, const Collision& b)
            {
                if(a.collision && b.collision)
                {
                    // determine closest
                    if(a.u < b.u)
                    {
                        return a;
                    }
                    else
                    {
                        return b;
                    }
                }
                else if(a.collision)
                {
                    return a;
                }
                else if(b.collision)
                {
                    return b;
                }
                else
                {
                    return Collision::NoCollision();
                }
            }

            bool collision;
            bool is_parallel;

            tpoint point;
            T      u;
            T      v;

        private:
            Collision()
                : collision(false), is_parallel(false), point(0, 0), u(0), v(0)
            {}
            Collision(const tpoint& p, T a, T b)
                : collision(true), is_parallel(false), point(p), u(a), v(b)
            {}
        };

        Collision
        GetIntersection(const Self& line) const
        {
            // https://stackoverflow.com/a/1968345/180307
            const tpoint p1 = position;
            const tpoint p2 = position + direction;
            const tpoint p3 = line.position;
            const tpoint p4 = line.position + line.direction;

            const T p0_x = p1.x;
            const T p0_y = p1.y;
            const T p1_x = p2.x;
            const T p1_y = p2.y;
            const T p2_x = p3.x;
            const T p2_y = p3.y;
            const T p3_x = p4.x;
            const T p3_y = p4.y;

            const T s1_x = p1_x - p0_x;
            const T s1_y = p1_y - p0_y;
            const T s2_x = p3_x - p2_x;
            const T s2_y = p3_y - p2_y;

            const T den = (-s2_x * s1_y + s1_x * s2_y);

            // todo: implement a check for zero for T
            if(Abs(den) < 0.00001f)
            {
                return Collision::Parallel();
            }

            const T s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / den;
            const T t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / den;

            if(s >= 0 && s <= 1 && t >= 0 && t <= 1)
            {
                const T x = p0_x + (t * s1_x);
                const T y = p0_y + (t * s1_y);
                return Collision::Collided(point(x, y), s, t);
            }

            return Collision::NoCollision();
        }

    private:
        tpoint position;
        vec    direction;

        line2(const tpoint& p, const vec& d) : position(p), direction(d) {}
    };

    typedef line2<float> line2f;
    typedef line2<int>   line2i;

}  // namespace euphoria::core

#endif  // CORE_LINE2_H
