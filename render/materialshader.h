#ifndef EUPHORIA_MATERIALSHADER_H
#define EUPHORIA_MATERIALSHADER_H

#include <vector>

#include "core/enum.h"
#include "render/shader.h"

namespace euphoria::render
{
    struct Light;

    struct MaterialShaderDefaultTexture
    {
    public:
        MaterialShaderDefaultTexture(
                const core::EnumValue& a_name,
                const std::string&     a_path);

        core::EnumValue name;
        std::string     path;
    };

    struct MaterialShaderBinding
    {
        MaterialShaderBinding(
                ShaderUniform          a_uniform,
                const core::EnumValue& a_name);

        ShaderUniform   uniform;
        core::EnumValue name;
    };

    /** Extends a regular Shader with uniform bindings for 3d rendering
     */
    struct MaterialShader
    {
    public:
        MaterialShader();

        bool
        Load(core::vfs::FileSystem* file_system, const std::string& path);

        void
        UseShader();

        void
        SetProjection(const core::mat4f& projection_data);

        void
        SetView(const core::mat4f& view_data);

        void
        SetModel(const core::mat4f& model_data);

        void
        SetupLight(const Light& light, const core::vec3f& camera);

        void
        SetColors(
                const core::Rgb& ambient_data,
                const core::Rgb& diffuse_data,
                const core::Rgb& specular_data,
                float            shininess_data);

        Shader shader;

        // private:
        ShaderUniform projection;
        ShaderUniform view;
        ShaderUniform model;

        ShaderUniform ambient;
        ShaderUniform diffuse;
        ShaderUniform specular;
        ShaderUniform shininess;

        bool          hasLight;
        ShaderUniform lightAmbient;
        ShaderUniform lightDiffuse;
        ShaderUniform lightSpecular;
        ShaderUniform lightPosition;
        ShaderUniform lightDirection;
        ShaderUniform lightType;
        ShaderUniform lightAttenuationConstant;
        ShaderUniform lightAttenuationLinear;
        ShaderUniform lightAttenuationQuadratic;
        ShaderUniform lightCutoffAngleOuter;
        ShaderUniform lightCutoffAngleInner;

        ShaderUniform normalMatrix;
        ShaderUniform viewPosition;

        std::vector<MaterialShaderBinding>        bindings;
        std::vector<MaterialShaderDefaultTexture> default_textures;
    };

}  // namespace euphoria::render

#endif  // EUPHORIA_MATERIALSHADER_H
