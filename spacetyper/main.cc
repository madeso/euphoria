#include <SDL2/SDL.h>
#include <iostream>

#include "spacetyper/gl.h"
#include "spacetyper/shader.h"
#include "spacetyper/spriterender.h"
#include "spacetyper/texture.h"

#include "generated_shaders.h"
#include "spacetyper/background.h"
#include "spacetyper/debuggl.h"
#include "spacetyper/ninepatch.h"
#include "spacetyper/fonts.h"
#include "spacetyper/texturecache.h"
#include "spacetyper/dictionary.h"
#include "spacetyper/enemies.h"

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

  TextureCache cache;
  Shader shader(shader_source_sprite_vert, shader_source_sprite_frag);
  Shader font_shader(shader_source_font_vert, shader_source_font_frag);
  Font font(&font_shader, "SourceCodePro-Regular.ttf", 30, " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ;:,.-_<>|1234567890!\"#¤%&/()'=?@£$€¥{[]}\\'*");
  Ninepatch ninepatch(cache.GetTexture("metalPanel_blueCorner.png"), 62, 14, 33, 14, glm::vec2(240, 240));
  SpriteRenderer renderer(&shader);

  Dictionary dictionary;

  Layer background(&renderer);
  Layer objects(&renderer);
  Background smallStars(25, width, height, cache.GetTexture("starSmall.png"), 20, &background);
  Background bigStars(15, width, height, cache.GetTexture("starBig.png"), 50, &background);

  Sprite player(cache.GetTexture("player.png"));
  objects.Add(&player);

  glm::vec2 shipPos(width / 2, height - player.GetHeight() / 2 - 10);
  player.SetPosition(shipPos);

  glm::mat4 projection =
      glm::ortho(0.0f, static_cast<GLfloat>(width),
                 static_cast<GLfloat>(height), 0.0f, -1.0f, 1.0f);
  Use(&shader);
  shader.SetInteger("image", 0);
  shader.SetMatrix4("projection", projection);

  Use(&font_shader);
  font_shader.SetInteger("image", 0);
  font_shader.SetMatrix4("projection", projection);

  glViewport(0, 0, width, height);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Text text(dictionary.Generate(), &font);
  text.SetSize(30);
  text.SetAlignment(Align::CENTER);
  text.SetHighlightRange(0, 1);

  Uint64 NOW = SDL_GetPerformanceCounter();
  Uint64 LAST = 0;

  SDL_StartTextInput();

  std::string data;

  Enemies enemies(&cache, &font, &objects, &dictionary, width);

  enemies.SpawnEnemies(5);

  bool running = true;
  while (running) {
    LAST = NOW;
    NOW = SDL_GetPerformanceCounter();
    const float dt = (NOW - LAST) * 1.0f / SDL_GetPerformanceFrequency();
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT) {
        running = false;
      }
      else if( e.type == SDL_TEXTINPUT ) {
        data += e.text.text;
        text.SetText(data);
      }
    }

    smallStars.Update(dt);
    bigStars.Update(dt);
    enemies.Update(dt);

    glClearColor(42.0f / 255, 45.0f / 255, 51.0f / 255, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    background.Render();
    objects.Render();
    enemies.Render();
    renderer.DrawNinepatch(ninepatch, glm::vec2(200,200));
    text.Draw(shipPos);
    SDL_GL_SwapWindow(window);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
