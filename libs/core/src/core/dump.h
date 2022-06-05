#pragma once



#include <optional>
#include <memory>
#include <fstream>

#include "core/vec2.h"
#include "core/vec3.h"
#include "core/rgb.h"
#include "core/colors.h"

namespace euphoria::core
{
    struct Plane;
    struct Ray3f;
}

namespace euphoria::core::dump2d
{
    namespace strokes
    {
        std::vector<int> dash(int size);
    }

    struct Poly
    {
        Rgbi stroke_color = NamedColor::black;
        std::optional<Rgbi> fill_color;
        bool is_closed = false;
        float stroke_width = 1.0f;
        std::vector<int> stroke;

        Poly& set_stroke(const std::vector<int>& new_stroke);

        Poly& close();
        Poly& fill(const Rgbi& fill_color);

        std::vector<Vec2f> points;
    };

    struct Text
    {
        Vec2f point;
        std::string label;
        Rgbi color;

        Text(const Vec2f& p, const std::string& t, const Rgbi& c = NamedColor::black);
    };

    struct Circle
    {
        Vec2f point;
        float radius;
        std::optional<Rgbi> line_color;
        std::optional<Rgbi> fill_color;

        Circle& set_line_color(const Rgbi& lc);

        Circle(const Vec2f& p, float r, std::optional<Rgbi> fill = std::nullopt);
    };

    struct Group;

    // todo(Gustav): replace with std::variant
    struct Item
    {
        explicit Item(const dump2d::Poly& p);
        explicit Item(const dump2d::Text& p);
        explicit Item(const dump2d::Group& g);
        explicit Item(const dump2d::Circle& c);

        std::shared_ptr<dump2d::Poly> poly;
        std::shared_ptr<dump2d::Text> text;
        std::shared_ptr<dump2d::Group> group;
        std::shared_ptr<dump2d::Circle> circle;
    };
    const Poly* as_poly(const Item* item);
    const Text* as_text(const Item* item);
    const Group* as_group(const Item* item);
    const Circle* as_circle(const Item* item);

    template<typename TBase>
    struct AddWrapper
    {
        template<typename TItem>
        TBase& operator<<(const TItem& sub_item)
        {
            return static_cast<TBase*>(this)->add(Item{sub_item});
        }
    };

    struct Group : public AddWrapper<Group>
    {
        std::vector<Item> items;

        Group& add(const Item& item);
    };

    struct Dumper : AddWrapper<Dumper>
    {
        Rgbi canvas_color = NamedColor::white;
        std::vector<Item> items;

        bool add_axis_when_writing = false;
        int point_size = -1;
        bool point_text = false;

        float grid_x =-1;
        float grid_y =-1;

        Dumper& add_axis();

        Dumper& add_grid(float xy);

        Dumper& enable_points_rendering(int size=3);

        Dumper& add(const Item& item);

        // calculate total area size and offset so that x+offset will never be lower than 0
        [[nodiscard]] std::pair<Vec2f,Vec2f> calculate_size_and_offset() const;

        void write(const std::string& path, int width=1280, int height=1024, int space = 6) const;
    };
}

namespace euphoria::core::dump3d
{
    struct Dumper
    {
        explicit Dumper(const std::string& path);
        ~Dumper();

        Dumper(const Dumper&) = delete;
        Dumper(Dumper&&) = delete;
        void operator=(const Dumper&) = delete;
        void operator=(Dumper&&) = delete;

        void add_sphere(const Vec3f& p, float radius, const Rgbi& color);
        auto add_lines(const std::vector<Vec3f>& lines, const Rgbi& color) -> void;
        void add_plane(const Plane& plane, const Rgbi& color);
        void add_arrow(const Ray3f& ray, const Rgbi& color);

        void add_axis();
        void add_grid();


        std::ofstream file;
    };
}
