#include <SDL2/SDL.h>
#include <iostream>

#include "spacetyper/gl.h"
#include "spacetyper/shader.h"
#include "spacetyper/spriterender.h"
#include "spacetyper/texture.h"

#include "generated_shaders.h"
#include "spacetyper/background.h"
#include "spacetyper/debuggl.h"

int main(int argc, char** argv) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0) {
    std::cerr << "Failed to init SDL: " << SDL_GetError() << "\n";
    return -1;
  }

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

  int width = 800;
  int height = 600;

  SDL_Window* window = SDL_CreateWindow("Space Typer", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, width, height,
                                        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  if (window == NULL) {
    std::cerr << "Failed to create window " << SDL_GetError() << "\n";
    return -1;
  }

  SDL_GL_CreateContext(window);
  std::cout << "Created OpenGL " << glGetString(GL_VERSION) << " context"
            << "\n";

  const GLenum err = glewInit();
  if (GLEW_OK != err) {
    std::cerr << "Failed to init glew, error: " << glewGetErrorString(err)
              << "\n";
    return -4;
  }

  SetupOpenglDebug();

  Texture2d ship("player.png");
  Texture2d starSmall("starSmall.png");
  Texture2d starBig("starBig.png");
  Shader shader(shader_source_sprite_vert, shader_source_sprite_frag);
  SpriteRenderer renderer(&shader);

  Background smallStars(25, width, height, &starSmall, 20);
  Background bigStars(15, width, height, &starBig, 50);

  glm::mat4 projection =
      glm::ortho(0.0f, static_cast<GLfloat>(width),
                 static_cast<GLfloat>(height), 0.0f, -1.0f, 1.0f);
  Use(&shader);
  shader.SetInteger("image", 0);
  shader.SetMatrix4("projection", projection);

  glViewport(0, 0, width, height);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Uint64 NOW = SDL_GetPerformanceCounter();
  Uint64 LAST = 0;

  bool quit = false;
  while (!quit) {
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();
    const float dt = (NOW - LAST) * 1.0f / SDL_GetPerformanceFrequency();
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    smallStars.Update(dt);
    bigStars.Update(dt);

    glClearColor(42.0f / 255, 45.0f / 255, 51.0f / 255, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    smallStars.Render(&renderer);
    bigStars.Render(&renderer);
    renderer.DrawSprite(
        ship, glm::vec2(width / 2, height - ship.height() / 2 - 10), 0.0f);
    SDL_GL_SwapWindow(window);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
