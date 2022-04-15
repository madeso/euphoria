#include "core/polybezier.h"

#include <array>
#include <type_traits>

#include "core/range.h"
#include "log/log.h"
#include "core/cint.h"

namespace euphoria::core { namespace
{
    bool
    is_anchor_point(size_t i)
    {
        return i % 3 == 0;
    }

    bool
    is_control_point(size_t i)
    {
        return !is_anchor_point(i);
    }

    template<typename T, bool b> struct Const { using Type = T; };
    template<typename T> struct Const<T, true> { using Type = std::add_const_t<T>; };

    template<typename Vec, typename Segment, bool is_const>
    struct PolyBezierWrapper
    {
        using TPoints = typename Const<std::vector<Vec>, is_const>::Type;
        using TBool = typename Const<bool, is_const>::Type;

        TPoints* ppoints;
        TBool* pis_closed;
        TBool* pautoset;

        PolyBezierWrapper
        (
            TPoints* points,
            TBool* is_closed,
            TBool* autoset
        )
            : ppoints(points)
            , pis_closed(is_closed)
            , pautoset(autoset)
        {
        }

        void reset(const Vec& left, const Vec& right, const Vec& up, const Vec& down, const Vec& center, float scale)
        {
            *pautoset = false;
            *pis_closed = false;
            ppoints->clear();
            ppoints->push_back(center + left * scale);
            ppoints->push_back(center + (left + up) * 0.5f * scale);
            ppoints->push_back(center + (right + down) * 0.5f * scale);
            ppoints->push_back(center + right * scale);
        }
        
        void add_point(const Vec& p)
        {
            const auto p2 = (*ppoints)[ppoints->size() - 2]; // control point
            const auto p3 = (*ppoints)[ppoints->size() - 1]; // anchor point

            const auto p4 = p3 + Vec::from_to(p2, p3);
            const auto p6 = p;
            const auto p5 = p4 + Vec::from_to(p4, p6);

            ppoints->push_back(p4);
            ppoints->push_back(p5);
            ppoints->push_back(p6);

            if(*pautoset)
            {
                auto_set_affected_control_points(c_sizet_to_int(ppoints->size()) - 1);
            }
        }

        void move_point(int i, const Vec& delta)
        {
            if(*pautoset && is_control_point(i))
            {
                // that point is on autoset
                return;
            }

            const auto r = make_range(*ppoints);
            (*ppoints)[i] += delta;

            if(*pautoset)
            {
                auto_set_affected_control_points(i);
                return;
            }

            if(is_anchor_point(i))
            {
                // anchor point, move control points too
                if(*pis_closed || is_within(r, i + 1))
                {
                    (*ppoints)[loop_index(i + 1)] += delta;
                }
                if(*pis_closed || is_within(r, i - 1))
                {
                    (*ppoints)[loop_index(i - 1)] += delta;
                }
            }
            else
            {
                // point is control point, move the opposite point
                const int corresponding_control_index = is_anchor_point(i + 1) ? i + 2 : i - 2;
                const int anchor_index = is_anchor_point(i + 1) ? i + 1 : i - 1;
                if(*pis_closed || is_within(r, corresponding_control_index))
                {
                    const auto cci = loop_index(corresponding_control_index);
                    const auto ai = loop_index(anchor_index);
                    const auto distance = Vec::from_to((*ppoints)[cci], (*ppoints)[ai]).get_length();
                    const auto direction = Vec::from_to((*ppoints)[i], (*ppoints)[ai]).get_normalized();
                    (*ppoints)[cci] = (*ppoints)[ai] + distance * direction;
                }
            }
        }

        [[nodiscard]] Segment
        get_segment(size_t i) const
        {
            const size_t b = i * 3;
            return
            {
                (*ppoints)[b + 0], // anchor
                (*ppoints)[b + 1], // ^ control
                (*ppoints)[loop_index(c_sizet_to_int(b) + 3)], // anchor
                (*ppoints)[b + 2] // ^ control
            };
        }

        size_t
        get_number_of_segments() const
        {
            return ppoints->size() / 3;
        }

