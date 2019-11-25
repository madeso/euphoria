#include "core/svg.h"

#include <functional>
#include <fstream>
#include <sstream>

#include "core/assert.h"


namespace euphoria::core::svg
{
    std::string ToHtml(const Rgbi& c)
    {
        std::ostringstream s;
        s << "rgb("
            << static_cast<unsigned int>(c.r) << ","
            << static_cast<unsigned int>(c.g) << ","
            << static_cast<unsigned int>(c.b) << ")";
        return s.str();
    }

    namespace strokes
    {
        std::vector<int> Dash(int size)
        {
            return {size, size};
        }
    }

    Poly& Poly::Stroke(const std::vector<int>& new_stroke)
    {
        stroke = new_stroke;
        return *this;
    }

    Poly& Poly::Close(const Rgbi& a_fill_color)
    {
        fill_color = a_fill_color;
        is_filled = true;
        return *this;
    }

    Text::Text(const vec2f& p, const std::string& t, const Rgbi& c)
        : point(p), text(t), color(c){}

    Item::Item(const Poly& p)  : poly(std::make_shared<Poly>(p)) {}
    Item::Item(const Text& p)  : text(std::make_shared<Text>(p)) {}
    Item::Item(const Group& g) : group(std::make_shared<Group>(g)) {}

    const Poly*  AsPoly(const Item* item)  { return item->poly?  item->poly.get()  : nullptr; }
    const Text*  AsText(const Item* item)  { return item->text?  item->text.get()  : nullptr; }
    const Group* AsGroup(const Item* item) { return item->group? item->group.get() : nullptr; }

    Group& Group::operator<<(const Item& item)
    {
        items.emplace_back(item);
        return *this;
    }

    namespace detail
    {
        struct Writer
        {
            std::ofstream file;

            std::function<double (double)> px;
            std::function<double (double)> py;

            explicit Writer(const std::string& path)
                : file(path.c_str()) {}
        };

        struct MinMax
        {
            vec2f min = vec2f::Zero();
            vec2f max = vec2f::Zero();

            MinMax& operator<<(const vec2f& point)
            {
                min.x = std::min(min.x, point.x);
                min.y = std::min(min.y, point.y);

                max.x = std::max(max.x, point.x);
                max.y = std::max(max.y, point.y);

                return *this;
            }
        };

        void WritePoly(Writer& writer, const Poly* poly)
        {
            writer.file << "<polyline points=\"";
            {bool first = true;
            for(const auto p: poly->points)
            {
                if(first) {first = false;}
                else {writer.file << " ";}

                writer.file << writer.px(p.x) << ","
                            << writer.py(p.y);
            }}
            writer.file << "\" style=\"fill:";
            if(!poly->is_filled)
            {
                writer.file << "none";
            }
            else
            {
                writer.file << ToHtml(poly->fill_color);
            }
            writer.file << ";stroke:";
            writer.file << ToHtml(poly->stroke_color);
            writer.file << ";stroke-width:1\"";
            if(!poly->stroke.empty())
            {
                writer.file << " stroke-dasharray=\"";
                {bool first = true;
                for(const auto d: poly->stroke)
                {
                    if(first) first = false;
                    else writer.file << ",";
                    writer.file << d;
                }}
                writer.file << "\"";
            }
            writer.file << "/>\n";
        }

        void WriteText(Writer& writer, const Text* text)
        {
            writer.file << "<text x=\"" << writer.px(text->point.x)
                            << "\" y=\""<< writer.py(text->point.y) << "\" fill=\"" << ToHtml(text->color) << "\">" << text->text << "</text>\n";
        }

        void WriteItem(Writer& writer, const Item& item)
        {
            auto poly = AsPoly(&item);
            auto text = AsText(&item);
            auto group = AsGroup(&item);
            if(poly)
            {
                WritePoly(writer, poly);
            }
            else if(text)
            {
                WriteText(writer, text);
            }
            else if(group)
            {
                writer.file << "<g>\n";
                for(const auto& i: group->items)
                {
                    WriteItem(writer, i);
                }
                writer.file << "</g>\n";
            }
            else
            {
                DIE("unhandled type");
            }
        }

        void FindMinMax(MinMax& mm, const Item& item)
        {
            auto poly = AsPoly(&item);
            auto text = AsText(&item);
            auto group = AsGroup(&item);
            if(poly)
            {
                for(const auto& point: poly->points)
                {
                    mm << point;
                }
            }
            else if(text)
            {
                mm << text->point;
            }
            else if(group)
            {
                for(const auto& i: group->items)
                {
                    FindMinMax(mm, i);
                }
            }
            else
            {
                DIE("unhandled type");
            }
        }
    }

    Svg& Svg::AddAxis()
    {
        add_axis = true;
        return *this;
    }

    Svg& Svg::Grid(double xy)
    {
        gridx = xy;
        gridy = xy;
        return *this;
    }

    Svg& Svg::DrawPoints(int size)
    {
        point_size = size;
        return *this;
    }

