#include "render/viewport.h"

#include "render/gl.h"

void SetupFullViewport(int width, int height) {
  glViewport(0, 0, width, height);
}
