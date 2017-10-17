#include "render/world.h"

#include "render/materialshader.h"

#include "render/camera.h"
#include "render/viewport.h"

#include "core/assert.h"

#include "render/gl.h"

World::World()
{
  outline_shader.reset(new MaterialShader());
  outline_shader->Compile(
      "#version 330 core\n"
      "in vec3 aPosition;\n"
      "in vec3 aNormal;\n"
      "in vec2 aTexCoord;\n"
      "\n"
      "uniform mat4 uProjection;\n"
      "uniform mat4 uView;\n"
      "uniform mat4 uModel;\n"
      "uniform mat3 uNormalMatrix;\n"
      "\n"
      "void main()\n"
      "{\n"
      "    gl_Position = uProjection * uView * uModel \n"
      "                  * vec4(aPosition + aNormal*0.02, 1.0);\n"
      "}\n",
      "#version 330 core\n"
      "out vec4 FragColor;\n"
      "void main()\n"
      "{\n"
      "    FragColor = vec4(0.04, 0.28, 0.26, 1.0);\n"
      "}\n");
}

void
World::AddActor(const std::shared_ptr<Actor>& actor)
{
  actors_.push_back(actor);
}

void
World::Render(const Viewport& viewport, const Camera& camera)
{
  ASSERT(viewport.IsActive());

  const auto projection_matrix =
      camera.CalculateProjectionMatrix(viewport.GetAspectRatio());
  const auto view_matrix = camera.CalculateViewMatrix();

  if(use_outline)
  {
    glEnable(GL_DEPTH_TEST);
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
  }

  for(const auto& actor : actors_)
  {
    // todo: instead of direct rendering, move to a material sorting/render
    // command system
    // general design: http://realtimecollisiondetection.net/blog/?p=86
    // distance from camera:
    // https://gamedev.stackexchange.com/questions/56810/how-do-games-deal-with-z-sorting-partially-transparent-foliage-textures
    // distance in integer:
    // http://aras-p.info/blog/2014/01/16/rough-sorting-by-depth/
    // useful?
    // https://gamedev.stackexchange.com/questions/45626/how-to-organize-rendering
    if(use_outline)
    {
      if(actor->has_outline)
      {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
      }
      else
      {
        // glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0x00);
      }
    }
    actor->Render(projection_matrix, view_matrix, camera.GetPosition(), light);
  }

  if(use_outline)
  {
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    outline_shader->UseShader();

    for(const auto& actor : actors_)
    {
      if(actor->has_outline)
      {
        actor->BasicRender(projection_matrix, view_matrix, outline_shader);
      }
    }

    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
  }
}
