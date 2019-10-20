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
        struct Path;
    }
}  // namespace euphoria::core

namespace euphoria::render
{
    struct Texture2d;
    struct MaterialShader;
    struct Light;

    struct MaterialShaderCache;
    struct TextureCache;

    // one part of the mesh, single material
    struct CompiledMeshPart
    {
        // todo(Gustav): move vertex buffer and point layout to the compile mesh instead
        VertexBuffer data;
        PointLayout
                     config;  // todo(Gustav): rename to something better... like layout?
        IndexBuffer  tris;
        int          tri_count;
        unsigned int material;
    };

    struct CompiledMeshMaterial
    {
        CompiledMeshMaterial();

        void
        SetTexture(
                const core::EnumValue&     name,
                std::shared_ptr<Texture2d> texture);

        void
        Apply(const core::mat4f& model_matrix,
              const core::mat4f& projection_matrix,
              const core::mat4f& view_matrix,
              const core::vec3f& camera,
              const Light&       light) const;

        /** Gets the default materials from the shader if they are null/not set.
         */
        void
        LoadDefaultMaterialsFromShader(TextureCache* cache);

        /** Asks the shader if all the textures are set, and if more than necessary are set.
         */
        bool
        Validate() const;

        core::Rgb                                             ambient;
        core::Rgb                                             diffuse;
        core::Rgb                                             specular;
        float                                                 shininess;
        std::shared_ptr<MaterialShader>                       shader;
        std::map<core::EnumValue, std::shared_ptr<Texture2d>> textures;
    };

    /** A collection of parts making up a mesh.
     */
    struct CompiledMesh
    {
        std::vector<std::shared_ptr<CompiledMeshPart>> parts;
        std::vector<CompiledMeshMaterial>              materials;

        std::vector<std::shared_ptr<CompiledMeshMaterial>>
        GetNoOverriddenMaterials() const;

        void
        Render(const core::mat4f& model_matrix,
               const core::mat4f& projection_matrix,
               const core::mat4f& view_matrix,
               const core::vec3f& camera,
               const Light&       light,
               const std::vector<std::shared_ptr<CompiledMeshMaterial>>&
                       overridden_materials);

        void
        BasicRender(
                const core::mat4f&              model_matrix,
                const core::mat4f&              projection_matrix,
                const core::mat4f&              view_matrix,
                std::shared_ptr<MaterialShader> shader);
    };

    std::shared_ptr<CompiledMesh>
    CompileMesh(
            const core::Mesh&      mesh,
            MaterialShaderCache*   shader_cache,
            TextureCache*          texture_cache,
            const core::vfs::Path& texture_folder);
}  // namespace euphoria::render

#endif  // EUPHORIA_COMPILEDMESH_H
