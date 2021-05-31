#include "core/dump.h"

#include <functional>
#include <fstream>
#include <sstream>

#include "core/assert.h"
#include "core/plane.h"
#include "core/ray.h"


namespace euphoria::core::dump2d
{
    std::string ToHtml(const rgbi& c)
    {
        std::ostringstream s;
        s << "rgb("
            << static_cast<unsigned int>(c.r) << ","
            << static_cast<unsigned int>(c.g) << ","
            << static_cast<unsigned int>(c.b) << ")";
        return s.str();
    }

    std::string ToHtmlOrNone(const std::optional<rgbi>& c)
    {
        if(!c)
        {
            return "none";
        }
        else
        {
            return ToHtml(*c);
        }
    }

    namespace strokes
    {
        std::vector<int> dash(int size)
        {
            return {size, size};
        }
    }

    poly& poly::set_stroke(const std::vector<int>& new_stroke)
    {
        stroke = new_stroke;
        return *this;
    }

    poly& poly::fill(const rgbi& a_fill_color)
    {
        fill_color = a_fill_color;
        return *this;
    }

    poly& poly::close()
    {
        is_closed = true;
        return *this;
    }

    text::text(const vec2f& p, const std::string& t, const rgbi& c)
        : point(p), label(t), color(c) {}

    circle&
    circle::set_line_color(const rgbi& lc)
    {
        line_color = lc;
        return *this;
    }
    circle::circle(const vec2f& p, float r, std::optional<rgbi> fill)
        : point(p), radius(r), fill_color(fill) {}

    item::item(const dump2d::poly& p) : poly(std::make_shared<dump2d::poly>(p)) {}
    item::item(const dump2d::text& p) : text(std::make_shared<dump2d::text>(p)) {}
    item::item(const dump2d::group& g) : group(std::make_shared<dump2d::group>(g)) {}
    item::item(const dump2d::circle& c) : circle(std::make_shared<dump2d::circle>(c)) {}

    const poly* as_poly(const item* item) { return item->poly? item->poly.get() : nullptr; }
    const text* as_text(const item* item) { return item->text? item->text.get() : nullptr; }
    const group* as_group(const item* item) { return item->group? item->group.get() : nullptr; }
    const circle* as_circle(const item* item) { return item->circle? item->circle.get() : nullptr; }

    group& group::add(const item& item)
    {
        items.emplace_back(item);
        return *this;
    }

    namespace detail
    {
        struct writer
        {
            std::ofstream file;

            std::function<float (float)> px;
            std::function<float (float)> py;

            float scale = 1.0f;

            explicit writer(const std::string& path)
                : file(path.c_str()) {}
        };

        struct min_max
        {
            vec2f min = vec2f::zero();
            vec2f max = vec2f::zero();

            min_max& include(const vec2f& point, float extra=0)
            {
                min.x = std::min(min.x, point.x - extra);
                min.y = std::min(min.y, point.y - extra);

                max.x = std::max(max.x, point.x + extra);
                max.y = std::max(max.y, point.y + extra);

                return *this;
            }

            min_max& operator<<(const vec2f& point)
            {
                return include(point);
            }
        };

        void write_poly(writer* writer, const poly* poly)
        {
            writer->file << "<polyline points=\"";
            {
                bool first = true;

                auto write_point = [&writer](const vec2f& p)
                {
                    writer->file << writer->px(p.x) << ","
                                << writer->py(p.y);
                };

                for(const auto p: poly->points)
                {
                    if(first) {first = false;}
                    else {writer->file << " ";}

                    write_point(p);
                }

                if(poly->is_closed && !poly->points.empty())
                {
                    writer->file << " ";
                    write_point(poly->points[0]);
                }
            }

            writer->file << "\" style=\"fill:";
            writer->file << ToHtmlOrNone(poly->fill_color);
            writer->file << ";stroke:";
            writer->file << ToHtml(poly->stroke_color);
            writer->file << ";stroke-width:1\"";
            if(!poly->stroke.empty())
            {
                writer->file << " stroke-dasharray=\"";
                {bool first = true;
                for(const auto d: poly->stroke)
                {
                    if(first) { first = false; }
                    else { writer->file << ","; }
                    writer->file << d;
                }}
                writer->file << "\"";
            }
            writer->file << "/>\n";
        }

