#pragma once

// todo(Gustav): figure out a beter name than canvas

#include "base/rect.h"

// #include "tred/types.h"
#include "base/vec2.h"
#include "base/vec3.h"
#include "base/vec4.h"

#include "render/shader.h"
#include "render/texture.h"

#include "base/colors.h"


namespace eu::render
{

struct SpriteBatch;
struct OpenglStates;
struct ShaderProgram;
struct Render2;


enum class ViewportStyle { black_bars, extended};


struct Vertex2
{
    v3 position;
    Color color;
    v2 texturecoord;
};

struct Quad
{
    std::optional<const Texture2d*> texture = std::nullopt;
    std::optional<Rect> texturecoord = std::nullopt;
    Color tint = eu::colors::white;

    void draw(SpriteBatch* batch, const Rect& screen);
};

struct SpriteBatch
{
    static constexpr int max_quads = 100;

    OpenglStates* states;
    std::vector<float> data;
    int quads = 0;
    const Texture2d* current_texture = nullptr;
    u32 va;
    u32 vb;
    u32 ib;
    Render2* render;
    Texture2d white_texture;

    SpriteBatch(OpenglStates* states, ShaderProgram* shader, Render2* r);
    ~SpriteBatch();

    SpriteBatch(const SpriteBatch&) = delete;
    void operator=(const SpriteBatch&) = delete;
    SpriteBatch(SpriteBatch&&) = delete;
    void operator=(SpriteBatch&&) = delete;

    void quad(std::optional<const Texture2d*> texture, const Vertex2& v0, const Vertex2& v1, const Vertex2& v2, const Vertex2& v3);

    void submit();
};

struct Render2
{
    explicit Render2(OpenglStates* states);

    ShaderVertexAttributes quad_description;
    CompiledShaderVertexAttributes quad_layout;
    ShaderProgram quad_shader;
    Uniform view_projection_uniform;
    Uniform transform_uniform;
    Uniform texture_uniform;

    SpriteBatch batch;
};

struct Layer
{
    Rect viewport_aabb_in_worldspace;
    Rect screen;

    v2 mouse_to_world(const v2& p) const;
};

struct RenderLayer2 : Layer
{
    SpriteBatch* batch;

    RenderLayer2(const Layer& l, SpriteBatch* batch);

    ~RenderLayer2();
};

struct RenderLayer3 : Layer
{
    RenderLayer3(const Layer& l);
};


struct LayoutData
{
    ViewportStyle style;
    float requested_width;
    float requested_height;
};

struct LerpData
{
    LayoutData lhs;
    LayoutData rhs;
    float t;
};


struct InputCommand
{
    Size size;
};

struct RenderCommand
{
    OpenglStates* states;
    Render2* render;
    Size size;

    // todo(Gustav): add clear to color function
    void clear(const Rgb& color, const LayoutData& ld) const;
    void clear(const Rgb& color, const LerpData& ld) const;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions


// todo(Gustav): pass camera to with_ functions
// todo(Gustav): move to member functions

RenderLayer2 with_layer2(const RenderCommand& rc, const LayoutData& ld);
RenderLayer3 with_layer3(const RenderCommand& rc, const LayoutData& ld);
Layer with_layer(const InputCommand& rc, const LayoutData& ld);

RenderLayer2 with_layer2(const RenderCommand& rc, const LerpData& ld);
RenderLayer3 with_layer3(const RenderCommand& rc, const LerpData& ld);
Layer with_layer(const InputCommand& rc, const LerpData& ld);

}
