#include "render/canvas.h"

#include "dependency_glad.h"

#include "assert/assert.h"
#include "base/rect.h"
#include "base/cint.h"
#include "base/mat4.h"

#include "render/shader.h"
#include "render/texture.h"
#include "render/opengl_utils.h"
#include "render/statechanger.h"


using namespace std::literals;


namespace eu::render
{

SpriteBatch::SpriteBatch(OpenglStates* states, ShaderProgram* quad_shader, Render2* r)
    : states(states)
    , render(r)
    , white_texture
    (
        load_image_from_color
        (
            SEND_DEBUG_LABEL_MANY("white_texture")
            color_from_rgba(255, 255, 255, 255),
            TextureEdge::clamp,
            TextureRenderStyle::pixel,
            Transparency::include,
            ColorData::dont_care
        )
    )
{
    quad_shader->use();

    glGenVertexArrays(1, &va);
    glBindVertexArray(va);

    constexpr auto vertex_size = 9 * sizeof(float);
    constexpr auto max_vertices = 4 * max_quads;
    constexpr auto max_indices = 6 * max_quads;

    glGenBuffers(1, &vb);
    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferData(GL_ARRAY_BUFFER, vertex_size * max_vertices, nullptr, GL_DYNAMIC_DRAW);

    auto relative_offset = [](unsigned int i)
    {
        return reinterpret_cast<void*>(i * sizeof(float));
    };

    unsigned int offset = 0;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_size, relative_offset(offset));
    offset += 3;

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertex_size, relative_offset(offset));
    offset += 4;

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertex_size, relative_offset(offset));
    offset += 2;


    std::vector<u32> indices;
    indices.reserve(max_indices);

    for(auto quad_index=0; quad_index<max_quads; quad_index+=1)
    {
        const auto base = quad_index * 4;
        indices.emplace_back(base + 0);
        indices.emplace_back(base + 1);
        indices.emplace_back(base + 2);

        indices.emplace_back(base + 2);
        indices.emplace_back(base + 3);
        indices.emplace_back(base + 0);
    }

    ASSERT(max_indices == indices.size());

    glGenBuffers(1, &ib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_indices * sizeof(u32), indices.data(), GL_STATIC_DRAW);
}


SpriteBatch::~SpriteBatch()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ib);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &vb);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &va);
}


void add_vertex(SpriteBatch* batch, const Vertex2& v)
{
    batch->data.push_back(v.position.x);
    batch->data.push_back(v.position.y);
    batch->data.push_back(v.position.z);

    batch->data.push_back(v.color.rgb.r);
    batch->data.push_back(v.color.rgb.g);
    batch->data.push_back(v.color.rgb.b);
    batch->data.push_back(v.color.alpha);

    batch->data.push_back(v.texturecoord.x);
    batch->data.push_back(v.texturecoord.y);
}

void SpriteBatch::quad(std::optional<Texture2d*> texture_argument, const Vertex2& v0, const Vertex2& v1, const Vertex2& v2, const Vertex2& v3)
{
    Texture2d* texture = texture_argument.value_or(&white_texture);

    if(quads == max_quads)
    {
        submit();
    }

    if(current_texture == nullptr)
    {
        current_texture = texture;
    }
    else if (current_texture != texture)
    {
        submit();
        current_texture = texture;
    }

    quads += 1;

    add_vertex(this, v0);
    add_vertex(this, v1);
    add_vertex(this, v2);
    add_vertex(this, v3);
}

void SpriteBatch::quad(std::optional<Texture2d*> texture, const Rect& scr, const std::optional<Rect>& texturecoord, const Color& tint)
{
    const auto tc = texturecoord.value_or(Rect::from_size({1.0f, 1.0f}));
    quad
    (
        texture,
        {.position = {scr.left, scr.bottom, 0.0f}, .color = tint, .texturecoord = {tc.left, tc.bottom}},
        {.position = {scr.right, scr.bottom, 0.0f}, .color = tint, .texturecoord = {tc.right, tc.bottom}},
        {.position = {scr.right, scr.top, 0.0f}, .color = tint, .texturecoord = {tc.right, tc.top}},
        {.position = {scr.left, scr.top, 0.0f}, .color = tint, .texturecoord = {tc.left, tc.top}}
    );
}

void SpriteBatch::submit()
{
    if(quads == 0)
    {
        return;
    }

    bind_texture_2d(states, render->texture_uniform, *current_texture);
    glBindVertexArray(va);

    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferSubData
    (
        GL_ARRAY_BUFFER,
        0,
        static_cast<GLsizeiptr>(sizeof(float) * data.size()),
        static_cast<const void*>(data.data())
    );
    glDrawElements(GL_TRIANGLES, 6 * quads, GL_UNSIGNED_INT, nullptr);

    data.resize(0);
    quads = 0;
    current_texture = nullptr;
}


