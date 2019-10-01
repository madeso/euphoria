#ifndef EUPHORIA_COMPILEDMESH_H
#define EUPHORIA_COMPILEDMESH_H

#include <memory>
#include <map>

#include "core/mesh.h"
#include "core/mat4.h"
#include "render/buffer.h"

namespace euphoria::core
{
    namespace vfs
    {
        class Path;
    }
}

namespace euphoria::render
{

    class Texture2d;
    class MaterialShader;
    class Light;

    class MaterialShaderCache;
    class TextureCache;

    // one part of the mesh, single material
    class CompiledMeshPart
    {
    public:
        Vbo          data;
        Vao          config;
        Ebo          tris;
        int          tri_count;
        unsigned int material;
    };

    class CompiledMeshMaterial
    {
    public:
        CompiledMeshMaterial();

        void
        SetShader(const std::shared_ptr<MaterialShader>& shader);

        void
        SetColors(
            const core::Rgb& ambient,
            const core::Rgb& diffuse,
            const core::Rgb& specular,
            float      shininess);

        void
        SetTexture(const core::EnumValue& name, std::shared_ptr<Texture2d> texture);

        void
        Apply(
            const core::mat4f& model_matrix,
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light& light) const;

        // gets the default materials from the shader if they are null/not set
        void
        LoadDefaultMaterialsFromShader(TextureCache* cache);

        // asks the shader if all the textures are set, and if more than necessary are
        // set
        bool
        Validate() const;

    private:
        core::Rgb                       ambient_;
        core::Rgb                       diffuse_;
        core::Rgb                       specular_;
        float                           shininess_;
        std::shared_ptr<MaterialShader> shader_;
        std::map<core::EnumValue, std::shared_ptr<Texture2d>> textures_;
    };

    // a collection of parts making up a mesh
    class CompiledMesh
    {
    public:
        std::vector<std::shared_ptr<CompiledMeshPart>> parts;
        std::vector<CompiledMeshMaterial>              materials;

        std::vector<std::shared_ptr<CompiledMeshMaterial>>
        GetNoOverriddenMaterials() const;

        void
        Render(
            const core::mat4f& model_matrix,
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light& light,
            const std::vector<std::shared_ptr<CompiledMeshMaterial>>&
                overridden_materials);

        void
        BasicRender(
            const core::mat4f&                    model_matrix,
            const core::mat4f&                    projection_matrix,
            const core::mat4f&                    view_matrix,
            std::shared_ptr<MaterialShader> shader);
    };

    std::shared_ptr<CompiledMesh>
    CompileMesh(
        const core::Mesh&          mesh,
        MaterialShaderCache* shader_cache,
        TextureCache*        texture_cache,
        const core::vfs::Path&          texture_folder);
}

#endif  // EUPHORIA_COMPILEDMESH_H
