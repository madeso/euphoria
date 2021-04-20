#include "render/init.h"

#include <iostream>

#include "core/log.h"
#include "core/assert.h"

#include "render/gl.h"


namespace euphoria::render
{
    Init::Init(LoaderFunction loader, Init::BlendHack blend_hack) : ok(true)
    {
        const int glad_result = gladLoadGLLoader(loader);
        if(glad_result == 0)
        {
            LOG_ERROR("Failed to init glad, error: {0}", glad_result);
            ok = false;
        }

        LOG_INFO("Vendor:         {0}", glGetString(GL_VENDOR));
        LOG_INFO("Renderer:       {0}", glGetString(GL_RENDERER));
        LOG_INFO("Version OpenGL: {0}", glGetString(GL_VERSION));
        LOG_INFO("Version GLSL:   {0}", glGetString(GL_SHADING_LANGUAGE_VERSION));

        int attribute_count = 0;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &attribute_count);
        LOG_INFO("Max shader attribute count: {0}", attribute_count);

        // todo(Gustav): move this to a better place
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_SCISSOR_TEST);  // need scissor test for the viewport clearing

        if(blend_hack == Init::BlendHack::EnableHack)
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


    core::mat4f
    Init::GetOrthoProjection(float width, float height) const
    {
        ASSERT(ok);
        return core::mat4f::Ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
    }


    void
    Init::Use2d() const
    {
        ASSERT(ok);
        glDisable(GL_DEPTH_TEST);
    }


    void
    Init::ClearScreen(const core::rgb& color) const
    {
        ASSERT(ok);
        glClearColor(color.r, color.g, color.b, 1.0f);
        // glClearStencil(0xFF);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

}  // namespace euphoria::render