Render2::Render2(OpenglStates* states)
    // todo(Gustav): move quad_description and quad_layout to a separate setup
    : quad_description
    (
        {
            {.type = VertexType::position2xy, .name = "position"},
            {.type = VertexType::color4, .name = "color"},
            {.type = VertexType::texture2, .name = "uv"}
        }
    )
    , quad_layout
    (
        compile_shader_layout(compile_attribute_layouts({quad_description}), quad_description, std::nullopt, std::nullopt)
    )
    , quad_shader
    (
        SEND_DEBUG_LABEL_MANY("QUAD SHADER")
        R"glsl(
            #version 450 core
            in vec3 position;
            in vec4 color;
            in vec2 uv;

            uniform mat4 view_projection;
            uniform mat4 transform;

            out vec4 varying_color;
            out vec2 varying_uv;

            void main()
            {
                varying_color = color;
                varying_uv = uv;
                gl_Position = view_projection * transform * vec4(position, 1.0);
            }
        )glsl",
        R"glsl(
            #version 450 core

            in vec4 varying_color;
            in vec2 varying_uv;

            uniform sampler2D uniform_texture;

            out vec4 color;

            void main()
            {
                color = texture(uniform_texture, varying_uv) * varying_color;
            }
        )glsl",
        quad_layout
    )
    , view_projection_uniform(quad_shader.get_uniform("view_projection"))
    , transform_uniform(quad_shader.get_uniform("transform"))
    , texture_uniform(quad_shader.get_uniform("uniform_texture"))
    , batch(states, &quad_shader, this)
{
    setup_textures(&quad_shader, {&texture_uniform});

    // todo(Gustav): verify mesh layout with SpriteBatch
}


struct ViewportDef
{
    Rect screen_rect;

    float virtual_width;
    float virtual_height;
};


ViewportDef
fit_with_black_bars
(
    float width,
    float height,
    int window_width,
    int window_height
)
{
    ASSERTX(width > 0, width);
    ASSERTX(height > 0, height);
    ASSERTX(window_width >= 0, window_width);
    ASSERTX(window_height >= 0, window_height);
    const float w = float_from_int(window_width) / width;
    const float h = float_from_int(window_height) / height;
    const float s = std::min(w, h);
    ASSERTX(s > 0, s, w, h);
    const float new_width = width * s;
    const float new_height = height * s;

    return ViewportDef
    {
        .screen_rect = Rect::from_size({new_width, new_height}).with_bottom_left_at
        ({
            (float_from_int(window_width) - new_width) / 2.0f,
            (float_from_int(window_height) - new_height) / 2.0f
        }),
        .virtual_width = width,
        .virtual_height = height
    };
}


float
DetermineExtendScale(float scale, float height, int window_height)
{
    const auto scaled_height = height * scale;
    const auto s = static_cast<float>(window_height) / scaled_height;
    return s;
}


ViewportDef
extended_viewport
(
    float width,
    float height,
    int window_width,
    int window_height
)
{
    ASSERTX(width >= 0, width);
    ASSERTX(height >= 0, height);
    ASSERTX(window_width >= 0, window_width);
    ASSERTX(window_height >= 0, window_height);
    const auto w = float_from_int(window_width) / width;
    const auto h = float_from_int(window_height) / height;
    const auto r = Rect::from_size({float_from_int(window_width), float_from_int(window_height)}).with_bottom_left_at({ 0, 0 });
    if(w < h)
    {
        const auto s = DetermineExtendScale(w, height, window_height);
        return ViewportDef {.screen_rect = r, .virtual_width = width, .virtual_height = height * s};
    }
    else
    {
        const auto s = DetermineExtendScale(h, width, window_width);
        return ViewportDef {.screen_rect = r, .virtual_width = width * s, .virtual_height = height};
    }
}


namespace
{
    float lerp(float lhs, float t, float rhs)
    {
        return lhs + t * (rhs - lhs);
    }

    Rect lerp(const Rect& lhs, float t, const Rect& rhs)
    {
        #define V(x) lerp(lhs.x, t, rhs.x)
        return Rect::from_left_right_top_bottom
        (
            V(left),
            V(right),
            V(top),
            V(bottom)
        );
        #undef V
    }
}


[[nodiscard]]
ViewportDef
lerp
(
    const ViewportDef& lhs,
    float t,
    const ViewportDef& rhs
)
{
    #define V(x) lerp(lhs.x, t, rhs.x)
    return
    {
        .screen_rect = V(screen_rect),
        .virtual_width = V(virtual_width),
        .virtual_height = V(virtual_height)
    };
    #undef V
}



