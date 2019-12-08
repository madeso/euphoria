#ifndef CORE_DUMP_H
#define CORE_DUMP_H

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
    struct Plane;
    struct Ray3f;
}

namespace euphoria::core::dump2d
{
    namespace strokes
    {
        std::vector<int> Dash(int size);
    }

    struct Poly
    {
        Rgbi stroke_color = Color::Black;
        std::optional<Rgbi> fill_color;
        bool is_closed = false;
        std::vector<int> stroke;

        Poly& Stroke(const std::vector<int>& new_stroke);

        Poly& Close();
        Poly& Fill(const Rgbi& fill_color);

        std::vector<vec2f> points;
    };

    struct Text
    {
        vec2f point;
        std::string text;
        Rgbi color;

        Text(const vec2f& p, const std::string& t, const Rgbi& c = Color::Black);
    };

    struct Group;

    // todo(Gustav): replace with std::variant
    struct Item
    {
        Item(const Poly& p);
        Item(const Text& p);
        Item(const Group& g);

        std::shared_ptr<Poly>  poly;
        std::shared_ptr<Text>  text;
        std::shared_ptr<Group> group;
    };
    const Poly*  AsPoly(const Item* item);
    const Text*  AsText(const Item* item);
    const Group* AsGroup(const Item* item);

    struct Group
    {
        std::vector<Item> items;

        Group& operator<<(const Item& item);
    };

    struct Dumper
    {
        std::vector<Item> items;

        bool add_axis = false;
        int point_size = -1;
        bool point_text = false;

        double gridx =-1;
        double gridy =-1;

        Dumper& AddAxis();

        Dumper& Grid(double xy);

        Dumper& DrawPoints(int size=3);

        Dumper& operator<<(const Item& item);

        // calculate total area size and offset so that x+offset will never be lower than 0
        std::pair<vec2f,vec2f> CalculateSizeAndOffset() const;

        void Write(const std::string& path, int width=1280, int height=1024, int space = 6) const;
    };
}

namespace euphoria::core::dump3d
{
    struct Dumper
    {
        Dumper(const std::string& path);
        ~Dumper();

        void AddSphere(const vec3f& p, float radius, const Rgbi& color);
        void AddLines(const std::vector<vec3f>& lines, const Rgbi& color);
        void AddPlane(const Plane& plane, const Rgbi& color);
        void AddArrow(const Ray3f& ray, const Rgbi& color);

        void AddAxis();
        void AddGrid();


        std::ofstream file;
    };
}

#endif  // CORE_DUMP_H