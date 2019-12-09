#include "core/dump.h"

#include <functional>
#include <fstream>
#include <sstream>

#include "core/assert.h"
#include "core/plane.h"
#include "core/ray.h"


namespace euphoria::core::dump2d
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

    std::string ToHtmlOrNone(const std::optional<Rgbi>& c)
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

    Poly& Poly::Fill(const Rgbi& a_fill_color)
    {
        fill_color = a_fill_color;
        return *this;
    }

    Poly& Poly::Close()
    {
        is_closed = true;
        return *this;
    }

    Text::Text(const vec2f& p, const std::string& t, const Rgbi& c)
        : point(p), text(t), color(c) {}

    Circle&
    Circle::Line(const Rgbi& lc)
    {
        line_color = lc;
        return *this;
    }
    Circle::Circle(const vec2f& p, float r, std::optional<Rgbi> c)
        : point(p), radius(r), fill_color(c) {}

    Item::Item(const Poly& p)  : poly(std::make_shared<Poly>(p)) {}
    Item::Item(const Text& p)  : text(std::make_shared<Text>(p)) {}
    Item::Item(const Group& g) : group(std::make_shared<Group>(g)) {}
    Item::Item(const Circle& c) : circle(std::make_shared<Circle>(c)) {}

    const Poly*  AsPoly(const Item* item)  { return item->poly?  item->poly.get()  : nullptr; }
    const Text*  AsText(const Item* item)  { return item->text?  item->text.get()  : nullptr; }
    const Group* AsGroup(const Item* item) { return item->group? item->group.get() : nullptr; }
    const Circle* AsCircle(const Item* item) { return item->circle? item->circle.get() : nullptr; }

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

            double scale = 1;

            explicit Writer(const std::string& path)
                : file(path.c_str()) {}
        };

        struct MinMax
        {
            vec2f min = vec2f::Zero();
            vec2f max = vec2f::Zero();

            MinMax& Include(const vec2f& point, float extra=0)
            {
                min.x = std::min(min.x, point.x - extra);
                min.y = std::min(min.y, point.y - extra);

                max.x = std::max(max.x, point.x + extra);
                max.y = std::max(max.y, point.y + extra);

                return *this;
            }

            MinMax& operator<<(const vec2f& point)
            {
                return Include(point);
            }
        };

        void WritePoly(Writer& writer, const Poly* poly)
        {
            writer.file << "<polyline points=\"";
            {
                bool first = true;

                auto write_point = [&writer](const vec2f& p)
                {
                    writer.file << writer.px(p.x) << ","
                                << writer.py(p.y);
                };

                for(const auto p: poly->points)
                {
                    if(first) {first = false;}
                    else {writer.file << " ";}

                    write_point(p);
                }

                if(poly->is_closed && !poly->points.empty())
                {
                    writer.file << " ";
                    write_point(poly->points[0]);
                }
            }

            writer.file << "\" style=\"fill:";
            writer.file << ToHtmlOrNone(poly->fill_color);
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

        void WriteCircle(Writer& writer, const Circle* circle)
        {
            writer.file
                << "<circle cx=\"" << writer.px(circle->point.x) << "\" cy=\""<< writer.py(circle->point.y)
                << "\" r=\"" << circle->radius*writer.scale << "\" stroke=\"" << ToHtmlOrNone(circle->line_color)
                << "\" fill=\"" << ToHtmlOrNone(circle->fill_color) << "\""
                << "/>\n";
        }

        void WriteItem(Writer& writer, const Item& item)
        {
            auto poly = AsPoly(&item);
            auto text = AsText(&item);
            auto group = AsGroup(&item);
            auto circle = AsCircle(&item);
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
            else if(circle)
            {
                WriteCircle(writer, circle);
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
            auto circle = AsCircle(&item);
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
            else if(circle)
            {
                mm.Include(circle->point, circle->radius);
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

    Dumper& Dumper::AddAxis()
    {
        add_axis = true;
        return *this;
    }

    Dumper& Dumper::Grid(double xy)
    {
        gridx = xy;
        gridy = xy;
        return *this;
    }

    Dumper& Dumper::DrawPoints(int size)
    {
        point_size = size;
        return *this;
    }

    Dumper& Dumper::operator<<(const Item& item)
    {
        items.emplace_back(item);
        return *this;
    }

    // calculate total area size and offset so that x+offset will never be lower than 0
    std::pair<vec2f,vec2f> Dumper::CalculateSizeAndOffset() const
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

    void Dumper::Write(const std::string& path, int width, int height, int space) const
    {
        // reference: https://www.w3schools.com/graphics/svg_path.asp
        // todo(Gustav): improve api
        // todo(Gustav): interactive zoom & pan grid view (in canvas)
        // todo(Gustav): list of polys and view and hover over points
        // todo(Gustav): hide polys
        // todo(Gustav): attach string->string properties
        // todo(Gustav): option to replace string with polylines so we can measure and layout text

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
        writer.scale = scale;
            
        writer.file << "<html style=\"height: 100%\">\n";
        writer.file << "<body style=\"background-color:" << ToHtml(Color::DarkGray) << "; height: 100%\">\n";

        writer.file << "<div style=\"display: grid; grid-template-columns: 1fr auto 1fr; grid-template-rows: 1fr auto 1fr; width:100%; height:100%\">\n";
        writer.file << "<div style=\"grid-row-start: 2; grid-column-start: 2;\">\n";

        writer.file << "<svg width=\""  << width << "\" height=\"" << height << "\">\n";
        writer.file << "<rect width=\"" << width << "\" height=\"" << height << "\""
                " style=\"fill:" << ToHtml(canvas_color) << ";stroke-width:0\" />\n";

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

namespace euphoria::core::dump3d
{

    Dumper::Dumper(const std::string& path)
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

    Dumper::~Dumper()
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


    std::string ToHex(const Rgbi& c)
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
    Dumper::AddSphere(const vec3f& p, float radius, const Rgbi& color)
    {
        file << s << "add_geom(new THREE.SphereGeometry(" << radius << "), " << ToHex(color) << ")\n"
             << s << "  .position.set("<<p.x<<", "<<p.y<<", "<<p.z<<");\n";
    }
    

    void
    Dumper::AddLines(const std::vector<vec3f>& points, const Rgbi& color)
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
    Dumper::AddPlane(const Plane& plane, const Rgbi& color)
    {
        constexpr auto size = 5;
        file
        << s << "scene.add( new THREE.PlaneHelper( new THREE.Plane( "
        << ToThree(plane.normal) << ", " << plane.distance
        << "), " << size << ", " << ToHex(color) << ") );\n"
        ;
    }


    void
    Dumper::AddArrow(const Ray3f& ray, const Rgbi& color)
    {
        file
        << s << "scene.add(new THREE.ArrowHelper("
        << ToThree(ray.dir.GetNormalized()) << ", "
        << ToThree(ray.from) << ", "
        << ray.dir.GetLength() << ", "
        << ToHex(color) << ") );\n";
    }


    void
    Dumper::AddAxis()
    {
        constexpr auto size = 2;
        file
        << s << "scene.add(new THREE.AxesHelper("<< size <<") );\n";
    }


    void
    Dumper::AddGrid()
    {
        constexpr auto size = 10;
        constexpr auto divisions = 10;

        file
        << s << "scene.add(new THREE.GridHelper("<< size <<", " << divisions<<") );\n";
    }
}
