#include <SDL2/SDL.h>
#include <iostream>

#include "render/shader.h"
#include "render/spriterender.h"
#include "spacetyper/spritefader.h"

#include "spacetyper/background.h"
#include "render/debuggl.h"
#include "render/scalablesprite.h"
#include "render/fonts.h"
#include "render/texturecache.h"
#include "spacetyper/dictionary.h"
#include "spacetyper/enemies.h"
#include "spacetyper/enemyword.h"
#include "spacetyper/bulletlist.h"
#include "core/interpolate.h"
#include "render/init.h"
#include "render/viewport.h"

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
  Init init;

  if (init.ok == false) {
    return -4;
  }

  SetupOpenglDebug();

  TextureCache cache;
  Shader shader("shaders/sprite");
  Shader font_shader("shaders/font");
  Shader back_shader("shaders/back");
  Font font(&font_shader, "SourceCodePro-Regular.ttf", 30, " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ;:,.-_<>|1234567890!\"#¤%&/()'=?@£$€¥{[]}\\'*");
  // (cache.GetTexture("metalPanel_blueCorner.png"), 62, 14, 33, 14, vec2f(240, 240));
  ScalableSprite target("crossair_white.png", vec2f(100, 100), &cache);
  SpriteRenderer renderer(&shader);


  Dictionary dictionary;

  Layer background(&renderer);
  Layer objects(&renderer);
  Layer foreground(&renderer);
  Background smallStars(25, width, height, cache.GetTexture("starSmall.png"), 20, &background);
  Background bigStars(15, width, height, cache.GetTexture("starBig.png"), 50, &background);

  SpriteFader fader(&foreground);

  // smoke effects
  fader.RegisterTexture(cache.GetTexture("explosion/spaceEffects_008.png"));
  fader.RegisterTexture(cache.GetTexture("explosion/spaceEffects_009.png"));
  fader.RegisterTexture(cache.GetTexture("explosion/spaceEffects_010.png"));
  fader.RegisterTexture(cache.GetTexture("explosion/spaceEffects_011.png"));
  fader.RegisterTexture(cache.GetTexture("explosion/spaceEffects_012.png"));
  fader.RegisterTexture(cache.GetTexture("explosion/spaceEffects_013.png"));
  fader.RegisterTexture(cache.GetTexture("explosion/spaceEffects_014.png"));
  fader.RegisterTexture(cache.GetTexture("explosion/spaceEffects_015.png"));
  fader.RegisterTexture(cache.GetTexture("explosion/spaceEffects_016.png"));

  // "laser"/explosion effects
  fader.RegisterTexture(cache.GetTexture("explosion/laserBlue08.png"));
  fader.RegisterTexture(cache.GetTexture("explosion/laserBlue10.png"));
  fader.RegisterTexture(cache.GetTexture("explosion/laserGreen14.png"));
  fader.RegisterTexture(cache.GetTexture("explosion/laserGreen16.png"));
  fader.RegisterTexture(cache.GetTexture("explosion/laserRed08.png"));
  fader.RegisterTexture(cache.GetTexture("explosion/laserRed10.png"));

  Sprite player(cache.GetTexture("player.png"));
  objects.Add(&player);

  vec2f shipPos(width / 2, height - player.GetHeight() / 2 - 10);
  player.SetPosition(shipPos);

  mat4f projection =
      mat4f::Ortho(0.0f, static_cast<float>(width),
                 static_cast<float>(height), 0.0f, -1.0f, 1.0f);
  Use(&shader);
  shader.SetInteger("image", 0);
  shader.SetMatrix4("projection", projection);

  Use(&font_shader);
  font_shader.SetInteger("image", 0);
  font_shader.SetMatrix4("projection", projection);

  Use(&back_shader);
  back_shader.SetMatrix4("projection", projection);

  SetupFullViewport(width, height);

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
  const float SCALE_TIME = 0.6f;

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
            player_rotation.Clear().BackOut(target_rotation, ROTATION_TIME);
            target_scale.SetValue(19.0f).Clear().CircOut(1.0f, SCALE_TIME);
          }
        }
        else {
          const bool hit = current_word->Type(input);
          if( hit ) {
            const float target_rotation = enemies.FireAt(shipPos, current_word);
            player_rotation.Clear().BackOut(target_rotation, ROTATION_TIME);
          }
          if( current_word->IsAlive() == false ) {
            enemies.Remove(current_word);
            current_word = nullptr;
            const float target_rotation = 0.0f;
            player_rotation.Clear().BackOut(target_rotation, ROTATION_TIME);
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

    init.ClearScreen();

    background.Render();
    objects.Render();
    enemies.Render();
    foreground.Render();
    // dont render ninepatch unless we are in a meny = not yet implemented :)
    if( current_word != nullptr ) {
      const vec2f extra_size(40, 40);
      const vec2f size = current_word->GetSize();
      const vec2f size_and_extra = size + extra_size;
      const vec2f scaled_size = size_and_extra * target_scale.GetValue();
      target.SetSize(scaled_size);
      renderer.DrawNinepatch(target, current_word->GetPosition() - scaled_size/2.0f);
    }
    SDL_GL_SwapWindow(window);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