    Svg& Svg::operator<<(const Item& item)
    {
        items.emplace_back(item);
        return *this;
    }

    // calculate total area size and offset so that x+offset will never be lower than 0
    std::pair<vec2f,vec2f> Svg::CalculateSizeAndOffset() const
    {
        detail::MinMax minmax;

        for(const auto& item: items)
        {
            detail::FindMinMax(minmax, item);
        }

        const auto& min = minmax.min;
        const auto& max = minmax.max;

        const auto offset = vec2f{-std::min(min.x,0.0f), -std::min(min.y, 0.0f)};
        const auto size = vec2f{max.x - min.x, max.y-min.y};

        return {size, offset};
    }

    void Svg::Write(const std::string& path, int width, int height, int space) const
    {
        // todo(Gustav): grab features/ideas from nbg/faddi code
        // todo(Gustav): background grid
        // todo(Gustav): improve api
        // todo(Gustav): zoom & pan grid view
        // todo(Gustav): list of polys and view and hover over points

        const auto r = CalculateSizeAndOffset();
        const auto size = r.first;
        const auto offset = r.second;
        const auto scale = std::min((width-space*2) / size.x, (height-space*2) / size.y);
        const auto dx = offset.x * scale;
        const auto dy = offset.y * scale;
        const auto px = [=](auto x) { return space + dx + x * scale; };
        const auto py = [=](auto y) { return height - (space + dy + y * scale); };

        detail::Writer writer(path);
        writer.px = px;
        writer.py = py;
            
        writer.file << "<html style=\"height: 100%\">\n";
        writer.file << "<body style=\"background-color:" << ToHtml(Color::DarkGray) << "; height: 100%\">\n";

        writer.file << "<div style=\"display: grid; grid-template-columns: 1fr auto 1fr; grid-template-rows: 1fr auto 1fr; width:100%; height:100%\">\n";
        writer.file << "<div style=\"grid-row-start: 2; grid-column-start: 2;\">\n";

        writer.file << "<svg width=\""  << width << "\" height=\"" << height << "\">\n";
        writer.file << "<rect width=\"" << width << "\" height=\"" << height << "\""
                " style=\"fill:" << ToHtml(Color::White) << ";stroke-width:0\" />\n";

        for(const auto& item: items)
        {
            detail::WriteItem(writer, item);
        }

        if(point_size > 0)
        {
            const auto c = Color::Black;

            for(const auto& item: items)
            {
                auto poly = AsPoly(&item);
                if(poly)
                {
                    int index = 0;
                    for(const auto p: poly->points)
                    {
                        writer.file << "<circle cx=\"" << px(p.x) << "\" cy=\""<< py(p.y) << "\" r=\"" << point_size << "\" stroke=\"none\" fill=\"" << ToHtml(c) << "\""
                            << " title=\""
                            // << "index: " << index
                            << index << "(" << p.x << " " << p.y << ")"
                            << "\"/>\n";
                        index += 1;
                    }
                }
            }
        }

        if(point_text)
        {
            const auto c = Color::Black;

            for(const auto& item: items)
            {
                auto poly = AsPoly(&item);
                if(poly)
                {
                    int index = 0;
                    for(const auto p: poly->points)
                    {
                        writer.file << "<text x=\"" << px(p.x) << "\" y=\""<< py(p.y) << "\" fill=\"" << ToHtml(c) << "\">"
                            << index << "(" << p.x << " " << p.y << ")"
                            << "</text>";
                        index += 1;
                    }
                }
            }
        }

        auto vline = [&](double x, const Rgbi& c) { writer.file << "<line x1=\"" << px(x) << "\" y1=\"0\""           " x2=\""<< px(x) <<"\" y2=\"" << height << "\" style=\"stroke:"<< ToHtml(c) <<";stroke-width:1\" />\n"; };
        auto hline = [&](double y, const Rgbi& c) { writer.file << "<line x1=\"0\""            " y1=\""<< py(y) << "\" x2=\""<< width <<"\" y2=\"" << py(y)  << "\" style=\"stroke:"<< ToHtml(c) <<";stroke-width:1\" />\n"; };

        const auto grid_color = Color::LightGray;

        if(gridx > 0 )
        {
            for(auto x =  gridx; px(x) < width; x += gridx) { vline(x, grid_color); }
            for(auto x = -gridx; px(x) > 0;     x -= gridx) { vline(x, grid_color); }
        }

        if(gridy > 0 )
        {
            for(auto y =  gridy; px(y) < height; y += gridy) { hline(y, grid_color); }
            for(auto y = -gridy; px(y) > 0;      y -= gridy) { hline(y, grid_color); }
        }

        if(add_axis)
        {
            const auto axis_color = Color::Black;
            hline(0, axis_color);
            vline(0, axis_color);
        }

        writer.file << "</svg>\n";

        writer.file << "</div>\n";
        writer.file << "</div>\n";

        writer.file << "</body>\n";
        writer.file << "</html>\n";
    }
}
