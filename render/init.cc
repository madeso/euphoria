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

  // todo: replace cout with log system
  std::cout << "Vendor:         " << glGetString(GL_VENDOR) << "\n";
  std::cout << "Renderer:       " << glGetString(GL_RENDERER) << "\n";
  std::cout << "Version OpenGL: " << glGetString(GL_VERSION) << "\n";
  std::cout << "Version GLSL:   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

  int attribute_count;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &attribute_count);
  std::cout << "Max shader attribute count: " << attribute_count << "\n";

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
