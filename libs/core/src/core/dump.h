#pragma once

#include <vector>
#include <string>
#include <optional>
#include <memory>
#include <fstream>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/rgb.h"
#include "core/colors.h"

namespace euphoria::core
{
    struct plane;
    struct ray3f;
}

namespace euphoria::core::dump2d
{
    namespace strokes
    {
        std::vector<int> Dash(int size);
    }

    struct poly
    {
        rgbi stroke_color = color::black;
        std::optional<rgbi> fill_color;
        bool is_closed = false;
        std::vector<int> stroke;

        poly& set_stroke(const std::vector<int>& new_stroke);

        poly& close();
        poly& fill(const rgbi& fill_color);

        std::vector<vec2f> points;
    };

    struct text
    {
        vec2f point;
        std::string label;
        rgbi color;

        text(const vec2f& p, const std::string& t, const rgbi& c = color::black);
    };

    struct circle
    {
        vec2f point;
        float radius;
        std::optional<rgbi> line_color;
        std::optional<rgbi> fill_color;

        circle& set_line_color(const rgbi& lc);

        circle(const vec2f& p, float r, std::optional<rgbi> fill = std::nullopt);
    };

    struct group;

    // todo(Gustav): replace with std::variant
    struct item
    {
        explicit item(const dump2d::poly& p);
        explicit item(const dump2d::text& p);
        explicit item(const dump2d::group& g);
        explicit item(const dump2d::circle& c);

        std::shared_ptr<dump2d::poly>  poly;
        std::shared_ptr<dump2d::text>  text;
        std::shared_ptr<dump2d::group> group;
        std::shared_ptr<dump2d::circle> circle;
    };
    const poly*  as_poly(const item* item);
    const text*  as_text(const item* item);
    const group* as_group(const item* item);
    const circle* as_circle(const item* item);

    template<typename TBase>
    struct add_wrapper
    {
        template<typename TItem>
        TBase& operator<<(const TItem& sub_item)
        {
            return static_cast<TBase*>(this)->add(item{sub_item});
        }
    };

    struct group : public add_wrapper<group>
    {
        std::vector<item> items;

        group& add(const item& item);
    };

    struct dumper : add_wrapper<dumper>
    {
        rgbi canvas_color = color::white;
        std::vector<item> items;

        bool add_axis_when_writing = false;
        int point_size = -1;
        bool point_text = false;

        float gridx =-1;
        float gridy =-1;

        dumper& add_axis();

        dumper& add_grid(float xy);

        dumper& enable_points_rendering(int size=3);

        dumper& add(const item& item);

        // calculate total area size and offset so that x+offset will never be lower than 0
        [[nodiscard]] std::pair<vec2f,vec2f> calculate_size_and_offset() const;

        void write(const std::string& path, int width=1280, int height=1024, int space = 6) const;
    };
}

namespace euphoria::core::dump3d
{
    struct dumper
    {
        explicit dumper(const std::string& path);
        ~dumper();

        dumper(const dumper&) = delete;
        dumper(dumper&&) = delete;
        void operator=(const dumper&) = delete;
        void operator=(dumper&&) = delete;

        void add_sphere(const vec3f& p, float radius, const rgbi& color);
        auto add_lines(const std::vector<vec3f>& lines, const rgbi& color) -> void;
        void add_plane(const plane& plane, const rgbi& color);
        void add_arrow(const ray3f& ray, const rgbi& color);

        void add_axis();
        void add_grid();


        std::ofstream file;
    };
}