        void
        set_closed(bool new_is_closed)
        {
            if(*pis_closed == new_is_closed)
            {
                return;
            }

            *pis_closed = new_is_closed;

            if(*pis_closed)
            {
                // anchor control anchor (again)
                const auto p1 = (*ppoints)[ppoints->size() - 1] + Vec::from_to((*ppoints)[ppoints->size() - 2], (*ppoints)[ppoints->size() - 1]);
                const auto p2 = (*ppoints)[0] + Vec::from_to((*ppoints)[1], (*ppoints)[0]);
                ppoints->push_back(p1);
                ppoints->push_back(p2);

                if(*pautoset)
                {
                    auto_set_anchor_control_points(0);
                    auto_set_anchor_control_points(c_sizet_to_int(ppoints->size()) - 3);
                }
            }
            else
            {
                ppoints->pop_back();
                ppoints->pop_back();

                if(*pautoset)
                {
                    auto_set_start_and_end_control_points();
                }
            }
        }

        void
        toggle_closed()
        {
            set_closed(!*pis_closed);
        }


        void
        set_auto_set_control_points(bool is_autoset)
        {
            if(is_autoset == *pautoset)
            {
                return;
            }

            *pautoset = is_autoset;

            if(*pautoset)
            {
                auto_set_all_control_points();
            }
        }

        void
        toggle_auto_set_control_points()
        {
            set_auto_set_control_points(!*pautoset);
        }


        size_t
        loop_index(int i) const
        {
            const auto s = ppoints->size();
            return (s + i) % s;
        }

        void
        auto_set_affected_control_points(int updated_anchor_index)
        {
            const auto r = make_range(*ppoints);
            for(int i = updated_anchor_index - 3; i <= updated_anchor_index + 3;
                i += 3)
            {
                if(*pis_closed || is_within(r, i))
                {
                    auto_set_anchor_control_points(c_sizet_to_int(loop_index(i)));
                }
            }

            // might be affected...
            auto_set_start_and_end_control_points();
        }

        void
        auto_set_all_control_points()
        {
            for(int i = 0; i < c_sizet_to_int(ppoints->size()); i += 3)
            {
                auto_set_anchor_control_points(i);
            }
            auto_set_start_and_end_control_points();
        }

        void
        auto_set_start_and_end_control_points()
        {
            if(*pis_closed)
            {
                return;
            }

            for(int i = 0; i < 2; i += 1)
            {
                const auto b = std::array<size_t, 2> {0, ppoints->size() - 3}[i];
                (*ppoints)[b + 1] = ((*ppoints)[b + 0] + (*ppoints)[b + 2]) * 0.5f;
            }
        }

        void
        auto_set_anchor_control_points(int anchor_index)
        {
            const auto r = make_range(*ppoints);
            const auto anchor_pos = (*ppoints)[anchor_index];
            auto dir = Vec::zero();
            auto distances = std::array<float, 2> {0, 0};

            auto f = [&](int scale, int dist_index)
            {
                const auto index = anchor_index - 3 * scale;
                if(*pis_closed || is_within(r, index))
                {
                    auto offset = (Vec::from_to(anchor_pos, (*ppoints)[loop_index(index)])).get_normalized_and_length();
                    dir += offset.second.vec() * c_int_to_float(scale);
                    distances[c_int_to_sizet(dist_index)] = offset.first * static_cast<float>(scale);
                }
            };
            f(1, 0);
            f(-1, 1);
            dir.normalize();

            for(int i = 0; i < 2; i += 1)
            {
                const auto control_index
                        = anchor_index + std::array<int, 2> {-1, 1}[i];
                if(*pis_closed || is_within(r, control_index))
                {
                    (*ppoints)[loop_index(control_index)]
                            = anchor_pos + dir * distances[i] * 0.5f;
                }
            }
        }
    };

    PolyBezierWrapper<euphoria::core::Vec2f, euphoria::core::BezierSegment2, false> make_wrapper(euphoria::core::PolyBezier2* self)
    {
        return
        {
            &self->points,
            &self->is_closed,
            &self->autoset
        };
    }

    PolyBezierWrapper<euphoria::core::Vec2f, euphoria::core::BezierSegment2, true> make_wrapper(const euphoria::core::PolyBezier2* self)
    {
        return
        {
            &self->points,
            &self->is_closed,
            &self->autoset
        };
    }
}}


