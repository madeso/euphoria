#include "core/polybezier.h"

#include <type_traits>

#include "core/range.h"
#include "log/log.h"
#include "core/cint.h"

namespace euphoria::core { namespace
{
    constexpr size_t indices_between_anchor_points  = 3;
    constexpr size_t indices_between_anchor_pointsi = 3;

    bool
    is_anchor_point(size_t i)
    {
        return i % indices_between_anchor_points == 0;
    }

    bool
    is_control_point(size_t i)
    {
        return !is_anchor_point(i);
    }

    template<typename T, bool b> struct Const { using Type = T; };
    template<typename T> struct Const<T, true> { using Type = std::add_const_t<T>; };

    template<typename Vec, typename Segment, typename Unit, bool is_const>
    struct PolyBezierWrapper
    {
        using TPoints = typename Const<std::vector<Vec>, is_const>::Type;
        using TBool = typename Const<bool, is_const>::Type;

        TPoints* points;
        TBool* is_closed;
        TBool* is_autoset_enabled;

        PolyBezierWrapper
        (
            TPoints* arg_points,
            TBool* arg_is_closed,
            TBool* arg_is_autoset_enabled
        )
            : points(arg_points)
            , is_closed(arg_is_closed)
            , is_autoset_enabled(arg_is_autoset_enabled)
        {
        }

        void reset(const Vec& left, const Vec& right, const Vec& up, const Vec& down, const Vec& center, float scale)
        {
            *is_autoset_enabled = false;
            *is_closed = false;
            points->clear();
            points->push_back(center + left * scale);
            points->push_back(center + (left + up) * 0.5f * scale);
            points->push_back(center + (right + down) * 0.5f * scale);
            points->push_back(center + right * scale);
        }
        
        void add_point(const Vec& p)
        {
            const auto p2 = (*points)[points->size() - 2]; // control point
            const auto p3 = (*points)[points->size() - 1]; // anchor point

            const auto p4 = p3 + Vec::from_to(p2, p3);
            const auto p6 = p;
            const auto p5 = p4 + Vec::from_to(p4, p6);

            points->push_back(p4);
            points->push_back(p5);
            points->push_back(p6);

            if(*is_autoset_enabled)
            {
                auto_set_affected_control_points(c_sizet_to_int(points->size()) - 1);
            }
        }

        void move_point(int i, const Vec& delta)
        {
            if(*is_autoset_enabled && is_control_point(i))
            {
                // that point is on is_autoset_enabled
                return;
            }

            const auto r = make_range(*points);
            (*points)[i] += delta;

            if(*is_autoset_enabled)
            {
                auto_set_affected_control_points(i);
                return;
            }

            if(is_anchor_point(i))
            {
                // anchor point, move control points too
                if(*is_closed || is_within(r, i + 1))
                {
                    (*points)[loop_index(i + 1)] += delta;
                }
                if(*is_closed || is_within(r, i - 1))
                {
                    (*points)[loop_index(i - 1)] += delta;
                }
            }
            else
            {
                // point is control point, move the opposite point
                const int corresponding_control_index = is_anchor_point(i + 1) ? i + 2 : i - 2;
                const int anchor_index = is_anchor_point(i + 1) ? i + 1 : i - 1;
                if(*is_closed || is_within(r, corresponding_control_index))
                {
                    const auto cci = loop_index(corresponding_control_index);
                    const auto ai = loop_index(anchor_index);
                    const auto distance = Vec::from_to((*points)[cci], (*points)[ai]).get_length();
                    const auto direction = Vec::from_to((*points)[i], (*points)[ai]).get_normalized();
                    (*points)[cci] = (*points)[ai] + distance * direction;
                }
            }
        }

        [[nodiscard]] Segment
        get_segment(size_t i) const
        {
            const size_t b = i * 3;
            return
            {
                (*points)[b + 0], // anchor
                (*points)[b + 1], // ^ control
                (*points)[loop_index(c_sizet_to_int(b)+indices_between_anchor_pointsi)], // anchor
                (*points)[b + 2] // ^ control
            };
        }

        size_t
        get_number_of_segments() const
        {
            return points->size() / indices_between_anchor_points;
        }

