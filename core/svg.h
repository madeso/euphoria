#ifndef CORE_SVG_H
#define CORE_SVG_H

#include <vector>
#include <string>
#include <optional>

#include "core/vec2.h"
#include "core/rgb.h"
#include "core/colors.h"

namespace euphoria::core::svg
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

    struct Svg
    {
        std::vector<Item> items;

        bool add_axis = false;
        int point_size = -1;
        bool point_text = false;

        double gridx =-1;
        double gridy =-1;

        Svg& AddAxis();

        Svg& Grid(double xy);

        Svg& DrawPoints(int size=3);

        Svg& operator<<(const Item& item);

        // calculate total area size and offset so that x+offset will never be lower than 0
        std::pair<vec2f,vec2f> CalculateSizeAndOffset() const;

        void Write(const std::string& path, int width=1280, int height=1024, int space = 6) const;
    };
}

#endif  // CORE_SVG_H