void set_gl_viewport(const Rect& r)
{
    const auto s = r.get_size();
    glViewport(int_from_float(r.left), int_from_float(r.bottom), int_from_float(s.x), int_from_float(s.y));
}


RenderLayer2::~RenderLayer2()
{
    batch->submit();
}


RenderLayer2::RenderLayer2(const Layer& l, SpriteBatch* b)
    : Layer(l)
    , batch(b)
{
}

RenderLayer3::RenderLayer3(const Layer& l)
    : Layer(l)
{
}


Layer create_layer(const ViewportDef& vp)
{
    return {.viewport_aabb_in_worldspace = Rect::from_size({vp.virtual_width, vp.virtual_height}), .screen = vp.screen_rect};
}


RenderLayer2 create_layer2(const RenderCommand& rc, const ViewportDef& vp)
{
    set_gl_viewport(vp.screen_rect);
    
    // prepare for 2d rendering
    StateChanger{rc.states}.depth_test(false).blending(true);

    const auto camera = m4_identity;
    const auto projection = m4::create_ortho_lrud(0.0f, vp.virtual_width, vp.virtual_height, 0.0f, -1.0f, 1.0f);

    rc.render->quad_shader.use();
    rc.render->quad_shader.set_mat(rc.render->view_projection_uniform, projection);
    rc.render->quad_shader.set_mat(rc.render->transform_uniform, camera);

    // todo(Gustav): transform viewport according to the camera
    return RenderLayer2{create_layer(vp), &rc.render->batch};
}

RenderLayer3 create_layer3(const RenderCommand& rc, const ViewportDef& vp)
{
    set_gl_viewport(vp.screen_rect);
    
    // todo(Gustav): prepare states for 3d rendering
    StateChanger{rc.states}.depth_test(true).blending(false);

    return RenderLayer3{create_layer(vp)};
}


v2 Layer::mouse_to_world(const v2& p) const
{
    // transform from mouse pixels to window 0-1
    const auto n = to_01(screen, p);
    return from_01(viewport_aabb_in_worldspace, n);
}


ViewportDef create_viewport(const LayoutData& ld, const Size& size)
{
    if(ld.style==ViewportStyle::black_bars)
    {
        return fit_with_black_bars(ld.requested_width, ld.requested_height, size.width, size.height);
    }
    else
    {
        return extended_viewport(ld.requested_width, ld.requested_height, size.width, size.height);
    }
}


ViewportDef create_viewport(const LerpData& ld, const Size& size)
{
    return lerp
    (
        create_viewport(ld.lhs, size),
        ld.t,
        create_viewport(ld.rhs, size)
    );
}


void
RenderCommand::clear(const Rgb& color, const LayoutData& ld) const
{
    if(ld.style == ViewportStyle::extended)
    {
        glClearColor(color.r, color.g, color.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    else
    {
        auto l = with_layer2(*this, ld);
        l.batch->quad(std::nullopt, l.viewport_aabb_in_worldspace, std::nullopt, color);
    }
}


bool
is_fullscreen(const LerpData& ld)
{
    if(ld.lhs.style == ld.rhs.style || (ld.t <= 0.0f || ld.t >= 1.0f))
    {
        const auto style = ld.t >= 1.0f ? ld.rhs.style : ld.lhs.style;
        return style == ViewportStyle::extended;
    }

    return false;
}


void
RenderCommand::clear(const Rgb& color, const LerpData& ld) const
{
    if(is_fullscreen(ld))
    {
        glClearColor(color.r, color.g, color.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    else
    {
        auto l = with_layer2(*this, ld);
        l.batch->quad(std::nullopt, l.viewport_aabb_in_worldspace, std::nullopt, color);
    }
}


RenderLayer2 with_layer2(const RenderCommand& rc, const LayoutData& ld)
{
    const auto vp = create_viewport(ld, rc.size);
    return create_layer2(rc, vp);
}


RenderLayer3 with_layer3(const RenderCommand& rc, const LayoutData& ld)
{
    const auto vp = create_viewport(ld, rc.size);
    return create_layer3(rc, vp);
}


Layer with_layer(const InputCommand& rc, const LayoutData& ld)
{
    const auto vp = create_viewport(ld, rc.size);
    return create_layer(vp);
}





RenderLayer2 with_layer2(const RenderCommand& rc, const LerpData& ld)
{
    const auto vp = create_viewport(ld, rc.size);
    return create_layer2(rc, vp);
}


RenderLayer3 with_layer3(const RenderCommand& rc, const LerpData& ld)
{
    const auto vp = create_viewport(ld, rc.size);
    return create_layer3(rc, vp);
}


Layer with_layer(const InputCommand& rc, const LerpData& ld)
{
    const auto vp = create_viewport(ld, rc.size);
    return create_layer(vp);
}

}
