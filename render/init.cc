#include "render/init.h"

#include "core/log.h"

#include <SDL2/SDL_video.h>
#include "render/gl.h"

#include <iostream>

LOG_SPECIFY_DEFAULT_LOGGER("render.init")

Init::Init() : ok(true) {
  const int glad_result = gladLoadGLLoader(SDL_GL_GetProcAddress);
  if(!glad_result) {
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
  // glDisable(GL_CULL_FACE);
  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Init::~Init() {
}

void Init::ClearScreen(const Rgb& color) {
  // 42.0f / 255, 45.0f / 255, 51.0f / 255
  glClearColor(color.GetRed(), color.GetGreen(), color.GetBlue(), 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
