#pragma once

#include <memory>
#include <map>

#include "core/mesh.h"
#include "core/mat4.h"
#include "render/buffer.h"


namespace euphoria::core::vfs
{
    struct DirPath;
}


namespace euphoria::render
{
    struct texture2d;
    struct MaterialShader;
    struct Light;
    struct MaterialOverride;
    struct MaterialShaderCache;
    struct TextureCache;


    // one part of the mesh, single material
    struct CompiledMeshPart
    {
        // todo(Gustav): move vertex buffer and point layout to the compile mesh instead
        VertexBuffer data;

        // todo(Gustav): rename config to something better... like layout?
        PointLayout config;
        IndexBuffer tris;
        int tri_count;
        unsigned int material;
    };


    struct CompiledMeshMaterial
    {
        CompiledMeshMaterial();


        void
        SetTexture
        (
            const core::EnumValue& name,
            std::shared_ptr<texture2d> texture
        );


        void
        Apply
        (
            const core::mat4f& model_matrix,
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light& light
        ) const;


        /** Gets the default materials from the shader if they are null/not set.
         */
        void
        LoadDefaultMaterialsFromShader(TextureCache* cache);


        /** Asks the shader if all the textures are set,
         * and if more than necessary are set.
         */
        [[nodiscard]] bool
        Validate() const;


        core::rgb ambient;
        core::rgb diffuse;
        core::rgb specular;
        float shininess;
        std::shared_ptr<MaterialShader> shader;
        std::map<core::EnumValue, std::shared_ptr<texture2d>> textures;
    };


    /** A collection of parts making up a mesh.
     */
    struct CompiledMesh
    {
        std::vector<std::shared_ptr<CompiledMeshPart>> parts;
        std::vector<CompiledMeshMaterial> materials;


        void
        Render
        (
            const core::mat4f& model_matrix,
            const core::mat4f& projection_matrix,
            const core::mat4f& view_matrix,
            const core::vec3f& camera,
            const Light& light,
            const std::shared_ptr<MaterialOverride>& overridden_materials
        );
    };


    std::shared_ptr<CompiledMesh>
    CompileMesh
    (
        const core::Mesh& mesh,
        MaterialShaderCache* shader_cache,
        TextureCache* texture_cache,
        const core::vfs::DirPath& texture_folder,
        const std::string& debug_name
    );
}
