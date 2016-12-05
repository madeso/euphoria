#include <SDL2/SDL.h>
#include <iostream>

#include "spacetyper/gl.h"
#include "spacetyper/shader.h"
#include "spacetyper/spriterender.h"
#include "spacetyper/spritefader.h"

#include "generated_shaders.h"
#include "spacetyper/background.h"
#include "spacetyper/debuggl.h"
#include "spacetyper/scalablesprite.h"
#include "spacetyper/fonts.h"
#include "spacetyper/texturecache.h"
#include "spacetyper/dictionary.h"
#include "spacetyper/enemies.h"
#include "spacetyper/enemyword.h"
#include "spacetyper/bulletlist.h"
#include "spacetyper/interpolate.h"

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
  Shader back_shader(shader_source_back_vert, shader_source_back_frag);
  Font font(&font_shader, "SourceCodePro-Regular.ttf", 30, " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ;:,.-_<>|1234567890!\"#¤%&/()'=?@£$€¥{[]}\\'*");
  // (cache.GetTexture("metalPanel_blueCorner.png"), 62, 14, 33, 14, glm::vec2(240, 240));
  std::shared_ptr<ScalableSprite> target = LoadScalableSprite("crossair_white.png", glm::vec2(100, 100), &cache);
  SpriteRenderer renderer(&shader);


  Dictionary dictionary;

  Layer background(&renderer);
  Layer objects(&renderer);
  Layer foreground(&renderer);
  Background smallStars(25, width, height, cache.GetTexture("starSmall.png"), 20, &background);
  Background bigStars(15, width, height, cache.GetTexture("starBig.png"), 50, &background);

  SpriteFader fader(&foreground);

  // smoke effects
  fader.RegesterTexture(cache.GetTexture("explosion/spaceEffects_008.png"));
  fader.RegesterTexture(cache.GetTexture("explosion/spaceEffects_009.png"));
  fader.RegesterTexture(cache.GetTexture("explosion/spaceEffects_010.png"));
  fader.RegesterTexture(cache.GetTexture("explosion/spaceEffects_011.png"));
  fader.RegesterTexture(cache.GetTexture("explosion/spaceEffects_012.png"));
  fader.RegesterTexture(cache.GetTexture("explosion/spaceEffects_013.png"));
  fader.RegesterTexture(cache.GetTexture("explosion/spaceEffects_014.png"));
  fader.RegesterTexture(cache.GetTexture("explosion/spaceEffects_015.png"));
  fader.RegesterTexture(cache.GetTexture("explosion/spaceEffects_016.png"));

  // "laser"/explosion effects
  fader.RegesterTexture(cache.GetTexture("explosion/laserBlue08.png"));
  fader.RegesterTexture(cache.GetTexture("explosion/laserBlue10.png"));
  fader.RegesterTexture(cache.GetTexture("explosion/laserGreen14.png"));
  fader.RegesterTexture(cache.GetTexture("explosion/laserGreen16.png"));
  fader.RegesterTexture(cache.GetTexture("explosion/laserRed08.png"));
  fader.RegesterTexture(cache.GetTexture("explosion/laserRed10.png"));

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

  Use(&back_shader);
  back_shader.SetMatrix4("projection", projection);

  glViewport(0, 0, width, height);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  TextBackgroundRenderer text_back(&back_shader);

  Uint64 NOW = SDL_GetPerformanceCounter();
  Uint64 LAST = 0;

  SDL_StartTextInput();

  BulletList bullets(&objects);
  Enemies enemies(&fader, &cache, &font, &text_back, &objects, &dictionary, width, &bullets);
  EnemyWord* current_word = nullptr;

  enemies.SpawnEnemies(5);

  FloatInterpolate player_rotation(0.0f);
  const float ROTATION_TIME = 0.5f;

  FloatInterpolate target_scale(1.0f);

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
        const std::string& input = e.text.text;
        if( current_word == nullptr ) {
          current_word = enemies.DetectWord(input);
          if( current_word != nullptr ) {
            const float target_rotation = enemies.FireAt(shipPos, current_word);
            player_rotation.Clear().CircOut(target_rotation, ROTATION_TIME);
            target_scale.SetValue(10.0f).Clear().CircOut(1.0f, ROTATION_TIME);
          }
        }
        else {
          const bool hit = current_word->Type(input);
          if( hit ) {
            const float target_rotation = enemies.FireAt(shipPos, current_word);
            player_rotation.Clear().CircOut(target_rotation, ROTATION_TIME);
          }
          if( current_word->IsAlive() == false ) {
            enemies.Remove(current_word);
            current_word = nullptr;
            const float target_rotation = 0.0f;
            player_rotation.Clear().CircOut(target_rotation, ROTATION_TIME);
          }
        }
      }
    }

    smallStars.Update(dt);
    bigStars.Update(dt);
    enemies.Update(dt);
    bullets.Update(dt);
    fader.Update(dt);
    player_rotation.Update(dt);
    target_scale.Update(dt);
    player.SetRotation(player_rotation);

    glClearColor(42.0f / 255, 45.0f / 255, 51.0f / 255, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    background.Render();
    objects.Render();
    enemies.Render();
    foreground.Render();
    // dont render ninepatch unless we are in a meny = not yet implemented :)
    if( current_word != nullptr ) {
      target->SetClientSize(current_word->GetSize() * target_scale.GetValue());
      renderer.DrawNinepatch(*target.get(), current_word->GetPosition() - target->GetSize()/2.0f - target->GetClientOffset());
    }
    SDL_GL_SwapWindow(window);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