namespace euphoria::core
{
    PolyBezier2::PolyBezier2(const Vec2f& center)
    {
        const auto left = Vec2f(-1, 0);
        const auto right = Vec2f(1, 0);
        const auto up = Vec2f(0, 1);
        const auto down = Vec2f(0, -1);

        make_wrapper(this).reset(left, right, up, down, center, 1.0f);
    }

    void
    PolyBezier2::add_point(const Vec2f& p)
    {
        make_wrapper(this).add_point(p);
    }

    bool
    PolyBezier2::is_anchor_point(size_t i)
    {
        return core::is_anchor_point(i);
    }

    bool
    PolyBezier2::is_control_point(size_t i)
    {
        return core::is_control_point(i);
    }


    void
    PolyBezier2::move_point(int i, const Vec2f& delta)
    {
        make_wrapper(this).move_point(i, delta);
    }

    size_t
    PolyBezier2::get_number_of_segments() const
    {
        return make_wrapper(this).get_number_of_segments();
    }

    BezierSegment2
    PolyBezier2::get_segment(size_t i) const
    {
        return make_wrapper(this).get_segment(i);
    }

    void
    PolyBezier2::set_closed(bool new_is_closed)
    {
        make_wrapper(this).set_closed(new_is_closed);
    }

    void
    PolyBezier2::toggle_closed()
    {
        make_wrapper(this).toggle_closed();
    }


    void
    PolyBezier2::set_auto_set_control_points(bool is_autoset)
    {
        make_wrapper(this).set_auto_set_control_points(is_autoset);
    }

    void
    PolyBezier2::toggle_auto_set_control_points()
    {
        make_wrapper(this).toggle_auto_set_control_points();
    }

    /*


    size_t
    PolyBezier2::loop_index(int i) const
    {
        const auto s = points.size();
        return (s + i) % s;
    }

    void
    PolyBezier2::auto_set_affected_control_points(int updated_anchor_index)
    {
        const auto r = make_range(points);
        for(int i = updated_anchor_index - 3; i <= updated_anchor_index + 3;
            i += 3)
        {
            if(is_closed || is_within(r, i))
            {
                auto_set_anchor_control_points(c_sizet_to_int(loop_index(i)));
            }
        }

        // might be affected...
        auto_set_start_and_end_control_points();
    }

    void
    PolyBezier2::auto_set_all_control_points()
    {
        for(int i = 0; i < c_sizet_to_int(points.size()); i += 3)
        {
            auto_set_anchor_control_points(i);
        }
        auto_set_start_and_end_control_points();
    }

    void
    PolyBezier2::auto_set_start_and_end_control_points()
    {
        if(is_closed)
        {
            return;
        }

        for(int i = 0; i < 2; i += 1)
        {
            const auto b = std::array<size_t, 2> {0, points.size() - 3}[i];
            points[b + 1] = (points[b + 0] + points[b + 2]) * 0.5f;
        }
    }

    void
    PolyBezier2::auto_set_anchor_control_points(int anchor_index)
    {
        const auto r = make_range(points);
        const auto anchor_pos = points[anchor_index];
        auto dir = Vec2f::zero();
        auto distances = std::array<float, 2> {0, 0};

        auto f = [&](int scale, int dist_index)
        {
            const auto index = anchor_index - 3 * scale;
            if(is_closed || is_within(r, index))
            {
                auto offset = (Vec2f::from_to(anchor_pos, points[loop_index(index)])).get_normalized_and_length();
                dir += offset.second.vec() * c_int_to_float(scale);
                distances[c_int_to_sizet(dist_index)] = offset.first * static_cast<float>(scale);
            }
        };
        f(1, 0);
        f(-1, 1);
        dir.normalize();

        for(int i = 0; i < 2; i += 1)
        {
            const auto control_index
                    = anchor_index + std::array<int, 2> {-1, 1}[i];
            if(is_closed || is_within(r, control_index))
            {
                points[loop_index(control_index)]
                        = anchor_pos + dir * distances[i] * 0.5f;
            }
        }
    }
    */

}
