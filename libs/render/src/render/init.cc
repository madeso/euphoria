#include "render/init.h"

#include <iostream>

#include "log/log.h"
#include "assert/assert.h"

#include "render/gl.h"


namespace euphoria::render
{
    Init::Init(LoaderFunction loader, Init::BlendHack blend_hack)
        : is_ok(true)
    {
        const int glad_result = gladLoadGLLoader(loader);
        if(glad_result == 0)
        {
            LOG_ERROR("Failed to init glad, error: {0}", glad_result);
            is_ok = false;
        }

        {
            const std::string gl_vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
            const std::string gl_renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
            const std::string gl_version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
            const std::string gl_shading_language_version = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

            LOG_INFO("Vendor:         {0}", gl_vendor);
            LOG_INFO("Renderer:       {0}", gl_renderer);
            LOG_INFO("Version OpenGL: {0}", gl_version);
            LOG_INFO("Version GLSL:   {0}", gl_shading_language_version);
        }

        int attribute_count = 0;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &attribute_count);
        LOG_INFO("Max shader attribute count: {0}", attribute_count);

        // todo(Gustav): move this to a better place
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_SCISSOR_TEST); // need scissor test for the viewport clearing

        if(blend_hack == Init::BlendHack::enable_hack)
        {
            LOG_INFO("Enabled blend hack");
            // need to be enabled for shitty 2d rendering to work
            // todo(Gustav): fix a proper blending/backface culling render stack
            glDisable(GL_CULL_FACE);
            // glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
    }


    Init::~Init() = default;


    core::Mat4f
    Init::get_ortho_projection(float width, float height) const
    {
        ASSERT(is_ok);
        return core::Mat4f::create_ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
    }


    void
    Init::use_2d() const
    {
        ASSERT(is_ok);
        glDisable(GL_DEPTH_TEST);
    }


    void
    Init::clear_screen(const core::Rgb& color) const
    {
        ASSERT(is_ok);
        glClearColor(color.r, color.g, color.b, 1.0f);
        // glClearStencil(0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

}
