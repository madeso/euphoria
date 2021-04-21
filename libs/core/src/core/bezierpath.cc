#include "core/bezierpath.h"

#include <array>

#include "core/range.h"
#include "core/log.h"
#include "core/cint.h"

namespace euphoria::core
{
    // LOG_SPECIFY_DEFAULT_LOGGER("painter")

    bezier_path2::bezier_path2(const vec2f& center)
    {
        const auto left = vec2f(-1, 0);
        const auto right = vec2f(1, 0);
        const auto up = vec2f(0, 1);
        const auto down = vec2f(0, -1);

        points.push_back(center + left);
        points.push_back(center + (left + up) * 0.5f);
        points.push_back(center + (right + down) * 0.5f);
        points.push_back(center + right);
    }

    void
    bezier_path2::add_point(const vec2f& p)
    {
        const auto p2 = points[points.size() - 2]; // control point
        const auto p3 = points[points.size() - 1]; // anchor point

        const auto p4 = p3 + vec2f::from_to(p2, p3);
        const auto p6 = p;
        const auto p5 = p4 + vec2f::from_to(p4, p6);

        points.push_back(p4);
        points.push_back(p5);
        points.push_back(p6);

        if(autoset_)
        {
            auto_set_affected_control_points(Csizet_to_int(points.size()) - 1);
        }
    }

    bool
    bezier_path2::is_anchor_point(size_t i)
    {
        return i % 3 == 0;
    }

    bool
    bezier_path2::is_control_point(size_t i)
    {
        return !is_anchor_point(i);
    }


    void
    bezier_path2::move_point(int i, const vec2f& delta)
    {
        if(autoset_ && is_control_point(i))
        {
            // that point is on autoset
            return;
        }

        const auto r = MakeRange(points);
        points[i] += delta;

        if(autoset_)
        {
            auto_set_affected_control_points(i);
            return;
        }

        if(is_anchor_point(i))
        {
            // anchor point, move control points too
            if(is_closed_ || IsWithin(r, i + 1))
            {
                points[loop_index(i + 1)] += delta;
            }
            if(is_closed_ || IsWithin(r, i - 1))
            {
                points[loop_index(i - 1)] += delta;
            }
        }
        else
        {
            // point is control point, move the opposite point
            const int corresponding_control_index
                    = is_anchor_point(i + 1) ? i + 2 : i - 2;
            const int anchor_index = is_anchor_point(i + 1) ? i + 1 : i - 1;
            if(is_closed_ || IsWithin(r, corresponding_control_index))
            {
                const auto cci = loop_index(corresponding_control_index);
                const auto ai = loop_index(anchor_index);
                const auto distance
                        = vec2f::from_to(points[cci], points[ai]).get_length();
                const auto direction
                        = vec2f::from_to(points[i], points[ai]).get_normalized();
                points[cci] = points[ai] + distance * direction;
            }
        }
    }

    bezier_seg2
    bezier_path2::get_points_in_segment(size_t i) const
    {
        const auto b = i * 3;
        return {
                points[b + 0], // anchor
                points[b + 1], // ^ control
                points[Cint_to_sizet(loop_index(Csizet_to_int(b) + 3))], // anchor
                points[b + 2] // ^ control
        };
    }

    size_t
    bezier_path2::get_number_of_segments() const
    {
        return points.size() / 3;
    }

    void
    bezier_path2::set_closed(bool is_closed)
    {
        if(is_closed_ == is_closed)
        {
            return;
        }

        is_closed_ = is_closed;

        if(is_closed)
        {
            // anchor control anchor (again)
            const auto p1 = points[points.size() - 1] + vec2f::from_to(points[points.size() - 2], points[points.size() - 1]);
            const auto p2 = points[0] + vec2f::from_to(points[1], points[0]);
            points.push_back(p1);
            points.push_back(p2);

            if(autoset_)
            {
                auto_set_anchor_control_points(0);
                auto_set_anchor_control_points(Cint_to_sizet(Csizet_to_int(points.size()) - 3));
            }
        }
        else
        {
            points.pop_back();
            points.pop_back();

            if(autoset_)
            {
                auto_set_start_and_end_control_points();
            }
        }
    }

    void
    bezier_path2::toggle_closed()
    {
        set_closed(!is_closed_);
    }


    void
    bezier_path2::set_auto_set_control_points(bool is_autoset)
    {
        if(is_autoset == autoset_)
        {
            return;
        }

        autoset_ = is_autoset;

        if(autoset_)
        {
            auto_set_all_control_points();
        }
    }

    void
    bezier_path2::toggle_auto_set_control_points()
    {
        set_auto_set_control_points(!autoset_);
    }


    size_t
    bezier_path2::loop_index(int i) const
    {
        const auto s = points.size();
        return (s + i) % s;
    }

    void
    bezier_path2::auto_set_affected_control_points(int updated_anchor_index)
    {
        const auto r = MakeRange(points);
        for(int i = updated_anchor_index - 3; i <= updated_anchor_index + 3;
            i += 3)
        {
            if(is_closed_ || IsWithin(r, i))
            {
                auto_set_anchor_control_points(loop_index(i));
            }
        }

        // might be affected...
        auto_set_start_and_end_control_points();
    }

    void
    bezier_path2::auto_set_all_control_points()
    {
        for(int i = 0; i < Csizet_to_int(points.size()); i += 3)
        {
            auto_set_anchor_control_points(i);
        }
        auto_set_start_and_end_control_points();
    }

    void
    bezier_path2::auto_set_start_and_end_control_points()
    {
        if(is_closed_)
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
    bezier_path2::auto_set_anchor_control_points(int anchor_index)
    {
        const auto r = MakeRange(points);
        const auto anchor_pos = points[anchor_index];
        auto dir = vec2f::zero();
        auto distances = std::array<float, 2> {0, 0};

        auto f = [&](int scale, int dist_index)
        {
            const auto index = anchor_index - 3 * scale;
            if(is_closed_ || IsWithin(r, index))
            {
                auto offset = (vec2f::from_to(anchor_pos, points[loop_index(index)])).get_normalized_and_length();
                dir += offset.second.vec() * scale;
                distances[Cint_to_sizet(dist_index)] = offset.first * static_cast<float>(scale);
            }
        };
        f(1, 0);
        f(-1, 1);
        dir.normalize();

        for(int i = 0; i < 2; i += 1)
        {
            const auto control_index
                    = anchor_index + std::array<int, 2> {-1, 1}[i];
            if(is_closed_ || IsWithin(r, control_index))
            {
                points[loop_index(control_index)]
                        = anchor_pos + dir * distances[i] * 0.5f;
            }
        }
    }

}