        void write_text(writer* writer, const text* text)
        {
            writer->file << "<text x=\"" << writer->px(text->point.x)
                            << "\" y=\""<< writer->py(text->point.y) << "\" fill=\"" << ToHtml(text->color) << "\">" << text->label << "</text>\n";
        }

        void write_circle(writer* writer, const circle* circle)
        {
            writer->file
                << "<circle cx=\"" << writer->px(circle->point.x) << "\" cy=\""<< writer->py(circle->point.y)
                << "\" r=\"" << circle->radius*writer->scale << "\" stroke=\"" << ToHtmlOrNone(circle->line_color)
                << "\" fill=\"" << ToHtmlOrNone(circle->fill_color) << "\""
                << "/>\n";
        }

        void write_item(writer* writer, const item& item)
        {
            const auto* poly = as_poly(&item);
            const auto* text = as_text(&item);
            const auto* group = as_group(&item);
            const auto* circle = as_circle(&item);
            if(poly != nullptr)
            {
                write_poly(writer, poly);
            }
            else if(text != nullptr)
            {
                write_text(writer, text);
            }
            else if(group != nullptr)
            {
                writer->file << "<g>\n";
                for(const auto& i: group->items)
                {
                    write_item(writer, i);
                }
                writer->file << "</g>\n";
            }
            else if(circle != nullptr)
            {
                write_circle(writer, circle);
            }
            else
            {
                DIE("unhandled type");
            }
        }

        void find_min_max(min_max* mm, const item& item)
        {
            const auto* poly = as_poly(&item);
            const auto* text = as_text(&item);
            const auto* group = as_group(&item);
            const auto* circle = as_circle(&item);

            if(poly != nullptr)
            {
                for(const auto& point: poly->points)
                {
                    *mm << point;
                }
            }
            else if(text != nullptr)
            {
                *mm << text->point;
            }
            else if(circle != nullptr)
            {
                mm->include(circle->point, circle->radius);
            }
            else if(group != nullptr)
            {
                for(const auto& i: group->items)
                {
                    find_min_max(mm, i);
                }
            }
            else
            {
                DIE("unhandled type");
            }
        }
    }

    dumper& dumper::add_axis()
    {
        add_axis_when_writing = true;
        return *this;
    }

    dumper& dumper::add_grid(float xy)
    {
        grid_x = xy;
        grid_y = xy;
        return *this;
    }

    dumper& dumper::enable_points_rendering(int size)
    {
        point_size = size;
        return *this;
    }

    dumper& dumper::add(const item& item)
    {
        items.emplace_back(item);
        return *this;
    }

    // calculate total area size and offset so that x+offset will never be lower than 0
    std::pair<vec2f,vec2f> dumper::calculate_size_and_offset() const
    {
        detail::min_max minmax;

        for(const auto& item: items)
        {
            detail::find_min_max(&minmax, item);
        }

        const auto& min = minmax.min;
        const auto& max = minmax.max;

        const auto offset = vec2f{-std::min(min.x,0.0f), -std::min(min.y, 0.0f)};
        const auto size = vec2f{max.x - min.x, max.y-min.y};

        return {size, offset};
    }

