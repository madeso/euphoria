#include "defaultfiles.h"

#include "core/vfs.h"

namespace euphoria::render
{

void
SetupDefaultFiles(std::shared_ptr<core::vfs::FileSystemRootCatalog> catalog)
{
  catalog->RegisterFileString(
      "default_shader.json",
      R"(  {"has_light": true, "ambient": "uMaterial.ambient", "diffuse": "uMaterial.diffuse", "specular": "uMaterial.specular", "shininess": "uMaterial.shininess", "textures": [ {"texture": "Diffuse", "uniform": "uDiffuseMap"}, {"texture": "Specular", "uniform": "uSpecularMap"} ],   "default_textures": [ {"texture": "Diffuse", "path": "img-plain/white"}, {"texture": "Specular", "path": "img-plain/white"} ]        }  )");
  catalog->RegisterFileString(
      "default_shader.vert",
      "#version 330 core\n"
      "in vec3 aPosition;\n"
      "in vec3 aNormal;\n"
      "in vec2 aTexCoord;\n"
      "\n"
      "out vec2 texCoord;\n"
      "out vec3 normal;\n"
      "out vec3 fragPositionWorld;\n"
      "\n"
      "uniform mat4 uProjection;\n"
      "uniform mat4 uView;\n"
      "uniform mat4 uModel;\n"
      "uniform mat3 uNormalMatrix;\n"
      "\n"
      "void main()\n"
      "{\n"
      "    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);\n"
      "    fragPositionWorld = vec3(uModel * vec4(aPosition, 1.0));\n"
      "    texCoord = aTexCoord;\n"
      "    normal = uNormalMatrix * aNormal;\n"
      "}\n");
  catalog->RegisterFileString(
      "default_shader.frag",
      "#version 330 core\n"
      "struct Material\n"
      "{\n"
      "  vec3 ambient;\n"
      "  vec3 diffuse;\n"
      "  vec3 specular;\n"
      "  float shininess;\n"
      "};\n"
      "\n"
      "struct Light\n"
      "{\n"
      "  int type;\n"
      "  vec3 ambient;\n"
      "  vec3 diffuse;\n"
      "  vec3 specular;\n"
      "  \n"
      "  float attConst;\n"
      "  float attLin;\n"
      "  float attQuad;\n"
      "  float cosCutoffAngleInner;\n"
      "  float cosCutoffAngleOuter;\n"
      "  \n"
      "  vec3 position;\n"
      "  vec3 direction;\n"
      "};\n"
      "\n"
      "out vec4 FragColor;\n"
      "\n"
      "in vec2 texCoord;\n"
      "in vec3 normal;\n"
      "in vec3 fragPositionWorld;\n"
      "\n"
      "uniform sampler2D uDiffuseMap;\n"
      "uniform sampler2D uSpecularMap;\n"
      "\n"
      "uniform Light uLight;\n"
      "uniform vec3 uViewPosition;\n"
      "uniform Material uMaterial;\n"
      "\n"
      "void main()\n"
      "{\n"
      "    vec3 ambient = uMaterial.ambient * uLight.ambient;\n"
      "    \n"
      "    vec3 norm = normalize(normal);\n"
      "    vec3 lightDir = uLight.type==0 ? normalize(-uLight.direction) :\n"
      "                    normalize(uLight.position - fragPositionWorld);\n"
      "    float diffuse_factor = max(0.0, dot(norm, lightDir));\n"
      "    vec3 diffuse = diffuse_factor * uMaterial.diffuse * \n"
      "                     uLight.diffuse;\n"
      "    \n"
      "    float specularStrength = 0.5;\n"
      "    vec3 viewDir = normalize(uViewPosition - fragPositionWorld);\n"
      "    vec3 reflectDir = reflect(-lightDir, norm);\n"
      "    float spec = pow(max(0, dot(viewDir, reflectDir)), \n"
      "                   uMaterial.shininess);\n"
      "    vec3 specular = specularStrength * uMaterial.specular * spec * \n"
      "                      uLight.specular;\n"
      "    \n"
      "    if(uLight.type == 2) {\n"
      "      float theta = dot(lightDir, normalize(-uLight.direction));\n"
      "      if(theta > uLight.cosCutoffAngleOuter) {\n"
      "        float epsilon = uLight.cosCutoffAngleInner - "
      "                        uLight.cosCutoffAngleOuter;\n"
      "        float intensity = clamp((theta - uLight.cosCutoffAngleOuter)\n"
      "                                / epsilon, 0.0, 1.0); \n"
      "        diffuse  *= intensity;\n"
      "        specular *= intensity;\n"
      "      }\n"
      "      else {\n"
      "        // outside of spotlight\n"
      "        diffuse = vec3(0,0,0);\n"
      "        specular = vec3(0,0,0);\n"
      "      }\n"
      "    }\n"
      "    \n"
      "    \n"
      "    if(uLight.type != 0) {\n"
      "      float dist = length(uLight.position - fragPositionWorld);\n"
      "      float att = 1.0 / (uLight.attConst + dist*uLight.attLin + \n"
      "                         dist*dist*uLight.attQuad);\n"
      "      ambient *= att;\n"
      "      diffuse *= att;\n"
      "      specular *= att;\n"
      "    }\n"
      "    \n"
      "    vec3 object_color = texture(uDiffuseMap, texCoord).rgb;\n"
      "    vec3 specular_map = texture(uSpecularMap, texCoord).rgb;\n"
      "    \n"
      "    vec3 result = (ambient + diffuse) * object_color + \n"
      "                    specular * specular_map;\n"
      "    \n"
      "    FragColor = vec4(result, 1.0);\n"
      "}\n");

  catalog->RegisterFileString(
      "basic_shader.json",
      R"(  {"diffuse": "uDiffuse", "has_light": false, "textures": [], "default_textures": []}  )");
  catalog->RegisterFileString(
      "basic_shader.vert",
      "#version 330 core\n"
      "in vec3 aPosition;\n"
      "\n"
      "\n"
      "uniform mat4 uProjection;\n"
      "uniform mat4 uView;\n"
      "uniform mat4 uModel;\n"
      "\n"
      "void main()\n"
      "{\n"
      "    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);\n"
      "}\n");
  catalog->RegisterFileString(
      "basic_shader.frag",
      "#version 330 core\n"
      "\n"
      "uniform vec3 uDiffuse;\n"
      "\n"
      "out vec4 FragColor;\n"
      "\n"
      "void main()\n"
      "{\n"
      "    FragColor = vec4(uDiffuse, 1.0);\n"
      "}\n");

  catalog->RegisterFileString(
      "texture_types.json",
      R"({"name" : ["Diffuse", "Specular"]})");
}

}
