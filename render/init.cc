#include "render/init.h"

#include "core/log.h"

#include "render/gl.h"

#include <iostream>

namespace euphoria::render
{

LOG_SPECIFY_DEFAULT_LOGGER("render.init")

Init::Init(LoaderFunction loader, Init::BlendHack blend_hack)
    : ok(true)
{
  const int glad_result = gladLoadGLLoader(loader);
  if(glad_result == 0)
  {
    LOG_ERROR("Failed to init glad, error: " << glad_result);
    ok = false;
  }

  // todo: replace cout with log system
  LOG_INFO("Vendor:         " << glGetString(GL_VENDOR));
  LOG_INFO("Renderer:       " << glGetString(GL_RENDERER));
  LOG_INFO("Version OpenGL: " << glGetString(GL_VERSION));
  LOG_INFO("Version GLSL:   " << glGetString(GL_SHADING_LANGUAGE_VERSION));

  int attribute_count;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &attribute_count);
  LOG_INFO("Max shader attribute count: " << attribute_count);

  // todo: move this to a better place
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_SCISSOR_TEST);  // need scissor test for the viewport clearing

  if(blend_hack == Init::BlendHack::EnableHack)
  {
    LOG_INFO("Enabled blend hack");
    // need to be enabled for shitty 2d rendering to work
    // todo: fix a proper blending/backface culling render stack
    glDisable(GL_CULL_FACE);
    // glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
}

Init::~Init() = default;

core::mat4f
Init::GetOrthoProjection(float width, float height)
{
  return core::mat4f::Ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
}

void
Init::Use2d()
{
  glDisable(GL_DEPTH_TEST);
}

void
Init::ClearScreen(const core::Rgb& color)
{
  // 42.0f / 255, 45.0f / 255, 51.0f / 255
  glClearColor(color.r, color.g, color.b, 1.0f);
  // glClearStencil(0xFF);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

}
