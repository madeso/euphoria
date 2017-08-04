#include <SDL2/SDL.h>
#include <iostream>
#include <core/mat4.h>
#include <render/init.h>
#include <render/debuggl.h>
#include <render/materialshader.h>
#include <memory>
#include <render/compiledmesh.h>
#include "render/shaderattribute3d.h"
#include "render/texture.h"
#include "core/filesystem.h"
#include "core/draw.h"
#include "core/random.h"
#include "core/shufflebag.h"
#include "render/world.h"
#include "render/viewport.h"
#include "render/camera.h"
#include "core/mat4.h"
#include "core/axisangle.h"

void SetupSdlOpenGlAttributes() {
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 4);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 0);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
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
}

class SdlTimer {
 public:
  SdlTimer()
      : current_time_(SDL_GetPerformanceCounter())
      , last_time_(0)
  { }

  float Update() {
    last_time_ = current_time_;
    current_time_ = SDL_GetPerformanceCounter();
    const float dt = (current_time_ - last_time_) * 1.0f / SDL_GetPerformanceFrequency();
    return dt;
  }

 private:
  Uint64 current_time_;
  Uint64 last_time_;
};

class Sdl {
 public:
  Sdl() : ok(false) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0) {
      std::cerr << "Failed to init SDL: " << SDL_GetError() << "\n";
      return;
    }

    ok = true;
  }

  ~Sdl() {
    SDL_Quit();
  }

  bool ok;
};

class SdlWindow {
 public:
  SdlWindow(const std::string& title, int width, int height) : window(nullptr) {
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, width, height,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (window == nullptr) {
      std::cerr << "Failed to create window " << SDL_GetError() << "\n";
    }
  }

  ~SdlWindow() {
    SDL_DestroyWindow(window);
  }

  SDL_Window* window;
};

class SdlGlContext {
 public:
  SdlGlContext(SdlWindow* window) : context(nullptr) {
    context = SDL_GL_CreateContext(window->window);
    if (context == nullptr) {
      std::cerr << "Failed to create GL context " << SDL_GetError() << "\n";
      return;
    }
  }

  ~SdlGlContext() {
    SDL_GL_DeleteContext(context);
  }

  SDL_GLContext context;
};

int main(int argc, char** argv) {
  Sdl sdl;
  if (sdl.ok == false) {
    return -1;
  }

  SetupSdlOpenGlAttributes();

  int width = 800;
  int height = 600;

  SdlWindow window {"Euphoria Demo", 800, 600};
  if (window.window == nullptr) {
    return -1;
  }

  SdlGlContext context {&window};

  if (context.context == nullptr) {
    return -1;
  }

  Init init;
  if (init.ok == false) {
    return -4;
  }

  SetupOpenglDebug();

  Viewport viewport { Recti::FromTopLeftWidthHeight(0,0, width, height) };
  viewport.Activate();

  FileSystem file_system;
  auto catalog = FileSystemRootCatalog::AddRoot(&file_system);
  FileSystemRootFolder::AddRoot(&file_system);
  catalog->RegisterFileString("default_shader.vert",
                              "#version 330 core\n"
                                  "in vec3 aPosition;\n"
                                  "in vec2 aTexCoord;\n"
                                  "\n"
                                  "out vec2 texCoord;\n"
                                  "\n"
                                  "uniform mat4 uProjection;\n"
                                  "uniform mat4 uView;\n"
                                  "uniform mat4 uModel;\n"
                                  "\n"
                                  "void main()\n"
                                  "{\n"
                                  "    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);\n"
                                  "    texCoord = aTexCoord;\n"
                                  "}\n");
  catalog->RegisterFileString("default_shader.frag",
                              "#version 330 core\n"
                                  "out vec4 FragColor;\n"
                                  "\n"
                                  "in vec2 texCoord;\n"
                                  "\n"
                                  "uniform sampler2D uTexture;\n"
                                  "\n"
                                  "void main()\n"
                                  "{\n"
                                  "    FragColor = texture(uTexture, texCoord);\n"
                                  "}\n");

  MaterialShader shader;
  const bool shader_compile = shader.Load(&file_system, "default_shader");

  if(!shader_compile) {
    return -3;
  }

  auto texture_uniform = shader.shader_.GetUniform("uTexture");

  Image image;
  image.Setup(256, 256, false);
  Draw drawer{&image};
  const auto wi = drawer.WholeImage();
  drawer
      .Clear(Rgb::From(Color::Red));
  Random random {42};

  for(int i=0; i<20; i+= 1) {
    const auto color = Rgb::From(random.NextDawnbringerPalette());
    const auto pos = random.NextPoint(wi);
    const auto outer = random.NextRange(55.0f, 100.0f);
    const auto inner = random.NextRange(50.0f);
    drawer.Circle(color, pos, outer, 10, inner);
  }
  drawer
      .LineAntialiased(Rgb::From(Color::Black), wi.TopLeft(), wi.BottomRight())
      .LineAntialiased(Rgb::From(Color::Black), wi.BottomLeft(), wi.TopRight())
    .Text(wi.GetAbsoluteCenterPos(), "Hello world", Rgb::From(Color::Black), 2)
      ;
  catalog->RegisterFileData("image", image.Write(ImageWriteFormat::PNG));


  Texture2d texture;
  texture.LoadFromFile(&file_system, "image", AlphaLoad::Remove, Texture2dLoadData{});

  bool running = true;

  SdlTimer timer;

  World world;
  std::shared_ptr<Actor> actor = std::make_shared<Actor>(CompileMesh(meshes::CreateCube(0.5f)));
  world.AddActor(actor);

  actor->SetPosition(vec3f{0,0.5f,0});
  actor->SetRotation(quatf::FromAxisAngle(AxisAngle::RightHandAround(vec3f::XAxis(), Angle::FromDegrees(-55))));

  Camera camera;
  camera.SetPosition(vec3f(0,0,3));

  while (running) {
    const float delta = timer.Update();

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {
        case SDL_QUIT:
          running = false;
          break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
          {
            const bool down = e.type == SDL_KEYDOWN;
            switch( e.key.keysym.sym ) {
              case SDLK_ESCAPE:
                if(down) {
                  running = false;
                }
                break;
              default:
                // ignore other keys
                break;
            }
          }
          break;
        default:
          // ignore other events
          break;
      }
    }

    init.ClearScreen(Rgb::From(Color::DarkslateGray));
    shader.UseShader();
    BindTextureToShader(&texture, &shader.shader_, texture_uniform, 0);
    world.Render(viewport, camera, &shader);

    SDL_GL_SwapWindow(window.window);
  }

  return 0;
}

#if 0
class View {
Camera[] cameras;
};

class CameraView {
Camera;
ViewPort area; // glViewport
};

class Camera {
Mode mode;
World world;
culling info from world, created when camera is added to world
viewMatrix; // position + rotation
projectionMatrix; // ortho/perspective

// 4 planes from the projection matrix
getFrustum();

toViewSpace(); // camera space, eye space
toWorldSpace();
};

// normal, thermo optics/powerup
class Mode {
Pass[] passes;
};

class Pass {
tag/list
Shader shader;
};

class Shader {
material list: list of material names mapped to shader variables
};

class World {
Actor[] objects;
};

class Actor {
Mesh mesh;
position;
rotation;
modelMatrix;
ToLocalSpace();
ToWorldSpace();
};

class Mesh {
MeshPart[] meshParts;
};

class MeshPart {
Material material;
};

class MaterialList {
maps id, material name, shader variable
};

class Material {
Texture[] textures;
};
#endif
