#include "defaultfiles.h"

#include "core/vfs.h"
#include "core/vfs_path.h"

namespace euphoria::render
{
    void
    setup_default_files(std::shared_ptr<core::vfs::read_root_catalog> catalog)
    {
        catalog->register_file_string
        (
            core::vfs::file_path{"~/texture_types.xml"},
            R"json(
                {
                    "name":
                    [
                        "Diffuse",
                        "Specular"
                    ]
                }
            )json"
        );

        catalog->register_file_string
        (
            core::vfs::file_path{"~/default_shader.xml"},
            R"json(
                {
                    "has_light": true,
                    "ambient": "uMaterial.ambient",
                    "diffuse": "uMaterial.diffuse",
                    "specular": "uMaterial.specular",
                    "shininess": "uMaterial.shininess",
                    "textures":
                    [
                        {
                            "texture": "Diffuse",
                            "uniform": "uDiffuseMap"
                        },
                        {
                            "texture": "Specular",
                            "uniform": "uSpecularMap"
                        }
                    ],
                    "default_textures":
                    [
                        {
                            "texture": "Diffuse",
                            "path": "img-plain/white"
                        },
                        {
                            "texture": "Specular",
                            "path": "img-plain/white"
                        }
                    ]
                }
            )json"
        );
        catalog->register_file_string
        (
            core::vfs::file_path{"~/default_shader.vert"},
            R"glsl(
                #version 330 core
                in vec3 aPosition;
                in vec3 aNormal;
                in vec2 aTexCoord;

                out vec2 texCoord;
                out vec3 normal;
                out vec3 fragPositionWorld;

                uniform mat4 uProjection;
                uniform mat4 uView;
                uniform mat4 uModel;
                uniform mat3 uNormalMatrix;

                void main()
                {
                    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
                    fragPositionWorld = vec3(uModel * vec4(aPosition, 1.0));
                    texCoord = aTexCoord;
                    normal = uNormalMatrix * aNormal;
                }
            )glsl"
        );
        catalog->register_file_string
        (
            core::vfs::file_path{"~/default_shader.frag"},
            R"glsl(
                #version 330 core
                struct Material
                {
                    vec3 ambient;
                    vec3 diffuse;
                    vec3 specular;
                    float shininess;
                };

                struct Light
                {
                    int type;
                    vec3 ambient;
                    vec3 diffuse;
                    vec3 specular;

                    float attConst;
                    float attLin;
                    float attQuad;
                    float cosCutoffAngleInner;
                    float cosCutoffAngleOuter;

                    vec3 position;
                    vec3 direction;
                };

                out vec4 FragColor;

                in vec2 texCoord;
                in vec3 normal;
                in vec3 fragPositionWorld;

                uniform sampler2D uDiffuseMap;
                uniform sampler2D uSpecularMap;

                uniform Light uLight;
                uniform vec3 uViewPosition;
                uniform Material uMaterial;

                void main()
                {
                    vec3 ambient = uMaterial.ambient * uLight.ambient;

                    vec3 norm = normalize(normal);
                    vec3 lightDir = uLight.type==0 ? normalize(-uLight.direction) : normalize(uLight.position - fragPositionWorld);
                    float diffuse_factor = max(0.0, dot(norm, lightDir));
                    vec3 diffuse = diffuse_factor * uMaterial.diffuse * uLight.diffuse;

                    float specularStrength = 0.5;
                    vec3 viewDir = normalize(uViewPosition - fragPositionWorld);
                    vec3 reflectDir = reflect(-lightDir, norm);
                    float spec = pow(max(0, dot(viewDir, reflectDir)), uMaterial.shininess);
                    vec3 specular = specularStrength * uMaterial.specular * spec * uLight.specular;

                    if(uLight.type == 2)
                    {
                        float theta = dot(lightDir, normalize(-uLight.direction));
                        if(theta > uLight.cosCutoffAngleOuter)
                        {
                            float epsilon = uLight.cosCutoffAngleInner - uLight.cosCutoffAngleOuter;
                            float intensity = clamp((theta - uLight.cosCutoffAngleOuter) / epsilon, 0.0, 1.0);
                            diffuse *= intensity;
                            specular *= intensity;
                        }
                        else
                        {
                            // outside of spotlight
                            diffuse = vec3(0,0,0);
                            specular = vec3(0,0,0);
                        }
                    }

                    if(uLight.type != 0)
                    {
                        float dist = length(uLight.position - fragPositionWorld);
                        float att = 1.0 / (uLight.attConst + dist*uLight.attLin + dist*dist*uLight.attQuad);
                        ambient *= att;
                        diffuse *= att;
                        specular *= att;
                    }

                    vec3 object_color = texture(uDiffuseMap, texCoord).rgb;
                    vec3 specular_map = texture(uSpecularMap, texCoord).rgb;

                    vec3 result = (ambient + diffuse) * object_color + specular * specular_map;

                    FragColor = vec4(result, 1.0);
                }
            )glsl"
        );


        ///////////////////////////////////////////////////////////////////////
        // basic_shader
        catalog->register_file_string
        (
            core::vfs::file_path{"~/basic_shader.xml"},
            R"json(
                {
                    "diffuse": "uDiffuse",
                    "has_light": false,
                    "textures": [],
                    "default_textures": []
                }
            )json"
        );
        catalog->register_file_string
        (
            core::vfs::file_path{"~/basic_shader.vert"},
            R"glsl(
                #version 330 core
                in vec3 aPosition;

                uniform mat4 uProjection;
                uniform mat4 uView;
                uniform mat4 uModel;

                void main()
                {
                    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
                }
            )glsl"
        );
        catalog->register_file_string
        (
            core::vfs::file_path{"~/basic_shader.frag"},
            R"glsl(
                #version 330 core

                uniform vec3 uDiffuse;

                out vec4 FragColor;

                void main()
                {
                    FragColor = vec4(uDiffuse, 1.0);
                }
            )glsl"
        );

        ///////////////////////////////////////////////////////////////////////
        // default_line_shader
        catalog->register_file_string
        (
            core::vfs::file_path{"~/default_line_shader.xml"},
            R"json(
                {
                    "has_light": false,
                    "textures": [],
                    "default_textures": []
                }
            )json"
        );
        catalog->register_file_string
        (
            core::vfs::file_path{"~/default_line_shader.vert"},
            R"glsl(
                #version 330 core
                in vec3 aPosition;
                in vec3 aColor;

                uniform mat4 uProjection;
                uniform mat4 uView;
                uniform mat4 uModel;

                out vec3 color;

                void main()
                {
                    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
                    color = aColor;
                }
            )glsl"
        );
        catalog->register_file_string
        (
            core::vfs::file_path{"~/default_line_shader.frag"},
            R"glsl(
                #version 330 core

                in vec3 color;

                out vec4 FragColor;

                void main()
                {
                    FragColor = vec4(color, 1.0);
                }
            )glsl"
        );
    }
}
