#include "render/init.h"

#include "render/gl.h"

#include <iostream>

Init::Init() : ok(true) {
  std::cout << "Created OpenGL " << glGetString(GL_VERSION) << " context"
            << "\n";

  const GLenum err = glewInit();
  if (GLEW_OK != err) {
    std::cerr << "Failed to init glew, error: " << glewGetErrorString(err)
              << "\n";
    ok = false;
  }

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Init::~Init() {
}

void Init::ClearScreen() {
  glClearColor(42.0f / 255, 45.0f / 255, 51.0f / 255, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}