        void
        set_closed(bool new_is_closed)
        {
            if(*is_closed == new_is_closed)
            {
                return;
            }

            *is_closed = new_is_closed;

            if(*is_closed)
            {
                // anchor control anchor (again)
                const auto d = Vec::from_to((*points)[points->size() - 2], (*points)[points->size() - 1]);
                const auto p1 = (*points)[points->size() - 1] + d;
                const auto p2 = (*points)[0] + Vec::from_to((*points)[1], (*points)[0]);
                points->push_back(p1);
                points->push_back(p2);

                if(*is_autoset_enabled)
                {
                    auto_set_anchor_control_points(0);
                    auto_set_anchor_control_points(c_sizet_to_int(points->size()) - indices_between_anchor_pointsi);
                }
            }
            else
            {
                points->pop_back();
                points->pop_back();

                if(*is_autoset_enabled)
                {
                    auto_set_start_and_end_control_points();
                }
            }
        }

        void
        toggle_closed()
        {
            set_closed(!*is_closed);
        }


        void
        set_auto_set_control_points(bool is_autoset)
        {
            if(is_autoset == *is_autoset_enabled)
            {
                return;
            }

            *is_autoset_enabled = is_autoset;

            if(*is_autoset_enabled)
            {
                auto_set_all_control_points();
            }
        }

        void
        toggle_auto_set_control_points()
        {
            set_auto_set_control_points(!*is_autoset_enabled);
        }


        size_t
        loop_index(int i) const
        {
            const auto s = points->size();
            return (s + i) % s;
        }

        void
        auto_set_affected_control_points(int updated_anchor_index)
        {
            const auto points_range = make_range(*points);
            
            const auto update_anchor_index = [&](int anchor_index)
            {
                if(*is_closed || is_within(points_range, anchor_index))
                {
                    auto_set_anchor_control_points(loop_index(anchor_index));
                }
            };

            update_anchor_index(updated_anchor_index - indices_between_anchor_pointsi);
            update_anchor_index(updated_anchor_index);
            update_anchor_index(updated_anchor_index + indices_between_anchor_pointsi);

            // might be affected...
            auto_set_start_and_end_control_points();
        }

        void
        auto_set_all_control_points()
        {
            for(int i = 0; i < c_sizet_to_int(points->size()); i += indices_between_anchor_pointsi)
            {
                auto_set_anchor_control_points(i);
            }
            auto_set_start_and_end_control_points();
        }

        void
        auto_set_start_and_end_control_points()
        {
            if(*is_closed)
            {
                return;
            }

            const auto set_index = [&](int r, int a, int b)
            {
                (*points)[loop_index(r)] = ((*points)[loop_index(a)] + (*points)[loop_index(b)]) * 0.5f;
            };
            set_index( 1,  0,  2);
            set_index(-2, -1, -3);
        }

        void
        auto_set_anchor_control_points(int anchor_index)
        {
            const auto points_range = make_range(*points);
            const auto anchor_pos = (*points)[anchor_index];

            const auto get_distance_and_dir = [&](int index) -> std::pair<float, Vec>
            {
                if(*is_closed || is_within(points_range, index))
                {
                    const auto ft = Vec::from_to(anchor_pos, (*points)[loop_index(index)]);
                    const auto offset = ft.get_normalized_and_length();
                    const auto dir = offset.normalized.to_vec();
                    return {offset.length, dir};
                }
                else
                {
                    return {0.0f, Vec::zero()};
                }
            };
            const auto set_control_point = [&](int control_index, float distance, const Unit& dir)
            {
                if(*is_closed || is_within(points_range, control_index))
                {
                    (*points)[loop_index(control_index)] = anchor_pos + dir * distance * 0.5f;
                }
            };

            const auto [distance_from_anchor_to_fst_neighbour, anchor_to_fst_neighbour] = get_distance_and_dir(anchor_index - indices_between_anchor_pointsi);
            const auto [distance_from_anchor_to_sec_neighbour, anchor_to_sec_neighbour] = get_distance_and_dir(anchor_index + indices_between_anchor_pointsi);
            const auto dir = Vec::from_to(anchor_to_sec_neighbour, anchor_to_fst_neighbour).get_normalized();

            set_control_point(anchor_index - 1,  distance_from_anchor_to_fst_neighbour, dir);
            set_control_point(anchor_index + 1, -distance_from_anchor_to_sec_neighbour, dir);
        }
    };

    PolyBezierWrapper<Vec2f, BezierSegment2, Unit2f, false> make_wrapper(euphoria::core::PolyBezier2* self)
    {
        return
        {
            &self->points,
            &self->is_closed,
            &self->is_autoset_enabled
        };
    }

    PolyBezierWrapper<Vec2f, BezierSegment2, Unit2f, true> make_wrapper(const euphoria::core::PolyBezier2* self)
    {
        return
        {
            &self->points,
            &self->is_closed,
            &self->is_autoset_enabled
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

        make_wrapper(this).reset(left, right, up, down, center, 50.0f);
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
}
