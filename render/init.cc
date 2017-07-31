#include "render/init.h"

#include <SDL2/SDL_video.h>
#include "render/gl.h"

#include <iostream>

Init::Init() : ok(true) {
  const int glad_result = gladLoadGLLoader(SDL_GL_GetProcAddress);
  if(!glad_result) {
    std::cerr << "Failed to init glad, error: " << glad_result
              << "\n";
    ok = false;
  }

  std::cout << "Vendor:         " << glGetString(GL_VENDOR) << "\n";
  std::cout << "Renderer:       " << glGetString(GL_RENDERER) << "\n";
  std::cout << "Version OpenGL: " << glGetString(GL_VERSION) << "\n";
  std::cout << "Version GLSL:   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

  // todo: move this to a better place
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Init::~Init() {
}

void Init::SetViewport(const Recti &viewport) {
  const vec2i top_left = viewport.TopLeft();
  glViewport(top_left.x, top_left.y, viewport.GetWidth(), viewport.GetHeight());
}

void Init::ClearScreen() {
  glClearColor(42.0f / 255, 45.0f / 255, 51.0f / 255, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}