    void dumper::write(const std::string& path, int width, int height, int space) const
    {
        // reference: https://www.w3schools.com/graphics/svg_path.asp
        // todo(Gustav): improve api
        // todo(Gustav): interactive zoom & pan grid view (in canvas)
        // todo(Gustav): list of polys and view and hover over points
        // todo(Gustav): hide polys
        // todo(Gustav): attach string->string properties
        // todo(Gustav): option to replace string with polylines so we can measure and layout text

        const auto r = calculate_size_and_offset();
        const auto size = r.first;
        const auto offset = r.second;
        const auto scale = std::min(static_cast<float>(width-space*2) / size.x, static_cast<float>(height-space*2) / size.y);
        const auto dx = offset.x * scale;
        const auto dy = offset.y * scale;
        const auto px = [=](float x) -> float { return static_cast<float>(space) + dx + x * scale; };
        const auto py = [=](float y) -> float { return static_cast<float>(height) - (static_cast<float>(space) + dy + y * scale); };

        detail::writer writer(path);
        writer.px = px;
        writer.py = py;
        writer.scale = scale;

        writer.file << "<html style=\"height: 100%\">\n";
        writer.file << "<body style=\"background-color:" << ToHtml(color::dark_gray) << "; height: 100%\">\n";

        writer.file << "<div style=\"display: grid; grid-template-columns: 1fr auto 1fr; grid-template-rows: 1fr auto 1fr; width:100%; height:100%\">\n";
        writer.file << "<div style=\"grid-row-start: 2; grid-column-start: 2;\">\n";

        writer.file << "<svg width=\""  << width << "\" height=\"" << height << "\">\n";
        writer.file << "<rect width=\"" << width << "\" height=\"" << height << "\""
                " style=\"fill:" << ToHtml(canvas_color) << ";stroke-width:0\" />\n";

        for(const auto& item: items)
        {
            detail::write_item(&writer, item);
        }

        if(point_size > 0)
        {
            const auto c = color::black;

            for(const auto& item: items)
            {
                const auto* poly = as_poly(&item);
                if(poly != nullptr)
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
            const auto c = color::black;

            for(const auto& item: items)
            {
                const auto* poly = as_poly(&item);
                if(poly != nullptr)
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

        auto vline = [&](float x, const rgbi& c) { writer.file << "<line x1=\"" << px(x) << "\" y1=\"0\""           " x2=\""<< px(x) <<"\" y2=\"" << height << "\" style=\"stroke:"<< ToHtml(c) <<";stroke-width:1\" />\n"; };
        auto hline = [&](float y, const rgbi& c) { writer.file << "<line x1=\"0\""            " y1=\""<< py(y) << "\" x2=\""<< width <<"\" y2=\"" << py(y)  << "\" style=\"stroke:"<< ToHtml(c) <<";stroke-width:1\" />\n"; };

        const auto grid_color = color::light_gray;

        if(grid_x > 0 )
        {
            for(auto x =  grid_x; px(x) < static_cast<float>(width); x += grid_x) { vline(x, grid_color); }
            for(auto x = -grid_x; px(x) > 0;                         x -= grid_x) { vline(x, grid_color); }
        }

        if(grid_y > 0 )
        {
            for(auto y =  grid_y; px(y) < static_cast<float>(height); y += grid_y) { hline(y, grid_color); }
            for(auto y = -grid_y; px(y) > 0;                          y -= grid_y) { hline(y, grid_color); }
        }

        if(add_axis_when_writing)
        {
            const auto axis_color = color::black;
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

namespace euphoria::core::dump3d
{

    dumper::dumper(const std::string& path)
        : file(path.c_str())
    {
        file <<
        R"html(<html>
  <head>
    <title>My first three.js app</title>
    <style>
      body { margin: 0; }
      canvas { width: 100%; height: 100% }
    </style>
  </head>
  <body>
    <script src="https://threejs.org/build/three.min.js"></script>
    <script src="https://threejs.org/examples/js/controls/OrbitControls.js"></script>

    <script>
      var scene = new THREE.Scene();
      scene.background = new THREE.Color( 0x0096ff );
      var camera = new THREE.PerspectiveCamera( 75, window.innerWidth/window.innerHeight, 0.1, 1000 );

      // https://threejs.org/examples/#webgl_shadowmesh

      var light = new THREE.AmbientLight(0xffffff);
      scene.add( light );
      // scene.add( new THREE.AxisHelper( 20 ) );

      var renderer = new THREE.WebGLRenderer();
      renderer.setSize( window.innerWidth, window.innerHeight );
      document.body.appendChild( renderer.domElement );
      function onWindowResize() {
        camera.aspect = window.innerWidth / window.innerHeight;
        camera.updateProjectionMatrix();
        renderer.setSize( window.innerWidth, window.innerHeight );
      }
      window.addEventListener( 'resize', onWindowResize, false );

      var add_geom = function(geom, c)
      {
          var material = new THREE.MeshPhongMaterial( {
              color: c,
              // shading: THREE.FlatShading,
              polygonOffset: true,
              polygonOffsetFactor: 1,
              polygonOffsetUnits: 1
          } );

          var mesh = new THREE.Mesh( geom, material );
          scene.add( mesh );
          return mesh;
      }

      var add_wire = function(geom, c)
      {
          mesh = add_geom(geom, c);
          var geo = new THREE.EdgesGeometry( mesh.geometry ); // or WireframeGeometry
          var mat = new THREE.LineBasicMaterial( { color: 0xffffff, linewidth: 2 } );
          var wireframe = new THREE.LineSegments( geo, mat );
          mesh.add( wireframe );
      }

      // add_wire(new THREE.BoxGeometry( 1, 1, 1 ));
)html"
        ;
    }

    namespace
    {
        constexpr auto s = "        ";
    }

    dumper::~dumper()
    {
        file <<
        R"html(
      camera.position.z = 5;
      new THREE.OrbitControls( camera, renderer.domElement );

      var animate = function () {
        requestAnimationFrame( animate );
        renderer.render( scene, camera );
      };

      animate();
    </script>
  </body>
</html>)html"
        ;
    }


    std::string ToHex(const rgbi& c)
    {
        std::stringstream ss;
        ss << "0x" << std::hex
            << static_cast<unsigned int>(c.r)
            << static_cast<unsigned int>(c.g)
            << static_cast<unsigned int>(c.b)
            ;
        return ss.str();
    }


    void
    dumper::add_sphere(const vec3f& p, float radius, const rgbi& color)
    {
        file << s << "add_geom(new THREE.SphereGeometry(" << radius << "), " << ToHex(color) << ")\n"
             << s << "  .position.set("<<p.x<<", "<<p.y<<", "<<p.z<<");\n";
    }


    void
    dumper::add_lines(const std::vector<vec3f>& points, const rgbi& color)
    {
        file
        << s << "(function() {\n"
        << s << "  var material = new THREE.LineBasicMaterial( { color: " << ToHex(color) <<" } );\n"
        << s << "  var geometry = new THREE.Geometry();\n"
        ;
        for(auto p: points)
        {
            file
            << s << "  geometry.vertices.push(new THREE.Vector3( "
                 << p.x <<", " << p.y << ", " << p.z << ") );\n"
            ;
        }
        file
        << s << "  var line = new THREE.Line( geometry, material );\n"
        << s << "  scene.add( line );\n"
        << s << "})();\n"
        ;
    }


    std::string
    ToThree(const vec3f& v)
    {
        std::ostringstream ss;
        ss << "new THREE.Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
        return ss.str();
    }


    void
    dumper::add_plane(const plane& plane, const rgbi& color)
    {
        constexpr auto size = 5;
        file
        << s << "scene.add( new THREE.PlaneHelper( new THREE.Plane( "
        << ToThree(plane.normal) << ", " << plane.distance
        << "), " << size << ", " << ToHex(color) << ") );\n"
        ;
    }


    void
    dumper::add_arrow(const ray3f& ray, const rgbi& color)
    {
        file
        << s << "scene.add(new THREE.ArrowHelper("
        << ToThree(ray.dir.get_normalized()) << ", "
        << ToThree(ray.from) << ", "
        << ray.dir.get_length() << ", "
        << ToHex(color) << ") );\n";
    }


    void
    dumper::add_axis()
    {
        constexpr auto size = 2;
        file
        << s << "scene.add(new THREE.AxesHelper("<< size <<") );\n";
    }


    void
    dumper::add_grid()
    {
        constexpr auto size = 10;
        constexpr auto divisions = 10;

        file
        << s << "scene.add(new THREE.GridHelper("<< size <<", " << divisions<<") );\n";
    }
}
